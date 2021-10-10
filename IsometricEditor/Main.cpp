#include "Renderer.hpp"
#include "Input.hpp"
#include <iostream>
#include <vector>

const Color RED = { 0xFF, 0x00, 0x00, 0xFF };
const Color GREEN = { 0x00, 0xFF, 0x00, 0xFF };
const Color BLUE = { 0x00, 0x00, 0xFF, 0xFF };
const Color BLACK = { 0x00, 0x00, 0x00, 0xFF };
const Color WHITE = { 0xFF, 0xFF, 0xFF, 0xFF };

const int BLOCK_SIZE = 48;

int main(int argc, char* argv) {
	int windowX = 1080;
	int windowY = 720;
	
	Renderer renderer(windowX, windowY, "Isometric Editor");
	if (renderer.status == false) {
		std::cout << "Renderer failed to initialize" << std::endl;
		return 1;
	}

	InputManager input;

	//------- Create Gridlines -------
	std::vector<Line*> gridLines;
	for (int x = 0; x < windowX; x += BLOCK_SIZE) {
		gridLines.push_back(renderer.drawLine(x, 0, x - (windowY * 2 + 1), windowY, BLACK));
		gridLines.push_back(renderer.drawLine(x + 2, 0, x + (windowY * 2 + 1), windowY, BLACK));
	}
	for (int y = (BLOCK_SIZE / 2) - 1; y < windowY; y += (BLOCK_SIZE / 2)) {
		if (windowX % 2 == 0) {
			gridLines.push_back(renderer.drawLine(0, y, windowX + 1, y + (windowX / 2 + 1), BLACK));
		} else {
			gridLines.push_back(renderer.drawLine(0, y, windowX, y + (windowX / 2 + 1), BLACK));
		}
	}
	for (int y = (BLOCK_SIZE / 2) - (((windowX - 1)% 48) / 2 + 1); y < windowY; y += (BLOCK_SIZE / 2)) {
		if (windowX % 2 == 0) {
			gridLines.push_back(renderer.drawLine(windowX, y, 0, y + (windowX / 2), BLACK));
		} else {
			gridLines.push_back(renderer.drawLine(windowX + 1, y, 1, y + (windowX / 2), BLACK));
		}
	}

	//TEMP: Draw something
	float x = 0;
	Texture *tex = renderer.createTexture("res/templateTest.png");
	tex->setBlendMode(ALPHA);
	Image* image = renderer.drawImage(89, 88, tex, 2);
	image->setClip(0, 0, 100, 100, true);

	bool quit = false;
	int i = 0;
	while (!quit) {
		quit = input.inputLoop();

		if (input.isDown("D")) {
			x += 0.25;
			image->setPosition((int)x, 0);
		} else if (input.mouseDown(LeftButton)) {
			x -= 0.25;
			image->setPosition((int)x, 0);
		}


		renderer.renderLoop();
	}

	return 0;
}