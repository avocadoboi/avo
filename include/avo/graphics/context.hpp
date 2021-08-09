#ifndef AVO_GRAPHICS_CONTEXT_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_GRAPHICS_CONTEXT_HPP_BJORN_SUNDIN_JUNE_2021

#include "../window.hpp"

namespace avo::graphics {

class Context {
public:

	explicit Context(Window const& window);

	~Context(); // = default in .cpp

	Context(Context&&) noexcept; // = default in .cpp
	Context& operator=(Context&&) noexcept; // = default in .cpp
	
	Context(Context const&) = delete;
	Context& operator=(Context const&) = delete;

private:
	class Implementation;

	std::unique_ptr<Implementation> implementation_;
};

} // namespace avo::graphics

#endif