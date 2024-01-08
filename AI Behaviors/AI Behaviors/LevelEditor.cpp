#include "LevelEditor.h"

LevelEditor::LevelEditor()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "Error - problem loading font in Level Editor" << std::endl;
    }

    initBar();
    initButtons();
    m_tiles.resize(numRows, std::vector<Tile>(numCols));
    randomGenerateLevel();
    m_levelEditorView.setSize(Global::S_WIDTH, Global::S_HEIGHT);
    m_levelEditorView.setCenter(Global::S_WIDTH / 2, Global::S_HEIGHT / 2);
}

void LevelEditor::update(sf::Time t_deltaTime)
{
   
}

void LevelEditor::render(sf::RenderWindow& m_window)
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
    m_window.draw(m_backgroundForTiles);
    m_window.draw(m_toGoBackButton);
    m_window.draw(m_toGoBackText);
}

void LevelEditor::renderLoadedLevel(sf::RenderWindow& m_window)
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            m_window.draw(m_tiles[i][j].m_tile);
        }
    }
}

void LevelEditor::handleMouseInput(sf::Vector2i mousePosition, GameState& gameState, sf::RenderWindow& m_window)
{
    sf::Vector2f guiMousePosition = m_window.mapPixelToCoords(mousePosition, m_levelEditorView);

    if (m_toGoBackButton.getGlobalBounds().contains(guiMousePosition))
    {
        gameState = GameState::MainMenu;
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
    m_backgroundForTiles.setFillColor(sf::Color(255, 255, 255, 200));
    m_backgroundForTiles.setSize(sf::Vector2f(Global::S_WIDTH, 200));
    m_backgroundForTiles.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 440);
    m_backgroundForTiles.setOrigin(m_backgroundForTiles.getGlobalBounds().width / 2, m_backgroundForTiles.getGlobalBounds().height / 2);
}

void LevelEditor::initButtons()
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

void LevelEditor::saveLevelToFile(const std::string& filename)
{
    std::ofstream file(filename, std::ios::out);
    if (file.is_open()) 
    {
        for (int i = 0; i < numRows; ++i) 
        {
            for (int j = 0; j < numCols; ++j) 
            {
                file << m_tiles[i][j].isWall << " ";
            }
            file << std::endl;
        }
        file.close();
        std::cout << "Level saved to " << filename << std::endl;
    }
    else 
    {
        std::cout << "Unable to save the level to file." << std::endl;
    }
}

void LevelEditor::loadLevelFromFile(const std::string& filename)
{
    std::ifstream inputFile(filename);

    if (inputFile.is_open())
    {
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                int isWallValue;
                inputFile >> isWallValue;

                m_tiles[i][j].isWall = (isWallValue == 1);
                if (m_tiles[i][j].isWall)
                {
                    m_tiles[i][j].m_tile.setFillColor(sf::Color::Blue);
                }
                else
                {
                    m_tiles[i][j].m_tile.setFillColor(sf::Color::Green);
                }
            }
        }
        inputFile.close();
        std::cout << "level opened from file: " << filename << std::endl;
    }
    else
    {
        std::cout << "Unable to open file: " << filename << std::endl;
    }
}
