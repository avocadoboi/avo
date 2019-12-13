#include "../AvoGUI.hpp"

//------------------------------

int32 const NUMBER_OF_COLUMNS = 7;
int32 const NUMBER_OF_ROWS = 6;

float const CELL_WIDTH = 100.f;


AvoGUI::Color const COLOR_PLAYER_1(1.f, 0.f, 0.f);
AvoGUI::Color const COLOR_PLAYER_2(0.f, 0.f, 1.f);

//------------------------------

class YouWon :
	public AvoGUI::View
{
private:
	bool m_isOpen;
	float m_openAnimationTime;
	float m_openAnimationValue;

	AvoGUI::Color m_winningColor;

	AvoGUI::Text* m_text;

public:
	YouWon(AvoGUI::View* p_parent) :
		View(p_parent), 
		m_isOpen(false), m_openAnimationTime(0.f), m_openAnimationValue(0.f)
	{
		setElevation(5.f);
		setCornerRadius(5.f);
		setSize(250, 150);
		setIsVisible(false);

		m_text = getGui()->getDrawingContext()->createText("won!", 20.f);
		m_text->setCenter(getWidth() * 0.65f, getHeight()*0.5f);
	}

	void open(AvoGUI::Color const& p_winningColor)
	{
		m_winningColor = p_winningColor;
		setIsVisible(true);
		m_isOpen = true;
		m_openAnimationTime = 0.f;
		queueAnimationUpdate();
	}
	void close()
	{
		m_isOpen = false;
		m_openAnimationTime = 0.f;
		queueAnimationUpdate();
	}

	void updateAnimations() override
	{
		if (m_isOpen)
		{
			if (m_openAnimationTime < 1.f)
			{
				m_openAnimationValue = getThemeEasing("out").easeValue(m_openAnimationTime += 0.1f);
				queueAnimationUpdate();
			}
		}
		else
		{
			if (m_openAnimationTime < 1.f)
			{
				m_openAnimationValue = 1.f - getThemeEasing("in").easeValue(m_openAnimationTime += 0.1f);
				queueAnimationUpdate();
			}
			else
			{
				setIsVisible(false);
			}
		}

		setCenter(getParent()->getWidth()*0.5f, getParent()->getHeight() * 0.5f * m_openAnimationValue);

		invalidate();
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(1.f));
		p_context->fillRectangle(getSize());

		p_context->setColor(m_winningColor);
		p_context->fillCircle(m_text->getLeft() - 50.f, m_text->getCenterY(), 35.f);
		p_context->drawText(m_text);
	}
};

//------------------------------

class FourInARow : 
	public AvoGUI::Gui
{
private:
	YouWon* m_youWon;

	uint32** m_cells;
	uint32 m_playerNumber;

public:
	FourInARow() :
		m_cells(0)
	{
		create("Four in a row", NUMBER_OF_COLUMNS*CELL_WIDTH, NUMBER_OF_ROWS*CELL_WIDTH, AvoGUI::WindowStyleFlags::DefaultNoResize);
	}

	//------------------------------

	void createContent() override
	{
		m_playerNumber = 1;

		m_cells = new uint32*[NUMBER_OF_COLUMNS];
		for (uint32 x = 0; x < NUMBER_OF_COLUMNS; x++)
		{
			m_cells[x] = new uint32[NUMBER_OF_ROWS];
			for (uint32 y = 0; y < NUMBER_OF_ROWS; y++)
			{
				m_cells[x][y] = 0;
			}
		}

		m_youWon = new YouWon(this);

		enableMouseEvents();
	}

	void handleMouseDown(AvoGUI::MouseEvent const& p_event) override
	{
		if (m_youWon->getIsVisible())
		{
			m_youWon->close();
			for (int32 x = 0; x < NUMBER_OF_COLUMNS; x++)
			{
				memset(m_cells[x], 0, NUMBER_OF_ROWS*4);
			}
		}
		else
		{
			int32 x = uint32(p_event.x / CELL_WIDTH);
			for (int32 y = NUMBER_OF_ROWS - 1; y >= 0; y--)
			{
				if (!m_cells[x][y])
				{
					m_cells[x][y] = m_playerNumber;

					for (int32 i_direction = 0; i_direction < 4; i_direction++)
					{
						int32 numberOfCellsInARow = -1;

						for (int32 forwardsBackwardsDirection = -1; forwardsBackwardsDirection <= 1; forwardsBackwardsDirection += 2)
						{
							int32 offsetX = 0;
							int32 offsetY = 0;

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
	void handleMouseDoubleClick(AvoGUI::MouseEvent const& p_event) override
	{
		handleMouseDown(p_event);
	}

	void draw(AvoGUI::DrawingContext* p_context) override
	{
		p_context->setColor(AvoGUI::Color(0.7f));
		for (uint32 x = 1; x < NUMBER_OF_COLUMNS; x++)
		{
			p_context->drawLine(x * CELL_WIDTH, 0, x * CELL_WIDTH, getHeight(), 2.f);
		}
		for (uint32 y = 1; y <  NUMBER_OF_ROWS; y++)
		{
			p_context->drawLine(0, y * CELL_WIDTH, getWidth(), y * CELL_WIDTH, 2.f);
		}

		for (uint32 x = 0; x < NUMBER_OF_COLUMNS; x++)
		{
			for (uint32 y = 0; y < NUMBER_OF_ROWS; y++)
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
	FourInARow* gui = new FourInARow();
	gui->waitForFinish();
}
