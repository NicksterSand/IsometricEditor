#ifndef RENDERER_HPP
#define RENDERER_HPP
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

enum BlendMode{NONE = SDL_BLENDMODE_NONE, ALPHA = SDL_BLENDMODE_BLEND, ADDITIVE = SDL_BLENDMODE_ADD, MULTIPLY = SDL_BLENDMODE_MOD};

struct Color {
	int r;
	int g;
	int b;
	int a;
};

struct Clip {
	int x;
	int y;
	int w;
	int h;
};

class Texture {
	public:
		Texture(SDL_Renderer* renderer, std::string path) {
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
			mRenderer = renderer;
			if (!loadFromFile(path)) {
				std::cout << "Failed to load image '" << path << "'!" << std::endl;
			};
		};
		~Texture() { free(); };
		bool loadFromFile(std::string path) {
			free();
			SDL_Texture* newTexture = NULL;

			SDL_Surface* loadedSurface = IMG_Load(path.c_str());
			if (loadedSurface == NULL) {
				std::cout << "Unable to load image '" << path << "'! SDLImageError: " << IMG_GetError() << std::endl;
			} else {
				//SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0x00, 0x00));
				newTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
				if (newTexture == NULL) {
					std::cout << "Could not create texture from '" << path << "'! SDL Error: " << SDL_GetError() << std::endl;
				} else {
					mWidth = loadedSurface->w;
					mHeight = loadedSurface->h;
				}
				SDL_FreeSurface(loadedSurface);
			}
			mTexture = newTexture;
			return mTexture != NULL;
		}
		void free() {
			if (mTexture != NULL) {
				SDL_DestroyTexture(mTexture);
				mTexture = NULL;
				mWidth = 0;
				mHeight = 0;
			}
		}
		void setBlendMode(BlendMode blending) { SDL_SetTextureBlendMode(mTexture, (SDL_BlendMode)blending); };
		void setAlpha(Uint8 alpha) { SDL_SetTextureAlphaMod(mTexture, alpha); };
		int getWidth() { return mWidth; };
		int getHeight() { return mHeight; };
		SDL_Texture* getTextureObject() {
			return mTexture;
		}
	private:
		SDL_Renderer* mRenderer;
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

class RenderObject {
	public:
		RenderObject(SDL_Renderer* renderer, std::vector<RenderObject*>* vect) {
			mRenderer = renderer;
			objVect = vect;
			alpha = 255;
			x = 0;
			y = 0;
			zIndex = 0;
			index = -1;
			srcRect = nullptr;
			destRect = nullptr;
		};

		int alpha;
		int x;
		int y;

		virtual void draw(SDL_Renderer* renderer) = 0;

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

		void draw(SDL_Renderer* renderer) {
			//SDL_Renderer* renderer = getRenderObj();
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}
};

class Image : public RenderObject {
	public:
		Image(Texture* tex, SDL_Renderer* render, std::vector<RenderObject*>* vect, int ox = 0, int oy = 0) : RenderObject(render, vect) {
			texture = tex;
			sizeX = texture->getWidth();
			sizeY = texture->getHeight();
			originX = ox;
			originY = oy;
			SDL_Rect* sRect = new SDL_Rect{ 0, 0, texture->getWidth(), texture->getHeight() };
			srcRect = sRect;
			SDL_Rect *dRect = new SDL_Rect{x - originX, y - originY, texture->getWidth(), texture->getHeight() };
			dstRect = dRect;
		};
		~Image() {free();};
		Texture* texture;
		void draw(SDL_Renderer *renderer) {
			//SDL_Renderer* renderer = getRenderObj();
			if (SDL_RenderCopy(renderer, texture->getTextureObject(), srcRect, dstRect) != 0) {
				std::cout << "Error drawing image, SDL_Error: " << SDL_GetError() << std::endl;
			}
		}
		void setScale(float x, float y) {
			sizeX = (int)(x * texture->getWidth());
			sizeY = (int)(y * texture->getHeight());
			dstRect->w = sizeX;
			dstRect->h = sizeY;
		};
		void setSize(int x, int y) {
			sizeX = x;
			sizeY = y;
			dstRect->w = sizeX;
			dstRect->h = sizeY;
		}
		void setPosition(int posX, int posY) {
			x = posX;
			y = posY;
			dstRect->x = x - originX;
			dstRect->y = y - originY;
		}
		void setClip(int xClip, int yClip, int width, int height, bool rescale) {
			srcRect->x = xClip;
			srcRect->y = yClip;
			srcRect->w = width;
			srcRect->h = height;
			if (rescale) {
				setSize(width, height);
			}
		}
		void setClip(Clip clip, bool rescale) {
			srcRect->x = clip.x;
			srcRect->y = clip.y;
			srcRect->w = clip.w;
			srcRect->h = clip.h;
			if (rescale) {
				setSize(clip.w, clip.h);
			}
		}
		void setOrigin(int ox, int oy) {
			originX = ox;
			originY = oy;
			dstRect->x = x + originX;
			dstRect->y = y + originY;
		}
		void free() {
			delete dstRect;
		}
	private:
		int sizeX;
		int sizeY;
		int originX;
		int originY;
		SDL_Rect* srcRect;
		SDL_Rect* dstRect;
};

class Renderer {
	public:
		Renderer(int w, int h, std::string windowName);
		~Renderer();

		int screenW;
		int screenH;
		bool status;
		Color bgColor;

		void renderLoop();
		Line *drawLine(int x1, int y1, int x2, int y2, Color color, int z = 0);
		Image *drawImage(int x, int y, Texture* tex, int z = 0, float scaleX = 1, float scaleY = 1, int ox = 0, int oy = 0);
		Texture *createTexture(std::string path);
		void free();
		std::vector<RenderObject*> renderObjects;
	private:
		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
		std::vector<Texture*> textures;
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

	//SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

Renderer::~Renderer() {
	free();
}

void Renderer::renderLoop() {
	SDL_SetRenderDrawColor(mRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderClear(mRenderer);
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i]->draw(mRenderer);
	}
	SDL_RenderPresent(mRenderer);
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

Image* Renderer::drawImage(int x, int y, Texture* tex, int z, float scaleX, float scaleY, int ox, int oy) {
	Image* image = new Image(tex, mRenderer, &renderObjects, ox, oy);
	image->setPosition(x, y);

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
		renderObjects.insert(iterator, image);
		image->setIndex(insertId);
	} else {
		renderObjects.push_back(image);
	}

	return image;
}

Texture *Renderer::createTexture(std::string path) {
	Texture *texture = new Texture(mRenderer, path);
	textures.push_back(texture);
	return texture;
}

void Renderer::free() {
	for (int i = 0; i < renderObjects.size(); i++) {
		delete renderObjects[i];
	}

	for (int i = 0; i < textures.size(); i++) {
		delete textures[i];
	}

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mRenderer = NULL;
	mWindow = NULL;

	IMG_Quit();
	SDL_Quit();
}

#endif