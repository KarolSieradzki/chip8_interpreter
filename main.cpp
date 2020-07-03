#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <cstdio>
#include "chip8.h"

Chip8 chip8;

void key_pressed(sf::Keyboard::Key key);
void key_released(sf::Keyboard::Key key);

const unsigned int W = 64;
const unsigned int H = 32; 

int main(int argc, char** argv) {

	if (argc < 2) {
		printf("usage: chip8_interpreter game.c8");
		return 1;
	}

	sf::RenderWindow window(sf::VideoMode(W, H), "chip8 interpreter");
	window.setFramerateLimit(120);

	sf::Uint8* pixels = new sf::Uint8[W * H * 4];
	sf::Texture texture;
	texture.create(W, H);
	sf::Sprite sprite(texture);

	chip8.initialize();

	chip8.load_game(argv[1]);
	
 	while (window.isOpen()) {

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				key_pressed(event.key.code);
			}
			if (event.type == sf::Event::KeyReleased) {
				key_released(event.key.code);
			}
		}

		chip8.emulate_cycle();

		if (chip8.draw_flag) {
			window.clear();
			
			for (register int i = 0, j = 0; i < W * H * 4; i += 4, j++) {
				pixels[i] = pixels[i + 1] = pixels[i + 2] = chip8.gfx[j] * 255; pixels[i + 3] = 255;
			}

			texture.update(pixels);

			window.draw(sprite);
			window.display();
		}
	}

}

void key_pressed(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Num1: { chip8.key[0x1] = 1; break; }
	case sf::Keyboard::Num2: { chip8.key[0x2] = 1; break; }
	case sf::Keyboard::Num3: { chip8.key[0x3] = 1; break; }
	case sf::Keyboard::Num4: { chip8.key[0xC] = 1; break; }

	case sf::Keyboard::Q: { chip8.key[0x4] = 1; break; }
	case sf::Keyboard::W: { chip8.key[0x5] = 1; break; }
	case sf::Keyboard::E: { chip8.key[0x6] = 1; break; }
	case sf::Keyboard::R: { chip8.key[0xD] = 1; break; }

	case sf::Keyboard::A: { chip8.key[0x7] = 1; break; }
	case sf::Keyboard::S: { chip8.key[0x8] = 1; break; }
	case sf::Keyboard::D: { chip8.key[0x9] = 1; break; }
	case sf::Keyboard::F: { chip8.key[0xE] = 1; break; }

	case sf::Keyboard::Z: { chip8.key[0xA] = 1; break; }
	case sf::Keyboard::X: { chip8.key[0x0] = 1; break; }
	case sf::Keyboard::C: { chip8.key[0xB] = 1; break; }
	case sf::Keyboard::V: { chip8.key[0xF] = 1; break; }

	case sf::Keyboard::Escape: { exit(0); }
	}
}
void key_released(sf::Keyboard::Key key) {
	switch (key) {
	case sf::Keyboard::Num1: { chip8.key[0x1] = 0; break; }
	case sf::Keyboard::Num2: { chip8.key[0x2] = 0; break; }
	case sf::Keyboard::Num3: { chip8.key[0x3] = 0; break; }
	case sf::Keyboard::Num4: { chip8.key[0xC] = 0; break; }

	case sf::Keyboard::Q: { chip8.key[0x4] = 0; break; }
	case sf::Keyboard::W: { chip8.key[0x5] = 0; break; }
	case sf::Keyboard::E: { chip8.key[0x6] = 0; break; }
	case sf::Keyboard::R: { chip8.key[0xD] = 0; break; }

	case sf::Keyboard::A: { chip8.key[0x7] = 0; break; }
	case sf::Keyboard::S: { chip8.key[0x8] = 0; break; }
	case sf::Keyboard::D: { chip8.key[0x9] = 0; break; }
	case sf::Keyboard::F: { chip8.key[0xE] = 0; break; }

	case sf::Keyboard::Z: { chip8.key[0xA] = 0; break; }
	case sf::Keyboard::X: { chip8.key[0x0] = 0; break; }
	case sf::Keyboard::C: { chip8.key[0xB] = 0; break; }
	case sf::Keyboard::V: { chip8.key[0xF] = 0; break; }
	}
}