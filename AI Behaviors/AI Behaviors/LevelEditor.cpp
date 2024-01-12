#include "LevelEditor.h"

    LevelEditor::LevelEditor()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "Error - problem loading font in Level Editor" << std::endl;
    }

    initBar();
    initBackButton();
    initbuttonsForToolEditor();
    m_tiles.resize(numRows, std::vector<Tile>(numCols));
    initGrid();
    //randomGenerateLevel();
    m_levelEditorView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
    m_levelEditorView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
}

void LevelEditor::update(sf::Time t_deltaTime)
{

}

void LevelEditor::render(sf::RenderWindow & m_window)
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            m_window.draw(m_tiles[i][j].m_tile);
        }
    }

    for (int i = 0; i <= numRows; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(i * m_tiles[0][0].tileSize, 0), sf::Color::Black),
            sf::Vertex(sf::Vector2f(i * m_tiles[0][0].tileSize, numRows * m_tiles[0][0].tileSize), sf::Color::Black)
        };
        m_window.draw(line, 2, sf::Lines);
    }

    for (int j = 0; j <= numCols; ++j)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, j * m_tiles[0][0].tileSize), sf::Color::Black),
            sf::Vertex(sf::Vector2f(numCols * m_tiles[0][0].tileSize, j * m_tiles[0][0].tileSize), sf::Color::Black)
        };
        m_window.draw(line, 2, sf::Lines);
    }
    m_window.setView(m_levelEditorView);
    m_window.draw(m_backgroundForTilesTools);
    m_window.draw(m_toGoBackButton);
    m_window.draw(m_toGoBackText);

    for (int i = 0; i < 4; i++)
    {
        m_window.draw(m_backgroundForBTF[i]);
    }
    m_window.draw(m_walkableText);
    m_window.draw(m_WallText);
    m_window.draw(m_ResourceText);
    m_window.draw(m_MiscText);

    for (int i = 0; i < numButtons; i++)
    {
        m_window.draw(m_buttons[i]);
    }
}

void LevelEditor::renderLoadedLevel(sf::RenderWindow & m_window)
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            m_window.draw(m_tiles[i][j].m_tile);
        }
    }
}

void LevelEditor::handleMouseInput(sf::Vector2i m_mousePosition, GameState & m_gameState, sf::RenderWindow & m_window)
{
    sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_levelEditorView);
    sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_window.getView());

    if (m_toGoBackButton.getGlobalBounds().contains(guiMousePosition))
    {
        m_gameState = GameState::MainMenu;
    }

    handleTileButtons(guiMousePosition, worldMousePosition, m_buttons, lastClickedButtonIndex, selectedButtonIndex);

}

void LevelEditor::handleTileButtons(sf::Vector2f guiMousePosition, sf::Vector2f worldMousePosition, sf::Sprite buttons[],int& lastClickedIndex, int& selectedIndex)
{
    bool isButtonClicked = false;

    for (int i = 0; i < numButtons; ++i)
    {
        if (buttons[i].getGlobalBounds().contains(guiMousePosition))
        {
            selectedIndex = i;
            buttons[i].setColor(sf::Color(255, 200, 200));
            isButtonClicked = true;
            lastClickedIndex = i;
        }
        else
        {
            buttons[i].setColor(sf::Color(255, 255, 255));
        }
    }

    if (!isButtonClicked && lastClickedIndex != -1)
    {
        buttons[lastClickedIndex].setColor(sf::Color(255, 200, 200));

        handleTilePlacement(worldMousePosition, lastClickedIndex, selectedIndex);
    }
}

void LevelEditor::handleTilePlacement(sf::Vector2f worldMousePosition, int lastClickedIndex, int selectedIndex)
{
    sf::IntRect buttonAreas[4] = {
        {17, 1808, 16, 16},
        {33, 1808, 16, 16},
        {49, 1808, 16, 16},
        {49, 1792, 16, 16}
    };

    sf::IntRect wallButtonAreas[5] = {
        {49, 1695, 16, 16},
        {242, 1808, 16, 16},
        {257, 1808, 16, 16},
        {226, 1776, 16, 16},
        {258, 1792, 16, 16}
    };

    sf::IntRect* buttonArea = (selectedIndex < 4) ? &buttonAreas[selectedIndex] : &wallButtonAreas[selectedIndex - 4];

    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            if (m_tiles[i][j].m_tile.getGlobalBounds().contains(worldMousePosition))
            {
                if (!isTileSelected && selectedIndex != -1)
                {
                    isTileSelected = true;
                    break;
                }
                else if (isTileSelected && selectedIndex != -1 && !m_backgroundForTilesTools.getGlobalBounds().contains(worldMousePosition))
                {
                    selectedTileX = i;
                    selectedTileY = j;
                    m_tiles[i][j].m_tile.setTexture(&m_underWaterTexture);
                    m_tiles[i][j].isWall = (selectedIndex >= 4);
                    m_tiles[i][j].isResource = false;
                    isOffsetApplied = false;

                    m_tiles[i][j].m_tile.setTextureRect(*buttonArea);
                    break;
                }
            }
        }
    }
}

void LevelEditor::handleRotationInput(sf::Event event)
{
    if (isTileSelected && selectedButtonIndex != -1 && !isOffsetApplied)
    {
        int rotationAngle = 90;

        sf::FloatRect tileBounds = m_tiles[selectedTileX][selectedTileY].m_tile.getLocalBounds();
        m_tiles[selectedTileX][selectedTileY].m_tile.setOrigin(tileBounds.width / 2.f, tileBounds.height / 2.f);

        m_tiles[selectedTileX][selectedTileY].m_tile.setPosition(
            m_tiles[selectedTileX][selectedTileY].m_tile.getPosition().x + offset,
            m_tiles[selectedTileX][selectedTileY].m_tile.getPosition().y + offset
        );

        isOffsetApplied = true;
    }

    if (isTileSelected && selectedButtonIndex != -1)
    {
        int rotationAngle = 90;

        switch (event.key.code)
        {
        case sf::Keyboard::Q:
            m_tiles[selectedTileX][selectedTileY].m_tile.rotate(-rotationAngle); // Rotate counter-clockwise
            break;
        case sf::Keyboard::E:
            m_tiles[selectedTileX][selectedTileY].m_tile.rotate(rotationAngle); // Rotate clockwise
            break;
        default:
            break;
        }
    }
}

void LevelEditor::initGrid()
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            m_tiles[i][j].m_tile.setPosition(j * m_tiles[i][j].tileSize, i * m_tiles[i][j].tileSize);
        }
    }
}

void LevelEditor::randomGenerateLevel()
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            // Set random wall probability
            int randomNumber = rand() % 100;
            if (randomNumber < 3)
            {
                m_tiles[i][j].isWall = true;
                m_tiles[i][j].m_tile.setFillColor(sf::Color::Blue); // Wall tiles
            }
            else
            {
                m_tiles[i][j].isWall = false;
                m_tiles[i][j].m_tile.setFillColor(sf::Color::Green); // Non-wall tiles
            }
            m_tiles[i][j].m_tile.setPosition(j * m_tiles[i][j].tileSize, i * m_tiles[i][j].tileSize);
        }
    }
}

void LevelEditor::initBar()
{
    m_backgroundForTilesTools.setFillColor(sf::Color(255, 255, 255, 200));
    m_backgroundForTilesTools.setSize(sf::Vector2f(Global::S_WIDTH, 200));
    m_backgroundForTilesTools.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 440);
    m_backgroundForTilesTools.setOrigin(m_backgroundForTilesTools.getGlobalBounds().width / 2, m_backgroundForTilesTools.getGlobalBounds().height / 2);

    const int numRectangles = 4;
    const float rectWidth = 100.f;
    const float spacing = 300.f;

    const float startX = (Global::S_WIDTH / 2) - (numRectangles * (rectWidth + spacing) / 2) + 100;
    const float startY = Global::S_HEIGHT - 180;

    for (int i = 0; i < numRectangles; ++i)
    {
        m_backgroundForBTF[i].setFillColor(sf::Color::Cyan);
        m_backgroundForBTF[i].setSize(sf::Vector2f(rectWidth, 40));
        float xPos = startX + i * (rectWidth + spacing);
        m_backgroundForBTF[i].setPosition(xPos, startY);

        sf::Text* textPtr = nullptr;
        switch (i)
        {
        case 0:
            textPtr = &m_walkableText;
            textPtr->setString("Walkable");
            break;
        case 1:
            textPtr = &m_WallText;
            textPtr->setString("Walls");
            break;
        case 2:
            textPtr = &m_ResourceText;
            textPtr->setString("Resource");
            break;
        case 3:
            textPtr = &m_MiscText;
            textPtr->setString("Misc");
            break;
        default:
            break;
        }

        textPtr->setFont(m_font);
        textPtr->setCharacterSize(20U);
        textPtr->setFillColor(sf::Color::Black);
        sf::FloatRect textRect = textPtr->getLocalBounds();
        textPtr->setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        textPtr->setPosition(m_backgroundForBTF[i].getPosition().x + rectWidth / 2.0f, m_backgroundForBTF[i].getPosition().y + 20.0f);
    }
}

void LevelEditor::initbuttonsForToolEditor()
{
    if (!m_underWaterTexture.loadFromFile("Assets\\Temp\\Labrynna Underwater.png"))
    {
        std::cout << "Error - problem loading Texture in Level Editor" << std::endl;
    }
    initButtonsForToolSet();
}

void LevelEditor::initButtonsForToolSet()
{
    const float buttonSize = 70.f;
    const float spacing = 20.f;

    // Walkable button areas
    sf::IntRect buttonAreasForWalkable[4] = {
        {17, 1808, 16, 16},
        {33, 1808, 16, 16},
        {49, 1808, 16, 16},
        {49, 1792, 16, 16}
    };

    // Wall button areas
    sf::IntRect buttonAreasForWalls[5] = {
        {49, 1695, 16, 16},
        {242, 1808, 16, 16},
        {257, 1808, 16, 16},
        {226, 1776, 16, 16},
        {258, 1792, 16, 16}
    };

    // Calculate starting positions for walkable and wall buttons
    const float startXForWalkable = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 50;
    const float startYForWalkable = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 80;

    const float startXForWalls = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 530;
    const float startYForWalls = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 80;

    for (int i = 0; i < numButtons; ++i)
    {
        m_buttons[i].setTexture(m_underWaterTexture);

        // Determine button area based on index
        sf::IntRect* buttonArea = (i < 4) ? &buttonAreasForWalkable[i] : &buttonAreasForWalls[i - 4];

        m_buttons[i].setTextureRect(*buttonArea);
        m_buttons[i].setScale(buttonSize / m_buttons[i].getGlobalBounds().width, buttonSize / m_buttons[i].getGlobalBounds().height);

        // Calculate position based on index and type of button
        float xPos = (i < 4) ? startXForWalkable + i * (buttonSize + spacing) : startXForWalls + (i - 4) * (buttonSize + spacing);
        float yPos = (i < 4) ? startYForWalkable : startYForWalls;
        m_buttons[i].setPosition(xPos, yPos);
    }
}

void LevelEditor::initBackButton()
{
    m_toGoBackButton.setFillColor(sf::Color(0, 200, 200));
    m_toGoBackButton.setSize(sf::Vector2f(100, 50));
    m_toGoBackButton.setPosition(80, 50);
    m_toGoBackButton.setOrigin(m_toGoBackButton.getGlobalBounds().width / 2, m_toGoBackButton.getGlobalBounds().height / 2);

    m_toGoBackText.setFont(m_font);
    m_toGoBackText.setString("Back");
    m_toGoBackText.setPosition(m_toGoBackButton.getPosition().x, m_toGoBackButton.getPosition().y - 5);
    m_toGoBackText.setCharacterSize(25U);
    m_toGoBackText.setOutlineColor(sf::Color::Black);
    m_toGoBackText.setFillColor(sf::Color::White);
    m_toGoBackText.setOutlineThickness(1.0f);
    m_toGoBackText.setOrigin(m_toGoBackText.getGlobalBounds().width / 2, m_toGoBackText.getGlobalBounds().height / 2);
}

void LevelEditor::saveLevelToFile(const std::string & m_filename)
{
    std::ofstream file(m_filename, std::ios::out);
    if (file.is_open())
    {
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                file << m_tiles[i][j].isWall << " ";
                file << m_tiles[i][j].m_tile.getTextureRect().left << " ";
                file << m_tiles[i][j].m_tile.getTextureRect().top << " ";
                file << m_tiles[i][j].m_tile.getTextureRect().width << " ";
                file << m_tiles[i][j].m_tile.getTextureRect().height << " ";
                file << m_tiles[i][j].m_tile.getPosition().x << " ";
                file << m_tiles[i][j].m_tile.getPosition().y << " ";
                file << m_tiles[i][j].m_tile.getRotation() << " ";
                file << m_tiles[i][j].m_tile.getOrigin().x << " ";
                file << m_tiles[i][j].m_tile.getOrigin().y << " ";
            }
            file << std::endl;
        }
        file.close();
        std::cout << "Level saved to " << m_filename << std::endl;
    }
    else
    {
        std::cout << "Unable to save the level to file." << std::endl;
    }
}

void LevelEditor::loadLevelFromFile(const std::string & m_filename)
{
    std::ifstream inputFile(m_filename);

    if (inputFile.is_open())
    {
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                int isWallValue;
                inputFile >> isWallValue;

                m_tiles[i][j].isWall = (isWallValue == 1);

                int left, top, width, height;
                float tileRotation, xPos, yPos, originX, originY;
                inputFile >> left >> top >> width >> height >> xPos >> yPos >> tileRotation >> originX >> originY;

                sf::IntRect textureRect(left, top, width, height);
                m_tiles[i][j].m_tile.setTexture(&m_underWaterTexture);
                m_tiles[i][j].m_tile.setTextureRect(textureRect);
                m_tiles[i][j].m_tile.setPosition(xPos, yPos);
                m_tiles[i][j].m_tile.setRotation(tileRotation);
                m_tiles[i][j].m_tile.setOrigin(originX, originY);
            }
        }
        inputFile.close();
        std::cout << "level opened from file: " << m_filename << std::endl;
    }
    else
    {
        std::cout << "Unable to open file: " << m_filename << std::endl;
    }
}