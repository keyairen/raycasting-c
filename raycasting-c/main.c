#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <SDL.h>
#include "constants.h"
#include "textures.h"
#include "graphics.h"
#include "map.h"

struct Player {
	float x;
	float y;
	float width;
	float height;
	int turnDirection; // -1 for left, +1 for right
	int walkDirection; // -1 for back, +1 for front
	float rotationAngle;
	float walkSpeed;
	float turnSpeed;
} player;

struct Ray {
	float rayAngle;
	float wallHitX;
	float wallHitY;
	float distance;
	bool wasHitVertical;
	int wallHitContent;
} rays[NUM_RAYS];


bool isGameRunning = false;
int ticksLastFrame = 0;

void setup(void) {
	player.x = WINDOW_WIDTH / 2;
	player.y = WINDOW_HEIGHT / 2;
	player.width = 1;
	player.height = 1;
	player.turnDirection = 0;
	player.walkDirection = 0;
	player.rotationAngle = PI / 2;
	player.walkSpeed = 100;
	player.turnSpeed = 45 * (PI / 180);

	// asks upng library to decode all png files and load the wall textures array
	loadWallTextures();
}

void movePlayer(float deltaTime) {
	player.rotationAngle += player.turnDirection * player.turnSpeed * deltaTime;
	float moveStep = player.walkDirection * player.walkSpeed * deltaTime;

	float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
	float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

	if (!mapHasWallAt(newPlayerX, newPlayerY)) {
		player.x = newPlayerX;
		player.y = newPlayerY;
	}
}

void renderPlayer(void) {
	//SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	//SDL_Rect playerRect = {
	//	player.x * MINIMAP_SCALE_FACTOR,
	//	player.y * MINIMAP_SCALE_FACTOR,
	//	player.width * MINIMAP_SCALE_FACTOR,
	//	player.height * MINIMAP_SCALE_FACTOR
	//};
	//SDL_RenderFillRect(renderer, &playerRect);

	//SDL_RenderDrawLine(
	//	renderer,
	//	MINIMAP_SCALE_FACTOR * player.x,
	//	MINIMAP_SCALE_FACTOR * player.y,
	//	MINIMAP_SCALE_FACTOR * player.x + cos(player.rotationAngle) * 40,
	//	MINIMAP_SCALE_FACTOR * player.y + sin(player.rotationAngle) * 40
	//);
}

float normalizeAngle(float angle) {
	angle = remainder(angle, TWO_PI);
	if (angle < 0) {
		angle += TWO_PI;
	}
	return angle;
}

float distanceBetweenPoints(float x1, float y1, float x2, float y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void castRay(float rayAngle, int stripId) {
	rayAngle = normalizeAngle(rayAngle);

	int isRayFacingDown = rayAngle > 0 && rayAngle < PI;
	int isRayFacingUp = !isRayFacingDown;
	
	int isRayFacingRight = rayAngle < 0.5 * PI || rayAngle > 1.5 * PI;
	int isRayFacingLeft = !isRayFacingRight;

	float xintercept, yintercept;
	float xstep, ystep;

	//////////////////////////////////////////
	// Horizontal Ray-Grid Intersection Code
	/////////////////////////////////////////
	int foundHorzWallHit = false;
	float horzWallHitX = 0;
	float horzWallHitY = 0;
	int horzWallContent = 0;

	// Find the y-coordinate of the closest horizontal grid intersection
	yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
	yintercept += isRayFacingDown ? TILE_SIZE : 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	xintercept = player.x + (yintercept - player.y) / tan(rayAngle);

	// Calculate the increment xstep and ystep
	ystep = TILE_SIZE;
	ystep *= isRayFacingUp ? -1 : 1;

	xstep = TILE_SIZE / tan(rayAngle);
	xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
	xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

	float nextHorzTouchX = xintercept;
	float nextHorzTouchY = yintercept;

	//increment xstep and ystep until we find a wall
	while (isInsideMap(nextHorzTouchX, nextHorzTouchY)) {
		float xToCheck = nextHorzTouchX;
		float yToCheck = nextHorzTouchY + (isRayFacingUp ? -1 : 0);

		if (mapHasWallAt(xToCheck, yToCheck)) {
			horzWallHitX = nextHorzTouchX;
			horzWallHitY = nextHorzTouchY;
			horzWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
			foundHorzWallHit = true;
			break;
		}
		else {
			nextHorzTouchX += xstep;
			nextHorzTouchY += ystep;
		}
	}

	//////////////////////////////////////////
	// Vertical Ray-Grid Intersection Code
	/////////////////////////////////////////
	int foundVertWallHit = false;
	float vertWallHitX = 0;
	float vertWallHitY = 0;
	int vertWallContent = 0;

	// Find the x-coordinate of the closest horizontal grid intersection
	xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
	xintercept += isRayFacingRight ? TILE_SIZE : 0;

	// Find the y-coordinate of the closest horizontal grid intersection
	yintercept = player.y + (xintercept - player.x) * tan(rayAngle);

	// Calculate the increment xstep and ystep
	xstep = TILE_SIZE;
	xstep *= isRayFacingLeft ? -1 : 1;

	ystep = TILE_SIZE * tan(rayAngle);
	ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
	ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

	float nextVertTouchX = xintercept;
	float nextVertTouchY = yintercept;

	//increment xstep and ystep until we find a wall
	while (isInsideMap(nextVertTouchX, nextVertTouchY)) {
		float xToCheck = nextVertTouchX + (isRayFacingLeft ? -1 : 0);
		float yToCheck = nextVertTouchY;

		if (mapHasWallAt(xToCheck, yToCheck)) {
			vertWallHitX = nextVertTouchX;
			vertWallHitY = nextVertTouchY;
			vertWallContent = getMapAt((int)floor(yToCheck / TILE_SIZE), (int)floor(xToCheck / TILE_SIZE));
			foundVertWallHit = true;
			break;
		}
		else {
			nextVertTouchX += xstep;
			nextVertTouchY += ystep;
		}
	}

	// Calculate both horizontal and vertical hit distances and choose the smallest one
	float horzHitDistance = foundHorzWallHit
		? distanceBetweenPoints(player.x, player.y, horzWallHitX, horzWallHitY)
		: FLT_MAX;
	float vertHitDistance = foundVertWallHit
		? distanceBetweenPoints(player.x, player.y, vertWallHitX, vertWallHitY)
		: FLT_MAX;
	
	if (vertHitDistance < horzHitDistance) {
		rays[stripId].distance = vertHitDistance;
		rays[stripId].wallHitX = vertWallHitX;
		rays[stripId].wallHitY = vertWallHitY;
		rays[stripId].wallHitContent = vertWallContent;
		rays[stripId].wasHitVertical = true;
		rays[stripId].rayAngle = rayAngle;
	}
	else {
		rays[stripId].distance = horzHitDistance;
		rays[stripId].wallHitX = horzWallHitX;
		rays[stripId].wallHitY = horzWallHitY;
		rays[stripId].wallHitContent = horzWallContent;
		rays[stripId].wasHitVertical = false;
		rays[stripId].rayAngle = rayAngle;
	}	
}

void castAllRays(void) {
	for (int col = 0; col < NUM_RAYS; col++) {
		float rayAngle = player.rotationAngle + atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
		castRay(rayAngle, col);
	}
}

void renderRays(void) {
	//SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	//for (int i = 0; i < NUM_RAYS; i++) {
	//	SDL_RenderDrawLineF(
	//		renderer,
	//		MINIMAP_SCALE_FACTOR * player.x,
	//		MINIMAP_SCALE_FACTOR * player.y,
	//		MINIMAP_SCALE_FACTOR * rays[i].wallHitX,
	//		MINIMAP_SCALE_FACTOR * rays[i].wallHitY
	//	);
	//}
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
	//renderPlayer();

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
