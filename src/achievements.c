
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

#include "achievements.h"
#include "drawDirtBackground.h"
#include "defines.h"


void Achievements(void)
{
	int24_t x, y, i, redraw;
	drawDirtBackground(0);
	gfx_SetColor(181);
	gfx_FillCircle(10, 10, 5);
	gfx_FillCircle(309, 10, 5);
	gfx_FillCircle(10, 229, 5);
	gfx_FillCircle(309, 229, 5);
	gfx_FillRectangle(10, 5, 300, 230);
	gfx_FillRectangle(5, 10, 310, 220);
	gfx_SetTextFGColor(0);
	gfx_PrintStringXY("Achievements:", 20, 15);
	gfx_SetTextFGColor(255);
	for (x = 2; x < 18; x++)
	{
		for (y = 2; y < 13; y++)
		{
			gfx_TransparentSprite(sprites[1], x * 16, y * 16);
		}
	}
	gfx_BlitBuffer();

	while (!(os_GetCSC()))
		;
	delay(100);
}