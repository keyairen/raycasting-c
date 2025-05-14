#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <stdlib.h>
#include "constants.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "wall.h"
#include "sprite.h"
#include "textures.h"

bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(void) {
	loadTextures();
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

void render(void) {
	clearColorBuffer(0xFF000000);

	// Render the walls and the sprites
	renderWallProjection();
	renderSpriteProjection();

	// Render the minimap objects
	renderMapGrid();
	renderMapRays();
	renderMapSprites();
	renderMapPlayer();

	renderColorBuffer();
}

void releaseResources(void) {
	freeTextures();
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

	return EXIT_SUCCESS;
}
