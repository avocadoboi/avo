#ifdef __GNUG__
#pragma GCC diagnostic push
// GCC doesn't know that these headers will only ever be included in this translation unit.
#pragma GCC diagnostic ignored "-Wsubobject-linkage"
#endif

#ifdef __linux__
#	include "linux/window.hpp"
#endif

#ifdef _WIN32
#	include "windows/window.hpp"
#endif

#ifdef __GNUG__
#pragma GCC diagnostic pop
#endif

//------------------------------

#include <avo/window.hpp>

namespace avo::window {

void Window::title(std::string_view const title) {
	implementation_->title(title);
}
std::string Window::title() const {
	return implementation_->title();
}

bool Window::toggle_fullscreen() {
	return implementation_->toggle_fullscreen();
}

void Window::position(math::Point<Pixels> const position) {
	implementation_->position(position);
}

void Window::min_max_size(MinMaxSize<Dip> const min_max) {
	implementation_->min_max_size(min_max);
}
MinMaxSize<Dip> Window::min_max_size() const {
	return implementation_->min_max_size();
}

void Window::min_size(math::Size<Dip> const min) {
	implementation_->min_size(min);
}
math::Size<Dip> Window::min_size() const {
	return implementation_->min_size();
}

void Window::max_size(math::Size<Dip> const max) {
	implementation_->max_size(max);
}
math::Size<Dip> Window::max_size() const {
	return implementation_->max_size();
}

void Window::size(math::Size<Dip> const size) {
	implementation_->size(size);
}
math::Size<Dip> Window::size() const {
	return implementation_->size();
}

float Window::dpi() const {
	return implementation_->dpi();
}

bool Window::is_open() const {
	return implementation_->is_open();
}

std::any Window::native_handle() const {
	return implementation_->native_handle();
}

Event Window::await_event() {
	return implementation_->await_event();
}

std::optional<Event> Window::take_event() {
	return implementation_->take_event();
}

Window::Window(Parameters const& parameters) :
	implementation_{std::make_unique<Implementation>(parameters)}
{}

Window::~Window() = default;

Window::Window(Window&&) noexcept = default;
Window& Window::operator=(Window&&) noexcept = default;

Window Builder::open() && {
	return Window{parameters_};
}

} // namespace avo::window
