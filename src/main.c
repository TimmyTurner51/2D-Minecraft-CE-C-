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
    uint24_t offset = 3;
	//void *ptr;
	//int numSprites, spriteSize, ptr, n, a;
	//uint24_t sprites[64] = { 0 };
    gfx_SetPalette(xlibc, sizeof_xlibc, 0);
	//wherever you need to load the sprites
	appvar = ti_Open("MCCESPR", "r");
	//loadTextures(ti_GetDataPtr(appvar)+3, ti_GetSize(appvar)-3, sprites); 
	//memcpy(ptr, ti_GetDataPtr(appvar), ti_GetSize(appvar));
	ptr = ti_GetDataPtr(appvar);
    while (offset < ti_GetSize(appvar)) {
		sprites[num] = (gfx_sprite_t*)(ptr + offset);
		num++;
        offset = offset + ((16 * 16) + 2);
    }
	ti_CloseAll();
	gfx_Begin();
	//gfx_SetPalette(myPalette, sizeof_myPalette, 0);

  	DrawMenu();
	
	gfx_End();
}

void DrawMenu(void) {
	int x, y, CursorY;
	gfx_SetDrawBuffer();
	//somewhere global
  	for (x = 0; x < 20; x++) {
  		for (y = 0; y < 15; y++) {
    		gfx_TransparentSprite_NoClip(sprites[4], x * 16, y * 16);
		}
  	}
  
	while (!(kb_IsDown(kb_Key2nd))) {  
  		kb_Scan();
  	
  	
    	gfx_BlitBuffer();
  	}
  
}
