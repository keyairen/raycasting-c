#pragma once
#include <stdint.h>
#include "constants.h"
#include "upng.h"

#define NUM_TEXTURES 13

upng_t* textures[NUM_TEXTURES];

void loadTextures(void);
void freeTextures(void);