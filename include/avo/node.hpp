
#ifndef AVO_NODE_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_NODE_HPP_BJORN_SUNDIN_JUNE_2021

#include <any>
#include <concepts>
#include <ranges>
#include <vector>

#include "utils/miscellaneous.hpp"

namespace avo {

/*
	A Node is a container that stores pointers to instances of itself.
	This means it's a tree data structure. Nodes can have IDs which can be 
	used to retrieve them from the tree. They can also store a pointer to
	an arbitrary object. 
	
	A node does not own its child nodes - child nodes are added to a node tree
	by constructing them with a reference to its parent. This means that nodes
	can be stored in any way you wish; on the stack or on the heap.

	This type can be used to build a tree of software components.
	Each component stores an instance of a Node constructed with its parent node.
	This enables retrieval of other software components in the tree by their IDs.

	Move constructing and move assigning *can* be expensive. This is because a
	Node holds a pointer to its current parent and to all of its current children.
	The child pointer of the parent of the node to be moved as well as the parent
	pointer of all of the children of the node to be moved need to be updated.
*/
class Node final {
public:
	using ContainerType = std::vector<Node*>;

private:
	template<bool is_const>
	class Iterator_ {
	public:
		using BaseIterator = std::ranges::iterator_t<utils::MaybeConst<ContainerType, is_const>>;

		using value_type = Node;
		using reference = utils::MaybeConst<Node, is_const>&;
		using pointer = utils::MaybeConst<Node, is_const>*;
		using difference_type = std::iter_difference_t<BaseIterator>;
		using iterator_category = std::iterator_traits<BaseIterator>::iterator_category;
		using iterator_concept = iterator_category;

		Iterator_& operator++() {
			++_base_iterator;
			return *this;
		}
		Iterator_ operator++(int) {
			return Iterator_{_base_iterator++};
		}
		Iterator_& operator--() {
			--_base_iterator;
			return *this;
		}
		Iterator_ operator--(int) {
			return Iterator_{_base_iterator--};
		}

		[[nodiscard]]
		reference operator*() const {
			return **_base_iterator;
		}

		[[nodiscard]]
		reference operator[](std::size_t const index) const {
			return *_base_iterator[index];
		}

		Iterator_& operator+=(difference_type const offset) {
			_base_iterator += offset;
			return *this;
		}
		Iterator_& operator-=(difference_type const offset) {
			_base_iterator -= offset;
			return *this;
		}
		
		[[nodiscard]]
		Iterator_ operator+(difference_type const offset) const {
			return Iterator_{_base_iterator + offset};
		}
		[[nodiscard]]
		friend Iterator_ operator+(difference_type const offset, Iterator_ const& iterator) {
			return Iterator_{offset + iterator._base_iterator};
		}
		[[nodiscard]]
		Iterator_ operator-(difference_type const offset) const {
			return Iterator_{_base_iterator - offset};
		}
		[[nodiscard]]
		friend Iterator_ operator-(difference_type const offset, Iterator_ const& iterator) {
			return Iterator_{offset - iterator._base_iterator};
		}
		[[nodiscard]]
		difference_type operator-(Iterator_ const& offset) const {
			return _base_iterator - offset._base_iterator;
		}

		[[nodiscard]]
		auto operator<=>(Iterator_ const&) const = default;
	
		Iterator_() = default;
		explicit Iterator_(BaseIterator const base_iterator) :
			_base_iterator{base_iterator}
		{}
	
	private:
		BaseIterator _base_iterator;
	};
	
public:
	using Iterator = Iterator_<false>;
	using ConstIterator = Iterator_<true>;

	[[nodiscard]]
	Iterator begin() {
		return Iterator{std::ranges::begin(_children)};
	}
	[[nodiscard]]
	ConstIterator begin() const {
		return ConstIterator{std::ranges::begin(_children)};
	}
	[[nodiscard]]
	Iterator end() {
		return Iterator{std::ranges::end(_children)};
	}
	[[nodiscard]]
	ConstIterator end() const {
		return ConstIterator{std::ranges::end(_children)};
	}

	[[nodiscard]]
	std::size_t size() const {
		return _children.size();
	}

	[[nodiscard]]
	Node& operator[](std::size_t const index) {
		return *_children[index];
	}
	[[nodiscard]]
	Node const& operator[](std::size_t const index) const {
		return *_children[index];
	}
	[[nodiscard]]
	Node& at(std::size_t const index) {
		return *_children.at(index);
	}
	[[nodiscard]]
	Node const& at(std::size_t const index) const {
		return *_children.at(index);
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
		return _parent;
	}
	[[nodiscard]]
	Node const* parent() const {
		return _parent;
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
			_remove_from_parent();
			
			_parent = &parent;

			_add_to_parent();
		}
		return *this;
	}
	/*
		Detaches the node from its parent, making it a root node.
	*/
	Node& detach() {
		_remove_from_parent();
		_parent = nullptr;
		return *this;
	}

	[[nodiscard]]
	Id id() const noexcept {
		return _id;
	}
	Node& id(Id const new_id) {
		_id = new_id;
		return *this;
	}

	[[nodiscard]]
	Node* find_by_id(Id const id) {
		return const_cast<Node*>(static_cast<Node const*>(this)->find_by_id(id));
	}
	[[nodiscard]]
	Node const* find_by_id(Id const id) const
	{
		if (auto const node = std::ranges::find(_id_nodes, id, &Node::_id);
			node != _id_nodes.end())
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
		return _id_nodes | std::views::filter([id](Node* node) { return node->id() == id; })
			| std::views::transform([](Node* node) -> Node& { return *node; });
	}
	/*
		Returns a view of all child nodes with a specific id.
	*/
	[[nodiscard]]
	std::ranges::view auto find_all_by_id(Id const id) const {
		return _id_nodes | std::views::filter([id](Node* node) { return node->id() == id; })
			| std::views::transform([](Node* node) -> Node const& { return *node; });
	}

	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<class Component_>
	[[nodiscard]]
	Component_* component() {
		return _get_component<Component_>();
	}
	/*
		Returns the component associated with this node.
		It's an arbitrary object that has been associated with it at construction.
	*/
	template<class Component_>
	[[nodiscard]]
	Component_ const* component() const {
		return _get_component<Component_>();
	}

	template<class Component_> 
	Node(Id const id, Component_& component) :
		_id{id},
		_component{&component}
	{}
	template<class Component_> 
	Node(Id const id) :
		_id{id}
	{}
	template<class Component_> 
	Node(Component_& component) :
		_component{&component}
	{}

	template<class Component_> 
	Node(Node& parent, Id const id, Component_& component) :
		_parent{&parent},
		_id{id},
		_component{&component}
	{
		_add_to_parent();
	}
	template<class Component_> 
	Node(Node& parent, Component_& component) :
		_parent{&parent},
		_component{&component}
	{
		_add_to_parent();
	}
	template<class Component_> 
	Node(Node& parent, Id const id) :
		_parent{&parent},
		_id{id}
	{
		_add_to_parent();
	}
	
	Node() = default;
	~Node() {
		_remove_from_tree();
	}

	Node(Node const&) = delete;
	Node& operator=(Node const&) = delete;
	
	Node(Node&& other) noexcept {
		_move_construct(std::move(other));
	}
	Node& operator=(Node&& other) noexcept {
		_remove_from_tree();
		_move_construct(std::move(other));
		return *this;
	}

private:
	template<class Component_>
	Component_* _get_component() const {
		if (_component.type() == typeid(Component_*)) {
			return std::any_cast<Component_*>(_component);
		}
		else {
			return nullptr;
		}
	}

	void _remove_from_parent() 
	{
		if (_parent) {
			utils::unordered_erase(_parent->_children, this);

			for (Node* const parent : utils::view_parents(*this)) {
				utils::unordered_erase(parent->_id_nodes, this);
			}
		}
	}
	void _add_to_parent() {
		if (_parent) {
			_parent->_children.push_back(this);

			for (Node* const parent : utils::view_parents(*this)) {
				parent->_id_nodes.push_back(this);
			}
		}
	}
	void _remove_from_tree() 
	{
		_remove_from_parent();
		if (!_children.empty()) {
			std::ranges::for_each(_children, &Node::detach);
			_children.clear();
			_id_nodes.clear();
		}
	}
	void _move_construct(Node&& other) 
	{
		_parent = other._parent;
		_id = other._id;

		if (_parent) 
		{ // If we have a parent, update all pointers that pointed to the old node.
			*std::ranges::find(_parent->_children, &other) = this;

			if (_id) {
				for (Node* const parent : utils::view_parents(*this)) {
					*std::ranges::find(parent->_id_nodes, &other) = this;
				}
			}
		}

		_children = std::move(other._children);
		for (Node* const child : _children) {
			child->_parent = this;
		}
		
		_id_nodes = std::move(other._id_nodes);
		_component = std::move(other._component);
	}

	Node* _parent{};
	ContainerType _children;
	std::vector<Node*> _id_nodes;

	Id _id{};
	std::any _component{};
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
