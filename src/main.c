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

	//define the world data list
	static char WorldData[200 * 200] = { 0 };
	
	//hopefully a static 'y' will fix some menu bugs...
	static uint24_t x, y, pos;
	//{ name of world 1, name of world 2, etc. }
	static char WorldsList[20] = { 0 };
	//{ name of server 1, name of server 2, etc. }
	static char Servers[20] = { 0 };
	static char FriendsList[20] = { 0 };

	//handle these annoying statics later...
	static uint24_t timeofday;
	static uint24_t playerX;
	static uint24_t playerY;
	static uint24_t curX;
	static uint24_t curY;
	static uint24_t curPos;
	static uint24_t hotbar[5] = { 0 };
	static uint24_t hotbarCur;
	
    gfx_TempSprite(logo, 16, 16);

void main(void);
void LoadBlocks(void);
void DrawMenu(void);
void PlayMenu(void);
void Achievements(void);
void WorldEngine(void);


void main(void) {
	gfx_Begin();
    gfx_SetPalette(xlibc, sizeof_xlibc, 0);
	gfx_SetClipRegion(0-16, 0-16, 336, 256);
	gfx_SetDrawBuffer();
	LoadBlocks();
	ti_CloseAll();
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_CloseAll();

  	DrawMenu();
		
		kb_Scan();
		if (y == 125) {
			PlayMenu();
				//generator...blah, blah, blah, yada yada yadda...
				for (x = 0; x < 20; x++) {
					for (y = 0; y < 15; y++) {
						gfx_TransparentSprite_NoClip(sprites[1], x * 16, y * 16);
					}
				}
				gfx_SetTextFGColor(254);
				gfx_PrintStringXY("Generating World", 102, 90);
				gfx_PrintStringXY("Building Terrain", 104, 104);
				gfx_SetColor(4);
				gfx_FillRectangle(90, 120, 320-180, 7);
				gfx_SetColor(0);
				gfx_Rectangle(90, 120, 320-180, 7);
				gfx_SetColor(6);
				gfx_BlitBuffer();
				for(pos = 799; pos < 40000; pos++) {
					//grass
					if ((pos > 999) && (pos < 1200)) WorldData[pos] = 1;
					//dirt
					if (pos > 1199) WorldData[pos] = 2;
					
				}
				for (x = 91; x < 320-92; x++) {
					//green progress bar... for looks at this point
					gfx_VertLine(x, 121, 5);
					delay(20);
					gfx_BlitBuffer();
				}
				
			delay(200);
			kb_Scan();
			WorldEngine();
		}
		if (y == 150) {
			Achievements();
			DrawMenu();
		}

		if (y == 175) {
				/* Game Settings */


		}
	gfx_End();

	os_ClrHome();
}

void DrawMenu(void) {

	uint24_t CursorY, x = 60, i, option, test, scroll = 16, scrollY, redraw = 1, timer = 0;
	y = 125;
	i = y;
	gfx_SetTransparentColor(255);
	while (!(kb_IsDown(kb_Key2nd))) {
        kb_Scan();
		if (redraw == 1) {
			for (test = 0; test < 20; test++) {
				for (scrollY = scroll; scrollY < 256; scrollY += 16) {
					gfx_TransparentSprite_NoClip(sprites[1], test * 16, scrollY - 16);
				}
			}
			gfx_SetTextFGColor(230);
			gfx_PrintStringXY("DEV_ALPHA v1.0.001", 3, 230);
			gfx_SetTextFGColor(0);
			gfx_ScaledTransparentSprite_NoClip(logo, 32, 20, 2, 2);
			/* buttons */
			gfx_SetColor(181);
			gfx_FillRectangle(60, 125, 192, 20);
			gfx_FillRectangle(60, 150, 192, 20);
			gfx_FillRectangle(60, 175, 192, 20);
			gfx_FillRectangle(60, 200, 192, 20);
			gfx_SetColor(140);
			/* redraw only the one button that needs it */
			gfx_SetColor(181);
			gfx_FillRectangle(60, i, 192, 20);
			/* button text */
			gfx_PrintStringXY("Play", 144, 130);
			gfx_PrintStringXY("Achievements", 116, 155);
			gfx_PrintStringXY("Settings", 130, 180);
			gfx_PrintStringXY("Quit", 144, 205);
        i = y;
		redraw = 0;
	}
		redraw = 1;
		timer = 0;
		scroll--;
		if (scroll < 1) scroll = 16;
		gfx_SetColor(0);
		gfx_Rectangle(60, y, 192, 20);
		gfx_Rectangle(61, y + 1, 190, 18);
        gfx_BlitBuffer();
		if (kb_IsDown(kb_KeyClear)) return;
        if (kb_IsDown(kb_KeyUp) && y > 125) {
            //delay(150);
			redraw = 1;
            y -= 25;
        }
        if (kb_IsDown(kb_KeyDown) && y < 200) {
            //delay(150);
			redraw = 1;
            y += 25;
        }
        if (kb_IsDown(kb_Key2nd))                   option = 1;
		gfx_BlitBuffer();
    }

	gfx_SetTransparentColor(252);

    return;
  
}

void PlayMenu(void) {

	uint24_t tab, CursorY, x, i, redraw, option, pos;
	for (x = 0; x < 20; x++) {
		for (y = 0; y < 15; y++) {
			gfx_TransparentSprite_NoClip(sprites[1], x * 16, y * 16);
		}
	}
	gfx_SetTransparentColor(255);
	tab = 0;
	CursorY = 40;
	redraw = 1;
		kb_Scan();
	while (!(kb_IsDown(kb_Key2nd))) {
		kb_Scan();
		if (redraw == 1) {
			gfx_SetColor(181);
			gfx_FillRectangle(20, 20, 280, 200);
			gfx_SetColor(0);
			gfx_Rectangle(20, 20, 280, 200);
			gfx_Rectangle(20, 20, 280, 20);
			gfx_SetColor(148);
			gfx_FillRectangle_NoClip(21 + (tab * 89), 21, 100, 18);
			gfx_PrintStringXY("My Worlds", 40, 25);
			gfx_PrintStringXY("Servers", 134, 25);
			gfx_PrintStringXY("Friends", 240, 25);
			redraw = 0;
		}

		if (kb_IsDown(kb_KeyLeft) && (tab > 0)) {
			delay(200);
			tab--;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyRight) && (tab < 2)) {
			delay(200);
			tab++;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyClear)) DrawMenu();

		gfx_BlitBuffer();

	}

}
void Achievements(void) {

	uint24_t CursorY, x, y, i, redraw, option;
	for (x = 0; x < 20; x++) {
		for (y = 0; y < 15; y++) {
			gfx_TransparentSprite_NoClip(sprites[1], x * 16, y * 16);
		}
	}
	gfx_SetColor(181);
	gfx_FillCircle_NoClip(10, 10, 5);
	gfx_FillCircle_NoClip(309, 10, 5);
	gfx_FillCircle_NoClip(10, 229, 5);
	gfx_FillCircle_NoClip(309, 229, 5);
	gfx_FillRectangle(10, 5, 300, 230);
	gfx_FillRectangle(5, 10, 310, 220);
	gfx_SetTextFGColor(0);
	gfx_PrintStringXY("Achievements:", 20, 15);
	gfx_SetTextFGColor(255);
	for (x = 2; x < 18; x++) {
		for (y = 2; y < 13; y++) {
			gfx_TransparentSprite_NoClip(sprites[10], x * 16, y * 16);
		}
	}
	
	gfx_BlitBuffer();

	while (!(os_GetCSC()));

}

void WorldEngine(void) {

	uint24_t playerX, playerY, curX, curY;
	gfx_SetDrawBuffer();
	timeofday = 0;
	gfx_FillScreen(191);
	
		//draw the world and player sprites, as well as the player cursor... (none of which exist just yet)

		while (!(kb_IsDown(kb_KeyClear))) {
			kb_Scan();

			gfx_BlitBuffer();

		}

		//save the world data, playerX, playerY, curPos, curX, curY, timeofday, etc...

	delay(100);
	DrawMenu();

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
