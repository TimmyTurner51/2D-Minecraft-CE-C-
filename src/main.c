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
#include <fileioc.h>

#include <srldrvce.h>
#include <usbdrvce.h>

//no need for this unless there's actual gfx data that needs to be written to the program :P
//#include "gfx/gfx.h"

#include "compressor.h" //zx7 compression routines

//no longer needed...we have length and height vars now... #define world_ground_height 64

	gfx_sprite_t *sprites[64];
	gfx_sprite_t dummy_sprite = {1,1,0};

	//define the world data list
	char WorldData[200 * 200] = { 0 };
	char *world_file = "MCCESAVE";
	//hopefully a static 'y' will fix some menu bugs...
	int16_t x, y, pos;
	//{ name of world 1, name of world 2, etc. }
	char WorldsList[20] = { 0 };
	//{ name of server 1, name of server 2, etc. }
	char Servers[20] = { 0 };
	char FriendsList[20] = { 0 };

	//handle these annoying statics later...
	int24_t timeofday;
	int24_t playerX;
	int24_t playerY;
	int24_t curX;
	int24_t curY;
	int24_t curPos;
	int24_t hotbar[5] = { 0 };
	int24_t hotbarCur;
	int24_t blockSel;
	bool loaded_world = 0;
    gfx_TempSprite(logo, 16, 16);

void LoadBlocks(const char *appvar); //now handled in an assembly function
void DrawMenu(void);
void PlayMenu(void);
void Achievements(void);
void WorldEngine(void);
void survivalinventory(void);
void creativeInventory(void);
void StringInput(void);

void drawDirtBackground(int scroll); //save space by drawing the dirt backdrop with a function
void compressAndWrite(void *data, int len, ti_var_t fp); //this routine compresses using zx7_Compression and huffman coding

	//these may need to stay static, used in both DrawMenu (for new world setup) and WorldEngine...
	static int24_t worldHeight = 200;
	static int24_t worldLength = 200;
	static int24_t worldType = 0;
	static int24_t hotbarSel = 0;

void main(void) {
	ti_var_t appvar;
	gfx_Begin(); //This sets the default palette, no need to set the palette again
	ti_CloseAll();
	gfx_SetClipRegion( -17, -17, 337, 257);
	LoadBlocks("MCCESPR");
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_Close(appvar);

  	DrawMenu();
	
	ti_CloseAll();
	gfx_End();

	os_ClrHome();
}

void DrawMenu(void) {

	int24_t CursorY, x, i, option, test, scroll, scrollY, redraw, timer, val, tab, worldLength, worldHeight;
	int24_t gamemode, worldSize, cheats;
	char *gamemodeStr[3] = {"Survival", "Creative", "Hardcore"};
	char *worldSizeStr[4] = {"Small", "Medium", "Large", "Infinite"};
	char *cheatsStr[2] = {"Off", "On"};
	char *worldTypesStr[3] = {"Standard", "Superflat", "Large Biomes"};
	char *worldNameStr[1];

	while (1) { //loop this code until this function returns
		y = 125;
		i = y;
		scroll = 16;
		redraw = 1;
		gfx_SetDrawBuffer();
		gfx_SetTransparentColor(255);
		while (!(kb_IsDown(kb_Key2nd))) { //draw the main menu
		    kb_Scan();
			if (redraw == 1) {
				drawDirtBackground(scroll);
				redraw = 0;
				gfx_SetTextFGColor(230);
				gfx_PrintStringXY("DEV_ALPHA v1.0.003", 3, 230);
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
		    if (kb_IsDown(kb_Key2nd)) option = 1;
		}

		gfx_SetTransparentColor(252);

			if (y == 125) { //"play"

				drawDirtBackground(0);
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
						gfx_FillRectangle(21 + (tab * 89), 21, 100, 18);
						gfx_PrintStringXY("My Worlds", 40, 25);
						gfx_PrintStringXY("Servers", 134, 25);
						gfx_PrintStringXY("Friends", 240, 25);
						redraw = 0;
						gfx_BlitBuffer();
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
					if (kb_IsDown(kb_KeyClear)) {
						break;
					}
				}

				CursorY = 80;
				worldNameStr[1] = "My World";
				gamemode = 0;
				worldSize = 0;
				cheats = 0;
				worldType = 0;
				redraw = 1;
				while (!(kb_IsDown(kb_KeyClear))) {
					if (redraw == 1) {
						redraw = 0;
						drawDirtBackground(0);
						for (y = 80; y < 200; y += 20) {
							gfx_SetColor(181);
							gfx_FillRectangle(50, y, 220, 16);
							gfx_SetColor(0);
							gfx_Rectangle(50, y, 220, 16);
							gfx_Rectangle(51, y+1, 218, 14);
						}
						gfx_PrintStringXY("NAME:", 74, 84);
						gfx_PrintStringXY(worldNameStr[1], 114, 84);
						gfx_PrintStringXY("Gamemode:", 74, 104);
						gfx_PrintStringXY(gamemodeStr[gamemode], 144, 104);
						gfx_PrintStringXY("World Size:", 74, 124);
						gfx_PrintStringXY(worldSizeStr[worldSize], 154, 124);
						gfx_PrintStringXY("Cheats:", 74, 144);
						gfx_PrintStringXY(cheatsStr[cheats], 144, 144);
						gfx_PrintStringXY("World Type:", 74, 164);
						gfx_PrintStringXY(worldTypesStr[worldType], 144, 164);
						gfx_PrintStringXY("Generate", 74, 184);
						gfx_SetColor(254);
						gfx_Rectangle(50, CursorY, 220, 16);
						gfx_Rectangle(51, CursorY + 1, 218, 14);
						gfx_BlitBuffer();
					}
					kb_Scan();
					if (kb_IsDown(kb_KeyUp) && (CursorY > 80)) {
						CursorY -= 20;
						redraw = 1;
					}
					if (kb_IsDown(kb_KeyDown) && (CursorY < 180)) {
						CursorY += 20;
						redraw = 1;
					}
					if (kb_IsDown(kb_Key2nd)) {
						redraw = 1;
						if (CursorY == 80) {
							//StringInput();
						}
						if (CursorY == 100) {
							gamemode++;
							if (gamemode > 2) {
								gamemode = 0;
							}
						}
						if (CursorY == 120) {
							worldSize++;
							if (worldSize > 3) {
								worldSize = 0;
							}
						}
						if (CursorY == 140) {
							cheats = (cheats == 0);
						}
						if (CursorY == 160) {
							worldType++;
							if (worldType > 2) {
								worldType = 0;
							}
						}
						if (CursorY == 180) {

							if (worldSize == 0) {
								worldLength = 100;
								worldHeight = 100;
							}
							if (worldSize == 1) {
								worldLength = 200;
								worldHeight = 200;
							}
							if (worldSize == 2) {
								worldLength = 500;
								worldHeight = 200;
							}
							if (worldSize == 3) {
								//infinite...well, not really... :(
								worldLength = 10000;
								worldHeight = 300;
							}

							WorldEngine();
							delay(200);
							redraw = 1;
							break;
						}
					}
					/*
					If KEY!=0 and KEY!=15
						Goto NEWWORLDDRAW
					}
					*/
				}

			}

			if (y == 150) { //"achievements"
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
				for (x = 2; x < 18; x++) {
					for (y = 2; y < 13; y++) {
						gfx_TransparentSprite(sprites[10], x * 16, y * 16);
					}
				}
				
				gfx_BlitBuffer();

				while (!(os_GetCSC()));



			}

			if (y == 175) { //"Settings"


			}
	}
}

void WorldEngine(void) {
	ti_var_t appvar;
	int redraw, x, y, scrollX, scrollY, playerX, playerY, playerPos, curX, curY, posX, playerPosTest, height;

	gfx_SetTransparentColor(252);

	if (appvar = ti_Open(world_file, "r")) {
		if (!memcmp(ti_GetDataPtr(appvar), "MCCESV", 6)){
			int world_offset;
			ti_Seek(6, SEEK_SET, appvar);
			ti_Read(&world_offset, 3, 1, appvar);
			ti_Read(&worldLength, 3, 1, appvar);
			ti_Read(&worldHeight, 3, 1, appvar);
			ti_Read(&scrollX, 3, 1, appvar);
			ti_Read(&scrollY, 3, 1, appvar);
			ti_Read(&playerX, 3, 1, appvar);
			ti_Read(&playerY, 3, 1, appvar);
			ti_Read(&playerPos, 3, 1, appvar);
			ti_Read(&curPos, 3, 1, appvar);
			ti_Read(&curX, 3, 1, appvar);
			ti_Read(&curY, 3, 1, appvar);
			ti_Read(&timeofday, 3, 1, appvar);
			ti_Read(&hotbarSel, 3, 1, appvar);
			ti_Read(&hotbar, 5, 3, appvar);
			ti_Seek(world_offset, SEEK_SET, appvar);
			zx7_Decompress(&WorldData, ti_GetDataPtr(appvar));
			ti_Close(appvar);
			loaded_world = 1;
		}
	}

	if (!loaded_world) {

					
					/*
					for (x = 0; x < 20; x++) {
						for (y = 0; y < 15; y++) {
							gfx_TransparentSprite(sprites[1], x * 16, y * 16);
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

			height = 20;

			for (x = 0; x < worldLength; x++) {

					if ((worldType != 1) && (randInt(0, 2) == 0)) {
						height += randInt(-1, 1);
						if  (height < 1) height += 2;
					}

				for (y = 0; y < height; y++) {           //air
					WorldData[x+y*200] = 0;
				}
				WorldData[x + (height*200)] = 1;                      //grass
				for (y = height + 1; y < height + 3; y++){            //dirt
					WorldData[x+y*200] = 2;
				}
				for (y = height + 3; y < worldHeight - (height + 3); y++) {  //stone
					WorldData[x+y*200] = 11;
				}
			}

					/*
					for (x = 91; x < 320-92; x++) {
						//green progress bar... for looks at this point
						gfx_VertLine(x, 121, 5);
						delay(20);
						gfx_BlitBuffer();
					}
					*/

		timeofday = 0;

		curX = 16*10;
		curY = 16*4;
		curPos = (curX / 16) + ((curY / 16) * 200);
		playerX = 0;
		playerY = 0;
		scrollX = 0;
		scrollY = 0;
		posX = 0;
		//0 is the first pos in the hotbar array (it's max size is 5 btw)
		hotbarSel = 0;
		redraw = 1;
		//set selected block to grass block...
		blockSel = 1;
	}

	//draw the world and player sprites, as well as the player cursor... (none of which exist just yet)

	gfx_SetDrawBuffer();

	while (1) {
		kb_Scan();

		if (redraw == 1) {
			redraw = 0;
			gfx_FillScreen(191);
			gfx_SetColor(0);
			playerPos = (playerX + (playerY * worldLength));
			for(y = scrollY; y < 241 + scrollY; y+=16) {
				for(x = scrollX; x < 321 + scrollX; x+=16) {
					if (WorldData[playerPos] != 0)   gfx_TransparentSprite(sprites[WorldData[playerPos] - 1], x, y);
					playerPos++;
				}
				playerPos += worldLength - 21;
			}
				gfx_Rectangle(curX, curY, 16, 16);
				//hotbar
					for (x = 0; x < 5; x++) {
						if (x == hotbarSel) {
							gfx_SetColor(5);
						}else{
							gfx_SetColor(0);
						}
						gfx_Rectangle(117 + (x * 18), 220, 18, 18);
						if (hotbar[x]) {
							gfx_TransparentSprite(sprites[hotbar[x] - 1], 118 + (x * 18), 221);
						}else{
							gfx_SetColor(181);
							gfx_FillRectangle(118 + (x * 18), 221, 16, 16);
						}
					}
		}


		if (kb_IsDown(kb_KeyClear)) break;

		if (kb_IsDown(kb_KeyYequ)) {
			hotbarSel = 0;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyWindow)) {
			hotbarSel = 1;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyZoom)) {
			hotbarSel = 2;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyTrace)) {
			hotbarSel = 3;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyGraph)) {
			hotbarSel = 4;
			redraw = 1;
		}

		if (kb_IsDown(kb_Key2nd) && (WorldData[curPos] == 0)) {
			delay(100);
			WorldData[curPos] = hotbar[hotbarSel];
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyDel)) {
			delay(100);
			WorldData[curPos] = 0;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyGraphVar)) {
			/* inventory */
			creativeInventory();
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
			curPos -= worldLength;
			redraw = 1;
		}
		if ((kb_IsDown(kb_KeyApps)) && (curY < 240)) {
			delay(100);
			curY += 16;
			curPos += worldLength;
			redraw = 1;
		}


		if ((kb_IsDown(kb_KeyLeft)) && (playerX > 0)) {
			redraw = 1;
				if (scrollX == 0) {
					scrollX = -16;
					posX--;
					playerX--;
					curPos--;
					curX -= 16;
				}
			scrollX += 2;
			curX += 2;
		}
		if ((kb_IsDown(kb_KeyRight)) && (playerX < worldLength)) {
			redraw = 1;
				if (scrollX == -16) {
					scrollX = 0;
					posX++;
					playerX++;
					curPos++;
					curX += 16;
				}
			scrollX -= 2;
			curX -= 2;
		}
		if ((kb_IsDown(kb_KeyUp)) && (playerY > 0)) {
			redraw = 1;
				if (scrollY == 0) {
					scrollY = -16;
					playerY--;
					curPos -= worldLength;
					curY -= 16;
				}
			scrollY += 2;
			curY += 2;
		}
		if ((kb_IsDown(kb_KeyDown)) && (playerY < worldHeight)) {
			redraw = 1;
				if (scrollY == -16) {
					scrollY = 0;
					playerY++;
					curPos += worldLength;
					curY += 16;
				}
			scrollY -= 2;
			curY -= 2;
		}

		gfx_BlitBuffer();

	}

	//save the world data, playerX, playerY, playerPos, curPos, curX, curY, timeofday, etc...
	if (appvar = ti_Open(world_file, "w")) {
		int world_offset;
		ti_Write("MCCESV", 6, 1, appvar);
		ti_Write((void*)0xFF0000, 3, 1, appvar); //this is overwritten later
		ti_Write(&worldLength, 3, 1, appvar);
		ti_Write(&worldHeight, 3, 1, appvar);
		ti_Write(&scrollX, 3, 1, appvar);
		ti_Write(&scrollX, 3, 1, appvar);
		ti_Write(&scrollY, 3, 1, appvar);
		ti_Write(&playerX, 3, 1, appvar);
		ti_Write(&playerY, 3, 1, appvar);
		ti_Write(&playerPos, 3, 1, appvar);
		ti_Write(&curPos, 3, 1, appvar);
		ti_Write(&curX, 3, 1, appvar);
		ti_Write(&curY, 3, 1, appvar);
		ti_Write(&timeofday, 3, 1, appvar);
		ti_Write(&hotbarSel, 3, 1, appvar);
		ti_Write(&hotbar, 5, 3, appvar);
		world_offset = ti_Tell(appvar);
		ti_Seek(6, SEEK_SET, appvar);
		ti_Write(&world_offset, 3, 1, appvar);
		ti_Seek(world_offset, SEEK_SET, appvar);
		compressAndWrite(&WorldData, worldLength * worldHeight, appvar);
		ti_SetArchiveStatus(1, appvar);
		ti_Close(appvar);
	}
	delay(100);
}

void creativeInventory(void) {

		int24_t scroll, redraw, selX, selY, posB;
		gfx_SetDrawBuffer();
		gfx_SetColor(181);
		gfx_FillCircle(10, 10, 5);
		gfx_FillCircle(309, 10, 5);
		gfx_FillCircle(10, 229, 5);
		gfx_FillCircle(309, 229, 5);
		gfx_FillRectangle(10, 5, 300, 230);
		gfx_FillRectangle(5, 10, 310, 220);
		gfx_SetTextFGColor(0);
		gfx_PrintStringXY("Inventory:", 14, 14);
		gfx_SetTextFGColor(255);
		selX = 10;
		selY = 30;
		posB = 1;
		redraw = 1;
		while (!(kb_IsDown(kb_KeyClear))) {
			kb_Scan();
			if (redraw == 1) {
				redraw = 0;
				pos = 0;
				for (y = 30; y < 10 * 18; y += 18) {
					for (x = 10; x < 10 * 28; x += 18) {
						gfx_SetColor(148);
						gfx_FillRectangle(x, y, 18, 18);
						gfx_SetColor(0);
						gfx_Rectangle(x, y, 18, 18);
						if (pos < 64) gfx_TransparentSprite(sprites[pos++], x + 1, y + 1);
					}
				}
				gfx_SetColor(254);
				gfx_FillRectangle(selX + 6, selY + 6, 3, 3);
				//hotbar
					for (x = 0; x < 5; x++) {
						if (x == hotbarSel) {
							gfx_SetColor(5);
						}else{
							gfx_SetColor(0);
						}
						gfx_Rectangle(117 + (x * 18), 220, 18, 18);
						if (hotbar[x] != 0) {
							gfx_TransparentSprite(sprites[hotbar[x] - 1], 118 + (x * 18), 221);
						}else{
							gfx_SetColor(181);
							gfx_FillRectangle(118 + (x * 18), 221, 16, 16);
						}
					}
				gfx_BlitBuffer();
				
			}

			if ((kb_IsDown(kb_KeyUp)) && (selY > 30)) {
				selY -= 18;
				posB -= 15;
				redraw = 1;
				delay(80);
			}
			if ((kb_IsDown(kb_KeyDown)) && (selY < 10 * 18)) {
				selY += 18;
				posB += 15;
				redraw = 1;
				delay(80);
			}
			if ((kb_IsDown(kb_KeyLeft)) && (selX > 10)) {
				selX -= 18;
				posB--;
				redraw = 1;
				delay(80);
			}
			if ((kb_IsDown(kb_KeyRight)) && (selX < 28 * 18)) {
				selX += 18;
				posB++;
				redraw = 1;
				delay(80);
			}

			if (kb_IsDown(kb_Key2nd)) {
				
				delay(130);
				selX = 0;
				redraw = 1;
				kb_Scan();
				while (!(kb_IsDown(kb_Key2nd))) {
					if (redraw == 1) {
						redraw = 0;
						pos = 0;
						for (y = 30; y < 10 * 18; y += 18) {
							for (x = 10; x < 10 * 28; x += 18) {
								gfx_SetColor(148);
								gfx_FillRectangle(x, y, 18, 18);
								gfx_SetColor(0);
								gfx_Rectangle(x, y, 18, 18);
								if (pos < 64) gfx_TransparentSprite(sprites[pos++], x + 1, y + 1);
							}
						}
						//hotbar
							for (x = 0; x < 5; x++) {
								if (x == selX) {
									gfx_SetColor(5);
								}else{
									gfx_SetColor(0);
								}
								gfx_Rectangle(117 + (x * 18), 220, 18, 18);
								if (hotbar[x] != 0) {
									gfx_TransparentSprite(sprites[hotbar[x] - 1], 118 + (x * 18), 221);
								}else{
									gfx_SetColor(181);
									gfx_FillRectangle(118 + (x * 18), 221, 16, 16);
								}
							}
						gfx_TransparentSprite(sprites[posB - 1], 118 + (selX * 18), 221);
						gfx_BlitBuffer();
					}

					kb_Scan();
					if ((kb_IsDown(kb_KeyLeft)) && (selX > 0)) {
						delay(100);
						selX--;
						redraw = 1;
					}
					if ((kb_IsDown(kb_KeyRight)) && (selX < 5)) {
						delay(100);
						selX++;
						redraw = 1;
					}
					if (kb_IsDown(kb_KeyClear)) {
						creativeInventory();
					}

				}
				hotbar[selX] = posB;
				selX = 10;
				redraw = 1;
				delay(200);
				return;
			}
			
		}
	


	redraw = 1;
	delay(200);
}

void drawDirtBackground(int scroll) {
	int scrollY, test;
	for (test = 0; test < 20; test++) {
		for (scrollY = scroll; scrollY < 256; scrollY += 16) {
			gfx_TransparentSprite(sprites[1], test * 16, scrollY - 16);
		}
	}
}

//Compress data via zx7 compression
void compressAndWrite(void *data, int len, ti_var_t fp) {
//	int i, j, k, num, num_i, new;
//	uint8_t b, m;
//	int *huffman_table = (void*)0xD52C00;
//	uint8_t *huffman_table_2 = &huffman_table[256];
//	uint8_t *huffman_output = &huffman_table_2[256];
//	void *back_buffer = (void*)0xD52C00;
//	uint8_t *ptr;
	int new_len;
	*((int*)0xE30010) = 0xD40000; //force the lcd to use the first half of VRAM so we can use the second half.
	gfx_SetDrawScreen();
	drawDirtBackground(0);
	gfx_SetTextFGColor(0xDF);
	gfx_PrintStringXY("Compressing data...", 90, 110);

/* Maybe do huffman coding at some point
	//start by counting the occurences of each unique byte in the data
	ptr = data;
	for (i=0; i<len; i++){
		huffman_table[*ptr++]++;
	}
	//then sort by highest number of occurences
	k = 1;
	for (i=0; i<256; i++){
		num = num_i = 0;
		for (j=0; j<256; j++) {
			if ((new=huffman_table[j]) > num) {
				num = new;
			}
		}
		if (!num) break;
		huffman_table_2[num_i] = k++;
		huffman_table[j] = 0;
	}
	ptr = data;
	m = 1;
	for (i=0; i<len; i++){
		b = huffman_table_2[*ptr++];
		k = 256;
		while (k>>=1){
			if (b & k){
				while (b) {
					
				}
			}
		}
	}
*/

	zx7_Compress((void*)0xD52C00, data, &new_len, len);
	ti_Write((void*)0xD52C00, new_len, 1, fp);
}


void StringInput(void) {
    const char* chars = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
    uint24_t key, i = 0;
    char buffer[50];
    gfx_Begin();
    while ((key = os_GetCSC()) != sk_Enter) {
        if (chars[key]) {
            buffer[i++] = chars[key];
            gfx_PrintStringXY(buffer, 1, 1);
        }
    }

    delay(100);
    return;
}