#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"
#include "ray.h"
#include "player.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(void) {

	// asks upng library to decode all png files and load the wall textures array
	loadWallTextures();
}

void processInput(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
		case SDL_QUIT: {
			isGameRunning = false;
			break;
		}
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				isGameRunning = false;
				break;
			}
			if (event.key.keysym.sym == SDLK_w) {
				player.walkDirection = +1;
				break;
			}
			if (event.key.keysym.sym == SDLK_s) {
				player.walkDirection = -1;
				break;
			}
			if (event.key.keysym.sym == SDLK_d) {
				player.turnDirection = +1;
				break;
			}
			if (event.key.keysym.sym == SDLK_a) {
				player.turnDirection = -1;
				break;
			}
		}
		case SDL_KEYUP: {
			if (event.key.keysym.sym == SDLK_w) {
				player.walkDirection = 0;
				break;
			}
			if (event.key.keysym.sym == SDLK_s) {
				player.walkDirection = 0;
				break;
			}
			if (event.key.keysym.sym == SDLK_d) {
				player.turnDirection = 0;
				break;
			}
			if (event.key.keysym.sym == SDLK_a) {
				player.turnDirection = 0;
				break;
			}
		}
	}
}

void update(void) {
	int timeToWait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticksLastFrame);

	if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGTH) {
		SDL_Delay(timeToWait);
	}

	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

	ticksLastFrame = SDL_GetTicks();

	movePlayer(deltaTime);
	castAllRays();
}

void renderWallProjection(void) {
	for (int x = 0; x < NUM_RAYS; x++) {
		float perpDistance = rays[x].distance * cos(rays[x].rayAngle - player.rotationAngle);
		float projectedWallHeight = (TILE_SIZE / perpDistance) * DIST_PROJ_PLANE;

		int wallStripHeight = (int)projectedWallHeight;

		int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
		wallTopPixel = wallTopPixel < 0 ? 0 : wallTopPixel;

		int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
		wallBottomPixel = wallBottomPixel > WINDOW_HEIGHT ? WINDOW_HEIGHT : wallBottomPixel;

		// set the color of the ceilling
		for (int y = 0; y < wallTopPixel; y++) {
			drawPixel(x, y, 0xFF444444);
		}

		int textureOffsetX;
		if (rays[x].wasHitVertical) {
			textureOffsetX = (int)rays[x].wallHitY % TILE_SIZE;
		}
		else {
			textureOffsetX = (int)rays[x].wallHitX % TILE_SIZE;
		}

		// get the correct texture id number from the map content
		int textNum = rays[x].wallHitContent - 1;

		int texture_width = wallTextures[textNum].width;
		int texture_height = wallTextures[textNum].height;

		// render the wall from wallTopPixel to wallBottomPixel
		for (int y = wallTopPixel; y < wallBottomPixel; y++) {
			int distanceFromTop = (y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2));
			int textureOffsetY = distanceFromTop * ((float)texture_height / wallStripHeight);

			// set the color of the wall based on the color from the texture
			uint32_t texelColor = wallTextures[textNum].texture_buffer[(texture_width * textureOffsetY) + textureOffsetX];
			drawPixel(x, y, texelColor);
		}

		// set the color of the floor
		for (int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
			drawPixel(x, y, 0xFF888888);
		}

	}
}

void render(void) {
	clearColorBuffer(0xFF000000);

	renderWallProjection();

	renderMap();
	//renderRays();
	renderPlayer();

	renderColorBuffer();
}

void releaseResources(void) {
	freeWallTextures();
	destroyWindow();
}

int main(int argc, char* args[]) {
	isGameRunning = initializeWindow();

	setup();

	while (isGameRunning) {
		processInput();
		update();
		render();
	}

	releaseResources();

	return 0;
}
