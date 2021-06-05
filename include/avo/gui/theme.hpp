/*
MIT License

Copyright (c) 2021 Björn Sundin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef AVO_GUI_THEME_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_GUI_THEME_HPP_BJORN_SUNDIN_JUNE_2021

#include "../graphics/color.hpp"
#include "../id.hpp"
#include "../math/easing.hpp"

#include <string_view>
#include <unordered_map>

namespace avo::gui {

namespace font_families {

constexpr auto 
	roboto = std::string_view{"Roboto"},
	material_icons = std::string_view{"Material Icons"};

} // namespace font_families

/*
	Default theme color IDs.
*/
namespace theme_colors {

inline auto const 
	background = Id::next(),
	on_background = Id::next(),

	primary = Id::next(),
	primary_on_background = Id::next(),
	on_primary = Id::next(),

	secondary = Id::next(),
	secondary_on_background = Id::next(),
	on_secondary = Id::next(),

	selection = Id::next(),
	shadow = Id::next();

} // namespace theme_colors

/*
	Default theme easing IDs.
*/
namespace theme_easings {

inline auto const 
	in = Id::next(),
	out = Id::next(),
	in_out = Id::next(),
	symmetrical_in_out = Id::next();

} // namespace theme_easings

/*
	Default theme value names.
*/
namespace theme_values {

inline auto const 
	hover_animation_speed = Id::next(),
	hover_animation_duration = Id::next();

} // namespace theme_values

/*
	A theme consists of different variables that change the look and feel of the parts of the GUI that are using the theme.
	Can be used for changing and accessing any values, colors and easings.
	All the default IDs are in avo::theme_colors, avo::theme_easings and avo::theme_values.
*/
struct Theme final {
	std::unordered_map<Id, graphics::Color> colors{
		{theme_colors::background, graphics::Color{0xfffefefe}},
		{theme_colors::on_background, graphics::Color{0xff070707}},
		{theme_colors::primary, graphics::Color{0xff6200ea}},
		{theme_colors::primary_on_background, graphics::Color{0xff512da8}},
		{theme_colors::on_primary, graphics::Color{1.f}},
		{theme_colors::secondary, graphics::Color{0xff1de9b6}},
		{theme_colors::secondary_on_background, graphics::Color{0xff00bfa5}},
		{theme_colors::on_secondary, graphics::Color{0xff070707}},
		{theme_colors::selection, graphics::Color{0x90488db5}},
		{theme_colors::shadow, graphics::Color{0x68000000}},
	};
	std::unordered_map<Id, math::Easing> easings{
		{theme_easings::in, math::Easing{{0.6f, 0.f}, {0.8f, 0.2f}}},
		{theme_easings::out, math::Easing{{0.1f, 0.9f}, {0.2f, 1.f}}},
		{theme_easings::in_out, math::Easing{{0.4f, 0.f}, {0.f, 1.f}}},
		{theme_easings::symmetrical_in_out, math::Easing{{0.6f, 0.f}, {0.4f, 1.f}}},
	};
	std::unordered_map<Id, float> values{
		// 1/frames where frames is the number of frames the animation takes to finish. If it's 0.5, it finishes in 2 frames.
		{theme_values::hover_animation_speed, 1.f/6.f},
		{theme_values::hover_animation_duration, 60.f},
	};
};

} // namespace avo::gui

#endif
