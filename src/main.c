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

void main(void);
void LoadBlocks(void);
void DrawMenu(void);
	
	/*
	//ptr points to sprite data after the header
	//len is length of sprite data
	//sprites points to sprite pointers. Make sure it is big enough to hold the sprites.
int loadTextures(void *ptr, int len, gfx_sprite_t **sprites) {
    int num = 0;
    int offset = 0;
    while (offset < len) {
        sprites[num++] = ptr + offset;
        offset += 16*16+2;
    }
    return num;
}
*/

//first, run LoadTextures(), then, draw the main menu...

void main(void) {
    uint24_t num = 0;
	uint24_t offset;
	int x, y;
	gfx_Begin();
    gfx_SetPalette(xlibc, sizeof_xlibc, 0);
	gfx_SetDrawBuffer();
	ti_CloseAll();
	appvar = ti_Open("MCCESPR", "r");
	dbg_sprintf(dbgout, "appvar is %p\n", appvar);
	ptr = ti_GetDataPtr(appvar);
	ti_CloseAll();
	offset = 3;
		while (num < 64) {
			sprites[num] = (gfx_sprite_t*)(ptr + offset);
			num++;
			offset += (16 * 16) + 2;
		}
  	for (x = 0; x < 20; x++) {
  		for (y = 0; y < 15; y++) {
    		gfx_TransparentSprite_NoClip(sprites[6], x * 16, y * 16);
		}
    	gfx_BlitBuffer();
  	}

	//dbg_sprintf(dbgout, "ptr is %p\n", ptr);
  	DrawMenu();
	
	gfx_End();
}

void DrawMenu(void) {
	int CursorY;
  
	while (!(kb_IsDown(kb_Key2nd))) {  
  		kb_Scan();
  	
  	
    	gfx_BlitBuffer();
  	}
  
}
