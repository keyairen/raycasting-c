#include "sprite.h"

#define NUM_SPRITES 5

static sprite_t sprites[NUM_SPRITES] = {
	{ .x = 640, .y = 630, .texture = 12 }, // armor
	{ .x = 660, .y = 690, .texture = 8 },  // barrel
	{ .x = 250, .y = 600, .texture = 10 }, // table
	{ .x = 250, .y = 600, .texture = 9 },  // table
	{ .x = 300, .y = 400, .texture = 11 }  // guard
};

void renderMapSprites(void) {
	for (int i = 0; i < NUM_SPRITES; i++) {
		drawRect(
			MINIMAP_SCALE_FACTOR * sprites[i].x,
			MINIMAP_SCALE_FACTOR * sprites[i].y,
			2,
			2,
			(sprites[i].visible) ? 0xFF00FFFF : 0xFF444444
			);
	}
}

void renderSpriteProjection(void) {
	sprite_t visibleSprites[NUM_SPRITES];
	int numVisibleSprites = 0;

	// Find sprites that are visible (inside the FOV)
	for (int i = 0; i < NUM_SPRITES; i++) {
		float angleSpritePlayer = player.rotationAngle - atan2(sprites[i].y - player.y, sprites[i].x - player.x);

		// Make sure the angle is always between 0 and 180 degrees
		if (angleSpritePlayer > PI) {
			angleSpritePlayer -= TWO_PI;
		}
		if (angleSpritePlayer < -PI) {
			angleSpritePlayer += TWO_PI;
		}
		angleSpritePlayer = fabs(angleSpritePlayer);

		// If sprite angle is less than half the FOV plus a small error margin
		const float EPSILON = 0.2;
		if (angleSpritePlayer < (FOV_ANGLE / 2) + EPSILON) {
			sprites[i].visible = true;
			sprites[i].angle = angleSpritePlayer;
			sprites[i].distance = distanceBetweenPoints(sprites[i].x, sprites[i].y, player.x, player.y);
			visibleSprites[numVisibleSprites] = sprites[i];
			numVisibleSprites++;
		}
		else {
			sprites[i].visible = false;
		}
	}

	// Sort sprites by distance using bubble-sort
	for (int i = 0; i < numVisibleSprites - 1; i++) {
		for (int j = i + 1; j < numVisibleSprites; j++) {
			if (visibleSprites[i].distance < visibleSprites[j].distance) {
				sprite_t temp = visibleSprites[i];
				visibleSprites[i] = visibleSprites[j];
				visibleSprites[j] = temp;
			}
		}
	}

	// Render the visible sprites
	for (int i = 0; i < numVisibleSprites; i++) {
		sprite_t sprite = visibleSprites[i];

		// Calculate the perpendicular distance of sprite to prevent fish-eye effect
		float perpDistance = sprite.distance * cos(sprite.angle);

		// Calculate the sprite projected height and width
		float spriteHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;
		float spriteWidth = spriteHeight;

		float spriteTopY = (WINDOW_HEIGHT / 2) - (spriteHeight / 2);
		spriteTopY = (spriteTopY < 0) ? 0 : spriteTopY;

		float spriteBottomY = (WINDOW_HEIGHT / 2) + (spriteHeight / 2);
		spriteBottomY = (spriteBottomY > WINDOW_HEIGHT) ? WINDOW_HEIGHT : spriteBottomY;

		// Calculate the sprite X position in the projection plane
		float spriteAngle = atan2(sprite.y - player.y, sprite.x - player.x) - player.rotationAngle;
		float spriteScreenPosX = tan(spriteAngle) * DIST_PROJ_PLANE;

		float spriteLeftX = (WINDOW_WIDTH / 2) + spriteScreenPosX - (spriteWidth / 2);

		float spriteRightX = spriteLeftX + spriteWidth;

		// Query the width and height of the texture
		int textureWidth = upng_get_width(textures[sprite.texture]);
		int textureHeight = upng_get_height(textures[sprite.texture]);


		for (int x = spriteLeftX; x < spriteRightX; x++) {
			float texelWidth = (textureWidth / spriteWidth);
			int textureOffsetX = (x - spriteLeftX) * texelWidth;
			for (int y = spriteTopY; y < spriteBottomY; y++) {
				if (x > 0 && x < WINDOW_WIDTH && y > 0 && y < WINDOW_HEIGHT) {
					int distanceFromTop = y + (spriteHeight / 2) - (WINDOW_HEIGHT / 2);
					int textureOffsetY = distanceFromTop * (textureHeight / spriteHeight);

					color_t* spriteTextureBuffer = (color_t*)upng_get_buffer(textures[sprite.texture]);
					color_t texelColor = spriteTextureBuffer[(textureWidth * textureOffsetY) + textureOffsetX];

					if (sprite.distance < rays[x].distance && texelColor != 0xFFFF00FF) {
						drawPixel(x, y, texelColor);
					}
				}
			}
		}
	}
}