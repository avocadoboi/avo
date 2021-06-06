#ifndef AVO_ID_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_ID_HPP_BJORN_SUNDIN_JUNE_2021

#include <fmt/format.h>

namespace avo {

/*
	To generate a new unique ID, use the function Id::next().
	To create an ID with a specific value (not guaranteed to be unique), use the constructor.
	An ID which converts to Id::value_type{} is considered invalid, and is the default value.
*/
class Id final {
public:
	using value_type = std::uint64_t;

	[[nodiscard]]
	constexpr explicit operator value_type() const noexcept {
		return count_;
	}
	[[nodiscard]]
	constexpr operator bool() const noexcept {
		return count_;
	}

	[[nodiscard]]
	constexpr value_type value() const noexcept {
		return count_;
	}

	[[nodiscard]]
	constexpr bool operator==(Id const& id) const = default;

	Id() = default;
	constexpr explicit Id(value_type const id) noexcept :
		count_{id}
	{}

	/*
		Generates a new unique ID, assuming all IDs are generated by this function.
	*/
	[[nodiscard]]
	static Id next() noexcept {
		static auto counter = value_type{};
		return Id{++counter};
	}

private:
	value_type count_{};
};

} // namespace avo

template<>
struct fmt::formatter<avo::Id> : fmt::formatter<avo::Id::value_type> {
	auto format(avo::Id const id, auto& context) {
		return formatter<avo::Id::value_type>::format(id.value(), context);
	}
};

template<>
struct std::hash<avo::Id> {
	std::size_t operator()(avo::Id const id) const noexcept {
		return hash<avo::Id::value_type>{}(id.value());
	}
};

#endif
