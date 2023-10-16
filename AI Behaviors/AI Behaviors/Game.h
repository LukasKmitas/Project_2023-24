#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GUI.h"
#include "Global.h"

class Game
{
public:
	Game();
	~Game();

	void run();

private:
	GUI m_gui;

	void processEvents();
	void processKeys(sf::Event t_event);
	void update(sf::Time t_deltaTime);
	void render();
	
	void setupSprite();

	sf::RenderWindow m_window;

	sf::Font m_ArialBlackfont;
	sf::Text m_welcomeMessage;
	sf::Texture m_logoTexture;
	sf::Sprite m_logoSprite;

	bool m_exitGame;

};

#endif

