#include "../Include/Utilities.hpp"
#include <iostream>
#include <algorithm>

namespace Colors
{
	class HslColor
	{
	public:
		HslColor(const sf::Color& rgbColor);

		sf::Color toRgb();

		void setHue(float newHue);
		const float getHue() const { return hue; }

	private:
		float hue{ 0.f };
		float saturation{ 0.f };
		float lightness{ 0.f };
	};
	
	std::vector<sf::Color> Colors::generateColors(int count)
	{
		// color count has to be >= 1, subtract 1 because the first color will always be the Constants::mainColor
		count = (count <= 0 ? 0 : count - 1);

		std::vector<sf::Color> result{ Constants::mainColor };

		HslColor mainColorHsl(Constants::mainColor);
		const float hueStep = 1.0f / (count + 1);
		auto currentColor = mainColorHsl;
		for (int i = 0; i < count; ++i) {
			mainColorHsl.setHue(mainColorHsl.getHue() + hueStep);
			if (mainColorHsl.getHue() >= 1.0f) {
				mainColorHsl.setHue(1.0f - mainColorHsl.getHue());
			}
			result.push_back(mainColorHsl.toRgb());
		}

		return result;
	}

	HslColor::HslColor(const sf::Color& rgbColor)
	{
		const float r = static_cast<float>(rgbColor.r) / 255.f;
		const float g = static_cast<float>(rgbColor.g) / 255.f;
		const float b = static_cast<float>(rgbColor.b) / 255.f;

		const float min = std::min({ r, g, b });
		const float max = std::max({ r, g, b });
		const float delta = max - min;

		lightness = (max + min) / 2.f;

		if (delta == 0) {
			hue = 0;
			saturation = 0.f;
		}
		else {
			saturation = lightness <= 0.5f ? (delta / (max + min)) : (delta / (2.f - max - min));

			hue = [delta, max, min, r, g, b]() {
				float result{ 0.f };
				if (r == max) {
					result = (g - b) / 6.f;
				}
				else if (g == max) {
					result = (1.0f / 3.f) + ((b - r) / 6.f);
				}
				else {
					result = (2.0f / 3.f) + ((r - g) / 6.f);
				}
				result /= delta;
				result = result < 0.f ? result + 1.f : (result > 1.f ? result - 1.f : result);

				return result;
			}();
		}
	}

	sf::Color HslColor::toRgb()
	{
		int r{ 0 };
		int g{ 0 };
		int b{ 0 };

		if (saturation == 0) {
			r = g = b = static_cast<int>(lightness * 255.f);
		}
		else {
			float v2 = lightness < 0.5f ? (lightness * (1.f + saturation)) : ((lightness + saturation) - (lightness * saturation));
			float v1 = 2.f * lightness - v2;

			auto hueToRgb = [v1, v2](float vH) {
				vH = vH < 0.f ? vH + 1.f : (vH > 1.f ? vH - 1.f : vH);
				if (6.f * vH < 1.f) {
					return v1 + (v2 - v1) * 6.f * vH;
				}
				if (2.f * vH < 1.f) {
					return v2;
				}
				if (3.f * vH < 2.f) {
					return v1 + (v2 - v1) * ((2.0f / 3.f - vH) * 6.f);
				}
				return v1;
			};

			r = static_cast<int>(255.f * hueToRgb(hue + (1.f / 3.f)));
			g = static_cast<int>(255.f * hueToRgb(hue));
			b = static_cast<int>(255.f * hueToRgb(hue - (1.f / 3.f)));
		}

		return sf::Color{ static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g), static_cast<sf::Uint8>(b) };
	}

	void HslColor::setHue(float newHue)
	{
		hue = newHue;
	}
}
