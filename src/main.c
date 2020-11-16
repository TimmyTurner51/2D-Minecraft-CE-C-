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

	//dbg_sprintf(dbgout, "ptr is %p\n", ptr);

void main(void) {
	gfx_Begin();
    gfx_SetPalette(xlibc, sizeof_xlibc, 0);
	gfx_SetDrawBuffer();
	LoadBlocks();
  	DrawMenu();
	
	gfx_End();
}

void DrawMenu(void) {
	int CursorY, x, y;
	for (x = 0; x < 20; x++) {
		for (y = 0; y < 15; y++) {
			gfx_TransparentSprite_NoClip(sprites[2], x * 16, y * 16);
		}
	}
	while (!(kb_IsDown(kb_Key2nd))) {  
  		kb_Scan();
  	
  	
    	gfx_BlitBuffer();
  	}
  
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
			
			gfx_ScaledTransparentSprite_NoClip(sprites[num], 106, 106, 2, 2);
			gfx_BlitBuffer();
			while (!(os_GetCSC()));
			
			num++;
			offset += (16 * 16) + 2;
		}

}