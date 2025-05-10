#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include "constants.h"

bool initializeWindow(void);
void destroyWindow(void);
void clearColorBuffer(uint32_t color);
void renderColorBuffer(void);
void drawPixel(int x, int y, uint32_t color);
void drawRect(int x, int y, int width, int height, uint32_t color);