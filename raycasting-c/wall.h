#pragma once

#include <math.h>
#include "constants.h"
#include "player.h"
#include "ray.h"
#include "graphics.h"
#include "textures.h"

void renderWallProjection(void);
void changeColorIntensity(color_t* color, float factor);