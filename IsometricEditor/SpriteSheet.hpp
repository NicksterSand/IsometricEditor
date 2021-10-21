#ifndef SPRITE_SHEET_HPP
#define SPRITE_SHEET_HPP
#include "Renderer.hpp"
#include <vector>

class SpriteSheetSprite {
	public:
		SpriteSheetSprite(int x, int y, int w, int h, int ox, int oy) {
			Clip newClip = { x, y, w, h };
			clip = newClip;
			originX = ox;
			originY = oy;
		};
		Clip clip;
		int originX;
		int originY;
};

class SpriteSheet {
	public:
		SpriteSheet(Texture* texture, std::vector<SpriteSheetSprite> tmpSprites) {
			tex = texture;
			sprites = tmpSprites;
		}
		Texture* tex;
		std::vector<SpriteSheetSprite> sprites;
		Image* drawSprite(Renderer* renderer, int spriteId, int x, int y, int z = 0, float scaleX = 1.0, float scaleY = 1.0) {
			Image* image = renderer->drawImage(x, y, tex, z, scaleX, scaleY, sprites[spriteId].originX, sprites[spriteId].originY);
			image->setClip(sprites[spriteId].clip, true);
			if (scaleX != 1.0 || scaleY != 1.0) {
				image->setSize(sprites[spriteId].clip.w * scaleX, sprites[spriteId].clip.h * scaleY);
			}
			return image;
		}
};

#endif