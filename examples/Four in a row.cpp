#include "../AvoGUI.hpp"

#include <array>

//------------------------------

constexpr auto numberOfColumns = 7;
constexpr auto numberOfRows = 6;

constexpr auto cellWidth = 100.f;

constexpr auto color_player_1 = Avo::Color{1.f, 0.f, 0.f};
constexpr auto color_player_2 = Avo::Color{0.f, 0.f, 1.f};

//------------------------------

class YouWon :
	public Avo::View
{
private:
	Avo::Color m_winningColor;
	Avo::Text m_text = getDrawingContext()->createText("won!", 20.f);

	Avo::Animation m_openAnimation{
		getGui(), getThemeEasing(Avo::ThemeEasings::out), 300ms,
		[=] (float p_value) {
			setBottom((getParent<View>()->getHeight() + getHeight()) * 0.5f * p_value);
			setOpacity(p_value);
			invalidate();
			if (p_value == 0.f)
			{
				setIsVisible(false);
			}
		}
	};

public:
	void open(Avo::Color const& p_winningColor)
	{
		setCenterX(getParent<View>()->getWidth() / 2);
		m_winningColor = p_winningColor;
		setIsVisible(true);
		m_openAnimation.play(false);
	}
	void close()
	{
		m_openAnimation.play(true);
	}

	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(1.f);
		p_context->fillRectangle(getSize());

		p_context->setColor(m_winningColor);
		p_context->fillCircle({m_text.getLeft() - 50.f, m_text.getCenterY()}, 35.f);
		p_context->drawText(m_text);
	}

	YouWon(Avo::View* p_parent) :
		View{p_parent}
	{
		setElevation(5.f);
		setCornerRadius(5.f);
		setSize({250, 150});
		setIsVisible(false);

		m_text.setCenter({getWidth() * 0.65f, getHeight() * 0.5f});
	}
};

//------------------------------

class FourInARow : public Avo::Gui
{
private:
	YouWon* m_youWon{};

	std::array<std::array<int, numberOfRows>, numberOfColumns> m_cells;
	void resetCells()
	{
		for (auto& column : m_cells)
		{
			column.fill(0);
		}
	}

	int m_playerNumber = 1;

public:
	auto handleMouseDown(Avo::MouseEvent const& p_event) -> void override
	{
		if (m_youWon->getIsVisible())
		{
			m_youWon->close();

			resetCells();
		}
		else
		{
			auto x = static_cast<Avo::Index>(p_event.x / cellWidth);
			for (auto y = Avo::Index{numberOfRows - 1}; y >= 0; y--)
			{
				if (!m_cells[x][y])
				{
					m_cells[x][y] = m_playerNumber;

					for (auto i_direction : Avo::Indices{4})
					{
						auto numberOfCellsInARow = Avo::Count{-1};

						for (auto forwardsBackwardsDirection = Avo::Index{-1}; 
						     forwardsBackwardsDirection <= 1; 
						     forwardsBackwardsDirection += 2)
						{
							auto offset = Avo::Vector2d<Avo::Index>{};
							while (x < numberOfColumns - offset.x && x >= -offset.y &&
								y < numberOfRows - offset.y && y >= -offset.y &&
								m_cells[x + offset.x][y + offset.y] == m_playerNumber)
							{
								offset.x += (i_direction < 3)*forwardsBackwardsDirection;
								offset.y += (!i_direction - (i_direction > 1))*forwardsBackwardsDirection;
								numberOfCellsInARow++;
							}
						}
						if (numberOfCellsInARow == 4)
						{
							m_youWon->open(m_playerNumber == 1 ? color_player_1 : color_player_2);
							break;
						}
					}

					m_playerNumber = m_playerNumber == 1 ? 2 : 1;
					break;
				}
			}
		}

		invalidate();
	}

	auto draw(Avo::DrawingContext* p_context) -> void override
	{
		p_context->setColor(0.7f);
		for (auto x : Avo::Indices{1, numberOfColumns})
		{
			p_context->drawLine({x * cellWidth, 0}, {x * cellWidth, getHeight()}, 2.f);
		}
		for (auto y : Avo::Indices{1, numberOfRows})
		{
			p_context->drawLine({0, y * cellWidth}, {getWidth(), y * cellWidth}, 2.f);
		}

		for (auto x : Avo::Indices{numberOfColumns})
		{
			for (auto y : Avo::Indices{numberOfRows})
			{
				if (m_cells[x][y])
				{
					p_context->setColor(m_cells[x][y] == 1 ? color_player_1 : color_player_2);
					p_context->fillCircle({(x + 0.5f) * cellWidth, (y + 0.5f) * cellWidth}, cellWidth*0.4f);
				}
			}
		}
	}

	FourInARow()
	{
		create("Four in a row", {numberOfColumns*cellWidth, numberOfRows*cellWidth}, Avo::WindowStyleFlags::DefaultNoResize);

		resetCells();

		m_youWon = new YouWon{this};

		enableMouseEvents();
		run();
	}
};

//------------------------------

auto main() -> int
{
	new FourInARow;
}
