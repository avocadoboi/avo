#ifndef AVO_NODE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_NODE_HPP_BJORN_SUNDIN_JUNE_2021

#include "id.hpp"
#include "util/miscellaneous.hpp"
#include "util/recursive_range.hpp"

#include <any>

namespace avo {

/*
	A Node is a container that stores pointers to instances of itself. 
	This means it's a tree data structure. 
	Nodes can have IDs which can be used to retrieve them from the tree. 
	They can also store a pointer to an arbitrary object. 
	
	A node does not own its child nodes - child nodes are added to a node tree by constructing them with a reference to its parent. 
	This means that nodes can be stored in any way you wish; on the stack or on the heap.

	This type can be used to build a tree of software components.
	Each component stores an instance of a Node constructed with its parent node.
	This enables retrieval of other software components in the tree by their IDs.

	Nodes cannot be copied nor moved since that would be error-prone. 
	The component pointer could be invalidated, for example.
	Instead wrap the node or the object that owns the node in a smart pointer.
*/
class Node final {
public:
	using ContainerType = std::vector<Node*>;

private:
	template<bool is_const>
	class Iterator_ {
	public:
		using BaseIterator = std::ranges::iterator_t<util::MaybeConst<ContainerType, is_const>>;

		using value_type = Node;
		using reference = util::MaybeConst<Node, is_const>&;
		using pointer = util::MaybeConst<Node, is_const>*;
		using difference_type = std::iter_difference_t<BaseIterator>;
		using iterator_category = std::iterator_traits<BaseIterator>::iterator_category;
		using iterator_concept = iterator_category;

		Iterator_& operator++() {
			++base_iterator_;
			return *this;
		}
		Iterator_ operator++(int) {
			return Iterator_{base_iterator_++};
		}
		Iterator_& operator--() {
			--base_iterator_;
			return *this;
		}
		Iterator_ operator--(int) {
			return Iterator_{base_iterator_--};
		}

		[[nodiscard]]
		reference operator*() const {
			return **base_iterator_;
		}

		[[nodiscard]]
		reference operator[](std::size_t const index) const {
			return *base_iterator_[index];
		}

		Iterator_& operator+=(difference_type const offset) {
			base_iterator_ += offset;
			return *this;
		}
		Iterator_& operator-=(difference_type const offset) {
			base_iterator_ -= offset;
			return *this;
		}
		
		[[nodiscard]]
		Iterator_ operator+(difference_type const offset) const {
			return Iterator_{base_iterator_ + offset};
		}
		[[nodiscard]]
		friend Iterator_ operator+(difference_type const offset, Iterator_ const& iterator) {
			return Iterator_{offset + iterator.base_iterator_};
		}
		[[nodiscard]]
		Iterator_ operator-(difference_type const offset) const {
			return Iterator_{base_iterator_ - offset};
		}
		[[nodiscard]]
		friend Iterator_ operator-(difference_type const offset, Iterator_ const& iterator) {
			return Iterator_{offset - iterator.base_iterator_};
		}
		[[nodiscard]]
		difference_type operator-(Iterator_ const& offset) const {
			return base_iterator_ - offset.base_iterator_;
		}

		[[nodiscard]]
		auto operator<=>(Iterator_ const&) const = default;
	
		Iterator_() = default;
		explicit Iterator_(BaseIterator const base_iterator) :
			base_iterator_{base_iterator}
		{}
	
	private:
		BaseIterator base_iterator_;
	};
	
public:
	using Iterator = Iterator_<false>;
	using ConstIterator = Iterator_<true>;

	[[nodiscard]]
	Iterator begin() {
		return Iterator{std::ranges::begin(children_)};
	}
	[[nodiscard]]
	ConstIterator begin() const {
		return ConstIterator{std::ranges::begin(children_)};
	}
	[[nodiscard]]
	Iterator end() {
		return Iterator{std::ranges::end(children_)};
	}
	[[nodiscard]]
	ConstIterator end() const {
		return ConstIterator{std::ranges::end(children_)};
	}

	[[nodiscard]]
	std::size_t size() const {
		return children_.size();
	}

	[[nodiscard]]
	Node& operator[](std::size_t const index) {
		return *children_[index];
	}
	[[nodiscard]]
	Node const& operator[](std::size_t const index) const {
		return *children_[index];
	}
	[[nodiscard]]
	Node& at(std::size_t const index) {
		return *children_.at(index);
	}
	[[nodiscard]]
	Node const& at(std::size_t const index) const {
		return *children_.at(index);
	}

	[[nodiscard]]
	Node* root() {
		return const_cast<Node*>(static_cast<Node const*>(this)->root());
	}
	[[nodiscard]]
	Node const* root() const {
		auto root = this;
		while (auto const parent = root->parent()) {
			root = parent;
		}
		return root;
	}

	[[nodiscard]]
	Node* parent() {
		return parent_;
	}
	[[nodiscard]]
	Node const* parent() const {
		return parent_;
	}
	/*
		Sets the parent of the node.
		Returns a reference to this node.
	*/
	Node& parent(Node& parent) {
		if (&parent == this) {
			detach();
		}
		else {
			remove_from_parent_();
			
			parent_ = &parent;

			add_to_parent_();
		}
		return *this;
	}
	/*
		Detaches the node from its parent, making it a root node.
	*/
	Node& detach() {
		remove_from_parent_();
		parent_ = nullptr;
		return *this;
	}

	[[nodiscard]]
	Id id() const noexcept {
		return id_;
	}
	Node& id(Id const new_id) {
		id_ = new_id;
		return *this;
	}

	[[nodiscard]]
	Node* find_by_id(Id const id) {
		return const_cast<Node*>(static_cast<Node const*>(this)->find_by_id(id));
	}
	[[nodiscard]]
	Node const* find_by_id(Id const id) const
	{
		if (auto const node = std::ranges::find(id_nodes_, id, &Node::id_);
			node != id_nodes_.end())
		{
			return *node;
		}
		return nullptr;
	}

	/*
		Returns a view of all child nodes with a specific id.
	*/
	[[nodiscard]]
	std::ranges::view auto find_all_by_id(Id const id) {
		return id_nodes_ | std::views::filter([id](Node* node) { return node->id() == id; })
			| std::views::transform([](Node* node) -> Node& { return *node; });
	}
	/*
		Returns a view of all child nodes with a specific id.
	*/
	[[nodiscard]]
	std::ranges::view auto find_all_by_id(Id const id) const {
		return id_nodes_ | std::views::filter([id](Node* node) { return node->id() == id; })
			| std::views::transform([](Node* node) -> Node const& { return *node; });
	}

	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<class Component_>
	[[nodiscard]]
	Component_* component() {
		return get_component_<Component_>();
	}
	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<class Component_>
	[[nodiscard]]
	Component_ const* component() const {
		return get_component_<Component_>();
	}

	template<class Component_> 
	Node(Id const id, Component_& component) :
		id_{id},
		component_{&component}
	{}
	template<class Component_> 
	Node(Id const id) :
		id_{id}
	{}
	template<class Component_> 
	Node(Component_& component) :
		component_{&component}
	{}

	template<class Component_> 
	Node(Node& parent, Id const id, Component_& component) :
		parent_{&parent},
		id_{id},
		component_{&component}
	{
		add_to_parent_();
	}
	template<class Component_> 
	Node(Node& parent, Component_& component) :
		parent_{&parent},
		component_{&component}
	{
		add_to_parent_();
	}
	template<class Component_> 
	Node(Node& parent, Id const id) :
		parent_{&parent},
		id_{id}
	{
		add_to_parent_();
	}
	
	Node() = default;
	~Node() {
		remove_from_tree_();
	}

	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;
	
	Node(Node&& other) = delete;
	Node& operator=(Node&& other) = delete;

private:
	template<class Component_>
	Component_* get_component_() const {
		if (component_.type() == typeid(Component_*)) {
			return std::any_cast<Component_*>(component_);
		}
		else {
			return nullptr;
		}
	}

	void remove_from_parent_() 
	{
		if (parent_) {
			util::unordered_erase(parent_->children_, this);

			for (Node* const parent : util::view_parents(*this)) {
				util::unordered_erase(parent->id_nodes_, this);
			}
		}
	}
	void add_to_parent_() {
		if (parent_) {
			parent_->children_.push_back(this);

			for (Node* const parent : util::view_parents(*this)) {
				parent->id_nodes_.push_back(this);
			}
		}
	}
	void remove_from_tree_() 
	{
		remove_from_parent_();
		if (not children_.empty()) {
			std::ranges::for_each(children_, &Node::detach);
			children_.clear();
			id_nodes_.clear();
		}
	}

	Node* parent_{};
	ContainerType children_;
	std::vector<Node*> id_nodes_;

	Id id_{};
	std::any component_{};
};

template<class Component_>
[[nodiscard]]
Component_ const* find_component_by_id(Node const& parent, Id const id) 
{
	if (auto const node = parent.find_by_id(id)) {
		return node->template component<Component_>();
	}
	else {
		return nullptr;
	}
}
template<class Component_>
[[nodiscard]]
Component_* find_component_by_id(Node& parent, Id const id) 
{
	return const_cast<Component_*>(find_component_by_id<Component_>(static_cast<Node const&>(parent), id));
}

template<class Component_, class Node_> requires std::same_as<std::remove_cvref_t<Node_>, Node>
[[nodiscard]]
std::ranges::view auto find_components_by_id(Node_& node, Id const id) 
{
	return node.find_all_by_id(id) | 
		std::views::transform([](auto& found) { 
			return found.template component<Component_>(); 
		}) | 
		std::views::filter([](auto* found) { 
			return found != nullptr; 
		}) | 
		std::views::transform([](auto* found) -> auto& { 
			return *found; 
		});
}

} // namespace avo

#endif
