#ifndef AVO_GRAPHICS_COLOR_HPP_BJORN_SUNDIN_JUNE_2021
#define AVO_GRAPHICS_COLOR_HPP_BJORN_SUNDIN_JUNE_2021

#include "../math/angle.hpp"
#include "../math/operations.hpp"

#include <fmt/format.h>

namespace avo::graphics {

/*
	ARGB formatted 32-bit packed color, where every channel has 8 bits.
*/
using ColorInt = std::uint32_t;

/*
	Object representing a color. A Color object stores the channels as 32-bit floats with a range of [0, 1].
	This means that a Color object is 4 times as big as a packed 32-bit color, but allows for more 
	precise and efficient operations.
*/
struct Color final {
	using value_type = float;
	static_assert(std::floating_point<value_type>);
	
	value_type red{}, green{}, blue{}, alpha{1.f};
	
	constexpr Color() = default;
	/*
		The channels are clamped to the range [0, 1].
	*/
	constexpr Color(value_type const p_red, value_type const p_green, value_type const p_blue, value_type const p_alpha = 1.f) :
		red{math::unit_clamp(p_red)}, 
		green{math::unit_clamp(p_green)}, 
		blue{math::unit_clamp(p_blue)}, 
		alpha{math::unit_clamp(p_alpha)}
	{}
	/*
		The channels are in the range [0, 255]
	*/
	constexpr Color(
		std::uint8_t const p_red, 
		std::uint8_t const p_green, 
		std::uint8_t const p_blue, 
		std::uint8_t const p_alpha = static_cast<std::uint8_t>(255)
	) :
		red{p_red / 255.f},
		green{p_green / 255.f},
		blue{p_blue / 255.f},
		alpha{p_alpha / 255.f}
	{}
	/*
		The channels are clamped to the range [0, 255]
	*/
	template<std::integral T>
	constexpr Color(T const p_red, T const p_green, T const p_blue, T const p_alpha = static_cast<T>(255)) :
		red{math::unit_clamp(p_red / 255.f)},
		green{math::unit_clamp(p_green / 255.f)},
		blue{math::unit_clamp(p_blue / 255.f)},
		alpha{math::unit_clamp(p_alpha / 255.f)}
	{}

	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 1].
	*/
	explicit constexpr Color(value_type const lightness, value_type const p_alpha = 1.f) :
		red{math::unit_clamp(lightness)},
		green{math::unit_clamp(lightness)},
		blue{math::unit_clamp(lightness)},
		alpha{math::unit_clamp(p_alpha)}
	{}
	/*
		Initializes the color with a grayscale value. The values are bytes in the range [0, 255].
	*/
	explicit constexpr Color(std::uint8_t const lightness, std::uint8_t const p_alpha = 255u) :
		red{lightness/255.f},
		green{red},
		blue{red},
		alpha{p_alpha/255.f}
	{}
	/*
		Initializes the color with a grayscale value. The values are clamped to the range [0, 255].
	*/
	template<std::integral T>
	explicit constexpr Color(T const lightness, T const p_alpha = static_cast<T>(255)) :
		red{math::unit_clamp(static_cast<value_type>(lightness) / 255.f)},
		green{red},
		blue{red},
		alpha{math::unit_clamp(static_cast<value_type>(p_alpha) / 255.f)}
	{}

	/*
		Creates a copy of another color but with a new alpha.
	*/
	constexpr Color(Color const color, value_type const p_alpha) :
		red{color.red}, 
		green{color.green}, 
		blue{color.blue}, 
		alpha{math::unit_clamp(p_alpha)}
	{}
	/*
		Creates a copy of another color but with a new alpha.
	*/
	constexpr Color(Color const color, std::uint8_t const p_alpha) :
		red{color.red / 255.f},
		green{color.green / 255.f},
		blue{color.blue / 255.f},
		alpha{p_alpha / 255.f}
	{}
	/*
		Creates a copy of another color but with a new alpha.
	*/
	template<std::integral T>
	constexpr Color(Color const color, T const p_alpha) :
		red{color.red},
		green{color.green},
		blue{color.blue},
		alpha{math::unit_clamp(p_alpha / 255.f)}
	{}

	/*
		Initializes with a 4-byte packed RGBA color.
	*/
	explicit constexpr Color(ColorInt const color) :
		red{red_channel(color) / 255.f},
		green{green_channel(color) / 255.f},
		blue{blue_channel(color) / 255.f},
		alpha{alpha_channel(color) / 255.f}
	{}

	constexpr Color& operator=(ColorInt const color) {
		return *this = Color{color};
	}

	[[nodiscard]]
	constexpr bool operator==(Color const&) const = default;

	[[nodiscard]]
	static constexpr Color rgba(value_type const red, value_type const green, value_type const blue, value_type const alpha = 1.f) {
		return Color{red, green, blue, alpha};
	}
	[[nodiscard]]
	static constexpr Color rgb(value_type const red, value_type const green, value_type const blue) {
		return Color{red, green, blue};
	}

	/*
		Creates a color from hue, saturation, brightness and alpha values.
		They are all clamped to the range [0, 1].
		The difference between HSB and HSL is that the lightness value goes from black to white 
		while brightness goes from black to full color brightness. 
		HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
	*/
	[[nodiscard]]
	static constexpr Color hsba(value_type hue, value_type const saturation, value_type brightness, value_type const alpha = 1.f)
	{
		hue -= math::floor<value_type>(hue);
		brightness = math::unit_clamp(brightness);
		auto const factor = brightness * math::unit_clamp(saturation);

		return Color{
			brightness + factor * (math::unit_clamp(1.f - (hue - 1.f / 6.f) * 6.f) + math::unit_clamp((hue - 4.f / 6.f) * 6.f) - 1.f),
			brightness + factor * (std::min(1.f, hue * 6.f) - math::unit_clamp((hue - 3.f / 6.f) * 6.f) - 1.f),
			brightness + factor * (math::unit_clamp((hue - 2.f / 6.f) * 6.f) - math::unit_clamp((hue - 5.f / 6.f) * 6.f) - 1.f),
			alpha,
		};
	}
	/*
		Calls Color::hsba.
	*/
	[[nodiscard]]
	static constexpr Color hsb(value_type const hue, value_type const saturation, value_type const brightness) {
		return hsba(hue, saturation, brightness);
	}
	[[nodiscard]]
	static constexpr Color hsba(math::IsAngle auto const hue, value_type const saturation, value_type const brightness, value_type const alpha = 1.f) {
		return hsba(math::normalized<value_type>(hue), saturation, brightness, alpha);
	}
	[[nodiscard]]
	static constexpr Color hsb(math::IsAngle auto const hue, value_type const saturation, value_type const brightness) {
		return hsba(math::normalized<value_type>(hue), saturation, brightness);
	}

	/*
		Creates a color from hue, saturation, lightness and alpha values.
		They are all value_types in the range [0, 1].
		The difference between HSB and HSL is that the lightness value goes from black to white 
		while brightness goes from black to full color brightness. 
		HSB can only be white if saturation is 0 while HSL is white as long as lightness is 1.
	*/
	[[nodiscard]]
	static constexpr Color hsla(value_type hue, value_type const saturation, value_type lightness, value_type const alpha = 1.f) 
	{
		hue -= math::floor<value_type>(hue);
		lightness = math::unit_clamp(lightness);
		auto const factor = 2.f * math::unit_clamp(saturation)*(lightness < 0.5f ? lightness : (1.f - lightness));

		return Color{
			lightness + factor * (math::unit_clamp(1.f - (hue - 1.f / 6.f) * 6.f) + math::unit_clamp((hue - 4.f / 6.f) * 6.f) - 0.5f),
			lightness + factor * (std::min(1.f, hue * 6.f) - math::unit_clamp((hue - 3.f / 6.f) * 6.f) - 0.5f),
			lightness + factor * (math::unit_clamp((hue - 2.f / 6.f) * 6.f) - math::unit_clamp((hue - 5.f / 6.f) * 6.f) - 0.5f),
			alpha
		};
	}
	/*
		Calls Color::hsla.
	*/
	[[nodiscard]]
	static constexpr Color hsl(value_type const hue, value_type const saturation, value_type const lightness) {
		return hsla(hue, saturation, lightness);
	}
	[[nodiscard]]
	static constexpr Color hsla(math::IsAngle auto const hue, value_type const saturation, value_type const lightness, value_type const alpha = 1.f) {
		return hsla(math::normalized<value_type>(hue), saturation, lightness, alpha);
	}
	[[nodiscard]]
	static constexpr Color hsl(math::IsAngle auto const hue, value_type const saturation, value_type const lightness) {
		return hsla(math::normalized<value_type>(hue), saturation, lightness);
	}

	/*
		Changes the hue of the color. The hue is a value_type in the range [0, 1].
	*/
	constexpr Color& hue(value_type new_hue) {
		new_hue -= math::floor<value_type>(new_hue);

		auto const min_channel = math::min(red, green, blue);
		auto const max_channel = math::max(red, green, blue);
		
		red = min_channel + (max_channel - min_channel) * 
			(math::unit_clamp(1.f - (new_hue - 1.f / 6.f) * 6.f) + math::unit_clamp((new_hue - 4.f / 6.f) * 6.f));
			
		green = min_channel + (max_channel - min_channel) * 
			(std::min(1.f, new_hue * 6.f) - math::unit_clamp((new_hue - 3.f / 6.f) * 6.f));
			
		blue = min_channel + (max_channel - min_channel) * 
			(math::unit_clamp((new_hue - 2.f / 6.f) * 6.f) - math::unit_clamp((new_hue - 5.f / 6.f) * 6.f));
			
		return *this;
	}
	constexpr Color& hue(math::IsAngle auto const hue_angle) {
		return hue(math::normalized<value_type>(hue_angle));
	}
	/*
		Returns the hue of the color. The hue is a value_type in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr value_type hue() const {
		if (red + green + blue == 0.f) {
			return 0.f;
		}

		if (red > green) {
			if (red > blue) {
				if (green > blue) {
					// (1, 0, 0) -> (1, 1, 0) : 0/6 < hue < 1/6, max = red, min = blue
					return (green - blue) / (red - blue) / 6.f;
				}
				else {
					// (1, 0, 1) -> (1, 0, 0) : 5/6 < hue < 6/6, max = red, min = green
					return 1.f - (blue - green) / (red - green) / 6.f;
				}
			}
			else {
				// (0, 0, 1) -> (1, 0, 1) : 4/6 < hue < 5/6, max = blue, min = green
				return (4.f + (red - green) / (blue - green)) / 6.f;
			}
		}
		else {
			if (green > blue) {
				if (red > blue) {
					// (1, 1, 0) -> (0, 1, 0) : 1/6 < hue < 2/6, max = green, min = blue
					return (2.f - (red - blue) / (green - blue)) / 6.f;
				}
				else {
					// (0, 1, 0) -> (0, 1, 1) : 2/6 < hue < 3/6, max = green, min = red
					return (2.f + (blue - red) / (green - red)) / 6.f;
				}
			}
			else {
				// (0, 1, 1) -> (0, 0, 1) : 3/6 < hue < 4/6, max = blue, min = red
				return (4.f - (green - red) / (blue - red)) / 6.f;
			}
		}
	}
	template<math::IsRadians Return_>
	[[nodiscard]]
	constexpr Return_ hue() const {
		using Value_ = typename Return_::value_type;
		return Return_{static_cast<Value_>(hue()*std::numbers::pi_v<typename Return_::value_type>*2)};
	}
	template<math::IsDegrees Return_>
	[[nodiscard]]
	constexpr Return_ hue() const {
		using Value_ = typename Return_::value_type;
		if constexpr (std::integral<Value_>) {
			return Return_{math::round<Value_>(hue()*360)};
		}
		return Return_{static_cast<Value_>(hue()*360)};
	}

	/*
		Sets the HSB saturation of the color. The saturation is a value_type in the range [0, 1].
		HSB saturation can change lightness, and HSL saturation can change brightness.
		Keep in mind that you can't change the saturation if the color is grayscale, because only RGBA values are stored.
	*/
	constexpr Color& hsb_saturation(value_type saturation) {
		if (red == green && red == blue) {
			return *this;
		}

		saturation = math::unit_clamp(saturation);
		auto const factor = saturation/hsb_saturation();

		auto const brightness = math::max(red, green, blue);
		red = brightness + factor*(red - brightness);
		green = brightness + factor*(green - brightness);
		blue = brightness + factor*(blue - brightness);

		return *this;
	}
	/*
		Returns the HSB saturation of the color. The saturation is a value_type in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr value_type hsb_saturation() const {
		if (auto const current_brightness = brightness()) {
			return 1.f - math::min(red, green, blue)/current_brightness;
		}
		return 0.f;
	}

	/*
		Sets the HSL saturation of the color. The saturation is a value_type in the range [0, 1].
		HSB saturation can change lightness, and HSL saturation can change brightness.
		Keep in mind that you can't change the saturation if the color is gray, since only RGBA values are stored.
	*/
	constexpr Color& hsl_saturation(value_type saturation) {
		saturation = math::unit_clamp(saturation);

		auto const saturation_before = hsl_saturation();
		if (saturation_before == 0.f) {
			return *this;
		}
		
		auto const factor = saturation/saturation_before;
		auto const current_lightness = lightness();
		red = current_lightness + factor*(red - current_lightness);
		green = current_lightness + factor*(green - current_lightness);
		blue = current_lightness + factor*(blue - current_lightness);

		return *this;
	}
	/*
		Returns the HSL saturation of the color. The saturation is a value_type in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr value_type hsl_saturation() const {
		auto const min_channel = math::min(red, green, blue);
		auto const max_channel = math::max(red, green, blue);
		if (min_channel == max_channel) {
			return 0.f;
		}
		return std::max(
			(max_channel - min_channel)/(max_channel + min_channel), 
			(max_channel - min_channel)/(2.f - max_channel - min_channel)
		);
	}

	/*
		Sets the brightness of the color. The brightness is a value_type in the range [0, 1]. A brightness of 0 makes the
		color black, and a brightness of 1 makes the color fully bright. This only makes it white if saturation is at 0.
	*/
	constexpr Color& brightness(value_type new_brightness) {
		new_brightness = math::unit_clamp(new_brightness);

		if (red == green && red == blue) {
			red = new_brightness;
			green = new_brightness;
			blue = new_brightness;
			return *this;
		}

		auto const old_brightness = brightness();
		red *= new_brightness/old_brightness;
		green *= new_brightness/old_brightness;
		blue *= new_brightness/old_brightness;

		return *this;
	}
	/*
		Returns the brightness of the color. The brightness is a value_type in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr value_type brightness() const {
		return math::max(red, green, blue);
	}

	/*
		Changes the lightness of the color. The lightness a value_type in the range [0, 1]. A lightness of 0 makes the
		color black, a lightness of 0.5 makes it normal and a lightness of 1 makes it white.
	*/
	constexpr Color& lightness(value_type new_lightness) {
		new_lightness = math::unit_clamp(new_lightness);

		if (red == green && red == blue) {
			red = new_lightness;
			green = new_lightness;
			blue = new_lightness;
			return *this;
		}

		if (auto const previous_lightness = lightness(); previous_lightness <= 0.5f) {
			if (new_lightness <= 0.5f) {
				red = red * new_lightness / previous_lightness;
				green = green * new_lightness / previous_lightness;
				blue = blue * new_lightness / previous_lightness;
			}
			else {
				red = (red - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
				green = (green - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
				blue = (blue - previous_lightness) * (1.f - new_lightness) / previous_lightness + new_lightness;
			}
		}
		else {
			if (new_lightness <= 0.5) {
				red = (red - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
				green = (green - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
				blue = (blue - previous_lightness) * new_lightness / (1.f - previous_lightness) + new_lightness;
			}
			else {
				red = (red - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
				green = (green - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
				blue = (blue - previous_lightness) * (1.f - new_lightness) / (1.f - previous_lightness) + new_lightness;
			}
		}
		return *this;
	}

	/*
		Returns the lightness of the color. The lightness is a value_type in the range [0, 1].
	*/
	[[nodiscard]]
	constexpr value_type lightness() const {
		return 0.5f*(math::min(red, green, blue) + math::max(red, green, blue));
	}

	/*
		A contrast of 0 makes the color gray, 0.5 leaves it unchanged and 1 is maximum contrast.
	*/
	constexpr Color& contrast(value_type const contrast) {
		if (contrast == 0.5) {
			return *this;
		}
		if (contrast < 0.5f) {
			red = (red - 0.5f) * contrast * 2.f + 0.5f;
			green = (green - 0.5f) * contrast * 2.f + 0.5f;
			blue = (blue - 0.5f) * contrast * 2.f + 0.5f;
		}
		else {
			red = (static_cast<value_type>(red >= 0.5f) - red) * (contrast * 2.f - 1.f) + red;
			green = (static_cast<value_type>(green >= 0.5f) - green) * (contrast * 2.f - 1.f) + green;
			blue = (static_cast<value_type>(blue >= 0.5f) - blue) * (contrast * 2.f - 1.f) + blue;
		}
		return *this;
	}

	/*
		Packs the color into a 32-bit integer in ARGB format.
	*/
	[[nodiscard]]
	constexpr ColorInt get_packed() const {
		return (static_cast<ColorInt>(alpha*0xff) << 24) | (static_cast<ColorInt>(red*0xff) << 16) | 
			(static_cast<ColorInt>(green*0xff) << 8) | (static_cast<ColorInt>(blue*0xff));
	}

	[[nodiscard]]
	constexpr Color operator+(Color const other) const {
		return Color{red + other.red, green + other.green, blue + other.blue, alpha + other.alpha};
	}
	constexpr Color& operator+=(Color const other) {
		return *this = *this + other;
	}
	[[nodiscard]]
	constexpr Color operator-(Color const other) const {
		return Color{red - other.red, green - other.green, blue - other.blue, alpha - other.alpha};
	}
	constexpr Color& operator-=(Color const other) {
		return *this = *this - other;
	}

	[[nodiscard]]
	constexpr Color operator*(value_type const factor) const {
		return Color{red * factor, green * factor, blue * factor, alpha};
	}
	constexpr Color& operator*=(value_type const factor) {
		return *this = *this * factor;
	}
	[[nodiscard]]
	constexpr Color operator/(value_type const divisor) const {
		return Color{red/divisor, green/divisor, blue/divisor, alpha};
	}
	constexpr Color& operator/=(value_type const divisor) {
		return *this = *this / divisor;
	}

	[[nodiscard]]
	constexpr Color operator+(value_type const delta) const {
		return Color{red + delta, green + delta, blue + delta};
	}
	constexpr Color& operator+=(value_type const delta) {
		return *this = *this + delta;
	}
	[[nodiscard]]
	constexpr Color operator-(value_type const delta) const {
		return Color{red - delta, green - delta, blue - delta};
	}
	constexpr Color& operator-=(value_type const delta) {
		return *this = *this - delta;
	}

	//------------------------------

	[[nodiscard]]
	static constexpr std::uint8_t red_channel(ColorInt const color) {
		return static_cast<std::uint8_t>(color >> 16 & 0xff);
	}
	[[nodiscard]]
	static constexpr std::uint8_t green_channel(ColorInt const color) {
		return static_cast<std::uint8_t>(color >> 8 & 0xff);
	}
	[[nodiscard]]
	static constexpr std::uint8_t blue_channel(ColorInt const color) {
		return static_cast<std::uint8_t>(color & 0xff);
	}
	[[nodiscard]]
	static constexpr std::uint8_t alpha_channel(ColorInt const color) {
		return static_cast<std::uint8_t>(color >> 24 & 0xff);
	}
};

[[nodiscard]]
constexpr Color operator*(Color::value_type const factor, Color const color) {
	return color * factor;
}
[[nodiscard]]
constexpr Color operator+(Color::value_type const factor, Color const color) {
	return color + factor;
}
[[nodiscard]]
constexpr Color operator-(Color::value_type const term, Color const color) {
	return Color{term - color.red, term - color.green, term - color.blue};
}

} // namespace avo::graphics

namespace avo::math {

/*
	Linearly interpolates a color between start and end. Each channel is faded individually.
	If progress is 0, start is returned. If progress is 1, end is returned.
*/
[[nodiscard]]
constexpr graphics::Color interpolate(graphics::Color const start, graphics::Color const end, graphics::Color::value_type const progress) {
	return graphics::Color{
		std::lerp(start.red, end.red, progress),
		std::lerp(start.green, end.green, progress),
		std::lerp(start.blue, end.blue, progress),
		std::lerp(start.alpha, end.alpha, progress),
	};
}

} // namespace avo::math

template<>
struct fmt::formatter<avo::graphics::Color> : fmt::formatter<avo::graphics::Color::value_type> {
	using formatter<avo::graphics::Color::value_type>::format;
	
	auto format(avo::graphics::Color const color, auto& context) {
		fmt::format_to(context.out(), "rgba(");
		format(color.red, context);
		fmt::format_to(context.out(), ", ");
		format(color.green, context);
		fmt::format_to(context.out(), ", ");
		format(color.blue, context);
		fmt::format_to(context.out(), ", ");
		format(color.alpha, context);
		return fmt::format_to(context.out(), ")");
	}
};

#endif
