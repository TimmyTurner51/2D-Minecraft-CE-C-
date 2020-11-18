////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE (C)
//  Authors: TimmyTurner51, LogicalJoe, and Beckadamtheinventor
//  License: ??
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

#define usb_callback_data_t usb_device_t

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

#include <srldrvce.h>
#include <usbdrvce.h>

#include "gfx/gfx.h"


	static ti_var_t appvar;
	gfx_sprite_t *sprites[64];
	gfx_sprite_t *ptr;
	
    gfx_TempSprite(logo, 16, 16);

void main(void);
void LoadBlocks(void);
void DrawMenu(void);
void Singleplayer(void);
void Multiplayer(void);


void main(void) {
	gfx_Begin();
    gfx_SetPalette(xlibc, sizeof_xlibc, 0);
	gfx_SetTransparentColor(255);
	gfx_SetDrawBuffer();
	LoadBlocks();
  	DrawMenu();
	gfx_SetTransparentColor(252);
	
	gfx_End();
}

void DrawMenu(void) {
	uint24_t CursorY, x, y, i, redraw, option, test;
	ti_CloseAll();
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_CloseAll();
	for (x = 0; x < 20; x++) {
		for (y = 0; y < 15; y++) {
			gfx_TransparentSprite_NoClip(sprites[1], x * 16, y * 16);
		}
	}
	gfx_ScaledTransparentSprite_NoClip(logo, 32, 10, 2, 2);
    /* buttons */
	gfx_SetColor(181);
	gfx_FillRectangle(60, 140, 192, 16);
	gfx_FillRectangle(60, 160, 192, 16);
	/* to be used later when we update the main menu (to look "prettier" maybe)
	gfx_FillRectangle(60, 180, 70, 16);
	gfx_FillRectangle(182, 180, 70, 16);
	*/
	gfx_FillRectangle(60, 180, 192, 16);
	gfx_FillRectangle(60, 200, 192, 16);
	redraw = 1;
	x = 60;
	y = 140;
	i = y;
	while (!(kb_IsDown(kb_Key2nd))) {
        gfx_SetColor(140);
        kb_Scan();
			if (redraw == 1) {
				redraw = 0;
				/* redraw only the one button that needs it */
				gfx_SetColor(181);
				gfx_FillRectangle(60, i, 192, 16);
				/* button text */
				gfx_PrintStringXY("Singleplayer", 119, 142);
				gfx_PrintStringXY("Multiplayer", 122, 162);
				gfx_PrintStringXY("Options", 132, 182);
				gfx_PrintStringXY("Quit", 143, 202);
			}
        i = y;
		gfx_SetColor(0);
		gfx_Rectangle(60, y, 192, 16);
		gfx_Rectangle(61, y + 1, 190, 14);
        gfx_BlitBuffer();
        if (kb_IsDown(kb_KeyUp) && y > 140) {
            delay(150);
			redraw = 1;
            y -= 20;
        }
        if (kb_IsDown(kb_KeyDown) && y < 200) {
            delay(150);
			redraw = 1;
            y += 20;
        }
        if (kb_IsDown(kb_Key2nd))                   option = 1;
    }
    if (y == 140) Singleplayer();
    if (y == 160) Multiplayer();
    return;
  
}

void Singleplayer(void) {

}
void Multiplayer(void) {

}


void LoadBlocks(void) {
	
    int num = 0;
	int offset, size;
	int x, y;
	offset = 1;
	ti_CloseAll();
	appvar = ti_Open("MCCESPR", "r");
	ptr = ti_GetDataPtr(appvar);
	size = ti_GetSize(appvar) - offset;
	ti_CloseAll();

		while (num < 64) {
			sprites[num] = (gfx_sprite_t*)(ptr + offset);
			num++;
			offset += 86;
		}

}