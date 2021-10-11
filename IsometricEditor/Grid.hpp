#ifndef GRID_HPP
#define GRID_HPP
#include <vector>
#include "Renderer.hpp"

struct Pair {
	int x;
	int y;
};

class Grid {
	public:
		Grid(int x, int y, int size, Color color, Renderer* render) {
			sizeX = x;
			sizeY = y;
			blockSize = size;
			gridColor = color;
			renderer = render;
		};
		int sizeX; //Grid Pixel Size (Without scaling from window)
		int sizeY; 
		int blockSize;
		void drawGrid() {
			for (int x = 0; x < sizeX; x += blockSize) {
				gridLines.push_back(renderer->drawLine(x, 0, x - (sizeY * 2 + 1), sizeY, gridColor));
				gridLines.push_back(renderer->drawLine(x + 2, 0, x + (sizeY * 2 + 1), sizeY, gridColor));
			}
			for (int y = (blockSize / 2) - 1; y < sizeY; y += (blockSize / 2)) {
				if (sizeX % 2 == 0) {
					gridLines.push_back(renderer->drawLine(0, y, sizeX + 1, y + (sizeX / 2 + 1), gridColor));
				} else {
					gridLines.push_back(renderer->drawLine(0, y, sizeX, y + (sizeX / 2 + 1), gridColor));
				}
			}
			for (int y = (blockSize / 2) - (((sizeX - 1) % 48) / 2 + 1); y < sizeY; y += (blockSize / 2)) {
				if (sizeX % 2 == 0) {
					gridLines.push_back(renderer->drawLine(sizeX, y, 0, y + (sizeX / 2), gridColor));
				} else {
					gridLines.push_back(renderer->drawLine(sizeX + 1, y, 1, y + (sizeX / 2), gridColor));
				}
			}
		}
		Pair getSquareFromGridPixel(int x, int y) {
			int modX = (x + 1) % blockSize;
			int modY = (y + 1) % (blockSize / 2);
			int xSection = (int)((x + 1) / blockSize);
			int ySection = (int)((y + 1) / (blockSize / 2));

			int y1Top;
			if (modX < (blockSize / 2) + 2) {
				y1Top = (int)((modX * 0.5) - 1);
			} else {
				y1Top = (int)((modX * -0.5) + (blockSize / 2) + .5);
			}
			if (y1Top < 0) {
				y1Top = 0;
			}
			int y1Bottom = (-1 * y1Top) + 22;
			int gridY;
			if (modY > y1Bottom) {
				gridY = 2 + (2 * ySection);
			} else if (modY < y1Top) {
				gridY = 2 * ySection;
			} else {
				gridY = 1 + (2 * ySection);
			}

			int x1Left;
			if (modY <= (blockSize / 4)) {
				x1Left = (-2 * modY) + 49;
			} else {
				x1Left = (2 * modY) + 1;
			}
			int gridX;
			if (modX < x1Left) {
				gridX = xSection;
			} else {
				gridX = xSection + 1;
			}

			return { gridX, gridY };
		}
		//Returns the top left corner of a square
		Pair getGridPixelFromSquare(int x, int y) {
			int pixelX, pixelY;

			if (y % 2 == 0) {
				pixelX = (x * blockSize) + 1;
				pixelY = ((y - 1) * (blockSize / 4));
			} else {
				pixelX = (x * blockSize) - ((blockSize / 2) - 1);
				pixelY = ((y - 1) * (blockSize / 4));
			}

			return { pixelX, pixelY };
		}
	private:
		std::vector<Line*> gridLines;
		Renderer* renderer;
		Color gridColor;
};

#endif