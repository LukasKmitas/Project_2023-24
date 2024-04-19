#include "LevelEditor.h"

LevelEditor::LevelEditor()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "Error - problem loading font in Level Editor" << std::endl;
    }
    if (!m_buttonTexture.loadFromFile("Assets\\Images\\GUI\\buttonStock2.png"))
    {
        std::cout << "Error - loading button texture" << std::endl;
    }
    if (!m_buttonTexture1.loadFromFile("Assets\\Images\\GUI\\Card X5.png"))
    {
        std::cout << "Error - loading button texture" << std::endl;
    }

    initBar();
    initBackButton();
    initbuttonsForToolEditor();
    m_tiles.resize(m_numRows, std::vector<Tile>(m_numCols));
    initGrid();
    initDragRectangle();
    //randomGenerateLevel();
    m_levelEditorView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
    m_levelEditorView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
}

/// <summary>
/// Updates the level editor content changes
/// </summary>
/// <param name="t_deltaTime"></param>
/// <param name="m_window"></param>
void LevelEditor::update(sf::Time t_deltaTime, sf::RenderWindow& m_window)
{
    animationForResources();
    animationForWeed();
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_isTileSelected) // used for dragging, creates the size
    {
        sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);
        sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(mousePosition, m_window.getView());
        sf::Vector2f newSize = worldMousePosition - m_startMousePos;
        m_dragRectangle.setSize(newSize);
    }
    if (m_buttonSelectedAnimationClock.getElapsedTime().asSeconds() > m_animationSpeed) 
    {
        m_currentAnimationFrame = (m_currentAnimationFrame + 1) % 4;
        m_buttonSelectedSprite.setTextureRect(sf::IntRect(m_currentAnimationFrame * 16, 0, 16, 16));
        m_buttonSelectedAnimationClock.restart();
    }
}

/// <summary>
/// Draws everything in the level editor mode
/// </summary>
/// <param name="m_window"></param>
void LevelEditor::render(sf::RenderWindow & m_window)
{
    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            m_window.draw(m_tiles[i][j].m_tile);
        }
    }

    for (int i = 0; i <= m_numRows; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(i * m_tiles[0][0].m_tileSize, 0), sf::Color::Black),
            sf::Vertex(sf::Vector2f(i * m_tiles[0][0].m_tileSize, m_numRows * m_tiles[0][0].m_tileSize), sf::Color::Black)
        };
        m_window.draw(line, 2, sf::Lines);
    }
    for (int j = 0; j <= m_numCols; ++j)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, j * m_tiles[0][0].m_tileSize), sf::Color::Black),
            sf::Vertex(sf::Vector2f(m_numCols * m_tiles[0][0].m_tileSize, j * m_tiles[0][0].m_tileSize), sf::Color::Black)
        };
        m_window.draw(line, 2, sf::Lines);
    }

    m_window.draw(m_dragRectangle);
    m_window.setView(m_levelEditorView); // Stuff above stays the same, stuff below move along side the view
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

    for (int i = 0; i < m_numButtons; i++)
    {
        m_window.draw(m_buttons[i]);
    }

    if (m_selectedButtonIndex >= 0) 
    {
        sf::Vector2f buttonPos = m_buttons[m_selectedButtonIndex].getPosition();
        m_buttonSelectedSprite.setPosition(buttonPos.x - 1, buttonPos.y - 1);

        m_window.draw(m_buttonSelectedSprite);
    }
}

/// <summary>
/// Draws the level - used in gameplay
/// </summary>
/// <param name="m_window"></param>
void LevelEditor::renderLoadedLevel(sf::RenderWindow & m_window)
{
    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            Tile& tile = m_tiles[i][j];
            switch (tile.m_fogStatus)
            {
            case Tile::FogStatus::Unexplored:
                tile.m_tile.setFillColor(sf::Color(0, 0, 0, 255));
                break;
            case Tile::FogStatus::Explored:
                tile.m_tile.setFillColor(sf::Color(128, 128, 128, 128));
                break;
            case Tile::FogStatus::Visible:
                tile.m_tile.setFillColor(sf::Color::White);
                break;
            }
            m_window.draw(tile.m_tile);
        }
    }
}

/// <summary>
/// handles the mouse input inside the level editor 
/// </summary>
/// <param name="m_mousePosition"></param>
/// <param name="m_gameState"></param>
/// <param name="m_window"></param>
void LevelEditor::handleMouseInput(sf::Vector2i m_mousePosition, GameState& m_gameState, sf::RenderWindow& m_window)
{
    sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_levelEditorView);
    sf::Vector2f worldMousePosition = m_window.mapPixelToCoords(m_mousePosition, m_window.getView());

    handleTileButtons(guiMousePosition, worldMousePosition, m_buttons, m_lastClickedButtonIndex, m_selectedButtonIndex);

    m_startMousePos = worldMousePosition;
    m_newMousePos = worldMousePosition;
    m_dragRectangle.setPosition(m_startMousePos);

    if (m_toGoBackButton.getGlobalBounds().contains(guiMousePosition))
    {
        m_dragRectangle.setSize(sf::Vector2f(0.0f, 0.0f));
        m_startMousePos = sf::Vector2f(0.f, 0.f);
        m_newMousePos = sf::Vector2f(0.f, 0.f);
        m_isButtonClicked = false;
        m_isTileSelected = false;
        m_lastClickedButtonIndex = -1;
        m_gameState = GameState::MainMenu;
    }
}

/// <summary>
/// Handles the selection of button presses for the bottom UI
/// </summary>
/// <param name="guiMousePosition"></param>
/// <param name="worldMousePosition"></param>
/// <param name="buttons"></param>
/// <param name="lastClickedIndex"></param>
/// <param name="selectedIndex"></param>
void LevelEditor::handleTileButtons(sf::Vector2f m_guiMousePosition, sf::Vector2f m_worldMousePosition, sf::Sprite m_buttons[],int& m_lastClickedIndex, int& m_selectedIndex)
{
    m_isButtonClicked = false;

    for (int i = 0; i < m_numButtons; ++i)
    {
        if (m_buttons[i].getGlobalBounds().contains(m_guiMousePosition))
        {
            m_selectedIndex = i;
            m_buttons[i].setColor(sf::Color(150, 150, 150));
            m_isButtonClicked = true;
            m_lastClickedIndex = i;
        }
        else
        {
            m_buttons[i].setColor(sf::Color(255, 255, 255));
        }
    }

    if (!m_isButtonClicked && m_lastClickedIndex != -1)
    {
        m_buttons[m_lastClickedIndex].setColor(sf::Color(150, 150, 150));
        handleTilePlacement(m_worldMousePosition, m_selectedIndex);
    }
}

/// <summary>
///  handles the placement for the tiles
/// </summary>
/// <param name="m_worldMousePosition"></param>
/// <param name="m_selectedIndex"></param>
void LevelEditor::handleTilePlacement(sf::Vector2f m_worldMousePosition, int m_selectedIndex)
{
    sf::IntRect* buttonArea = nullptr;
    if (m_selectedIndex < 4)
    {
        buttonArea = &m_buttonAreaForWalkable[m_selectedIndex];
    }
    else if (m_selectedIndex < 9)
    {
        buttonArea = &m_buttonAreaForWalls[m_selectedIndex - 4];
    }
    else if (m_selectedIndex < 10)
    {
        buttonArea = &m_buttonAreaForResources;
    }
    else
    {
        buttonArea = &m_buttonAreaForMiscs[m_selectedIndex - 10];
    }

    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            if (m_tiles[i][j].m_tile.getGlobalBounds().contains(m_worldMousePosition))
            {
                if (!m_isTileSelected && m_selectedIndex != -1)
                {
                    m_isTileSelected = true;
                    break;
                }
                else if (m_isTileSelected && m_selectedIndex != -1 && !m_backgroundForTilesTools.getGlobalBounds().contains(m_worldMousePosition))
                {
                    m_selectedTileX = i;
                    m_selectedTileY = j;
                    m_tiles[i][j].m_tile.setTexture(&m_underWaterTexture);
                    m_tiles[i][j].m_isWall = (m_selectedIndex >= 4 && m_selectedIndex < 9);
                    m_tiles[i][j].m_isResource = (m_selectedIndex == 9);
                    m_tiles[i][j].m_animationWeed = (m_selectedIndex == 10);
                    m_tiles[i][j].m_tile.setRotation(0);
                    m_tiles[i][j].m_tile.setTextureRect(*buttonArea);
                    break;
                }
            }
        }
    }
}

/// <summary>
/// Adds all the texture inside the dragRectangle shape
/// </summary>
void LevelEditor::placeTilesInDragRectangle()
{
    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            if (m_dragRectangle.getGlobalBounds().intersects(m_tiles[i][j].m_tile.getGlobalBounds()))
            {
                sf::IntRect* buttonArea = nullptr;
                if (m_selectedButtonIndex < 4)
                {
                    buttonArea = &m_buttonAreaForWalkable[m_selectedButtonIndex];
                }
                else if (m_selectedButtonIndex < 9)
                {
                    buttonArea = &m_buttonAreaForWalls[m_selectedButtonIndex - 4];
                }
                else if (m_selectedButtonIndex < 10)
                {
                    buttonArea = &m_buttonAreaForResources;
                }
                else
                {
                    buttonArea = &m_buttonAreaForMiscs[m_selectedButtonIndex - 10];
                }
                m_tiles[i][j].m_tile.setTexture(&m_underWaterTexture);
                m_tiles[i][j].m_isWall = (m_selectedButtonIndex >= 4 && m_selectedButtonIndex < 9);
                m_tiles[i][j].m_isResource = (m_selectedButtonIndex == 9);
                m_tiles[i][j].m_animationWeed = (m_selectedButtonIndex == 10);
                m_tiles[i][j].m_tile.setRotation(0);
                m_tiles[i][j].m_tile.setTextureRect(*buttonArea);
            }
        }
    }
}

/// <summary>
/// able to rotate the first tile you place
/// </summary>
/// <param name="event"></param>
void LevelEditor::handleRotationInput(sf::Event m_event)
{
    int rotationAngle = 90;

    switch (m_event.key.code)
    {
    case sf::Keyboard::Q:
        m_tiles[m_selectedTileX][m_selectedTileY].m_tile.rotate(-rotationAngle); // Rotate counter-clockwise
        break;
    case sf::Keyboard::E:
        m_tiles[m_selectedTileX][m_selectedTileY].m_tile.rotate(rotationAngle); // Rotate clockwise
        break;
    default:
        break;
    }
}

/// <summary>
/// initializes the grid m_tiles
/// </summary>
void LevelEditor::initGrid()
{
    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            m_tiles[i][j].m_tile.setPosition(j * m_tiles[i][j].m_tileSize + 25, i * m_tiles[i][j].m_tileSize + 25);
            m_tiles[i][j].m_tile.setOrigin(m_tiles[i][j].m_tileSize / 2.f, m_tiles[i][j].m_tileSize / 2.f);
        }
    }
}

/// <summary>
/// to randomise the map 
/// </summary>
void LevelEditor::randomGenerateLevel()
{
    for (int i = 0; i < m_numRows; ++i)
    {
        for (int j = 0; j < m_numCols; ++j)
        {
            // Set random wall probability
            int randomNumber = rand() % 100;
            if (randomNumber < 3)
            {
                m_tiles[i][j].m_isWall = true;
                m_tiles[i][j].m_tile.setFillColor(sf::Color::Blue); // Wall tiles
            }
            else
            {
                m_tiles[i][j].m_isWall = false;
                m_tiles[i][j].m_tile.setFillColor(sf::Color::Green); // Non-wall tiles
            }
            m_tiles[i][j].m_tile.setPosition(j * m_tiles[i][j].m_tileSize, i * m_tiles[i][j].m_tileSize);
        }
    }
}

/// <summary>
/// initializes the UI bar
/// </summary>
void LevelEditor::initBar()
{
    m_backgroundForTilesTools.setFillColor(sf::Color(0, 0, 0, 200));
    m_backgroundForTilesTools.setSize(sf::Vector2f(Global::S_WIDTH, 200));
    m_backgroundForTilesTools.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 440);
    m_backgroundForTilesTools.setOrigin(m_backgroundForTilesTools.getGlobalBounds().width / 2, m_backgroundForTilesTools.getGlobalBounds().height / 2);

    const int numRectangles = 4;
    const float rectWidth = 150.f;
    const float spacing = 320.f;

    const float startX = (Global::S_WIDTH / 2) - (numRectangles * (rectWidth + spacing) / 2) + 100;
    const float startY = Global::S_HEIGHT - 180;

    for (int i = 0; i < numRectangles; ++i)
    {
        m_backgroundForBTF[i].setSize(sf::Vector2f(rectWidth, 40));
        float xPos = startX + i * (rectWidth + spacing);
        m_backgroundForBTF[i].setPosition(xPos, startY);
        m_backgroundForBTF[i].setTexture(&m_buttonTexture1);

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
        textPtr->setCharacterSize(24U);
        textPtr->setFillColor(sf::Color(225, 245, 255));
        textPtr->setOutlineColor(sf::Color::Blue);
        textPtr->setOutlineThickness(0.2f);
        sf::FloatRect textRect = textPtr->getLocalBounds();
        textPtr->setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        textPtr->setPosition(m_backgroundForBTF[i].getPosition().x + rectWidth / 2.0f, m_backgroundForBTF[i].getPosition().y + 20.0f);
    }
}

/// <summary>
/// initializes the tool editor
/// </summary>
void LevelEditor::initbuttonsForToolEditor()
{
    if (!m_underWaterTexture.loadFromFile("Assets\\Images\\LevelEditor\\Labrynna Underwater.png"))
    {
        std::cout << "Error - problem loading Texture in Level Editor" << std::endl;
    }
    if (!m_buttonSelectedTexture.loadFromFile("Assets\\Images\\GUI\\buttonSelected.png"))
    {
        std::cout << "Error - loading button texture" << std::endl;
    }
    m_buttonSelectedSprite.setTexture(m_buttonSelectedTexture);
    m_buttonSelectedSprite.setTextureRect(sf::IntRect(0, 0, 16, 16));
    m_buttonSelectedSprite.setOrigin(m_buttonSelectedSprite.getLocalBounds().width / 2, m_buttonSelectedSprite.getLocalBounds().height / 2);
    m_buttonSelectedSprite.setScale(4.2,4.2);
    initButtonsForToolSet();
}

/// <summary>
/// initializes the buttons for the UI
/// </summary>
void LevelEditor::initButtonsForToolSet()
{
    const float buttonSize = 60.f;
    const float spacing = 20.f;

    // Walkable button areas
    sf::IntRect buttonAreaForWalkable[4] = 
    {
        {17, 1808, 16, 16},
        {33, 1808, 16, 16},
        {49, 1808, 16, 16},
        {49, 1792, 16, 16}
    };

    // Wall button areas
    sf::IntRect buttonAreaForWalls[5] = 
    {
        {49, 1695, 16, 16},
        {242, 1808, 16, 16},
        {257, 1808, 16, 16},
        {226, 1776, 16, 16},
        {258, 1792, 16, 16}
    };

    sf::IntRect buttonAreaForResources = {971, 1798, 16, 16 };

    sf::IntRect buttonAreaForMiscs[2] =
    {
        {971, 1764, 16, 16},
        {854, 1776, 16, 16}
    };

    // Calculate starting positions for each button type
    const float startXForWalkable = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 70;
    const float startYForWalkable = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 120;

    const float startXForWalls = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 500;
    const float startYForWalls = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 120;

    const float startXForResource = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 1100;
    const float startYForResource = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 120;

    const float startXForMisc = m_backgroundForTilesTools.getPosition().x - m_backgroundForTilesTools.getSize().x / 2 + 1500;
    const float startYForMisc = m_backgroundForTilesTools.getPosition().y - m_backgroundForTilesTools.getSize().y / 2 + 120;

    for (int i = 0; i < m_numButtons; ++i)
    {
        m_buttons[i].setTexture(m_underWaterTexture);

        // Determine button area based on index
        sf::IntRect* buttonArea = nullptr;
        if (i < 4)
        {
            buttonArea = &buttonAreaForWalkable[i];
        }
        else if (i < 9)
        {
            buttonArea = &buttonAreaForWalls[i - 4];
        }
        else if (i < 10)
        {
            buttonArea = &buttonAreaForResources;
        }
        else
        {
            buttonArea = &buttonAreaForMiscs[i - 10];
        }

        m_buttons[i].setTextureRect(*buttonArea);
        m_buttons[i].setScale(buttonSize / m_buttons[i].getGlobalBounds().width, buttonSize / m_buttons[i].getGlobalBounds().height);
        m_buttons[i].setOrigin(m_buttons[i].getLocalBounds().width / 2, m_buttons[i].getLocalBounds().height / 2);

        // Calculate position based on index and type of button
        float xPos, yPos;
        if (i < 4)
        {
            xPos = startXForWalkable + i * (buttonSize + spacing);
            yPos = startYForWalkable;
        }
        else if (i < 9)
        {
            xPos = startXForWalls + (i - 4) * (buttonSize + spacing);
            yPos = startYForWalls;
        }
        else if (i < 10)
        {
            xPos = startXForResource + (i - 9) * (buttonSize + spacing);
            yPos = startYForResource;
        }
        else
        {
            xPos = startXForMisc + (i - 10) * (buttonSize + spacing);
            yPos = startYForMisc;
        }
        m_buttons[i].setPosition(xPos, yPos);
    }
}

/// <summary>
/// initializes the back button
/// </summary>
void LevelEditor::initBackButton()
{
    m_toGoBackButton.setSize(sf::Vector2f(100, 50));
    m_toGoBackButton.setPosition(80, 50);
    m_toGoBackButton.setOrigin(m_toGoBackButton.getGlobalBounds().width / 2, m_toGoBackButton.getGlobalBounds().height / 2);
    m_toGoBackButton.setTexture(&m_buttonTexture);

    m_toGoBackText.setFont(m_font);
    m_toGoBackText.setString("Back");
    m_toGoBackText.setPosition(m_toGoBackButton.getPosition().x, m_toGoBackButton.getPosition().y - 5);
    m_toGoBackText.setCharacterSize(25U);
    m_toGoBackText.setFillColor(sf::Color(225, 245, 255));
    m_toGoBackText.setOutlineColor(sf::Color::Blue);
    m_toGoBackText.setOutlineThickness(1.0f);
    m_toGoBackText.setOrigin(m_toGoBackText.getGlobalBounds().width / 2, m_toGoBackText.getGlobalBounds().height / 2);
}

/// <summary>
/// Saves the level with the correct m_tiles properties
/// </summary>
/// <param name="m_filename"></param>
void LevelEditor::saveLevelToFile(const std::string & m_filename)
{
    std::ifstream fileExistsCheck(m_filename);

    if (fileExistsCheck)
    {
        fileExistsCheck.close();
        std::cout << "The level file already exists. Do you want to overwrite it? (Y/N): ";
        char response;
        std::cin >> response;

        if (response != 'Y' && response != 'y')
        {
            std::cout << "Level not saved." << std::endl;
            return;
        }
    }

    std::ofstream file(m_filename, std::ios::out);

    if (file.is_open())
    {
        for (int i = 0; i < m_numRows; ++i)
        {
            for (int j = 0; j < m_numCols; ++j)
            {
                file << m_tiles[i][j].m_isWall << " ";
                file << m_tiles[i][j].m_isResource << " ",
                file << m_tiles[i][j].m_animationWeed << " ",
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

/// <summary>
/// Loads the level with the correct m_tile properties
/// </summary>
/// <param name="m_filename"></param>
void LevelEditor::loadLevelFromFile(const std::string & m_filename)
{
    std::ifstream inputFile(m_filename);

    if (inputFile.is_open())
    {
        for (int i = 0; i < m_numRows; ++i)
        {
            for (int j = 0; j < m_numCols; ++j)
            {
                int isWallValue, isResourceValue, isAnimationWeed;
                inputFile >> isWallValue >> isResourceValue >> isAnimationWeed;

                m_tiles[i][j].m_isWall = (isWallValue == 1);
                m_tiles[i][j].m_isResource = (isResourceValue == 1);
                m_tiles[i][j].m_animationWeed = (isAnimationWeed == 1);

                int left, top, width, height, xPos, yPos, tileRotation, originX, originY;
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

/// <summary>
/// Loads the level in the "selection level" state  
/// </summary>
void LevelEditor::loadLevelForLevelEditor()
{
    std::cout << "Do you want to load a level? (Y/N): ";
    char response;
    std::cin >> response;

    if (response == 'Y' || response == 'y')
    {
        std::string filename;
        std::cout << "Enter the filename to load: ";
        std::cin >> filename;

        if (!filename.empty())
        {
            loadLevelFromFile("Assets\\SaveFiles\\" + filename + ".txt");
            std::cout << "Level loaded." << std::endl;
        }
        else
        {
            std::cout << "Invalid filename. Level not loaded." << std::endl;
        }
    }
}

/// <summary>
/// animates the resources tiles
/// </summary>
void LevelEditor::animationForResources()
{
    if (m_tileResourceAnimationClock.getElapsedTime().asMilliseconds() > 200)
    {
        m_currentResourceTileAnimationFrame = (m_currentResourceTileAnimationFrame + 1) % m_numTileFrames;
        for (int i = 0; i < 50; ++i)
        {
            for (int j = 0; j < 50; ++j)
            {
                if (m_tiles[i][j].m_isResource == true)
                {
                    m_tiles[i][j].m_tile.setTextureRect(m_tileResourceAnimationFrames[m_currentResourceTileAnimationFrame]);
                }
            }
        }
        m_tileResourceAnimationClock.restart();
    }
}

/// <summary>
/// Animates the weed tiles
/// </summary>
void LevelEditor::animationForWeed()
{
    if (m_tileAnimationClock.getElapsedTime().asMilliseconds() > 200) 
    {
        m_currentTileWeedAnimationFrame = (m_currentTileWeedAnimationFrame + 1) % m_numTileFrames;
        for (int i = 0; i < 50; ++i)
        {
            for (int j = 0; j < 50; ++j)
            {
                if (m_tiles[i][j].m_animationWeed == true)
                {
                    m_tiles[i][j].m_tile.setTextureRect(m_tileWeedAnimationFrames[m_currentTileWeedAnimationFrame]);
                }
            }
        }
        m_tileAnimationClock.restart();
    }
}

/// <summary>
/// reset the stuff for dragRectangle
/// </summary>
void LevelEditor::releaseDragRect()
{
    if (m_isTileSelected)
    {
        placeTilesInDragRectangle();
    }
    m_dragRectangle.setSize(sf::Vector2f(0.0f,0.0f));
    m_startMousePos = sf::Vector2f(0.f, 0.f);
    m_newMousePos = sf::Vector2f(0.f, 0.f);
}

/// <summary>
/// initializes the dragRectangle shape
/// </summary>
void LevelEditor::initDragRectangle()
{
    sf::Vector2f initialSize = sf::Vector2f(0.f, 0.f);
    m_dragRectangle.setSize(initialSize);
    m_dragRectangle.setFillColor(sf::Color(0, 255, 0, 50));
    m_dragRectangle.setOutlineColor(sf::Color::Black);
    m_dragRectangle.setOutlineThickness(1.0f);
}

/// <summary>
/// Resets the fog
/// </summary>
void LevelEditor::resetFogOfWar()
{
    for (auto& row : m_tiles)
    {
        for (auto& tile : row)
        {
            tile.m_fogStatus = Tile::FogStatus::Unexplored;
        }
    }
}
