
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

#include <srldrvce.h>
#include <usbdrvce.h>

#include <debug.h>

#include "drawDirtBackground.h"
#include "defines.h"


void drawDirtBackground(int scroll)
{
	int scrollY, test;
	for (test = 0; test < 20; test++)
	{
		for (scrollY = scroll; scrollY < 256; scrollY += 16)
		{
			gfx_TransparentSprite(sprites[3], test * 16, scrollY - 16);
		}
	}
}
