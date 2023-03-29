#include "Engine.h"

Engine::Engine()
{
	this->initVariables();
	this->initRam();
	this->initFonts();
	this->initWindow();
}

Engine::~Engine()
{
	delete this->window;
}

void Engine::initVariables()
{
	this->window = nullptr;
}

void Engine::initFonts()
{
	if (!this->font.loadFromFile("Fonts/cour.ttf"))
	{
		std::cout << "ERROR::INITFONTS::Failed to load font!" << "\n";
	}
}

void Engine::initWindow()
{
	this->videoMode.height = 768;
	this->videoMode.width = 1024;
	this->window = new sf::RenderWindow(this->videoMode, "CPU Test", sf::Style::Titlebar | sf::Style::Close);
}

const bool Engine::running() const
{
	return this->window->isOpen();
}

void Engine::poolEvents()
{
	while (this->window->pollEvent(this->event))
	{
		switch (this->event.type)
		{
		case sf::Event::Closed:
			this->window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->event.key.code == sf::Keyboard::Escape)
				this->window->close();

			if (this->event.key.code == sf::Keyboard::Space)
			{
				do
				{
					this->nes.cpu.clock();
				} while (!this->nes.cpu.complete());
			}

			if (this->event.key.code == sf::Keyboard::R)
			{
				this->nes.cpu.reset();
			}

			if (this->event.key.code == sf::Keyboard::I)
			{
				this->nes.cpu.irq();
			}

			if (this->event.key.code == sf::Keyboard::N)
			{
				this->nes.cpu.nmi();
			}

			break;
		}
	}
}

void Engine::update()
{
	this->poolEvents();
	this->updateScreen();
}

void Engine::renderText(sf::RenderTarget & target)
{
	for (size_t i = 0; i < this->uiText.size(); i++)
	{
		target.draw(this->uiText[i]);
	}
}

void Engine::render()
{
	this->window->clear(sf::Color(0, 0, 139)); // Dark blue

	// Rendering things here
	this->renderText(*this->window);

	this->window->display();
}

// All the emulation stuffs

std::string Engine::hex(uint32_t n, uint8_t d)
{
	std::string s(d, '0');
	for (int i = d - 1; i >= 0; i--, n >>= 4)
		s[i] = "0123456789ABCDEF"[n & 0xF];
	return s;
}

void Engine::initRam()
{
	// Load Program (assembled at https://www.masswerk.at/6502/assembler.html)
		/*
			*=$8000
			LDX #10
			STX $0000
			LDX #3
			STX $0001
			LDY $0000
			LDA #0
			CLC
			loop
			ADC $0001
			DEY
			BNE loop
			STA $0002
			NOP
			NOP
			NOP
		*/

	// Convert hex string into bytes for RAM
	std::stringstream ss;
	ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";
	uint16_t nOffset = 0x8000;
	while (!ss.eof())
	{
		std::string b;
		ss >> b;
		this->nes.ram[nOffset++] = (uint8_t)std::stoul(b, nullptr, 16);
	}

	// Set Reset Vector
	this->nes.ram[0xFFFC] = 0x00;
	this->nes.ram[0xFFFD] = 0x80;

	// Extract dissassembly
	this->mapAsm = this->nes.cpu.disassemble(0x0000, 0xFFFF);

	// Reset
	this->nes.cpu.reset();
}

void Engine::drawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
{
	int nRamX = x, nRamY = y;
	for (int row = 0; row < nRows; row++)
	{
		std::string sOffset = "$" + this->hex(nAddr, 4) + ":";
		for (int col = 0; col < nColumns; col++)
		{
			sOffset += " " + this->hex(this->nes.cpuRead(nAddr, true), 2);
			nAddr += 1;
		}
		this->drawString(nRamX, nRamY, sOffset);
		nRamY += 20;
	}
}

void Engine::drawCpu(int x, int y)
{
	std::string status = "STATUS: ";
	this->drawString(x, y, "STATUS:", sf::Color::White);
	this->drawString(x + 100, y, "N", this->nes.cpu.status & Cpu::N ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 130, y, "V", this->nes.cpu.status & Cpu::V ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 160, y, "-", this->nes.cpu.status & Cpu::U ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 190, y, "B", this->nes.cpu.status & Cpu::B ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 220, y, "D", this->nes.cpu.status & Cpu::D ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 250, y, "I", this->nes.cpu.status & Cpu::I ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 280, y, "Z", this->nes.cpu.status & Cpu::Z ? sf::Color::Green : sf::Color::Red);
	this->drawString(x + 310, y, "C", this->nes.cpu.status & Cpu::C ? sf::Color::Green : sf::Color::Red);
	this->drawString(x, y + 30, "PC: $" + this->hex(this->nes.cpu.pc, 4));
	this->drawString(x, y + 60, "A: $" + this->hex(this->nes.cpu.a, 2) + "  [" + std::to_string(this->nes.cpu.a) + "]");
	this->drawString(x, y + 90, "X: $" + this->hex(this->nes.cpu.x, 2) + "  [" + std::to_string(this->nes.cpu.x) + "]");
	this->drawString(x, y + 120, "Y: $" + this->hex(this->nes.cpu.y, 2) + "  [" + std::to_string(this->nes.cpu.y) + "]");
	this->drawString(x, y + 150, "Stack P: $" + this->hex(this->nes.cpu.stkp, 4));
}

void Engine::drawCode(int x, int y, int nLines)
{
	auto it_a = this->mapAsm.find(this->nes.cpu.pc);
	int nLineY = (nLines >> 1) * 20 + y;
	if (it_a != this->mapAsm.end())
	{
		this->drawString(x, nLineY, (*it_a).second, sf::Color::Cyan);
		while (nLineY < (nLines * 20) + y)
		{
			nLineY += 20;
			if (++it_a != this->mapAsm.end())
			{
				this->drawString(x, nLineY, (*it_a).second);
			}
		}
	}

	it_a = this->mapAsm.find(this->nes.cpu.pc);
	nLineY = (nLines >> 1) * 20 + y;
	if (it_a != this->mapAsm.end())
	{
		while (nLineY > y)
		{
			nLineY -= 20;
			if (--it_a != this->mapAsm.end())
			{
				this->drawString(x, nLineY, (*it_a).second);
			}
		}
	}
}

void Engine::drawString(int32_t x, int32_t y, const std::string & sText, sf::Color color)
{
	sf::Text txt;
	txt.setFont(this->font);
    txt.setCharacterSize(18);
	txt.setPosition((float)x, (float)y);
    txt.setFillColor(color);
    txt.setString(sText);

	this->uiText.push_back(txt);
}

void Engine::updateScreen()
{
	this->uiText.clear();
	// Draw Ram Page 0x00		
	this->drawRam(4, 2, 0x0000, 16, 16);
	this->drawRam(4, 340, 0x8000, 16, 16);
	this->drawCpu(650, 2);
	this->drawCode(650, 200, 26);

	this->drawString(4, 686, "SPACE = Step Instruction   R = RESET   I = IRQ   N = NMI");
	this->drawString(4, 706, "ESC = Exit");
}
