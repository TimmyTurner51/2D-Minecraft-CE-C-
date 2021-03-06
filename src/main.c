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

#include <debug.h>
#include "gfx/gfx.h"

#include <ctype.h>

//no need for this unless there's actual gfx data that needs to be written to the program :P

#include "compressor.h" //zx7 compression routines

void MainMenu(void);
void Achievements(void);
void Settings(void);
void playMenu(void);
void creativeInventory(void);
void drawDirtBackground(int scroll);
void findAppvars(const char *str);
void WorldEngine(void);

#define STONE 1
#define GRASS 2
#define DIRT 3
#define COBBLESTONE 4
#define OAKPLANK 5
#define OAKSAPLING 6
#define BEDROCK 7
#define WATER 8
#define NOTHING1 9
#define LAVA 10
#define NOTHING2 11
#define SAND 12
#define GRAVEL 13
#define GOLDORE 14
#define IRONORE 15
#define COALORE 16
#define OAKLOGS 17
#define OAKLEAVES 18
#define SPONGE 19
#define GLASS 20
#define LAPIZORE 21
#define LAPIZBLOCK 22
#define DISPENSER 23
#define SANDSTONE 24
#define NOTEBLOCK 25
#define BEDBACK 26
#define BEDFRONT 27
#define REDSTONEDUSTOFF 28
#define REDSTONEDUSTON 29
#define REGULARPISTONRIGHTOFF 30
#define COBWEB 31

#define WATERENTITY 32
#define LAVAENTITY 33

//static char list[20][9]; memcpy(list[found++], name, 9);
//no longer needed...we have length and height vars now... #define world_ground_height 64

gfx_sprite_t *sprites[254];
gfx_sprite_t dummy_sprite = {1, 1, 0};

//define the world data list
char WorldData[256 * 256] = { 0 };
// 0 = redstone off/null; 1 = redstone on; 2 = Water where sand/gravel fall; 3 = Lava where sand/gravel falls;
// anything > 3 = timer for the corresponding block in the same pos in WorldData array; 
char BlockData[20 * 16] = { 0 };
char *world_file = "MCCESAVE";
//hopefully a static 'y' will fix some menu bugs...
int16_t x, y, pos;
//{ name of world 1, name of world 2, etc. }
char WorldsList[20][9];
//{ name of server 1, name of server 2, etc. }
char Servers[20][20];
char FriendsList[20][20];

int24_t timeofday, difficulty;
int24_t playerX;
int24_t playerY;
int24_t curX;
int24_t curY;
int24_t curPos;
int24_t hotbar[5] = {0};
int24_t hotbarCur;
int24_t blockSel;
int24_t gamemode;
bool loaded_world = 0;
gfx_TempSprite(logo, 16, 16);
int natureBlocks[13] = {GRASS, DIRT, STONE, COBBLESTONE, SAND, GRAVEL, OAKLOGS, OAKLEAVES, BEDROCK, COALORE, IRONORE, GOLDORE, LAPIZORE};
int buildingBlocks[3] = {OAKPLANK, GLASS, SPONGE};
int redstoning[3] = {REDSTONEDUSTOFF, NOTEBLOCK, REGULARPISTONRIGHTOFF};
int toolsEtc[3] = { BEDBACK, WATER, LAVA };

int typesvalues[4] = {13, 3, 3, 3};

uint8_t foundCount; // used to stop the code from finding too many appvars
uint8_t dayColors[5] = {191, 158, 125, 51, 9};

uint8_t user_input(char *buffer, size_t length); //user input subroutine. src/asm/user_input.asm

void compressAndWrite(void *data, int len, ti_var_t fp); //this routine compresses using zx7_Compression

int24_t worldHeight = 200;
int24_t worldLength = 200;
int24_t worldType = 0;
int24_t hotbarSel = 0;
int24_t seed = 0;

char worldNameStr[20] = "My World";
char seedStr[20] = "Random Seed";

int32_t hashstr(char *s);

int32_t hashstr(char *s) {
    int32_t hash = 0;
    size_t len = strlen(s);
    char c;
    while ((c = *s) != '\0') {
        int coe;
        if (isdigit(c)) {
            coe = c - '0';
        } else {
            coe = c - 'A' + 10;
        }
        hash += coe*pow(36, len);
        s++;
        len--;
    }
    seed = hash;
}

void main(void)
{
	ti_var_t appvar;
	gfx_Begin(); //This sets the default palette, no need to set the palette again
	ti_CloseAll();
	gfx_SetClipRegion(-17, -17, 337, 257);
	LoadBlocks("MCEDEFT");
	appvar = ti_Open("MC2DDAT", "r");
	logo = ti_GetDataPtr(appvar);
	ti_Close(appvar);
	y = 125;
	gfx_SetDrawBuffer();
	MainMenu();
}

void MainMenu(void)
{
	int24_t i, scroll, redraw;
	i = y;
	scroll = 16;
	redraw = 1;
	gfx_SetTransparentColor(255);
	while (!(kb_IsDown(kb_Key2nd)))
	{ //draw the main menu
		kb_Scan();

		drawDirtBackground(scroll);
		gfx_SetTextFGColor(230);
		gfx_PrintStringXY("DEV_ALPHA v1.0.04", 3, 230);
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
		gfx_SetColor(0);
		gfx_Rectangle(60, y, 192, 20);
		gfx_Rectangle(61, y + 1, 190, 18);
		gfx_BlitBuffer();

		scroll--;
		if (scroll < 1)
			scroll = 16;

		if (kb_IsDown(kb_KeyUp) && y > 125)
		{
			//delay(150);
			redraw = 1;
			y -= 25;
		}
		if (kb_IsDown(kb_KeyDown) && y < 200)
		{
			//delay(150);
			redraw = 1;
			y += 25;
		}
		if (kb_IsDown(kb_KeyClear))
		{
			gfx_End();
			return;
		}
	}

	gfx_SetTransparentColor(252);

	if (y == 125)
	{
		playMenu();
		y = 125;
		delay(100);
		MainMenu();
	}
	if (y == 150)
	{ //"achievements"
		Achievements();
		y = 150;
		delay(100);
		MainMenu();
	}
	if (y == 175)
	{ //"Settings"
		Settings();
		y = 175;
		delay(100);
		MainMenu();
	}
	gfx_End();
}

void playMenu(void)
{

	int24_t CursorY, i, scroll, redraw, tab;
	int24_t worldLength, worldHeight, cheats, scrollYb;
	char *gamemodeStr[3] = {"Survival", "Creative", "Hardcore"};
	char *difficultyStr[4] = {"Peaceful", "Easy", "Normal", "Hard"};
	char *cheatsStr[2] = {"Off", "On"};
	char *worldTypesStr[3] = {"Standard", "Superflat", "Large Biomes"};
	findAppvars("MCCESV");
	drawDirtBackground(0);
	gfx_SetTransparentColor(255);
	tab = 0;
	scroll = 0;
	CursorY = 40;
	kb_Scan();
	while (!(kb_IsDown(kb_KeyClear)))
	{
		kb_Scan();

		drawDirtBackground(0);
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

		if (tab == 0)
		{
			gfx_SetColor(148);
			gfx_FillRectangle(21, 40, 278, 25);
			gfx_PrintStringXY("Create New World", 110, 47);
			gfx_SetColor(254);
			gfx_Rectangle(21, CursorY, 278, 25 - ((CursorY != 40) * 8));
			gfx_Rectangle(22, CursorY + 1, 276, 23 - ((CursorY != 40) * 8));
			if (foundCount == 0)
			{
				gfx_PrintStringXY("No Worlds were found!", 90, 120);
			}
			else
			{
				y = 0;
				for (i = scroll; i < scroll + 2; i++)
				{
					if (i < foundCount)
						gfx_PrintStringXY(WorldsList[i], 40, 70 + y);
					y += 17;
				}
			}

			if (kb_IsDown(kb_KeyUp) && (CursorY == 65))
			{
				delay(50);
				CursorY = 40;
				scrollYb = -1;
			}
			else
			{
				if (kb_IsDown(kb_KeyUp) && (scrollYb > 0) && (CursorY != 40))
				{
					delay(50);
					CursorY -= 17;
					scrollYb--;
				}
			}
			if (kb_IsDown(kb_KeyDown) && (CursorY == 40) && (foundCount != 0))
			{
				delay(50);
				CursorY = 65;
				scrollYb = 0;
				scroll = 0;
			}
			else
			{
				if (kb_IsDown(kb_KeyDown) && (scrollYb < foundCount - 1) && (scrollYb < 10))
				{
					delay(50);
					CursorY += 17;
					scrollYb++;
				}
				if (kb_IsDown(kb_KeyDown) && (scrollYb < foundCount - 1) && (scrollYb > 9))
				{
					delay(50);
					scroll++;
					scrollYb++;
				}
			}
			if (kb_IsDown(kb_Key2nd) && (CursorY != 40))
			{
				world_file = WorldsList[scrollYb];
				WorldEngine();
			}
			if (kb_IsDown(kb_Key2nd) && (CursorY == 40))
			{
				CursorY = 80;
				gamemode = 0;
				difficulty = 0;
				cheats = 0;
				worldType = 0;
				redraw = 1;
				while (!(kb_IsDown(kb_KeyClear)))
				{
					if (redraw == 1)
					{
						redraw = 0;
						drawDirtBackground(0);
						for (y = 80; y < 220; y += 20)
						{
							gfx_SetColor(181);
							gfx_FillRectangle(50, y, 220, 16);
							gfx_SetColor(0);
							gfx_Rectangle(50, y, 220, 16);
							gfx_Rectangle(51, y + 1, 218, 14);
							gfx_SetColor(148);
							if (y < 200) gfx_FillRectangle(152, y + 3, 94, 10);
						}
						gfx_PrintStringXY("World Name:", 73, 84);
						gfx_PrintStringXY(&worldNameStr, 154, 84);
						gfx_PrintStringXY("Seed:", 73, 104);
						gfx_PrintStringXY(&seedStr, 154, 104);
						gfx_PrintStringXY("Gamemode:", 73, 124);
						gfx_PrintStringXY(gamemodeStr[gamemode], 154, 124);
						gfx_PrintStringXY("Difficulty:", 73, 144);
						gfx_PrintStringXY(difficultyStr[difficulty], 154, 144);
						gfx_PrintStringXY("Cheats:", 73, 164);
						gfx_PrintStringXY(cheatsStr[cheats], 154, 164);
						gfx_PrintStringXY("World Type:", 73, 184);
						gfx_PrintStringXY(worldTypesStr[worldType], 154, 184);
						gfx_PrintStringXY("Generate", 128, 204);
						gfx_SetColor(254);
						gfx_Rectangle(50, CursorY, 220, 16);
						gfx_Rectangle(51, CursorY + 1, 218, 14);
						gfx_BlitBuffer();
					}
					kb_Scan();
					if (kb_IsDown(kb_KeyUp) && (CursorY > 80))
					{
						CursorY -= 20;
						redraw = 1;
					}
					if (kb_IsDown(kb_KeyDown) && (CursorY < 200))
					{
						CursorY += 20;
						redraw = 1;
					}
					if (kb_IsDown(kb_Key2nd))
					{
						redraw = 1;
						if (CursorY == 80)
						{
							user_input(worldNameStr, 16);
							delay(100);
						}

						if (CursorY == 100)
						{
							user_input(seedStr, 16);
							delay(100);
						}

						if (CursorY == 120)
						{
							gamemode++;
							if (gamemode > 2)
							{
								gamemode = 0;
							}
						}
						if (CursorY == 140)
						{
							difficulty++;
							if (difficulty > 3)
							{
								difficulty = 0;
							}
						}
						if (CursorY == 160)
						{
							cheats = (cheats == 0);
						}
						if (CursorY == 180)
						{
							worldType++;
							if (worldType > 2)
							{
								worldType = 0;
							}
						}
						if (CursorY == 200)
						{
							worldLength = 256;
							worldHeight = 256;
						
							//supposed to take the first 8 letters of the worldName string and copy to world_file
							world_file = "        ";
							memcpy(world_file, worldNameStr, 8);

							dbg_sprintf(dbgout, "%s", world_file);

							if (seedStr == "Random Seed") srand(rtc_Time());
							hashstr(seedStr);
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
		}
		if (tab == 1)
		{
			gfx_SetColor(148);
			gfx_FillRectangle(21, 40, 278, 25);
			gfx_PrintStringXY("Add Server", 116, 47);
			gfx_SetColor(254);
			gfx_Rectangle(21, CursorY, 278, 25 - ((CursorY != 40) * 8));
			gfx_Rectangle(22, CursorY + 1, 276, 23 - ((CursorY != 40) * 8));
		}
		if (tab == 2)
		{
			gfx_PrintStringXY("Friends are not Available", 80, 47);
		}

		gfx_BlitBuffer();

		if (kb_IsDown(kb_KeyLeft) && (tab > 0))
		{
			delay(200);
			tab--;
			CursorY = 40;
		}
		if (kb_IsDown(kb_KeyRight) && (tab < 2))
		{
			delay(200);
			tab++;
			CursorY = 40;
		}
	}
}

void Achievements(void)
{
	int24_t x, y;
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

void Settings(void)
{
	int24_t x, y, i, redraw;
	x = 10;
	y = 20;
	redraw = 1;
	while (!(kb_IsDown(kb_KeyClear)))
	{
		if (redraw == 1)
		{
			redraw = 0;
			drawDirtBackground(0);
			for (i = 20; i < 180; i += 20)
			{
				gfx_SetColor(181);
				gfx_FillRectangle(10, i, 140, 16);
				gfx_FillRectangle(170, i, 140, 16);
			}
			gfx_SetColor(254);
			gfx_Rectangle(x, y, 140, 16);
			gfx_Rectangle(x + 1, y + 1, 138, 14);
			gfx_BlitBuffer();
		}
		kb_Scan();
		if (kb_IsDown(kb_KeyUp) && (y > 20))
		{
			y -= 20;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyDown) && (y < 160))
		{
			y += 20;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyLeft))
		{
			x = 10;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyRight))
		{
			x = 170;
			redraw = 1;
		}
	}
	delay(100);
}

void WorldEngine(void)
{
	ti_var_t appvar;
	int24_t redraw, x, y, scrollX, scrollY, playerX, playerY, playerPos, curX, curY, posX, jump;
	int24_t chunkX = 0, multiplier = 0, playerPosTest, height, flymode, timer, temptimer;
	int8_t angle, i, health, hunger, exp, bgColor, cloudX, fall;
	//{grass, sand, water, lava}
	int ticks[5] = { 0, 0, 0, 0 };
	uint8_t xb, yb;
	gfx_sprite_t *tempSpr = gfx_MallocSprite(14, 14);

		timeofday = 0;
		flymode = 0;
		timer = 0;

		posX = 0;
		hotbar[0] = 0;
		hotbar[1] = 0;
		hotbar[2] = 0;
		hotbar[3] = 0;
		hotbar[4] = 0;
		// the * 2 is for half hearts
		health = 9 * 2;
		hunger = 9 * 2;
		exp = 0;
		//0 is the first pos in the hotbar array (it's max size is 5 btw)
		hotbarSel = 0;
		ticks[0] = 1;
		curX = 16 * 10;
		curY = 16 * 6;
		playerX = 1;
		playerY = 0;
		curPos = (curX / 16) + ((curY / 16) * worldLength) + playerX;
		scrollX = 0;
		scrollY = 0;
		playerPos = 0;

		//set selected block to grass block...
		//blockSel = 1;
	if (appvar = (ti_Open(world_file, "r")))
	{
		if (!memcmp(ti_GetDataPtr(appvar), "MCCESV", 6))
		{
			int world_offset;
			ti_Seek(6, SEEK_SET, appvar);
			ti_Read(&world_offset, 3, 1, appvar);
			ti_Read(&worldLength, 3, 1, appvar);
			ti_Read(&worldHeight, 3, 1, appvar);
			ti_Read(&flymode, 3, 1, appvar);
			ti_Read(&gamemode, 3, 1, appvar);
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
			ti_Read(&health, 3, 1, appvar);
			ti_Read(&hunger, 3, 1, appvar);
			ti_Read(&exp, 3, 1, appvar);
			ti_Seek(world_offset, SEEK_SET, appvar);
			zx7_Decompress(&WorldData, ti_GetDataPtr(appvar));
			ti_Close(appvar);
			loaded_world = 1;
		}
	}

	if (!loaded_world)
	{

		drawDirtBackground(0);
		gfx_SetTextFGColor(254);
		gfx_PrintStringXY("Generating World", 102, 90);
		gfx_PrintStringXY("Building Terrain", 104, 104);
		gfx_SetColor(148);
		gfx_FillRectangle(90, 120, 320 - 180, 7);
		gfx_SetColor(0);
		gfx_Rectangle(90, 120, 320 - 180, 7);
		gfx_SetColor(6);

		//this is an updated generator, and it to be used with the new textures...we now have bedrock too!
		//thank u LogicalJoe!!
		srand(seed*multiplier+chunkX); // set the seed
		height = randInt(60, 120);

		//from Zeroko: srand(worldSeed*multiplier+chunkX)

		chunkX = 0;
		
		multiplier = seed / 2;
		// handle generation of air first, so everything can generate properly. 
		// This looks bad, but the generator is pretty fast, and that's all I'm worried about.
		for (x = 0; x < worldLength * worldHeight; x++)
		{
			WorldData[x] = 0;
		}
		//rest of generation
		for (x = 0; x < worldLength; x++)
		{
			gfx_VertLine(91 + (x / 2), 121, 5);
			gfx_BlitBuffer();
			if (x % 16) chunkX++;
			srand(seed*multiplier+chunkX); // set the seed
			if ((worldType != 1) && (randInt(0, 2) == 0))
			{
				height += randInt(-1, 1);
				if (height < 1)
					height += 2;
			}
			if (randInt(1, 10) == 1)  //tree
			{
				WorldData[x + ((height - 1) * worldLength)] = OAKLOGS + 1;
				WorldData[x + ((height - 2) * worldLength)] = OAKLOGS + 1;
				WorldData[x + ((height - 3) * worldLength)] = OAKLOGS + 1;
				WorldData[x + ((height - 4) * worldLength)] = OAKLOGS + 1;
				WorldData[x + ((height - 5) * worldLength)] = OAKLEAVES + 1;
				WorldData[x + ((height - 4) * worldLength) - 1] = OAKLEAVES + 1;
				WorldData[x + ((height - 4) * worldLength) + 1] = OAKLEAVES + 1;
			}
			if (randInt(0, 6) == 2)
			{
				WorldData[x + (height * worldLength)] = WATER + 1;
				for (y = height + 1; y < height + randInt(10, 30); y++)
				{
					WorldData[x + (y * worldLength)] = WATERENTITY;
				}
				height = y - randInt(10, 30);
			}else{
				WorldData[x + (height * worldLength)] = GRASS + 1;
				for (y = height + 1; y < height + 3; y++)
				{
					WorldData[x + y * worldLength] = DIRT + 1;
				}
			}
			for (y = height + 3; y < worldHeight; y++)
			{
				WorldData[x + y * worldLength] = STONE + 1;
				// cave generation
				if (randInt(1,13) == 3)
				{
					for (i = height + randInt(23, 30); i < height + randInt(23, 30); i++)
					{
						WorldData[x + i * worldLength] = 0;
					}
				}
				
				//ore generation
				if ((randInt(1, 40) == 10) && (WorldData[x + y * worldLength] != 0) && (height > 30))
					WorldData[x + y * worldLength] = COALORE;
				if ((randInt(1, 40) == 10) && (WorldData[x + y * worldLength] != 0) && (height > 130))
					WorldData[x + y * worldLength] = IRONORE;
				if ((randInt(1, 40) == 10) && (WorldData[x + y * worldLength] != 0) && (height > 160))
					WorldData[x + y * worldLength] = GOLDORE;
				//if ((randInt(1, 40) == 10) && (WorldData[x + y * worldLength] != 0) && (height > 160))
				//	WorldData[x + y * worldLength] = DIAMONDORE;
				
				//stronghold
				if ((randInt(1, 900) == 20) && (WorldData[x + y * worldLength] != 0) && (x > 10))
				{
					for (xb = x - 8; xb < x; xb++) {
						for (yb = y; yb < y + 8; yb++) {
							WorldData[xb + yb * worldLength] = 0;
							if ((xb == x - 8) || (xb == x - 1) || (yb == y) || (yb == y + 7))
							WorldData[xb + yb * worldLength] = COBBLESTONE + 1;
						}
					}
				}

			}
			WorldData[x + worldHeight * worldLength] = BEDROCK + 1;
		}
		for (x = 130; x < 320 - 92; x++)
		{
			//green progress bar... for looks at this point
			gfx_VertLine(x, 121, 5);
			delay(20);
			gfx_BlitBuffer();
		}

	}

	jump = 0;
	angle = 0;
	
	while (WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] == 0)
	{
		playerY++;
		curPos += worldLength;
	}

	LoadBlocks("MCEDEFT");
	gfx_SetDrawBuffer();
	fall = 0;
	cloudX = 2;
	while (1)
	{
		kb_Scan();

		bgColor = dayColors[timeofday / 6000];
		
		if ((kb_IsDown(kb_Key7)) && (health > 0)) 
		{
			health--;
			bgColor = 224;
			gfx_BlitBuffer();
			delay(120);
			ticks[0] = 1;
		}
		if ((hunger == 18) && (health != 18) && (timer == 0)) 
		{
			timer = 1;
			temptimer = 1;
		}

		if (temptimer != 0) temptimer++;
		if ((timer == 1) && (temptimer == 80))
		{
			timer = 0;
			health++;
		}
		if ((flymode == 0) && (WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] == 0))
		{
			ticks[0] = 1;
			scrollY -= 4;
			curY -= 4;
			if (scrollY < -15)
			{
				if (jump == 0) fall++;
				scrollY = 0;
				playerY++;
				curPos += worldLength;
				curY += 16;
				jump = 0;
				if ((fall > 4) && (gamemode != 1) && (WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] != 0))
				{
					bgColor = 224;
					health -= fall - 3;
					fall = 0;
				}
			}
		}

		if ((ticks[0] != 0) || (ticks[1] != 0) || (ticks[2] != 0) || (ticks[3] != 0))
		{
			if (ticks[0] != 0) ticks[0]--;
			if (ticks[2] != 0) ticks[2]--;
			if (ticks[3] != 0) ticks[3]--;
			gfx_FillScreen(bgColor);
			gfx_SetColor(32);
			playerPos = (playerX + playerY * worldLength);
			//behaviors w/ block drawing, and fixing cursor bugs by forcing it to "snap to the grid"		
			gfx_SetTransparentColor(252);
			for (y = scrollY; y < 241 + scrollY; y += 16)
			{
				for (x = scrollX; x < 321 + scrollX; x += 16)
				{
					if (BlockData[playerPos] != 0) BlockData[playerPos]--;
					// basic shadowing (v1.2)
					if (WorldData[playerPos] != 0)
					{
						// draw the shadowing box (not for water, lava, etc.)
						if ((WorldData[playerPos] != BEDBACK + 1) && (WorldData[playerPos] != BEDFRONT + 1) && (WorldData[playerPos] != WATER + 1) && (WorldData[playerPos] != LAVA + 1))
						gfx_FillRectangle(x, y, 16, 16);
						// check if air is on the left face, right face, top face, and bottom face
						// (in that order), and draw the block if so
						if ((WorldData[playerPos] < 256) && ((WorldData[playerPos] == WATER + 1) || (WorldData[playerPos] == LAVA + 1) || (WorldData[playerPos - 1] == 0) || (WorldData[playerPos + 1] == 0) || (WorldData[playerPos - worldLength] == 0) || (WorldData[playerPos + worldLength] == 0)))
							gfx_TransparentSprite(sprites[WorldData[playerPos] - 1], x, y);
						if (WorldData[playerPos] == WATERENTITY)
							gfx_TransparentSprite(sprites[WATER], x, y);
						if (WorldData[playerPos] == LAVAENTITY)
							gfx_TransparentSprite(sprites[LAVA], x, y);
						
						// behaviors
						if (WorldData[playerPos] == WATERENTITY)
						{
							ticks[4] = 0;
							ticks[5] = 0;
						}
						// sand falls
						if ((WorldData[playerPos] == SAND + 1) && ((WorldData[playerPos + worldLength] == 0) || (WorldData[playerPos + worldLength] == WATER + 1) || (WorldData[playerPos + worldLength] == LAVA + 1)) && (BlockData[playerPos] == 0))
						{
							BlockData[playerPos] = WorldData[playerPos + worldLength];
							if (WorldData[playerPos + worldLength] == WATER + 1)
								BlockData[playerPos - worldLength] = WATER + 1;
							if (WorldData[playerPos + worldLength] == LAVA + 1)
								BlockData[playerPos - worldLength] = LAVA + 1;
							WorldData[playerPos] = BlockData[playerPos];
							WorldData[playerPos + worldLength] = SAND + 1;
							BlockData[playerPos + worldLength] = 5;
						}
						// water interacts with lava (source and non-source)
						if ((WorldData[playerPos] == WATER + 1) || (WorldData[playerPos] == WATERENTITY))
						{
							if (WorldData[playerPos + 1] == LAVA + 1) WorldData[playerPos + 1] = NOTHING2 + 1;
							if (WorldData[playerPos - 1] == LAVA + 1) WorldData[playerPos - 1] = NOTHING2 + 1;
							if (WorldData[playerPos + 1] == LAVAENTITY) WorldData[playerPos + 1] = COBBLESTONE + 1;
							if (WorldData[playerPos - 1] == LAVAENTITY) WorldData[playerPos - 1] = COBBLESTONE + 1;
						}
						if (((WorldData[playerPos] == LAVA + 1) || (WorldData[playerPos] == LAVAENTITY)) && (WorldData[playerPos + worldLength] == WATER + 1))
							WorldData[playerPos + worldLength] = STONE + 1;
						// water can be destroyed
						if ((WorldData[playerPos] == WATERENTITY) && (WorldData[playerPos - worldLength] != WATER + 1) && (WorldData[playerPos - worldLength] != WATERENTITY) && (ticks[2] == 0))
						{
							WorldData[playerPos] = 0;
							ticks[2] = 10;
						}
						// lava can be destroyed
						if ((WorldData[playerPos] == LAVAENTITY) && (WorldData[playerPos - worldLength] != LAVA + 1) && (WorldData[playerPos - worldLength] != LAVAENTITY) && (ticks[3] == 0))
						{
							WorldData[playerPos] = 0;
							ticks[3] = 18;
						}
						// water flows down
						if ((WorldData[playerPos] == WATER + 1) && (WorldData[playerPos + worldLength] == 0) && (ticks[2] == 0))
						{
							WorldData[playerPos + worldLength] = WATERENTITY;
							ticks[2] = 10;
						}
						if ((WorldData[playerPos] == WATERENTITY) && (WorldData[playerPos + worldLength] == 0) && (ticks[2] == 0))
						{
							WorldData[playerPos + worldLength] = WATERENTITY;
							ticks[2] = 10;
						}
						//water flows sideways (right, then left)
						if ((WorldData[playerPos] == WATERENTITY) && (WorldData[playerPos - 1] == 0) && (WorldData[playerPos + (worldLength * 2)] != 0) && (ticks[4] < 6) && (ticks[2] == 0))
						{
							if (WorldData[playerPos - 1 + worldLength] == 0)
								WorldData[playerPos - 1 + worldLength] = WATERENTITY;
							ticks[4]++;
							ticks[2] = 10;
						}
						if ((WorldData[playerPos] == WATERENTITY) && (WorldData[playerPos + 1] == 0) && (WorldData[playerPos + (worldLength * 2)] != 0) && (ticks[5] < 6) && (ticks[2] == 0))
						{
							if (WorldData[playerPos + 1 + worldLength] == 0)
								WorldData[playerPos + 1 + worldLength] = WATERENTITY;
							ticks[5]++;
							ticks[2] = 10;
						}
						ticks[4] = 0;
						ticks[5] = 0;
						//lava flows sideways (right, then left)
						if ((WorldData[playerPos] == LAVAENTITY) && (WorldData[playerPos - 1] == 0) && (WorldData[playerPos + (worldLength * 2)] != 0) && (ticks[4] < 6) && (ticks[3] == 0))
						{
							if (WorldData[playerPos - 1 + worldLength] == 0)
								WorldData[playerPos - 1 + worldLength] = LAVAENTITY;
							ticks[4]++;
							ticks[3] = 18;
						}
						if ((WorldData[playerPos] == LAVAENTITY) && (WorldData[playerPos + 1] == 0) && (WorldData[playerPos + (worldLength * 2)] != 0) && (ticks[5] < 6) && (ticks[3] == 0))
						{
							if (WorldData[playerPos + 1 + worldLength] == 0)
								WorldData[playerPos + 1 + worldLength] = LAVAENTITY;
							ticks[5]++;
							ticks[3] = 18;
						}
						// lava flows down
						if (((WorldData[playerPos] == LAVA + 1) || (WorldData[playerPos] == LAVAENTITY)) && (WorldData[playerPos + worldLength] == 0) && (ticks[3] == 0))
						{
							WorldData[playerPos + worldLength] = LAVAENTITY;
							ticks[3] = 18;
						}
						// grass turns to dirt when blocks are on top of grass
						if ((WorldData[playerPos] != 0) && (WorldData[playerPos + worldLength] == GRASS + 1) && (ticks[0] == 0))
						{
							WorldData[playerPos + worldLength] = DIRT + 1;
							ticks[0] = 120;
						}
					}
					playerPos++;
				}
				playerPos += worldLength - 21;
			}
			playerPos = (playerX + playerY * worldLength);
			
			if (health < 1)
			{
				gfx_SetColor(148);
				gfx_FillRectangle(80, 200, 160, 20);
				gfx_SetTextScale(2, 2);
				gfx_PrintStringXY("RESPAWN", 84, 204);
				gfx_BlitBuffer();
				while (!(kb_IsDown(kb_Key2nd)));
				WorldEngine();
			}

			gfx_Rectangle(curX, curY, 16, 16);
			//hotbar
			for (x = 0; x < 5; x++)
			{
				if (x == hotbarSel)
				{
					gfx_SetColor(5);
				}
				else
				{
					gfx_SetColor(0);
				}
				gfx_Rectangle(117 + (x * 18), 220, 18, 18);
				if (hotbar[x] != 0)
				{
					gfx_TransparentSprite(sprites[hotbar[x]], 118 + (x * 18), 221);
				}
				else
				{
					gfx_SetColor(181);
					gfx_FillRectangle(118 + (x * 18), 221, 16, 16);
				}
			}
			if (gamemode != 1)
			{
				// health bar (hearts)
				i = 0;
				for (x = 5; x < 5 + 9 * 9; x += 9)
				{
					if (i < health)
						gfx_TransparentSprite(heart_full, x, 5);
					if (i >= health)
						gfx_TransparentSprite(heart_empty, x, 5);
						if ((i == health) && (!(health % 2)))
							gfx_TransparentSprite(heart_half, x, 5);
					i++;
				}
				// hunger bar
				i = 18;
				for (x = 220; x < 220 + 9 * 9; x += 9)
				{
					if (i < hunger)
						gfx_TransparentSprite(hunger_full, x, 5);
					if (i >= hunger)
						gfx_TransparentSprite(hunger_empty, x, 5);
					if ((i == hunger) && (!(hunger % 2)))
						gfx_TransparentSprite(hunger2, x, 5);
					i--;
				}
				// exp bar
				gfx_SetColor(6);
				gfx_FillRectangle(60, 202, 15 * 6, 5);
				gfx_SetColor(3);
				i = 0;
				for (x = 60; x < 60 + 15 * 6; x += 6)
				{
					if (i < exp)
					{
						gfx_FillRectangle(x, 202, 3, 3);
					}
					i++;
				}
				gfx_SetColor(32);
			}
			gfx_TransparentSprite(Head_1, 16 * 9 + 3, 16 * 5 + 16);
			gfx_TransparentSprite(Body_1, 16 * 9 + 5, 16 * 5 + 24);
			gfx_TransparentSprite(Leg_1, 16 * 9 + 5, 16 * 5 + 35);
			//memcpy(tempSpr, Leg_1, 14 * 14);
			//gfx_RotatedScaledTransparentSprite_NoClip(Head_1, 16 * 9 + 2, 16 * 5 + 14, 0, 64);
			//gfx_RotatedScaledTransparentSprite_NoClip(Body_1, 16 * 9 + 4, 16 * 5 + 22, 0, 64);
			//gfx_RotatedScaledTransparentSprite_NoClip(Arm_1, 16 * 9 + 4, 16 * 5 + 22, 256 - angle, 64);
			//gfx_RotatedScaledTransparentSprite_NoClip(tempSpr, 16 * 9 + 4, 16 * 5 + 33, angle, 64);
		}

		
		if (kb_IsDown(kb_KeyClear))
			break;

		if (kb_IsDown(kb_KeyYequ))
		{
			hotbarSel = 0;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyWindow))
		{
			hotbarSel = 1;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyZoom))
		{
			hotbarSel = 2;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyTrace))
		{
			hotbarSel = 3;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyGraph))
		{
			hotbarSel = 4;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_Key2nd) && (WorldData[curPos] == 0) && (hotbar[hotbarSel] != 0) && ((WorldData[curPos - 1] != 0) || (WorldData[curPos + 1] != 0) || (WorldData[curPos - worldLength] != 0) || (WorldData[curPos + worldLength] != 0)))
		{
			delay(100);
			WorldData[curPos] = hotbar[hotbarSel] + 1;
			BlockData[curPos] = 0;
			if (hotbar[hotbarSel] == BEDBACK) WorldData[curPos + 1] = BEDFRONT + 1;
			if (hotbar[hotbarSel] == BEDFRONT) WorldData[curPos - 1] = BEDBACK + 1;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyDel) && (WorldData[curPos] != 0))
		{
			delay(100);
			if (WorldData[curPos] == BEDBACK) WorldData[curPos + 1] = 0;
			if (WorldData[curPos] == BEDFRONT) WorldData[curPos - 1] = 0;
			WorldData[curPos] = 0;
			ticks[0] = 1;
		}
		if (kb_IsDown(kb_KeyGraphVar))
		{
			/* inventory */
			creativeInventory();
			ticks[0] = 1;
		}
		curX = curX + (16 * (kb_IsDown(kb_KeyStat) && (curX < 320)));
		curX = curX - (16 * (kb_IsDown(kb_KeyAlpha) && (curX > 0)));
		curPos = curPos + (kb_IsDown(kb_KeyStat) && (curX < 320));
		curPos = curPos - (kb_IsDown(kb_KeyAlpha) && (curX > 0));
		if ((kb_IsDown(kb_KeyAlpha)) || (kb_IsDown(kb_KeyStat)))
		{
			delay(100);
			ticks[0] = 1;
		}
		curY = curY + (16 * (kb_IsDown(kb_KeyApps) && (curY < 240)));
		curY = curY - (16 * (kb_IsDown(kb_KeyMode) && (curY > 0)));
		curPos = curPos + (worldLength * (kb_IsDown(kb_KeyApps) && (curY < 240)));
		curPos = curPos - (worldLength * (kb_IsDown(kb_KeyMode) && (curY > 0)));
		if ((kb_IsDown(kb_KeyApps)) || (kb_IsDown(kb_KeyMode)))
		{
			delay(100);
			ticks[0] = 1;
		}

		if (curX <= 0) curX += 16;
		if (curX >= 320) curX -= 16;
		if (curY <= 0) curY += 16;
		if (curY >= 240) curY += 16;

		if ((kb_IsDown(kb_KeyLeft)) && (playerX > 0))
		{
			ticks[0] = 1;
			if (scrollX == 0)
			{
				scrollX = -16;
				posX--;
				playerX--;
				curPos--;
				curX -= 16;
			}
			scrollX += 2;
			curX += 2;
		}
		if ((kb_IsDown(kb_KeyRight)) && (playerX < worldLength - 10))
		{
			ticks[0] = 1;
			if (scrollX == -16)
			{
				scrollX = 0;
				posX++;
				playerX++;
				curPos++;
				curX += 16;
			}
			scrollX -= 2;
			curX -= 2;
		}

		gfx_SetTextFGColor(0);
		gfx_SetTextXY(30, 20);
		gfx_PrintInt(seed, 1);
		gfx_SetTextXY(30, 40);
		gfx_PrintInt(256 - playerY, 1);
		gfx_SetTextFGColor(254);


		//if (timer != 0) timer++;
		//if (timer > 8) timer = 0;
		
		//if (kb_IsDown(kb_KeyUp) && (timer < 4)) timer = 0;

		if (kb_IsDown(kb_KeyVars))
		{
			gfx_SetTextFGColor(32);
			
			timer = 3;
			temptimer = 0;
			ticks[0] = 1;
			delay(100);
		}
		if (timer == 2) {
			temptimer++;
			if (flymode == 1)
			gfx_PrintStringXY("Flying mode toggled to On", 4, 20);
			if (flymode == 0)
			gfx_PrintStringXY("Flying mode toggled to Off", 4, 20);
			gfx_BlitBuffer();
			if (temptimer > 100)
			{
				temptimer = 0;
				timer = 0;
				ticks[0] = 1;
			}
		}
		if ((timer == 3) && (scrollX < 2))
		{
			flymode = (flymode == 0);
			timer = 2;
		}
		if (flymode == 1) jump = 0;

		if ((kb_IsDown(kb_KeyUp)) && (jump == 0) && (playerY > 0) && (WorldData[ playerX + 9 + ((playerY + 7) * worldLength) ] == 0))
		{
			ticks[0] = 1;
			//scrollY = 0;
			playerY--;
			curPos -= worldLength;
			jump = 1;
		}
		//swimming up in water...
		if ((kb_IsDown(kb_KeyUp)) && (WorldData[ playerX + 9 + ((playerY + 7) * worldLength) ] == WATER + 1))
		{
			ticks[0] = 1;
			scrollY += 4;
			curY += 4;
			if (scrollY > -1)
			{
				scrollY = -16;
				playerY--;
				curPos -= worldLength;
				curY -= 16;
			}
		}
		if ((kb_IsDown(kb_KeyDown)) && (playerY < worldHeight) && (WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] == 0))
		{
			ticks[0] = 1;
			if (flymode == 1)
			{
				scrollY -= 4;
				curY -= 4;
			}
			if (scrollY < -15)
			{
				scrollY = 0;
				playerY++;
				curPos += worldLength;
				curY += 16;
			}
		}

		//gravity, water, and lava (swimming? Don't swim in Lava!)...
		
		if ((WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] == WATER + 1) && (!kb_IsDown(kb_KeyUp)))
		{
			ticks[0] = 1;
			scrollY -= 4;
			curY -= 4;
			if (scrollY < -15)
			{
				scrollY = 0;
				playerY++;
				curPos += worldLength;
				curY += 16;
				jump = 0;
			}
		}

		if (timeofday % 6000) ticks[0] = 1;
		timeofday++;
		gfx_BlitBuffer();
	}

	//save the world data, playerX, playerY, playerPos, curPos, curX, curY, timeofday, etc...
	world_file = "        ";
	memcpy(world_file, worldNameStr, 8);

	dbg_sprintf(dbgout, "%s", world_file);

	if (appvar = ti_Open(world_file, "w"))
	{
		int world_offset;
		ti_Write("MCCESV", 6, 1, appvar);
		ti_Write((void *)0xFF0000, 3, 1, appvar); //this is overwritten later
		ti_Write(&worldLength, 3, 1, appvar);
		ti_Write(&worldHeight, 3, 1, appvar);
		ti_Write(&flymode, 3, 1, appvar);
		ti_Write(&gamemode, 3, 1, appvar);
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
		ti_Write(&health, 3, 1, appvar);
		ti_Write(&hunger, 3, 1, appvar);
		ti_Write(&exp, 3, 1, appvar);
		world_offset = ti_Tell(appvar);
		ti_Seek(6, SEEK_SET, appvar);
		ti_Write(&world_offset, 3, 1, appvar);
		ti_Seek(world_offset, SEEK_SET, appvar);
		compressAndWrite(&WorldData, worldLength * worldHeight, appvar);
		ti_SetArchiveStatus(1, appvar);
		ti_Close(appvar);
	}

	dbg_sprintf(dbgout, "%u", appvar);

	delay(100);
}


void creativeInventory(void)
{
	int24_t tab, scroll, redraw, selX, selY, posB, oldBlock, newBlock, timer, selXb, selYb, selPos, i, maxSize;
	char *names[4] = { "Nature", "Building", "Redstone", "Tools" };
	gfx_FillScreen(dayColors[timeofday / 6000]);
	gfx_SetTextFGColor(0);
	LoadBlocks("MCEDEFT");
	selX = 10;
	selY = 30;
	posB = 1;
	tab = 0;
	oldBlock = 0;
	newBlock = 0;
	timer = 0;
	redraw = 1;
	while (!(kb_IsDown(kb_KeyClear)))
	{
		if (redraw == 1)
		{
			timer = 0;
			redraw = 0;
			maxSize = typesvalues[tab];
			gfx_SetColor(181);
			gfx_FillCircle(10, 10, 5);
			gfx_FillCircle(309, 10, 5);
			gfx_FillCircle(10, 229, 5);
			gfx_FillCircle(309, 229, 5);
			gfx_FillRectangle(10, 5, 300, 230);
			gfx_FillRectangle(5, 10, 310, 220);
			i = (tab != 0);
			pos = 14 + ((tab != 0) * 20);
			for (x = pos; x < pos + (3 * 82); x += 82)
			{
				if (tab != i)
					gfx_SetColor(148);
				if (tab == i)
					gfx_SetColor(181);
				gfx_FillRectangle(x, 11, 82, 20);
				gfx_SetColor(0);
				gfx_Rectangle(x, 11, 82, 20);
				gfx_PrintStringXY(names[i], x + 18, 16);
				if (i == 0)
					gfx_TransparentSprite(sprites[natureBlocks[0]], x + 1, 12);
				if (i == 1)
					gfx_TransparentSprite(sprites[buildingBlocks[0]], x + 1, 12);
				if (i == 2)
					gfx_TransparentSprite(sprites[redstoning[0]], x + 1, 12);
				if (i == 3)
					gfx_TransparentSprite(sprites[toolsEtc[0]], x + 1, 12);
				i++;
			}
			gfx_SetColor(148);
			if (tab < 1) gfx_FillRectangle(x, 11, 20, 20);
			if (tab > 0) gfx_FillRectangle(14, 11, 20, 20);
			gfx_SetColor(0);
			if (tab < 1) gfx_Rectangle(x, 11, 20, 20);
			if (tab > 0) gfx_Rectangle(14, 11, 20, 20);
			gfx_SetColor(4);
			if (tab < 1) gfx_FillTriangle(x + 6, 15, x + 6, 25, x + 14, 20);
			if (tab > 0) gfx_FillTriangle(28, 15, 28, 25, 19, 20);
			pos = 0;
			
			gfx_SetTextFGColor(0);
			for (y = 30; y < 10 * 18; y += 18)
			{
				for (x = 10; x < 10 * 28; x += 18)
				{
					gfx_SetColor(148);
					gfx_FillRectangle(x, y, 18, 18);
					gfx_SetColor(0);
					gfx_Rectangle(x, y, 18, 18);
					if (pos < typesvalues[tab])
					{
						if (tab == 0)
							gfx_TransparentSprite(sprites[natureBlocks[pos++]], x + 1, y + 1);
						if (tab == 1)
							gfx_TransparentSprite(sprites[buildingBlocks[pos++]], x + 1, y + 1);
						if (tab == 2)
							gfx_TransparentSprite(sprites[redstoning[pos++]], x + 1, y + 1);
						if (tab == 3)
							gfx_TransparentSprite(sprites[toolsEtc[pos++]], x + 1, y + 1);
					}
				}
			}
			//hotbar
			for (x = 0; x < 5; x++)
			{
				if (x == hotbarSel)
				{
					gfx_SetColor(5);
				}
				else
				{
					gfx_SetColor(0);
				}
				gfx_Rectangle(117 + (x * 18), 210, 18, 18);
				if (hotbar[x] != 0)
				{
					gfx_TransparentSprite(sprites[hotbar[x]], 118 + (x * 18), 211);
				}
				else
				{
					gfx_SetColor(148);
					gfx_FillRectangle(118 + (x * 18), 211, 16, 16);
				}
			}
			if (newBlock != 0) gfx_TransparentSprite(sprites[newBlock], selX + 8, selY + 8);
			//cursor
			gfx_SetColor(254);
			gfx_FillRectangle(selX + 3, selY + 8, 5, 2);
			gfx_FillRectangle(selX + 10, selY + 8, 5, 2);
			
			gfx_FillRectangle(selX + 8, selY + 3, 2, 5);
			gfx_FillRectangle(selX + 8, selY + 10, 2, 5);

			gfx_BlitBuffer();
		}
		kb_Scan();

		if (kb_IsDown(kb_KeyYequ) && (tab > 0))
		{
			tab--;
			redraw = 1;
			delay(100);
		}
		if (kb_IsDown(kb_KeyGraph) && (tab < 3))
		{
			tab++;
			redraw = 1;
			delay(100);
		}

		if (selY == 210)
		{
			if (kb_IsDown(kb_Key2nd))
			{
				dbg_sprintf(dbgout, "%u", newBlock);
				oldBlock = hotbar[posB];
				hotbar[posB] = newBlock;
				newBlock = oldBlock;
				oldBlock = 0;
				redraw = 1;
				delay(100);
			}
		}
		else
		{
			if (kb_IsDown(kb_Key2nd) && (newBlock != 0))
			{

				newBlock = oldBlock;
				redraw = 1;
				delay(100);
			}
			if (kb_IsDown(kb_Key2nd) && (newBlock != 0) && (posB > maxSize))
			{

				newBlock = 0;
				redraw = 1;
				delay(100);
			}

			if (kb_IsDown(kb_Key2nd) && (newBlock == 0) && (posB < maxSize + 1))
			{
				if (tab == 0)
					newBlock = natureBlocks[posB - 1];
				if (tab == 1)
					newBlock = buildingBlocks[posB - 1];
				if (tab == 2)
					newBlock = redstoning[posB - 1];
				if (tab == 3)
					newBlock = toolsEtc[posB - 1];
				dbg_sprintf(dbgout, "%u", newBlock);
				redraw = 1;
				delay(100);
			}
		}

		if (selY < 9 * 18)
		{
			if ((kb_IsDown(kb_KeyLeft)) && (selX > 10))
			{
				selX -= 18;
				posB--;
				redraw = 1;
				delay(80);
			}
			if ((kb_IsDown(kb_KeyRight)) && (selX < 14 * 18))
			{
				selX += 18;
				posB++;
				redraw = 1;
				delay(80);
			}
		}
		else
		{
			if (kb_IsDown(kb_KeyLeft) && (selX > 117))
			{
				posB--;
				selX -= 18;
				redraw = 1;
				delay(80);
			}
			if (kb_IsDown(kb_KeyRight) && (selX < 189))
			{
				posB++;
				selX += 18;
				redraw = 1;
				delay(80);
			}
		}

		if (kb_IsDown(kb_KeyUp) && (selY == 210))
		{
			selX = selXb;
			selY = selYb - 18;
			posB = selPos - 15;
			redraw = 1;
			delay(80);
		}

		if ((kb_IsDown(kb_KeyUp)) && (selY > 30) && (selY < 10 * 18))
		{
			selY -= 18;
			posB -= 15;
			redraw = 1;
			delay(80);
		}
		if (kb_IsDown(kb_KeyDown))
		{
			if (selY < 210)
			{
				selY += 18;
				posB += 15;
				redraw = 1;
				delay(80);
			}
			if (selY == 30 + 9 * 18)
			{
				selXb = selX;
				selYb = selY;
				selPos = posB;
				selX = 117;
				selY = 210;
				posB = 0;
				redraw = 1;
				delay(80);
			}
		}
	}

	redraw = 1;
	delay(200);
}

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

void findAppvars(const char *str)
{

#define WORLD_IDENTIFIER str
// we have a limit of 10 appvars
#define WORLD_COUNT_LIMIT 10
	char *foundAppvar;
	void *searchPosition; // used by ti_Detect()
	// code for world scanning given by KryptonicDragon via cemetech.net
	// if you define this somewhere global instead of just in this function, you can also
	// use "WORLD_IDENTIFIER" in the code that saves worlds.
	//
	// also, this needs to be unique to your program so other programs don't mistake
	// your worlds for their appvars (or vice versa), but it also shouldn't be too long
	// since that slows down the search and takes up more space on everyone's calc (remember
	// that *every single* world appvar has this at the beginning of its data)

	// documentation states we must set this to NULL to start the search
	searchPosition = NULL;
	foundCount = 0;

	// note how the next line uses = and not ==, because we want to assign what ti_Detect()
	// returns into the foundAppvar variable. = is for assigning, == is for checking equality
	//
	// this loop will keep running so long as ti_Detect() find something. When it's done,
	// ti_Detect() will return NULL, which will get put into foundAppvar, and then
	// the condition "NULL != (NULL)" will become false and end the loop
	//
	// this loop also stops if we find too many programs (which is set by the "WORLD_COUNT_LIMIT"
	// define.
	/*
		while (foundCount < WORLD_COUNT_LIMIT && NULL != (foundAppvar = ti_Detect(&searchPosition, WORLD_IDENTIFIER))) {
			// now the foundAppvar variable has a C string of the name of an appvar 
			// that was found, and we know for sure it's a MC2D world.
				memcpy(WorldsList[foundCount], foundAppvar, 9);
				foundCount++;
		}*/
	for (;;)
	{
		foundAppvar = ti_Detect(&searchPosition, WORLD_IDENTIFIER);
		if (foundAppvar == NULL)
		{
			break;
		}

		memcpy(WorldsList[foundCount++], foundAppvar, 9);
		if (foundCount >= WORLD_COUNT_LIMIT)
		{
			break;
		}
	}
}

//Compress data via zx7 compression
void compressAndWrite(void *data, int len, ti_var_t fp)
{
	//	int i, j, k, num, num_i, new;
	//	uint8_t b, m;
	//	int *huffman_table = (void*)0xD52C00;
	//	uint8_t *huffman_table_2 = &huffman_table[256];
	//	uint8_t *huffman_output = &huffman_table_2[256];
	//	void *back_buffer = (void*)0xD52C00;
	//	uint8_t *ptr;
	int new_len;
	*((int *)0xE30010) = 0xD40000; //force the lcd to use the first half of VRAM so we can use the second half.
	gfx_SetDrawScreen();
	drawDirtBackground(0);
	gfx_SetTextFGColor(0xDF);
	gfx_PrintStringXY("Saving World...", 113, 110);

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

	zx7_Compress((void *)0xD52C00, data, &new_len, len);
	ti_Write((void *)0xD52C00, new_len, 1, fp);
	y = 125;
	gfx_End();
	MainMenu();
}
