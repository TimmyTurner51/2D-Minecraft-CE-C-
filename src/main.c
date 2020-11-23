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
	static uint16_t x, y, pos;
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
	gfx_SetClipRegion(0-17, 0-17, 336, 256);
	LoadBlocks();
	ti_CloseAll();
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_CloseAll();

  	DrawMenu();
	
	gfx_End();

	os_ClrHome();
}

void DrawMenu(void) {

	uint24_t CursorY, x, i, option, test, scroll, scrollY, redraw, timer;
	y = 125;
	i = y;
	scroll = 16;
	redraw = 1;
	gfx_SetDrawBuffer();
	gfx_SetTransparentColor(255);
	while (!(kb_IsDown(kb_Key2nd))) {
        kb_Scan();
		if (redraw == 1) {
		redraw = 0;
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
		gfx_BlitBuffer();
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
    }

	gfx_SetTransparentColor(252);

		if (y == 125) {
			PlayMenu();
				//generator...blah, blah, blah, yada yada yadda...
				
				/*
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
				*/

				for(pos = 0; pos < 40000; pos++) {
					WorldData[pos] = 0;
					//grass
					if ((pos > 1199) && (pos < 1400)) WorldData[pos] = 1;
					//dirt
					if ((pos > 1399) && (pos < 1600)) WorldData[pos] = 2;
					//stone
					if (pos > 1599) WorldData[pos] = 11;
				
				}



				/*
				for (x = 91; x < 320-92; x++) {
					//green progress bar... for looks at this point
					gfx_VertLine(x, 121, 5);
					delay(20);
					gfx_BlitBuffer();
				}
				*/

				//here is where we'll make the world's data appvar(s)...
				//we need to save pretty much all vars in the void WorldEngine code, and the world data itself...

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
		if (kb_IsDown(kb_KeyClear)) main();

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

	int24_t redraw, x, playerX, playerY, playerPos, curX, curY, posX, testA, testB, testC, blockSel;

	gfx_SetDrawBuffer();

	gfx_SetTransparentColor(252);

	timeofday = 0;

	curX = 0;
	curY = 0;
	curPos = 1;
	playerX = 0;
	playerY = 0;
	playerPos = 1;
	posX = 0;
	redraw = 1;

	//set selected block to grass block...
	blockSel = 1;
	
		//draw the world and player sprites, as well as the player cursor... (none of which exist just yet)
		
		//gfx_SetClipRegion(-200 * 16, -200 * 16, 200 * 16, 200 * 16);
		while (!(kb_IsDown(kb_KeyClear))) {
			kb_Scan();

			if (redraw == 1) {
				redraw = 0;
				gfx_FillScreen(191);
				testA = 321 + playerX;
				testB = 241 + playerY;
				testC = playerPos;
				//for(x = playerX; x < 320 - testA; x+=16) {
					//for(y = playerY; y < 240 - testB; y+=16) {
				for(y = playerY; y < testB; y+=16) {
					for(x = playerX; x < testA; x+=16) {
						//block behaviors, etc. may go here?...
						if ((x < 321) && (y < 241)) {
							if (WorldData[testC] != 0) gfx_Sprite_NoClip(sprites[WorldData[testC] - 1], x, y);
						}
						testC++;
					}
					testC += 180;
				}
				gfx_SetColor(0);
				gfx_Rectangle_NoClip(curX, curY, 16, 16);
				gfx_Rectangle_NoClip(302, 0, 18, 18);
				if (blockSel != 0) gfx_Sprite_NoClip(sprites[blockSel - 1], 303, 1);
				/*
				gfx_SetTextXY(5, 5);
				gfx_SetTextFGColor(32);
				gfx_PrintInt(playerPos, 1);
				*/
			}

			if (kb_IsDown(kb_Key2nd)) {
				delay(100);
				WorldData[curPos] = blockSel;
				redraw = 1;
			}
			if (kb_IsDown(kb_KeyDel)) {
				delay(100);
				WorldData[curPos] = 0;
				redraw = 1;
			}
			if (kb_IsDown(kb_KeyGraphVar)) {
				delay(100);
				blockSel++;	
				redraw = 1;
			}
			if (blockSel > 64) {
				blockSel = 1;
				redraw = 1;
			}

			if ((kb_IsDown(kb_KeyAlpha)) && (curX > 0)) {
				delay(100);
				curX -= 16;
				curPos--;
				redraw = 1;
			}
			if ((kb_IsDown(kb_KeyStat)) && (curX < 320)) {
				delay(100);
				curX += 16;
				curPos++;
				redraw = 1;
			}
			if ((kb_IsDown(kb_KeyMode)) && (curY > 0)) {
				delay(100);
				curY -= 16;
				curPos -= 201;
				redraw = 1;
			}
			if ((kb_IsDown(kb_KeyApps)) && (curY < 240)) {
				delay(100);
				curY += 16;
				curPos += 201;
				redraw = 1;
			}


			if ((kb_IsDown(kb_KeyLeft)) && (posX > 0) && (playerPos > 0)) {
				redraw = 1;
					if (playerX == 16) {
						posX--;
						playerX = 0;
						playerPos--;
						curPos--;
						curX -= 16;
					}
				playerX++;
				curX++;
			}
			if ((kb_IsDown(kb_KeyRight)) && (posX < 200)) {
				redraw = 1;
					if (playerX == 0) {
						posX++;
						playerX = 16;
						playerPos++;
						curPos++;
						curX += 16;
					}
				playerX--;
				curX--;
			}
			if ((kb_IsDown(kb_KeyUp)) && (playerPos - 201 > 0)) {
				redraw = 1;
					if (playerY == 16) {
						playerY = 0;
						playerPos -= 201;
						curPos -= 201;
						curY -= 16;
					}
				playerY++;
				curY++;
			}
			if ((kb_IsDown(kb_KeyDown)) && (playerPos + 200 < 40000)) {
				redraw = 1;
					if (playerY == 0) {
						playerY = 16;
						playerPos += 201;
						curPos += 201;
						curY += 16;
					}
				playerY--;
				curY--;
			}

			gfx_BlitBuffer();

		}

		//save the world data, playerX, playerY, playerPos, curPos, curX, curY, timeofday, etc...

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
