#ifndef RENDERER_HPP
#define RENDERER_HPP
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

struct Color {
	int r;
	int g;
	int b;
	int a;
};

class RenderObject {
	public:
		RenderObject(SDL_Renderer* renderer, std::vector<RenderObject*>* vect) {
			mRenderer = renderer;
			objVect = vect;
			alpha = 255;
			x = 0;
			y = 0;
			srcRect = nullptr;
			destRect = nullptr;
		};

		int alpha;
		int x;
		int y;

		virtual void draw() { std::cout << "TEST" << std::endl; };

		void setZIndex(int z) {
			zIndex = z;
			//TODO: sort array
		};
		int getZIndex() {return zIndex;};
		void setIndex(int i) {index = i;};
		SDL_Renderer* getRenderObj() { return mRenderer; };
	private:
		int zIndex;
		int index;
		SDL_Rect* srcRect;
		SDL_Rect* destRect;
		SDL_Renderer* mRenderer;
		std::vector<RenderObject*>* objVect;
};

class Line : public RenderObject {
	public:
		int x1;
		int y1;
		int x2;
		int y2;
		Color color;

		Line(int firstX, int firstY, int secondX, int secondY, int z, Color drawColor, SDL_Renderer* render, std::vector<RenderObject*>* vect) : RenderObject(render, vect) {
			x1 = firstX;
			y1 = firstY;
			x2 = secondX;
			y2 = secondY;
			color = drawColor;
			x = ((firstX > secondX) ? secondX : firstX);
			y = ((firstY > secondY) ? secondY : firstY);
		}

		void draw() {
			SDL_Renderer* mRenderer = getRenderObj();
			SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawLine(mRenderer, x1, y1, x2, y2);
		}
};

class Renderer {
	public:
		Renderer(int w, int h, std::string windowName);
		~Renderer();

		int screenW;
		int screenH;
		bool status;
		Color bgColor;

		bool renderLoop();
		Line *drawLine(int x1, int y1, int x2, int y2, Color color, int z = 0);
		void free();
		std::vector<RenderObject*> renderObjects;
	private:
		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
};

Renderer::Renderer(int w, int h, std::string windowName) : renderObjects(0, NULL) {
	status = true;
	screenW = w;
	screenH = h;
	bgColor.r = 0xFF;
	bgColor.g = 0xFF;
	bgColor.b = 0xFF;
	bgColor.a = 0xFF;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Could not initialize SDL! SDL_Error: " << SDL_GetError() << std::endl;
		status = false;
	}

	mWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (mWindow == NULL) {
		std::cout << "Could not create window! SDL_Error: " << SDL_GetError() << std::endl;
		status = false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	if (mRenderer == NULL) {
		std::cout << "Could not create renderer! SDL_Error: " << SDL_GetError() << std::endl;
		status = false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cout << "Could not initialize SDL_image! SDL_image_Error: " << IMG_GetError() << std::endl;
		status = false;
	}

	SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

Renderer::~Renderer() {
	free();
}

bool Renderer::renderLoop() {
	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			quit = true;
		}
	}
	SDL_SetRenderDrawColor(mRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderClear(mRenderer);
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i]->draw();
	}
	SDL_RenderPresent(mRenderer);

	return quit;
}

Line *Renderer::drawLine(int x1, int y1, int x2, int y2, Color color, int z) {
	Line* line = new Line(x1, y1, x2, y2, z, color, mRenderer, &renderObjects);

	int insertId = 0;
	bool foundLarger = false;
	for (int i = 0; i < renderObjects.size(); i++) {
		if (renderObjects[i]->getZIndex() > z) {
			insertId = i;
			foundLarger = true;
			break;
		}
	}
	if (foundLarger) {
		auto iterator = renderObjects.begin() + insertId;
		renderObjects.insert(iterator, line);
		line->setIndex(insertId);
	} else {
		renderObjects.push_back(line);
	}

	return line;
}

void Renderer::free() {
	for (int i = 0; i < renderObjects.size(); i++) {
		delete renderObjects[i];
	}

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mRenderer = NULL;
	mWindow = NULL;

	IMG_Quit();
	SDL_Quit();
}

#endif