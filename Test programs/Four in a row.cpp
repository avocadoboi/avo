#include "../AvoGUI.hpp"

#include <array>

//------------------------------

constexpr Count NUMBER_OF_COLUMNS = 7;
constexpr Count NUMBER_OF_ROWS = 6;

constexpr float CELL_WIDTH = 100.f;

constexpr Avo::Color COLOR_PLAYER_1{ 1.f, 0.f, 0.f };
constexpr Avo::Color COLOR_PLAYER_2{ 0.f, 0.f, 1.f };

//------------------------------

class YouWon :
	public Avo::View
{
private:
	Avo::Color m_winningColor;
	Avo::Text m_text{ getDrawingContext()->createText("won!", 20.f) };

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
		p_context->fillCircle(m_text.getLeft() - 50.f, m_text.getCenterY(), 35.f);
		p_context->drawText(m_text);
	}

	YouWon(Avo::View* p_parent) :
		View{ p_parent }
	{
		setElevation(5.f);
		setCornerRadius(5.f);
		setSize(250, 150);
		setIsVisible(false);

		m_text.setCenter(getWidth() * 0.65f, getHeight() * 0.5f);
	}
};

//------------------------------

class FourInARow : public Avo::Gui
{
private:
	YouWon* m_youWon = nullptr;

	std::array<std::array<int, NUMBER_OF_ROWS>, NUMBER_OF_COLUMNS> m_cells;
	void resetCells()
	{
		for (auto& column : m_cells)
		{
			column.fill(0);
		}
	}

	int m_playerNumber = 1;

public:
	FourInARow()
	{
		create("Four in a row", NUMBER_OF_COLUMNS*CELL_WIDTH, NUMBER_OF_ROWS*CELL_WIDTH, Avo::WindowStyleFlags::DefaultNoResize);

		resetCells();

		m_youWon = new YouWon{ this };

		enableMouseEvents();
		run();
	}

	//------------------------------

	void handleMouseDown(Avo::MouseEvent const& p_event) override
	{
		if (m_youWon->getIsVisible())
		{
			m_youWon->close();

			resetCells();
		}
		else
		{
			auto x = static_cast<Index>(p_event.x / CELL_WIDTH);
			for (Index y = NUMBER_OF_ROWS - 1; y >= 0; y--)
			{
				if (!m_cells[x][y])
				{
					m_cells[x][y] = m_playerNumber;

					for (auto i_direction : Indices{ 4 })
					{
						Count numberOfCellsInARow = -1;

						for (Index forwardsBackwardsDirection = -1; forwardsBackwardsDirection <= 1; forwardsBackwardsDirection += 2)
						{
							Index offsetX = 0;
							Index offsetY = 0;

							while (x < NUMBER_OF_COLUMNS - offsetX && x >= -offsetX &&
								y < NUMBER_OF_ROWS - offsetY && y >= -offsetY &&
								m_cells[x + offsetX][y + offsetY] == m_playerNumber)
							{
								offsetX += (i_direction < 3)*forwardsBackwardsDirection;
								offsetY += (!i_direction - (i_direction > 1))*forwardsBackwardsDirection;
								numberOfCellsInARow++;
							}
						}
						if (numberOfCellsInARow == 4)
						{
							m_youWon->open(m_playerNumber == 1 ? COLOR_PLAYER_1 : COLOR_PLAYER_2);
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
	void handleMouseDoubleClick(Avo::MouseEvent const& p_event) override
	{
		handleMouseDown(p_event);
	}

	void draw(Avo::DrawingContext* p_context) override
	{
		p_context->setColor(0.7f);
		for (auto x : Indices{ 1, NUMBER_OF_COLUMNS })
		{
			p_context->drawLine(x * CELL_WIDTH, 0, x * CELL_WIDTH, getHeight(), 2.f);
		}
		for (auto y : Indices{ 1, NUMBER_OF_ROWS })
		{
			p_context->drawLine(0, y * CELL_WIDTH, getWidth(), y * CELL_WIDTH, 2.f);
		}

		for (auto x : Indices{ NUMBER_OF_COLUMNS })
		{
			for (auto y : Indices{ NUMBER_OF_ROWS })
			{
				if (m_cells[x][y])
				{
					p_context->setColor(m_cells[x][y] == 1 ? COLOR_PLAYER_1 : COLOR_PLAYER_2);
					p_context->fillCircle((x + 0.5f) * CELL_WIDTH, (y + 0.5f) * CELL_WIDTH, CELL_WIDTH*0.4f);
				}
			}
		}
	}
};

//------------------------------

int main()
{
	new FourInARow;
}
