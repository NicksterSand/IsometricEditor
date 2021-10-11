#include "Renderer.hpp"
#include "Input.hpp"
#include "Grid.hpp"
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

	Grid grid(windowX, windowY, BLOCK_SIZE, BLACK, &renderer);
	grid.drawGrid();

	Texture *tex = renderer.createTexture("res/highlight.png");
	tex->setBlendMode(ALPHA);
	Image* highlighter = renderer.drawImage(0, 0, tex, 1);

	bool quit = false;
	int i = 0;
	while (!quit) {
		quit = input.inputLoop();

		Pair highlightedSquare = grid.getSquareFromGridPixel(input.mouseX, input.mouseY);
		Pair highlightPos = grid.getGridPixelFromSquare(highlightedSquare.x, highlightedSquare.y);
		highlighter->setPosition(highlightPos.x, highlightPos.y);

		if (input.isDown("A")) {
			std::cout << input.mouseX << ", " << input.mouseY << std::endl;
			std::cout << highlightedSquare.x << ", " << highlightedSquare.y << std::endl;
		}

		renderer.renderLoop();
	}

	return 0;
}