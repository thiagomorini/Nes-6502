#pragma once

#include<iostream>
#include <sstream>
#include<vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include "Bus.h"
#include "Cpu.h"

class Engine
{
private:
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event event;

	sf::Font font;
	std::vector<sf::Text> uiText;

	void initVariables();
	void initFonts();
	void initWindow();

public:
	// Constructor and destructor
	Engine();
	~Engine();

	const bool running() const;

	// Functions
	void poolEvents();
	void update();

	void renderText(sf::RenderTarget& target);
	void render();

	// Emulation stuffs
	Bus nes;
	std::map<uint16_t, std::string> mapAsm;
	std::string hex(uint32_t n, uint8_t d);

	void initRam();

	void drawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns);
	void drawCpu(int x, int y);
	void drawCode(int x, int y, int nLines);
	void drawString(int32_t x, int32_t y, const std::string& sText, sf::Color color = sf::Color::White);

	void updateScreen();
};

