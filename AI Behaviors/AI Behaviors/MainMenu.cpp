#include "MainMenu.h"

MainMenu::MainMenu()
{
	initButtons();
    initBackgroundImage();
}

void MainMenu::update(sf::Time t_deltaTime)
{
    titleAnimation(t_deltaTime);
    m_deltaTime = t_deltaTime;

    static float time = 0.0f;
    time += t_deltaTime.asSeconds();
    m_backgroundShader.setUniform("time", time);

}

void MainMenu::render(sf::RenderWindow& m_window)
{
    m_window.draw(m_backgroundSprite, &m_backgroundShader);
    m_window.draw(m_playButton);
    m_window.draw(m_editorButton);
    m_window.draw(m_loadLevelButton);
    m_window.draw(m_NeuralNetworkButton);
    m_window.draw(m_exitButton);
    m_window.draw(m_playText);
    m_window.draw(m_editorText);
    m_window.draw(m_loadLevelText);
    m_window.draw(m_neuralNetworkText);
    m_window.draw(m_exitText);

    m_window.draw(outlineText);
    m_window.draw(m_titleText, &m_titleShader);

    buttonTabAnimation(m_window, m_deltaTime);
}

void MainMenu::handleButtonClick(const sf::Vector2f& m_mousePosition, GameState& m_gameState)
{
    if (m_playButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::PlayGame;
    }
    else if (m_editorButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::LevelEditor;
    }
    else if (m_loadLevelButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::LevelLoad;
    }
    else if (m_NeuralNetworkButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::NeuralNetworks;
    }
    else if (m_exitButton.getGlobalBounds().contains(m_mousePosition))
    {
        m_gameState = GameState::Exit;
    }
}

void MainMenu::initButtons()
{
    if (!m_font.loadFromFile("Assets\\Fonts\\ManicSea_19.ttf"))
    {
        std::cout << "Error - loading font in Main Menu" << std::endl;
    }

    if (!m_titleShader.loadFromFile("Assets\\Shaders\\titleEffect.frag", sf::Shader::Fragment))
    {
        std::cout << "Error - loading shader" << std::endl;
    }

    if (!m_backgroundShader.loadFromFile("Assets\\Shaders\\backgroundEffect.frag", sf::Shader::Fragment)) 
    {
        std::cout << "Error - loading background shader" << std::endl;
    }

    if (!m_buttonTexture.loadFromFile("Assets\\Images\\GUI\\buttonStock2.png"))
    {
        std::cout << "Error - loading button texture" << std::endl;
    }

    if (!m_buttonTabTexture.loadFromFile("Assets\\Images\\GUI\\tab_selected1.png"))
    {
        std::cout << "Error - loading buttonTab texture" << std::endl;
    }

    m_buttonTabSprite.setTexture(m_buttonTabTexture);
    m_buttonTabSprite.setOrigin(m_buttonTabSprite.getLocalBounds().width / 2, m_buttonTabSprite.getLocalBounds().height / 2);

    // Title message
    m_titleText.setFont(m_font);
    m_titleText.setString("Gills & Glory");
    m_titleText.setStyle(sf::Text::Italic | sf::Text::Bold);
    m_titleText.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 10);
    m_titleText.setCharacterSize(80U);
    m_titleText.setFillColor(sf::Color(10, 10, 80));
    m_titleText.setOrigin(m_titleText.getGlobalBounds().width / 2, m_titleText.getGlobalBounds().height / 2);
    outlineText = m_titleText;
    outlineText.setFillColor(sf::Color::Transparent);
    outlineText.setOutlineColor(sf::Color(255, 215, 0));
    outlineText.setOutlineThickness(1.0f);

    // Play Button
    m_playButton.setSize(sf::Vector2f(300, 85));
    m_playButton.setOrigin(m_playButton.getSize().x / 2, m_playButton.getSize().y / 2);
    m_playButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 - 200);
    m_playButton.setTexture(&m_buttonTexture);
    m_playText.setFont(m_font);
    m_playText.setString("Play");
    m_playText.setCharacterSize(35);
    m_playText.setFillColor(sf::Color(225,245,255));
    m_playText.setOutlineColor(sf::Color::Blue);
    m_playText.setOutlineThickness(1.0f);
    m_playText.setOrigin(m_playText.getGlobalBounds().width / 2, m_playText.getGlobalBounds().height / 2);
    m_playText.setPosition(m_playButton.getPosition().x, m_playButton.getPosition().y - 5);

    // Level Editor Button
    m_editorButton.setSize(sf::Vector2f(300, 85));
    m_editorButton.setOrigin(m_editorButton.getSize().x / 2, m_editorButton.getSize().y / 2);
    m_editorButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 - 50);
    m_editorButton.setTexture(&m_buttonTexture);
    m_editorText.setFont(m_font);
    m_editorText.setString("Level Editor");
    m_editorText.setCharacterSize(35);
    m_editorText.setFillColor(sf::Color(225, 245, 255));
    m_editorText.setOutlineColor(sf::Color::Blue);
    m_editorText.setOutlineThickness(1.0f);
    m_editorText.setOrigin(m_editorText.getGlobalBounds().width / 2, m_editorText.getGlobalBounds().height / 2);
    m_editorText.setPosition(m_editorButton.getPosition().x, m_editorButton.getPosition().y - 5);

    // Load Level Button
    m_loadLevelButton.setSize(sf::Vector2f(300, 85));
    m_loadLevelButton.setOrigin(m_loadLevelButton.getSize().x / 2, m_loadLevelButton.getSize().y / 2);
    m_loadLevelButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 100);
    m_loadLevelButton.setTexture(&m_buttonTexture);
    m_loadLevelText.setFont(m_font);
    m_loadLevelText.setString("Load Level");
    m_loadLevelText.setCharacterSize(35);
    m_loadLevelText.setFillColor(sf::Color(225, 245, 255));
    m_loadLevelText.setOutlineColor(sf::Color::Blue);
    m_loadLevelText.setOutlineThickness(1.0f);
    m_loadLevelText.setOrigin(m_loadLevelText.getGlobalBounds().width / 2, m_loadLevelText.getGlobalBounds().height / 2);
    m_loadLevelText.setPosition(m_loadLevelButton.getPosition().x, m_loadLevelButton.getPosition().y - 5);

    // Exit Button
    m_exitButton.setSize(sf::Vector2f(300, 85));
    m_exitButton.setOrigin(m_exitButton.getSize().x / 2, m_exitButton.getSize().y / 2);
    m_exitButton.setPosition(Global::S_WIDTH / 2, Global::S_HEIGHT / 2 + 250);
    m_exitButton.setTexture(&m_buttonTexture);
    m_exitText.setFont(m_font);
    m_exitText.setString("Exit");
    m_exitText.setCharacterSize(35);
    m_exitText.setFillColor(sf::Color(225, 245, 255));
    m_exitText.setOutlineColor(sf::Color::Blue);
    m_exitText.setOutlineThickness(1.0f);
    m_exitText.setOrigin(m_exitText.getGlobalBounds().width / 2, m_exitText.getGlobalBounds().height / 2);
    m_exitText.setPosition(m_exitButton.getPosition().x, m_exitButton.getPosition().y - 5);

    // Neural Network Button
    m_NeuralNetworkButton.setSize(sf::Vector2f(300, 85));
    m_NeuralNetworkButton.setOrigin(m_NeuralNetworkButton.getSize().x / 2, m_NeuralNetworkButton.getSize().y / 2);
    m_NeuralNetworkButton.setPosition(Global::S_WIDTH / 2 - 600, Global::S_HEIGHT / 2 + 100);
    m_NeuralNetworkButton.setTexture(&m_buttonTexture);
    m_neuralNetworkText.setFont(m_font);
    m_neuralNetworkText.setString("Neural Networks");
    m_neuralNetworkText.setCharacterSize(35);
    m_neuralNetworkText.setFillColor(sf::Color(225, 245, 255));
    m_neuralNetworkText.setOutlineColor(sf::Color::Blue);
    m_neuralNetworkText.setOutlineThickness(1.0f);
    m_neuralNetworkText.setOrigin(m_neuralNetworkText.getGlobalBounds().width / 2, m_neuralNetworkText.getGlobalBounds().height / 2);
    m_neuralNetworkText.setPosition(m_NeuralNetworkButton.getPosition().x, m_NeuralNetworkButton.getPosition().y - 5);

}

void MainMenu::initBackgroundImage()
{
    if (!m_backgroundTexture.loadFromFile("Assets\\Images\\Menu\\Down the Tubes.png"))
    {
        std::cout << "Error - Failed to load background image" << std::endl;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture);

    m_backgroundSprite.setScale(static_cast<float>(Global::S_WIDTH) / m_backgroundSprite.getGlobalBounds().width,
        static_cast<float>(Global::S_HEIGHT) / m_backgroundSprite.getGlobalBounds().height);
}

void MainMenu::buttonTabAnimation(sf::RenderWindow& m_window, sf::Time t_deltaTime)
{
    static float animationTime = 0.0f;
    animationTime += t_deltaTime.asSeconds();

    float offset = sin(animationTime * 0.08) * 20;

    sf::Vector2f mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window));

    auto animateTab = [&](const sf::RectangleShape& button)
        {
            m_buttonTabSprite.setPosition((button.getPosition().x - button.getSize().x / 2) - 50 + offset, button.getPosition().y);
            m_buttonTabSprite.setScale(1.5f, 1.5f);
            m_window.draw(m_buttonTabSprite);

            m_buttonTabSprite.setPosition((button.getPosition().x + button.getSize().x / 2) + 50 - offset, button.getPosition().y);
            m_buttonTabSprite.setScale(-1.5f, 1.5f);
            m_window.draw(m_buttonTabSprite);
        };

    if (m_playButton.getGlobalBounds().contains(mousePos))
    {
        animateTab(m_playButton);
    }
    else if (m_editorButton.getGlobalBounds().contains(mousePos)) 
    {
        animateTab(m_editorButton);
    }
    else if (m_loadLevelButton.getGlobalBounds().contains(mousePos))
    {
        animateTab(m_loadLevelButton);
    }
    else if (m_NeuralNetworkButton.getGlobalBounds().contains(mousePos))
    {
        animateTab(m_NeuralNetworkButton);
    }
    else if (m_exitButton.getGlobalBounds().contains(mousePos)) 
    {
        animateTab(m_exitButton);
    }
}

void MainMenu::titleAnimation(sf::Time t_deltaTime)
{
    static float time = 0.0f;
    time += t_deltaTime.asSeconds();

    m_titleShader.setUniform("texture", sf::Shader::CurrentTexture);
    m_titleShader.setUniform("fillColor", sf::Glsl::Vec3(m_titleText.getFillColor().r / 255.f, m_titleText.getFillColor().g / 255.f, m_titleText.getFillColor().b / 255.f));
    m_titleShader.setUniform("time", time);

    m_titleAnimationTime += t_deltaTime.asSeconds() * m_titleAnimationSpeed;
    float scale = m_titleMinScale + (sin(m_titleAnimationTime) + 1) / 2 * (m_titleMaxScale - m_titleMinScale);
    m_titleText.setScale(scale, scale);
    m_titleText.setOrigin(m_titleText.getLocalBounds().width / 2, m_titleText.getLocalBounds().height / 2);
    outlineText.setScale(scale, scale);
    outlineText.setOrigin(outlineText.getLocalBounds().width / 2, outlineText.getLocalBounds().height / 2);
}
