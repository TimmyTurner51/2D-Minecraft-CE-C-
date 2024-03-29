////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE *lite* (C) v1.0.2a
//  Authors: TimmyTurner51, LogicalJoe, and Beckadamtheinventor
//  License: GNU GPL v3
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

//#include <stdio.h>
//#include <stdbool.h>
//#include <stddef.h>
//#include <stdint.h>
#include <tice.h>
#include <math.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <debug.h>

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

// include when multiplayer is reasonable
//#include <srldrvce.h>
//#include <usbdrvce.h>

#include "gfx/gfx.h"

#include <ctype.h>

#include "compressor.h" //zx7 compression routines
#include "defines.h"

//#define usb_callback_data_t usb_device_t

void LoadTextures(char *name, int16_t spritenumber, int16_t spritesize, int16_t arraynum);
void LoadResourcesScreen(void);
void DrawCenteredText(char *inputstr, int centerXpos, int ypos);
void DrawDirtBackground(int16_t scrollVal);
void Generator(void);
void Game(void);
void RenderEngine(void);
void deathScreen(void);
void Behaviors(int16_t position, int16_t timerPos);
void survivalInventory(void);
int craftingCheck(int16_t inputVal);
void giveItem(int16_t blockID, int16_t amount);
void creativeInventory(void);
void pauseMenu(void);
void input(char *string, int size);

// Amount of chunks to render (chunks are 16x16) max is 5 by 5 for now
int16_t renderX = 1, renderY = 1;

// Amount of Pixels to scroll. Ranges from 1 - 16. 1 being smooth but slowest, 16 being jumpy but fastest.
int16_t pixelAmount = 4;
// Amount of Pixels to scroll for falling/gravity (use 1, 2, 4, 6, 8, or 16)
int16_t gravityPixelAmount = 5;

int main(void)
{
	gfx_Begin();
	ti_CloseAll();
	gfx_SetClipRegion(-17, -17, 337, 257);
	gfx_SetDrawBuffer();
	gfx_SetTextFGColor(0);
	LoadResourcesScreen();
	MainMenu();
	// quit the game
	gfx_End();
	appvar = ti_Open("MCESETT", "w+");
	ti_Write(gameSettings, sizeof gameSettings, 1, appvar);
	ti_CloseAll();
}

void LoadTextures(char *name, int16_t spritenumber, int16_t spritesize, int16_t arraynum)
{
	uint8_t *ptr;
	if (spritenumber >= 0 && arraynum > 0 && arraynum <= 4) {
		ptr = os_GetAppVarData(name, NULL)->data;
		if (arraynum == 1) BlockTextures[spritenumber] = (gfx_sprite_t *)(ptr + (spritenumber * (spritesize + 2)));
		if (arraynum == 2) ItemTextures[spritenumber] = (gfx_sprite_t *)(ptr + (spritenumber * (spritesize + 2)));
		if (arraynum == 3) PlantTextures[spritenumber] = (gfx_sprite_t *)(ptr + (spritenumber * (spritesize + 2)));
		if (arraynum == 4) FoliageTextures[spritenumber] = (gfx_sprite_t *)(ptr + (spritenumber * (spritesize + 2)));
	}
}

void LoadResourcesScreen(void) {
	int16_t pos, num, speedVal = 1;
	appvar = ti_Open("MCESETT", "r");
	if (appvar) ti_Read(gameSettings, sizeof gameSettings, 1, appvar);
	ti_Close(appvar);
	if (gameSettings[1] > 0) {
		for (pos = 0; pos < gameSettings[1]; pos++) {
			speedVal = speedVal + speedVal;
		}
	}
	pixelAmount = speedVal;
	shadowing = gameSettings[2];
	// load languages/menu elements
	if (gameSettings[0] == 0) {
		memcpy(gamemodeStr, gamemodeStrEN, sizeof gamemodeStrEN);
		memcpy(worldSizeStr, worldSizeStrEN, sizeof worldSizeStrEN);
		memcpy(worldTypesStr, worldTypesStrEN, sizeof worldTypesStrEN);
		memcpy(togglesOnOff, togglesOnOffEN, sizeof togglesOnOffEN);
		memcpy(SpeedStr, SpeedStrEN, sizeof SpeedStrEN);
		memcpy(GameOptionStr, GameOptionStrEN, sizeof GameOptionStrEN);
		memcpy(MenuElements, MenuElementsEN, sizeof MenuElementsEN);
		memcpy(NewWorldMenuElements, NewWorldMenuElementsEN, sizeof NewWorldMenuElementsEN);
		memcpy(About, AboutEN, sizeof AboutEN);
	}
	if (gameSettings[0] == 1) {
		memcpy(gamemodeStr, gamemodeStrNL, sizeof gamemodeStrNL);
		memcpy(worldSizeStr, worldSizeStrNL, sizeof worldSizeStrNL);
		memcpy(worldTypesStr, worldTypesStrNL, sizeof worldTypesStrNL);
		memcpy(togglesOnOff, togglesOnOffNL, sizeof togglesOnOffNL);
		memcpy(SpeedStr, SpeedStrNL, sizeof SpeedStrNL);
		memcpy(GameOptionStr, GameOptionStrNL, sizeof GameOptionStrNL);
		memcpy(MenuElements, MenuElementsNL, sizeof MenuElementsNL);
		memcpy(NewWorldMenuElements, NewWorldMenuElementsNL, sizeof NewWorldMenuElementsNL);
		memcpy(About, AboutNL, sizeof AboutNL);
	}

	// load default textures
	gfx_FillScreen(224);
	// rectangle outline
	gfx_SetColor(255);
	gfx_Rectangle(157 - 2 * 30, 139, 4 * 30, 20);
	gfx_Rectangle(158 - 2 * 30, 140, 4 * 30 - 2, 18);
	for (pos = 1; pos < 5; pos++) {
		gfx_FillRectangle(160 - 2 * 30, 142, (pos * 30) - 6, 14);
		gfx_BlitBuffer();
		for (num = 0; num < MaxSprites[pos - 1]; num++) {
			if (pos == 1) LoadTextures("CLASSICB", num, 16 * 16, pos);
			if (pos == 2) LoadTextures("CLASSICI", num, 16 * 16, pos);
			if (pos == 3) LoadTextures("CLASSICP", num, 16 * 16, pos);
			if (pos == 4) LoadTextures("CLASSICF", num, 16 * 16, pos);
		}
	}
}

void DrawCenteredText(char *inputstr, int centerXpos, int ypos)
{
	gfx_PrintStringXY(inputstr, centerXpos - gfx_GetStringWidth(inputstr) / 2, ypos);
}

void Generator(void)
{
	uint16_t x = 0, y = 0, xb = 91, blockType = 0, terrainVal, biomeVal = 0;
	uint16_t xa, ya;
	uint16_t lengthA = 0, lengthB = 0, testX = 0, testY = 0;
	uint16_t groundLevel = 33, groundLevelB, pos = 0, posb = 0, posc = 0, posd = 0;
	memset(WorldData, 0, sizeof WorldData);
	DrawDirtBackground(0);
	gfx_SetTextFGColor(254);
	DrawCenteredText(MenuElements[19], 160, 90);
	DrawCenteredText(MenuElements[20], 160, 104);
	gfx_SetColor(148);
	gfx_FillRectangle(90, 120, 320 - 180, 7);
	gfx_SetColor(0);
	gfx_Rectangle(90, 120, 320 - 180, 7);
	gfx_SetColor(6);
	gfx_SetTextFGColor(0);
	// 0 = off, 1 = on
	flymode = 0;
	x = 0;

	while (x < MaxX)
	{
		gfx_VertLine(xb, 121, 5);
		gfx_BlitBuffer();
		if (xb < 228)
			xb++;
		// tries to further randomize the world, to prevent repetitive chunks
		if (x % 20) srand(rtc_Time() * x / 20);

		// hills
		if (worldType != 1) {
			// randomize biome
			// 0 = plains, 1 = desert, 2 = forest, 3 = pond/lake/ocean, 4 = plains village, 5 = sand village
			if (randInt(0, 8) == 0) biomeVal = randInt(0, 5);
			terrainVal = 1 + (randInt(0, 1) * randInt(0, 3) * (biomeVal != 3));
			if ((groundLevel > 1) && (groundLevel < MaxY) && (randInt(0, 12) != 1))
				groundLevel += (worldType != 1) * randInt(0 - terrainVal, terrainVal);
		}else{
			biomeVal = 0;
			groundLevel = 34;
		}

		if (genTrees == 1 && worldType != 1) {
			// tree generation
			if (biomeVal != 1 && randInt(0, 8 - ((biomeVal == 1) * 3)) == 0 && WorldData[x + (groundLevel * MaxX)] != 234)
			{
				// oak tree
				pos = groundLevel - 7;
				WorldData[x + ((pos + 2) * MaxX)] = 151;
				WorldData[x + ((pos + 3) * MaxX)] = 151;
				WorldData[x + ((pos + 4) * MaxX)] = 151;
				WorldData[x + ((pos + 5) * MaxX)] = 151;
				WorldData[x + ((pos + 6) * MaxX)] = 151;
				WorldData[x + ((pos + 7) * MaxX)] = 151;
				WorldData[x + (pos * MaxX)] = 222;
				WorldData[x - 1 + (pos * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
				WorldData[x + 1 + (pos * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
				WorldData[x + ((pos + 1) * MaxX)] = 222;
				WorldData[x - 1 + ((pos + 1) * MaxX)] = 222;
				WorldData[x + 1 + ((pos + 1) * MaxX)] = 222;
				WorldData[x + ((pos + 2) * MaxX)] = 222;
				WorldData[x - 1 + ((pos + 2) * MaxX)] = 222;
				WorldData[x - 1 + ((pos + 3) * MaxX)] = 222;
				WorldData[x - 2 + ((pos + 2) * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
				WorldData[x - 2 + ((pos + 3) * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
				WorldData[x + 1 + ((pos + 2) * MaxX)] = 222;
				WorldData[x + 1 + ((pos + 3) * MaxX)] = 222;
				WorldData[x + 2 + ((pos + 2) * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
				WorldData[x + 2 + ((pos + 3) * MaxX)] = (222 * ((randInt(0, 3) == 1) * WorldData[x - 2 + ((pos + 2) * MaxX)] == 0));
			}
			if (biomeVal != 1 && (randInt(0, 8 - ((biomeVal == 1) * 3)) == 0) && (WorldData[x + (groundLevel * MaxX)] != 234))
			{
				// spruce
				pos = groundLevel - 6;
				WorldData[x + (pos * MaxX)] = 223;
				WorldData[x + ((pos + 1) * MaxX)] = 223;
				WorldData[x - 1 + ((pos + 1) * MaxX)] = 223;
				WorldData[x + 1 + ((pos + 1) * MaxX)] = 223;
				WorldData[x + ((pos + 2) * MaxX)] = 199;
				WorldData[x + ((pos + 3) * MaxX)] = 199;
				WorldData[x + ((pos + 4) * MaxX)] = 199;
				WorldData[x + ((pos + 5) * MaxX)] = 199;
				if (randInt(0, 3) == 1)
				{
					WorldData[x - 1 + ((pos + 3) * MaxX)] = 223;
					WorldData[x + 1 + ((pos + 3) * MaxX)] = 223;
					WorldData[x - 1 + ((pos + 5) * MaxX)] = 223;
					WorldData[x + 1 + ((pos + 5) * MaxX)] = 223;
					WorldData[x - 2 + ((pos + 5) * MaxX)] = 223;
					WorldData[x + 2 + ((pos + 5) * MaxX)] = 223;
				}
			}
			// jungle = 118
			// dark oak = 72
			// acacia = 3
		}

		// foliage start at MaxSprites[0] + 1
		// plants start at MaxSprites[0] + MaxSprites[3] + 1
		// flowers (anything non-desert)
		if (genFlowers == 1 && (biomeVal == 0 || biomeVal == 2) && randInt(0, 2) == 0 && WorldData[x + ((groundLevel - 1) * MaxX)] == 0)
		{
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 2;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 3;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 9;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 19;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 23;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 31;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 37;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 38;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 41;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 42;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 48;
			// berries
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 56;
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 74;		// 73
			// rose bush
			if (randInt(0, 2) == 0)
			{
				if (randInt(0, 2) == 0)
				{
					WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 49;
					WorldData[x + ((groundLevel - 2) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 50;
				}
				else
				{
					WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 50;
				}
			}
			if (randInt(0, 2) == 0)
			{
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 29;
				WorldData[x + ((groundLevel - 2) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 30;
			}
			if (randInt(0, 2) == 0)
			{
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 39;
				WorldData[x + ((groundLevel - 2) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 40;
			}
		}
		// desert foliage (dead shrubs, cactus, etc.)
		if (biomeVal == 1 && randInt(0, 2) == 0 && WorldData[x + ((groundLevel - 1) * MaxX)] == 0)
		{
			// dead shrubs
			if (randInt(0, 2) == 0)
				WorldData[x + ((groundLevel - 1) * MaxX)] = MaxSprites[0] + MaxSprites[3] + 25;
			// cactus
			if (randInt(0, 2) == 0)
			{
				for (terrainVal = groundLevel - 1 - randInt(0, 2); terrainVal < groundLevel; terrainVal++)
				{
					WorldData[x + (terrainVal * MaxX)] = 40;
				}
			}
		}

		
		// Village Generation
		lengthA = randInt(3, 7);
		if (genVillages == 1 && biomeVal == 4 && (groundLevel >= 34 || worldType == 1) && x < MaxX - lengthA * 7)
		{
			// 7 is the size of a 5-block long/small house, plus 2 for 1 block ground spacing on each side
			// we can generate wider buildings, that are 12 blocks long + 2 for ground blocks (again, 1 on each side)
			lengthB = 0;
			posd = 8;
			// left-right
			for (posb = x; posb < x + lengthA * 7; posb++)
			{
				// up-down
				for (posc = groundLevel; posc < groundLevel + posd; posc++)
				{
					WorldData[posb + (posc * MaxX)] = 0;
					// gravel path
					if (posc == groundLevel)
						WorldData[posb + (posc * MaxX)] = 84;
					// dirt
					if (posc > groundLevel)
						WorldData[posb + (posc * MaxX)] = 78;
				}
				// houses
				// small house (5x5)
				// Oak planks (ID is 152)
				if (posb >= (x + (lengthA * 7)) + (lengthB + 1) && posb <= (x + (lengthA * 7)) + (lengthB + 5)) {
					// left wall
					WorldData[(lengthB + 1) + ((groundLevel) * MaxX)] = 151;
					WorldData[(lengthB + 1) + ((groundLevel - 1) * MaxX)] = 151;
					WorldData[(lengthB + 1) + ((groundLevel - 2) * MaxX)] = 151;
					WorldData[(lengthB + 1) + ((groundLevel - 3) * MaxX)] = 151;
					WorldData[(lengthB + 1) + ((groundLevel - 4) * MaxX)] = 151;
					// right wall
					WorldData[(lengthB + 5) + ((groundLevel) * MaxX)] = 151;
					WorldData[(lengthB + 5) + ((groundLevel - 1) * MaxX)] = 151;
					WorldData[(lengthB + 5) + ((groundLevel - 2) * MaxX)] = 151;
					WorldData[(lengthB + 5) + ((groundLevel - 3) * MaxX)] = 151;
					WorldData[(lengthB + 5) + ((groundLevel - 4) * MaxX)] = 151;
					// roof
					WorldData[(lengthB + 2) + ((groundLevel - 4) * MaxX)] = 152;
					WorldData[(lengthB + 3) + ((groundLevel - 4) * MaxX)] = 152;
					WorldData[(lengthB + 4) + ((groundLevel - 4) * MaxX)] = 152;
					// fill
					for (testX = lengthB + 2; testX < lengthB + 5; testX++) {
						for (testY = 0; testY < 4; testY++) {
							WorldData[testX + ((groundLevel - testY) * MaxX)] = ((testY == 0) * 54);
						}
					}
					// randomize for chest, crafting table, and/or bed
						testX = randInt(0, 4);
					// chest
					if (testX == 0) {
						WorldData[(lengthB + 4) + ((groundLevel - 1) * MaxX)] = 9;
						// add more later for its contents
					}
					// crafting table
					if (testX == 1) {
						WorldData[(lengthB + 4) + ((groundLevel - 1) * MaxX)] = 58;
					}
					// bed
					if (testX == 2) {
						WorldData[(lengthB + 2) + ((groundLevel - 1) * MaxX)] = 7;
						WorldData[(lengthB + 3) + ((groundLevel - 1) * MaxX)] = 8;
						testX = randInt(0, 3);
						// crafting table
						if (testX == 0) {
							WorldData[(lengthB + 4) + ((groundLevel - 1) * MaxX)] = 58;
						}
						// chest
						if (testX == 1) {
							WorldData[(lengthB + 4) + ((groundLevel - 1) * MaxX)] = 58;
							// add more later for it's contents
						}
					}
				}
				// large house (12x5)


				if (posb == (x + (lengthA * 7)) + lengthB + 7) lengthB += 7;
			}
		}

		// cave generation
		if (genCaves == 1 && randInt(0, 2) == 1 && x >= 21) {
			for (xa = x - 20; xa < x - 20 + randInt(6, 19); xa++) {
				for (ya = groundLevel + 10 + randInt(1, 4); ya < groundLevel + 10 + randInt(3, 6); ya++) {
					WorldData[xa + ya * MaxX] = 0;
				}
			}
		}

		// water generation
		if ((biomeVal == 3 || randInt(1, 3) == 2) && groundLevel >= 34 && worldType != 1)
		{
			groundLevelB = groundLevel;
			// pos is left-to-right size
			lengthA = randInt(6, 20);
			for (posb = x - lengthA; posb < x; posb++)
			{
				// generates up to down
				for (posc = 0; posc < groundLevel - posd + randInt(2, 5); posc++)
				{
					WorldData[posb + (posc * MaxX)] = 0;
					if (posc >= groundLevel)
						WorldData[posb + (posc * MaxX)] = 215;
				}
			}
			groundLevel = groundLevelB;
		}

		// terrain
		for (y = groundLevel; y < MaxY; y++)
		{
			// grass block
			blockType = 95;
			// dirt
			if (y > groundLevel)
				blockType = 78;
			// desert
			if (biomeVal == 1 && biomeVal != 5)
			{
				// sand
				blockType = 186;
				// sandstone
				if (y > groundLevel + randInt(0, 4))
					blockType = 187;
			}
			// stone
			if (y > groundLevel + 4)
				blockType = 202;
			// ores
			if ((y > groundLevel + 10) && (y < groundLevel + 80) && (randInt(0, 10) == 0))
				blockType = 52;
			if ((y > groundLevel + 18) && (y < groundLevel + 80) && (randInt(0, 10) == 0))
				blockType = 51;
			if ((y > groundLevel + 100) && (y < groundLevel + 130) && (randInt(0, 20) == 0))
				blockType = 76;
			if ((y > groundLevel + 130) && (y < groundLevel + 140) && (randInt(0, 20) == 0))
				blockType = 83;
			WorldData[x + (y * MaxX)] = blockType;
		}
		// bedrock
		WorldData[x + ((y - 1) * MaxX)] = 15;

		x++;
	}
	playerX = 10;
	playerY = 0;
	curPos = MaxX + 11;
	curX = 11;
	curY = 6;
	health = 18;
	hunger = 18;
	damageAmount = 0;
	damageDealt = 0;
	while (WorldData[(playerX + ((playerY + 3) * MaxX))] == 0) {
		playerY++;
		curPos += MaxX;
		WorldTimerPosY++;
		if (WorldTimerPosY > 32) WorldTimerPosY = 0;
	}
	scrollX = 0;
	scrollY = 0;

	hotbar[0] = 215;
	hotbar[5] = 32;

}

void Game(void)
{
	int16_t gravityVar = gravityPixelAmount, listPosition = 0;
	int16_t blockAboveHead = 0, blockAtCenter = 0;
	int16_t blockLeftCenter = 0, blockLeftBottom = 0;
	int16_t blockRightCenter = 0, blockRightBottom = 0;
	int16_t blockAtFeet = 0, blockBelowFeet = 0;
	int16_t Up_Count, rtc_Up;
	gfx_SetClipRegion(0 - 17, 0 - 17, 337, 257);

	while (!(kb_IsDown(kb_KeyClear)))
	{
		kb_Scan();

		RenderEngine();

		if (gameSettings[3] == 1) {
			// debug
			gfx_SetTextFGColor(0);
			gfx_SetTextXY(25, 20);
			gfx_PrintString("X:");
			gfx_PrintInt(playerX, 1);
			gfx_SetTextXY(98, 20);
			gfx_PrintString("Y:");
			gfx_PrintInt(playerY, 1);
			gfx_SetTextXY(25, 30);
			gfx_PrintString("FPS:");
			gfx_PrintInt(fps, 1);
			gfx_SetTextXY(85, 30);
			gfx_PrintString("Block_AtCursor:");
			gfx_PrintInt(WorldData[curPos], 1);
		}

		// cursor
		if (timeofday <= 2) gfx_SetColor(0);
		if (timeofday > 2 || WorldData[curPos] != 0) gfx_SetColor(148);
		gfx_Rectangle(scrollX + (curX * 16), scrollY + (curY * 16), 16, 16);

		// add health, hunger, and exp bars for survival and adventure modes
		if (gamemode == 0 || gamemode == 2) {
			gfx_SetTransparentColor(148);
			y = 0;
			for (x = 4; x < 9 * 8; x += 8) {
				if (y > health) {
				gfx_TransparentSprite(heart_empty, x, 4);
				}else{
					if (y <= health)
					gfx_TransparentSprite(heart_full, x, 4);
					if (!(y % 2) && y == health)
					gfx_TransparentSprite(heart_half, x, 4);
				}
				y += 2;
			}
			gfx_SetTransparentColor(7);
		}

		if (health <= 0) deathScreen();

		// dialog/pop up text
		if (dialog != 0)
		{
			gfx_SetColor(181);
			gfx_FillRectangle(2, 2, 316, 14);
			gfx_SetTextFGColor(0);
			gfx_PrintStringXY(dialogString, 6, 6);
		}


		blockAboveHead =  WorldData[playerX + (playerY * MaxX)];
		blockLeftCenter =  WorldData[playerX - 1 + ((playerY + 1) * MaxX)];
		blockLeftBottom =  WorldData[playerX - 1 + ((playerY + 2) * MaxX)];
		blockRightCenter =  WorldData[playerX + 1 + ((playerY + 1) * MaxX)];
		blockRightBottom =  WorldData[playerX + 1 + ((playerY + 2) * MaxX)];
		blockAtCenter = WorldData[playerX + ((playerY + 1) * MaxX)]; 
		blockAtFeet =  WorldData[playerX + ((playerY + 2) * MaxX)];
		blockBelowFeet =  WorldData[playerX + ((playerY + 3) * MaxX)];

		if (kb_IsDown(kb_KeyStat) || kb_IsDown(kb_KeyAlpha) || kb_IsDown(kb_KeyApps) || kb_IsDown(kb_KeyMode)) {
			delay(100);
			curPos += (kb_IsDown(kb_KeyStat) && curX < 20) - (kb_IsDown(kb_KeyAlpha) && curX > 1);
			curX += (kb_IsDown(kb_KeyStat) && curX < 20) - (kb_IsDown(kb_KeyAlpha) && (curX > 1));
			curPos += ((kb_IsDown(kb_KeyApps) && curY < 15) * MaxX) - ((kb_IsDown(kb_KeyMode) && curY > 1) * MaxX);
			curY += (kb_IsDown(kb_KeyApps) && curY < 15) - (kb_IsDown(kb_KeyMode) && curY > 1);
		}

		if (kb_IsDown(kb_KeyGraphVar)) {
			if (gamemode == 0 || gamemode == 2) survivalInventory();
			if (gamemode == 1) creativeInventory();
		}else{
			// revamped hotbar
			// (update: Disappears when entering the inventory, becuase this was visible even when overlayed)
			gfx_SetColor(149);
			gfx_SetTextFGColor(254);
			gfx_Rectangle(116, 218, 5 * 18 + 2, 20);
			for (x = 0; x < 5; x++) {
				gfx_SetColor(149);
				if (x == hotbarSel) gfx_SetColor(74);
				gfx_Rectangle(117 + (x * 18), 219, 18, 18);
				gfx_Rectangle(118 + (x * 18), 220, 17, 17);
				if (hotbar[x] != 0) {
					if (hotbar[x] <= MaxSprites[0])
						gfx_TransparentSprite(BlockTextures[hotbar[x] - 1], 119 + (x * 18), 221);
					// check if it's foliage
					if (hotbar[x] > MaxSprites[0] && hotbar[x] <= MaxSprites[0] + MaxSprites[3])
						gfx_TransparentSprite(FoliageTextures[hotbar[x] - 1 - MaxSprites[0]], 119 + (x * 18), 221);
					// check if it's plants
					if (hotbar[x] > MaxSprites[0] + MaxSprites[3] && hotbar[x] <= MaxSprites[0] + MaxSprites[3] + MaxSprites[2])
						gfx_TransparentSprite(PlantTextures[hotbar[x] - 1 - (MaxSprites[0] + MaxSprites[3])], 119 + (x * 18), 221);
				}
			}
			if (gamemode == 0 || gamemode == 2) {
				for (x = 0; x < 5; x++) {
					gfx_SetTextXY(117 + (x * 18) + 10, 231);
					if (hotbar[x] != 0) gfx_PrintInt(hotbar[x + 5], 1);
				}
			}
		}

		if (kb_IsDown(kb_KeyYequ)) hotbarSel = 0;
		if (kb_IsDown(kb_KeyWindow)) hotbarSel = 1;
		if (kb_IsDown(kb_KeyZoom)) hotbarSel = 2;
		if (kb_IsDown(kb_KeyTrace)) hotbarSel = 3;
		if (kb_IsDown(kb_KeyGraph)) hotbarSel = 4;


		if (dialog != 0) {
			dialogTimer--;
			if (dialogTimer == 0) dialog = 0;
		}

		// gives max count (64) to selected block/item in hotbar if in creative mode
		if (gamemode == 1 && hotbar[hotbarSel] != 0) hotbar[hotbarSel + 5] = 64;

		if (kb_IsDown(kb_Key2nd) && hotbar[hotbarSel] != 0 && hotbar[hotbarSel + 5] > 0)
		{
			if (WorldData[curPos] == 0 || WorldData[curPos] == 233 || WorldData[curPos] >= WATERENTITY) {
				WorldData[curPos] = hotbar[hotbarSel];
				hotbar[hotbarSel + 5]--;
				if (hotbar[hotbarSel + 5] < 1) hotbar[hotbarSel] = 0;
			}
		}
		if (kb_IsDown(kb_KeyDel) && WorldData[curPos] != 0) {
			WorldData[curPos] = 0;
			// add code here for survival and adventure (block breaks)
			giveItem(WorldData[curPos], 1);
		}

		if (kb_IsDown(kb_KeyMath))
		{
			input(dialogString, 40);
			dialogTimer = 100;
			dialog = 1;
			// 0 = survival, 1 = creative, 2 = adventure
		}
		// allows movement in a 1-block wide area
		if (kb_IsDown(kb_KeyLeft) && playerX > 0 && (blockLeftCenter != 0) && scrollX < 0) scrollX += pixelAmount;
		if (kb_IsDown(kb_KeyRight) && playerX < MaxX && (blockRightCenter != 0) && scrollX > -16 + 6) scrollX -= pixelAmount;
		// standard left/right movement
		if (kb_IsDown(kb_KeyLeft) && playerX > 0 && (blockLeftCenter == 0 || blockLeftCenter > MaxSprites[0])) scrollX += pixelAmount;
		if (kb_IsDown(kb_KeyRight) && playerX < MaxX && (blockRightCenter == 0 || blockRightCenter > MaxSprites[0])) scrollX -= pixelAmount;
		if (flymode == 1) {
			if (kb_IsDown(kb_KeyUp) && playerY > 5 && blockAboveHead == 0) scrollY += pixelAmount;
			if (kb_IsDown(kb_KeyDown) && playerY < MaxY && blockBelowFeet == 0) scrollY -= pixelAmount;
			// try fixing the scrollY var glitch when you jump and noclip into a block
			if (scrollY < 0 && jump == 1 && flymode == 0 && (WorldData[(playerX + ((playerY + 1) * MaxX))] != 0 || WorldData[(playerX + ((playerY + 2) * MaxX))] != 0))
			{
				scrollY = 0;
				curY--;
				curPos -= MaxX;
				jump = 0;
			}
		}else{
			// auto jumps on a block
			if ((playerY > 0) && (jump == 0) && (blockAtFeet > 0) && (blockAtFeet - 2 < MaxSprites[0] + 4) && (blockAtFeet != 233) && (blockAtFeet != WATERENTITY))
			{
				curPos -= MaxX;
				scrollY = 0;
				playerY--;
				WorldTimerPosY--;
				// need to overwrite this variable so gravity isn't buggy after auto-jumping
				blockBelowFeet = 1;
			}
			if (kb_IsDown(kb_KeyUp) && playerY > 5 && blockAboveHead == 0 && jump == 0) {
				jump = 1;
				scrollY += 16;
				curY++;
				playerY--;
				WorldTimerPosY--;
			}else{
				if ((blockBelowFeet == 0 || blockAtFeet >= WATERENTITY) && jump != 1) {
					scrollY -= pixelAmount;
					if (scrollY <= -16) {
						playerY++;
						WorldTimerPosY++;
						curPos += MaxX;
						scrollY = 0;
						jump = 0;
					}
				}
				if (jump == 1) {
					scrollY -= pixelAmount;
					if (scrollY <= -16) {
						playerY++;
						curY--;
						WorldTimerPosY++;
						scrollY = -16;
						jump = 0;
					}
				}
			}
		}
		playerX += (scrollX < -16) - (scrollX > 0);
		playerY += (scrollY < -16) - (scrollY > 0);
		WorldTimerPosX += (scrollX < -16) - (scrollX > 0);
		WorldTimerPosY += (scrollY < -16) - (scrollY > 0);
		if (scrollX > 0) {
			curPos--;
			//scrollX = -16 + (pixelAmount - -scrollX);
			scrollX -= 16;
			WorldTimerPosX--;
		}
		if (scrollX < -16) {
			curPos++;
			//scrollX = (scrollX + pixelAmount) + 16;
			scrollX += 16;
			WorldTimerPosX++;
		}
		if (scrollY > 0) {
			curPos -= MaxX;
			//scrollY = -16 + (pixelAmount - -scrollY);
			scrollY -= 15;
			WorldTimerPosY--;
		}
		if (scrollY < -16) {
			curPos += MaxX;
			//scrollY = (scrollY + pixelAmount) + 16;
			scrollY += 16;
			WorldTimerPosY++;
		}


		if (gamemode != 0 && gamemode != 2) {
			// double up to toggle fly mode
			if (kb_IsDown(kb_KeyUp)) {
				if (Up_Count == 1 && rtc_Up + 1 == rtc_Seconds) {
					rtc_Up = rtc_Seconds;
					timer = 0;
					dialogTimer = 50;
					dialog = 1;
					flymode = (flymode == 0);
					strcpy(dialogString, "Fly Mode toggled Off");
					if (flymode == 1)
						strcpy(dialogString, "Fly Mode toggled On");
				}
				Up_Count = (Up_Count == 0);
				if (Up_Count == 0) rtc_Up = rtc_Seconds;
			}
			// turn off fly mode if you touch the ground
			if (kb_IsDown(kb_KeyDown) && flymode == 1 && playerY < MaxY - 15 && WorldData[(playerX + ((playerY + 3) * MaxX))] != 0 && WorldData[(playerX + ((playerY + 3) * MaxX))] != 233 && WorldData[(playerX + ((playerY + 3) * MaxX))] != WATERENTITY)
				flymode = 0;
		}

		gfx_BlitBuffer();
	}
	kb_Scan();
	pauseMenu();
}

void RenderEngine(void)
{
	int16_t blockVal = 0, blockValLeft = 0, blockValRight = 0, blockValTop = 0, blockValBottom = 0;
	int16_t testX, testY, timerX, timerY;
	int16_t counter = 0, second_Last = rtc_Seconds;
	// draw sky
	// try gfx_Darken(colorValue) as time var increments
	// so maybe day/night transitioning would appear smoother
	// speedup by filling screen red and overwriting if no damage was dealt
	gfx_FillScreen(224);
	if (health >= 0)
	gfx_FillScreen(dayColors[timeofday]);
	damageDealt = 0;
	dayTimer++;
	if (dayTimer >= 500)
	{
		timeofday++;
		dayTimer = 0;
	}
	if (timeofday > 4)
		timeofday = 0;

	testX = playerX - 10;
	testY = playerY - 5;
	if (WorldTimerPosX > 32) WorldTimerPosX = 0;
	if (WorldTimerPosY > 32) WorldTimerPosY = 0;
	timerX = WorldTimerPosX;
	timerY = WorldTimerPosY;
	drawX = scrollX;
	drawY = scrollY;
	for (render = 0; render < 21 * 16; render++)
	{
		gfx_SetColor(0);
		pos = testX + testY * MaxX;
		// draw the shadowing box (not for water, lava, etc.)
		blockVal = WorldData[pos];
		blockValLeft = WorldData[pos - 1];
		blockValRight = WorldData[pos + 1];
		blockValTop = WorldData[pos - MaxX];
		blockValBottom = WorldData[pos + MaxX];
		// overrides shadowing for lava, water, and any values above or equal to what WATERENTITY is defined as.
		if (blockVal == 232 || blockVal == 233 || blockVal >= WATERENTITY)
			blockValTop = 0;
		if ((blockVal != 0) && testX >= 0 && testX <= MaxX - 10 && ((shadowing == 0) || (shadowing != 0 && ((blockValLeft == 0 || blockValRight == 0 || blockValTop == 0 || blockValBottom == 0) || (blockValLeft > MaxSprites[0] || blockValRight > MaxSprites[0] || blockValTop > MaxSprites[0] || blockValBottom > MaxSprites[0])))))
		{
			// check if it's a block
			if (blockVal <= MaxSprites[0])			// WorldData[pos] - 1
				gfx_TransparentSprite(BlockTextures[blockVal - 1], drawX, drawY);
			// check if it's foliage
			if (blockVal >= MaxSprites[0] && blockVal <= MaxSprites[0] + MaxSprites[3])
				gfx_TransparentSprite(FoliageTextures[blockVal - 1 - MaxSprites[0]], drawX, drawY);
			// check if it's plants
			if (blockVal >= MaxSprites[0] + MaxSprites[3] && blockVal <= MaxSprites[0] + MaxSprites[3] + MaxSprites[2])
				gfx_TransparentSprite(PlantTextures[blockVal - 1 - (MaxSprites[0] + MaxSprites[3])], drawX, drawY);
			// load water sprite if neccessary
			if (blockVal >= WATERENTITY && blockVal <= WATERENTITY + 7)
				gfx_TransparentSprite(BlockTextures[214], drawX, drawY);
			gfx_SetColor(dayColors[timeofday]);
			if (blockVal >= WATERENTITY && blockVal <= WATERENTITY + 7)
				gfx_FillRectangle(drawX, drawY, 16, (blockVal - WATERENTITY) * 2);
		}
		gfx_SetColor(0);
		Behaviors(pos, timerX + timerY * 32);
		WorldDataTimer[timerX + timerY * 32] -= WorldDataTimer[timerX + timerY * 32] > 0;
		// shading
		BlockLightVals[timerX + timerY * 32] = 0;
		if (BlockLightVals[timerX + timerY * 32] == 0 && blockVal != 0 && blockVal <= MaxSprites[0] && (blockValLeft != 0 && blockValRight != 0 && blockValTop != 0 && blockValBottom != 0))
			BlockLightVals[timerX + timerY * 32] = lightVal;
		/*if (BlockLightVals[timerX + timerY * 32] > 0 && blockVal != 0 && blockVal <= MaxSprites[0]) {
			if (blockValTop != 0 && BlockLightVals[timerX + timerY * 32] < BlockLightVals[timerX + ((timerY - 1) * 32)]) 
				BlockLightVals[timerX + timerY * 32]--;
		}*/

		if (BlockLightVals[timerX + timerY * 32] > 0)
		gfx_FillRectangle(drawX, drawY, 16, BlockLightVals[timerX + timerY * 32]);
		// fps counter
		counter++;
		if (rtc_Seconds >= second_Last) {
			second_Last = rtc_Seconds;
			fps = counter; // + (pixelAmount - shadowing)
			counter = 0;
		}

		drawX += 16;
		testX++;
		timerX++;
		if (timerX > 32) timerX = 0;
		if (testX == playerX + 11) {
			testY++;
			timerY++;
			if (timerY > 32) timerY = 0;
			testX = playerX - 10;
			timerX = WorldTimerPosX;
			drawX = scrollX;
			drawY += 16;
		}
	}

	// player
	gfx_TransparentSprite(Head_1, 16 * 9 + 9 + 8, 16 * 5 + 15);
	gfx_TransparentSprite(Body_1, 16 * 9 + 11 + 8, 16 * 5 + 23);
	gfx_TransparentSprite(Leg_1, 16 * 9 + 11 + 8, 16 * 5 + 35);
}

void deathScreen(void)
{
	int16_t y;
	// death screen
	for (y = 100; y < 140; y += 20) {
		gfx_SetColor(148);
		gfx_FillRectangle(40, y, 240, 15);
		gfx_SetColor(74);
		gfx_Rectangle(40, y, 240, 15);
		gfx_Rectangle(41, y + 1, 238, 13);
	}
	y = 100;
	while (!(kb_IsDown(kb_KeyClear))) {
		kb_Scan();
		DrawCenteredText(MenuElements[19], 169, 105);
		DrawCenteredText(MenuElements[18], 160, 125);
		gfx_SetColor(254);
		if ((kb_IsDown(kb_KeyUp) && y != 100) && (kb_IsDown(kb_KeyDown) && y != 120))
			gfx_SetColor(74);
		gfx_Rectangle(40, y, 240, 15);
		gfx_Rectangle(41, y + 1, 238, 13);
		if (kb_IsDown(kb_KeyUp)) y = 100;
		if (kb_IsDown(kb_KeyDown)) y = 120;
		gfx_BlitBuffer();
	}
}

void Behaviors(int16_t position, int16_t timerPos)
{

	// grass turns to dirt

	// remove flowing down water when source was removed
	if (WorldData[position] == WATERENTITY && WorldData[position - MaxX] != 215 && WorldData[position - MaxX] < WATERENTITY && WorldDataTimer[timerPos - MaxX] < 1) {
		WorldData[position] = 0;
		WorldDataTimer[timerPos] = 3;
	}
	if (WorldData[position] >= WATERENTITY && WorldData[position - MaxX] == 0 && WorldData[position - 1] == 0 && WorldData[position + 1] == 0 && WorldDataTimer[timerPos - MaxX] < 1) {
		WorldData[position] = 0;
		WorldDataTimer[timerPos] = 3;
	}
	// water flows sideways
	if (WorldData[position + MaxX] != 0 && WorldData[position + MaxX] < WATERENTITY && WorldDataTimer[timerPos] < 1) {
		if (WorldData[position] == WATERENTITY + 7 && WorldData[position - 1] < WATERENTITY && WorldData[position + 1] < WATERENTITY)
			WorldData[position] = 0;
		if ((WorldData[position - 1] == 0 || WorldData[position - 1] == WATERENTITY || WorldData[position - 1] >= MaxSprites[0] + MaxSprites[3] + 1) && WorldDataTimer[timerPos] < 1 && WorldData[position] >= WATERENTITY && WorldData[position] < WATERENTITY + 7 && WorldData[position - 1] + 2 <= WorldData[position]) {
			if (WorldData[position - 1] >= MaxSprites[0] + MaxSprites[3] + 1)
				giveItem(WorldData[position - 1], 0);
			WorldData[position - 1] = WorldData[position] + 1;
			WorldDataTimer[timerPos - 1] = 3;
		}
		if ((WorldData[position + 1] == 0 || WorldData[position + 1] == WATERENTITY || WorldData[position + 1] >= MaxSprites[0] + MaxSprites[3] + 1) && WorldDataTimer[timerPos] < 1 && WorldData[position] >= WATERENTITY && WorldData[position] < WATERENTITY + 7 && WorldData[position + 1] + 2 <= WorldData[position]) {
			if (WorldData[position - 1] >= MaxSprites[0] + MaxSprites[3] + 1)
				giveItem(WorldData[position + 1], 0);
			WorldData[position + 1] = WorldData[position] + 1;
			WorldDataTimer[timerPos + 1] = 3;
		}
	}
	// removal of water (flowing sideways)
	if (WorldData[position] > WATERENTITY && WorldData[position] < WATERENTITY + 7 && WorldData[position] > WorldData[position - 1] + 1 && WorldData[position] > WorldData[position - MaxX] && WorldData[position] < WorldData[position + 1] + 1)
		WorldData[position]++;
	if (WorldData[position] > WATERENTITY && WorldData[position] < WATERENTITY + 7 && WorldData[position] < WorldData[position - 1] + 1 && WorldData[position] > WorldData[position - MaxX] && WorldData[position] > WorldData[position + 1] + 1)
		WorldData[position]++;
	if (WorldData[position] > WATERENTITY && WorldData[position] < WATERENTITY + 7 && WorldData[position - 1] >= WorldData[position] && WorldData[position + 1] >= WorldData[position])
		WorldData[position]++;
	if (WorldData[position] == WATERENTITY + 7 && (WorldData[position - 1] == 0 || WorldData[position - 1] == WorldData[position]) && (WorldData[position + 1] == 0 || WorldData[position - 1] == WorldData[position]) && WorldData[position - MaxX] < WorldData[position]) {
		WorldData[position] = 0;
		WorldDataTimer[timerPos] = 3;
	}
	// water flows downward
	if ((WorldData[position] == 215 || (WorldData[position] >= WATERENTITY && WorldData[position] <= WATERENTITY + 7)) && (WorldData[position + MaxX] == 0 || WorldData[position + MaxX] >= MaxSprites[0] + MaxSprites[3] + 1) && WorldDataTimer[timerPos] < 1) {
		if (WorldData[position + MaxX] >= MaxSprites[0] + MaxSprites[3] + 1 && WorldData[position] < WATERENTITY)
			giveItem(WorldData[position], 0);
		if (WorldData[position + MaxX] != WATERENTITY && WorldData[position + MaxX] != LAVAENTITY) {
			WorldData[position + MaxX] = WATERENTITY;
			WorldDataTimer[timerPos + 80] = 3;
		}
	}

}

void survivalInventory(void) {
	int16_t xpos = 80, ypos = 140, num = 0, pos = 0;
	while (!(kb_IsDown(kb_KeyClear)))
	{
		kb_Scan();
		gfx_SetColor(148);
		gfx_FillRectangle(40, 60, 240, 162);
		// main inventory slots
		pos = 0;
		for (y = 140; y < 140 + (3 * 18); y += 18) {
			for (x = 80; x < 80 + (9 * 18); x += 18) {
				gfx_SetColor(0);
				gfx_Rectangle(x, y, 18, 18);
				if (Inventory[pos] > 0) {
				if (Inventory[pos] <= MaxSprites[0])
						gfx_TransparentSprite(BlockTextures[Inventory[pos] - 1], x + 1, y + 1);
					if (Inventory[pos] > MaxSprites[0] && Inventory[pos] <= MaxSprites[1])
						gfx_TransparentSprite(ItemTextures[Inventory[pos] - 1], x + 1, y + 1);
					if (Inventory[pos] > MaxSprites[1] && Inventory[pos] <= MaxSprites[2])
						gfx_TransparentSprite(PlantTextures[Inventory[pos] - 1], x + 1, y + 1);
					if (Inventory[pos] > MaxSprites[2] && Inventory[pos] <= MaxSprites[3])
						gfx_TransparentSprite(FoliageTextures[Inventory[pos] - 1], x + 1, y + 1);
					gfx_SetTextFGColor(254);
				}else{
					gfx_SetColor(74);
					gfx_FillRectangle(x + 1, y + 1, 16, 16);
				}
				pos++;
			}
		}
		pos = 0;
		for (y = 140; y < 140 + (3 * 18); y += 18) {
			for (x = 80; x < 80 + (9 * 18); x += 18) {
				if (Inventory[pos] > 0) {
					gfx_SetTextXY(x + 10, y + 12);
					gfx_PrintInt(Inventory[pos + 27], 1);
				}
				pos++;
			}
		}
		// hotbar
		for (x = 0; x < 5; x++) {
			if (x == hotbarSel) {
				gfx_SetColor(5);
			}else{
				gfx_SetColor(0);
			}
			gfx_Rectangle(117 + (x * 18), 200, 18, 18);

			if (hotbar[x] != 0) {
				if (hotbar[x] <= MaxSprites[0])
					gfx_TransparentSprite(BlockTextures[hotbar[x] - 1], 118 + (x * 18), 201);
				// check if it's foliage
				if (hotbar[x] > MaxSprites[0] && hotbar[x] <= MaxSprites[0] + MaxSprites[3])
					gfx_TransparentSprite(FoliageTextures[hotbar[x] - 1 - MaxSprites[0]], 118 + (x * 18), 201);
				// check if it's plants
				if (hotbar[x] > MaxSprites[0] + MaxSprites[3] && hotbar[x] <= MaxSprites[0] + MaxSprites[3] + MaxSprites[2])
					gfx_TransparentSprite(PlantTextures[hotbar[x] - 1 - (MaxSprites[0] + MaxSprites[3])], 118 + (x * 18), 201);
			}else{
				gfx_SetColor(181);
				gfx_FillRectangle(118 + (x * 18), 201, 16, 16);
			}
		}
		gfx_BlitBuffer();

	}
	delay(200);
	kb_Scan();
	Game();
}

int craftingCheck(int16_t inputVal) {
	int16_t item = 0;

	return item;
}

void giveItem(int16_t blockID, int16_t breaking) {
	int16_t pos, movetoinventory = 0;

	if (breaking == 1) {
		// add code for breaking blocks based on their ID.
	}

	// first scan hotbar for the blockID
	for (pos = 0; pos < 5; pos++) {
		if (hotbar[pos] == 0) {
			hotbar[pos] = blockID;
			hotbar[pos + 5]++;
			break;
		}else{
			if (hotbar[pos] == blockID && hotbar[pos + 5] < 64) {
				hotbar[pos + 5]++;
				break;
			}
			if (hotbar[pos] == blockID && hotbar[pos + 5] == 64) {
				movetoinventory = 1;
				break;
			}
		}
	}

	if (movetoinventory == 1) {
		// the hotbar is full, so find an empty space to put the item
		for (pos = 0; pos < 26; pos++) {
			if (Inventory[pos] == 0) {
				Inventory[pos] = blockID;
				Inventory[pos + 27]++;
				movetoinventory = 2;
				break;
			}else{
				if (Inventory[pos] == blockID && Inventory[pos + 27] < 64) {
					Inventory[pos + 27]++;
					movetoinventory = 2;
					break;
				}
			}
		}
	}

	// Check if movetoinventory is still 1.
	// If so, there's not enough room anywhere in player storage for the item

}

void creativeInventory(void)
{
	int16_t xpos = 28, ypos = 25, num, spritenum, tab = 0, pos = 0, hotbarpos = 0, lastPos;
	int16_t scroll = 0, oldBlock = 0, newBlock = 0, InvCurPos = 0, lastXpos = 0, selectedFromTab = 0;
	int16_t newBlockCount = 0, oldBlockCount = 0;
	char *tabNames[5] = { "Building Blocks", "Items", "Plants", "Foliage", "Player" };
	while (!(kb_IsDown(kb_KeyClear)))
	{
		kb_Scan();
		num = 0;
		gfx_SetColor(148);
		gfx_FillRectangle(15, 5, 290, 230);
		// scroll bar rectangle
		gfx_SetColor(74);
		gfx_FillRectangle(28 + (14 * 18), 24, 7, 182);
		// scroll bar level indicator
		gfx_SetColor(181);
		gfx_FillRectangle(29 + (14 * 18), 25 + (scroll * (180 / 9)), 5, 180 / 9);
		// tab names
		if (tab == 0) gfx_TransparentSprite(BlockTextures[101], 30, 8);
		if (tab == 1) gfx_TransparentSprite(ItemTextures[0], 30, 8);
		if (tab == 2) gfx_TransparentSprite(PlantTextures[0], 30, 8);
		if (tab == 3) gfx_TransparentSprite(FoliageTextures[0], 30, 8);
		if (tab == 4) gfx_TransparentSprite(BlockTextures[10], 30, 8);
		gfx_SetColor(181);
		gfx_FillRectangle(48, 8, 14 * 18 - 20, 10);
		gfx_SetTextFGColor(0);
		gfx_PrintStringXY(tabNames[tab], 50, 9);
		for (y = 25; y < 25 + (10 * 18); y += 18) {
			for (x = 28; x < 28 + (14 * 18); x += 18) {
				gfx_SetColor(0);
				gfx_Rectangle(x, y, 18, 18);
				gfx_SetColor(74);
				spritenum = scroll * 14 + num;
				if (tab == 0 && spritenum <= MaxSprites[0])
					gfx_TransparentSprite(BlockTextures[spritenum], x + 1, y + 1);
				if (tab == 1 && spritenum <= MaxSprites[1])
					gfx_TransparentSprite(ItemTextures[spritenum], x + 1, y + 1);
				if (tab == 2 && spritenum <= MaxSprites[2])
					gfx_TransparentSprite(PlantTextures[spritenum], x + 1, y + 1);
				if (tab == 3 && spritenum <= MaxSprites[3])
					gfx_TransparentSprite(FoliageTextures[spritenum], x + 1, y + 1);
				if (spritenum >= MaxSprites[tab])
					gfx_FillRectangle(x + 1, y + 1, 16, 16);
				num++;
			}
		}
		// hotbar
		for (x = 0; x < 5; x++) {
			gfx_SetColor(0);
			if (x == hotbarSel)
			gfx_SetColor(5);
			gfx_Rectangle(117 + (x * 18), 210, 18, 18);
			if (hotbar[x] != 0) {
				if (hotbar[x] <= MaxSprites[0])
					gfx_TransparentSprite(BlockTextures[hotbar[x] - 1], 118 + (x * 18), 211);
				// check if it's foliage
				if (hotbar[x] > MaxSprites[0] && hotbar[x] <= MaxSprites[0] + MaxSprites[3])
					gfx_TransparentSprite(FoliageTextures[hotbar[x] - 1 - MaxSprites[0]], 118 + (x * 18), 211);
				// check if it's plants
				if (hotbar[x] > MaxSprites[0] + MaxSprites[3] && hotbar[x] <= MaxSprites[0] + MaxSprites[3] + MaxSprites[2])
					gfx_TransparentSprite(PlantTextures[hotbar[x] - 1 - (MaxSprites[0] + MaxSprites[3])], 118 + (x * 18), 211);
			}else{
				gfx_SetColor(181);
				gfx_FillRectangle(118 + (x * 18), 211, 16, 16);
			}
		}
		// draw a item if it was selected
		if (newBlock != 0)
		{
			if (selectedFromTab == 0 && newBlock <= MaxSprites[0])
				gfx_TransparentSprite(BlockTextures[newBlock - 1], xpos + 5, ypos + 5);
			if (selectedFromTab == 1 && newBlock <= MaxSprites[1])
				gfx_TransparentSprite(ItemTextures[newBlock - 1], xpos + 5, ypos + 5);
			if (selectedFromTab == 2 && newBlock <= MaxSprites[2])
				gfx_TransparentSprite(PlantTextures[newBlock - 1], xpos + 5, ypos + 5);
			if (selectedFromTab == 3 && newBlock <= MaxSprites[3])
				gfx_TransparentSprite(FoliageTextures[newBlock - 1], xpos + 5, ypos + 5);
		}
		// cursor (shaped like a + )
		gfx_SetColor(255);
		gfx_FillRectangle(xpos + 2, ypos + 8, 14, 2);
		gfx_FillRectangle(xpos + 8, ypos + 2, 2, 14);


		gfx_SetTextFGColor(0);
		gfx_SetTextXY(12, 12);
		gfx_PrintInt(pos + 1, 1);


		gfx_SetTextFGColor(254);
		gfx_SetTextXY(xpos + 14, ypos + 14);
		if (newBlockCount > 1) gfx_PrintInt(newBlockCount, 1);
		gfx_BlitBuffer();

		// move cursor down to hotbar if the cursor is at the bottom, and back up
		if (ypos == 25 + (9 * 18) && kb_IsDown(kb_KeyDown) && scroll == 8)
		{
			InvCurPos = 1;
			lastXpos = xpos;
			lastPos = pos;
			hotbarpos = 0;
			xpos = 117;
			ypos = 210;
		}
		if (ypos == 210 && kb_IsDown(kb_KeyUp) && InvCurPos == 1)
		{
			InvCurPos = 0;
			xpos = lastXpos;
			pos = lastPos + 14;
			ypos = 25 + (10 * 18);
		}
		// clears the selected block if user clicks an empty space in the inventory
		if (kb_IsDown(kb_Key2nd) && (pos >= MaxSprites[tab] || (newBlock != pos + 1 && newBlock != 0)) && ypos != 210)
		{
			newBlock = 0;
			newBlockCount = 0;
		}else{
			// select an item if the user presses 2nd in the inventory (not hotbar)
			if (kb_IsDown(kb_Key2nd) && pos < MaxSprites[tab] && newBlockCount < 64 && ypos != 210)
			{
				selectedFromTab = tab;
				if (newBlock == pos + 1) {
					newBlockCount++;
				}else{
					newBlockCount = 1;
					newBlock = pos + 1;
				}
			}
		}
		// select/swap an item if the user presses 2nd in the hotbar only
		if (kb_IsDown(kb_Key2nd) && ypos == 210)
		{
			oldBlock = hotbar[hotbarpos];
			if (newBlock == 0)
			{
				// check if it's blocks
				if (hotbar[hotbarpos] <= MaxSprites[0])
					selectedFromTab = 0;
				// check if it's items
				if (hotbar[hotbarpos] > MaxSprites[0] && hotbar[hotbarpos] <= MaxSprites[0] + MaxSprites[1])
					selectedFromTab = 1;
				// check if it's foliage
				if (hotbar[hotbarpos] > MaxSprites[0] + MaxSprites[1] && hotbar[hotbarpos] <= MaxSprites[0] + MaxSprites[1] + MaxSprites[2])
					selectedFromTab = 2;
				// check if it's plants
				if (hotbar[hotbarpos] > MaxSprites[0] + MaxSprites[1] + MaxSprites[2] && hotbar[hotbarpos] <= MaxSprites[0] + MaxSprites[1] + MaxSprites[2] + MaxSprites[3])
					selectedFromTab = 3;
			}else{
				hotbar[hotbarpos] = newBlock;
			}
			newBlock = oldBlock;
		}
		// tabs
		if (kb_IsDown(kb_KeyYequ) || kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyGraph)) {
			if (kb_IsDown(kb_KeyYequ)) tab = 0;
			if (kb_IsDown(kb_KeyWindow)) tab = 1;
			if (kb_IsDown(kb_KeyZoom)) tab = 2;
			if (kb_IsDown(kb_KeyTrace)) tab = 3;
			if (kb_IsDown(kb_KeyGraph)) tab = 4;
			scroll = 0;
			pos = 0;
			xpos = 28;
			ypos = 25;
		}
		// scroll bar movement
		if (ypos == 25 + (9 * 18) && kb_IsDown(kb_KeyDown) && scroll < 8)
		{
			scroll++;
			pos += 14;
		}
		if (ypos == 25 && kb_IsDown(kb_KeyUp) && scroll > 0)
		{
			scroll--;
			pos -= 14;
		}
		// move cursor left and right if cursor is at the hotbar
		xpos += 18 * (kb_IsDown(kb_KeyRight) && xpos < 117 + (4 * 18)) * (ypos == 210) - 18 * (kb_IsDown(kb_KeyLeft) && xpos > 117) * (ypos == 210);
		hotbarpos += (kb_IsDown(kb_KeyRight) && hotbarpos < 5) * (ypos == 210) - (kb_IsDown(kb_KeyLeft) && hotbarpos > 0) * (ypos == 210);
		// cursor movement
		pos += (kb_IsDown(kb_KeyRight) && xpos < 28 + (13 * 18)) * (ypos != 210) - (kb_IsDown(kb_KeyLeft) && xpos > 28) * (ypos != 210);
		pos += 14 * (kb_IsDown(kb_KeyDown) && ypos < 25 + (9 * 18)) * (ypos != 210) - 14 * (kb_IsDown(kb_KeyUp) && ypos > 25) * (ypos != 210);
		xpos += 18 * (kb_IsDown(kb_KeyRight) && xpos < 28 + (13 * 18)) * (ypos != 210) - 18 * (kb_IsDown(kb_KeyLeft) && xpos > 28) * (ypos != 210);
		ypos += 18 * (kb_IsDown(kb_KeyDown) && ypos < 25 + (9 * 18)) * (ypos != 210) - 18 * (kb_IsDown(kb_KeyUp) && ypos > 25) * (ypos != 210);
	}
	delay(200);
	kb_Scan();
	Game();
}

void pauseMenu(void) {
	int16_t posX, posY = 100, y;
	RenderEngine();
	gfx_SetTextFGColor(0);
	while (!(kb_IsDown(kb_KeyClear)) && !(kb_IsDown(kb_Key2nd) && posY == 100)) {
		for (y = 100; y < 175; y += 25) {
			gfx_SetColor(149);
			gfx_FillRectangle(40, y, 130, 20);
			gfx_FillRectangle(41, y + 1, 128, 18);
			gfx_SetColor(0);
			if (posY == y) gfx_SetColor(255);
			gfx_Rectangle(40, y, 130, 20);
			gfx_Rectangle(41, y + 1, 128, 18);
		}
		for (pos = 15; pos < 18; pos++) {
			DrawCenteredText(MenuElements[pos], 105, 106 + ((pos - 15) * 25));
		}
		gfx_BlitBuffer();
		kb_Scan();
		if (kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown)) delay(80);
		if (kb_IsDown(kb_KeyUp) && posY > 100) posY -= 25;
		if (kb_IsDown(kb_KeyDown) && posY < 150) posY += 25;
		delay(100 * (kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown)));
		if (kb_IsDown(kb_Key2nd) && posY == 125) {
			Settings(1);
			RenderEngine();
		}
		if (kb_IsDown(kb_Key2nd) && posY == 150) {
			// save the world data, playerX, playerY, curPos, curX, curY, timeofday, etc...
			//world_file = "        ";
			memcpy(world_file, worldNameStr, 8);
			appvar = ti_Open(world_file, "a+");
			int world_offset;
			ti_Write("MCCESV", 6, 1, appvar);
			ti_Write((void *)0xFF0000, 3, 1, appvar); // this is overwritten later
			ti_Write(&curX, 3, 1, appvar);
			ti_Write(&curY, 3, 1, appvar);
			ti_Write(&MaxX, 3, 1, appvar);
			ti_Write(&MaxY, 3, 1, appvar);
			ti_Write(&flymode, 3, 1, appvar);
			ti_Write(&scrollX, 3, 1, appvar);
			ti_Write(&scrollY, 3, 1, appvar);
			ti_Write(&playerX, 3, 1, appvar);
			ti_Write(&playerY, 3, 1, appvar);
			ti_Write(&curPos, 3, 1, appvar);
			ti_Write(&timeofday, 3, 1, appvar);
			ti_Write(&hotbarSel, 3, 1, appvar);
			ti_Write(&hotbar, 5, 3, appvar);
			world_offset = ti_Tell(appvar);
			ti_Seek(6, SEEK_SET, appvar);
			ti_Write(&world_offset, 3, 1, appvar);
			ti_Seek(world_offset, SEEK_SET, appvar);
			compressAndWrite(&WorldData, MaxX * MaxY, appvar);
			ti_SetArchiveStatus(1, appvar);
			ti_Close(appvar);
		}
	}
	delay(200);
	kb_Scan();
	Game();
}

void MainMenu()
{
	int8_t scroll = 16, list;
	curY = 125;
	// fix closing the game unnecessarily.
	kb_Scan();
	while (!(kb_IsDown(kb_KeyClear)) && !(kb_IsDown(kb_Key2nd) && curY == 200))
	{
		// draw the main menu
		kb_Scan();
		DrawDirtBackground(scroll);
		gfx_SetTextFGColor(230);
		gfx_PrintStringXY("v1.0.2", 3, 230);
		gfx_SetTextFGColor(0);
		appvar = ti_Open("MC2DDAT", "r");
		logo = ti_GetDataPtr(appvar);
		ti_Close(appvar);
		gfx_SetTransparentColor(255);
		gfx_ScaledTransparentSprite_NoClip(logo, 32, 20, 2, 2);
		gfx_SetTransparentColor(7);
		/* buttons */
		for (y = 125; y < 225; y += 25) {
			gfx_SetColor(181);
			gfx_FillRectangle(60, y, 192, 20);
			gfx_SetColor(0);
			gfx_Rectangle(60, y, 192, 20);
			gfx_Rectangle(61, y + 1, 190, 18);
		}
		/* button text */
		for (list = 0; list < 4; list++) {
			DrawCenteredText(MenuElements[list], 160, 131 + (list * 25));
		}
		gfx_SetColor(254);
		gfx_Rectangle(60, curY, 192, 20);
		gfx_Rectangle(61, curY + 1, 190, 18);
		gfx_BlitBuffer();

		scroll++;
		if (scroll > 16)
			scroll = 0;

		if (kb_IsDown(kb_KeyUp) && curY > 125)
		{
			// delay(150);
			curY -= 25;
		}
		if (kb_IsDown(kb_KeyDown) && curY < 200)
		{
			// delay(150);
			curY += 25;
		}
		if (kb_IsDown(kb_Key2nd))
		{
			if (curY == 125)
			{ //"play menu"
				delay(100);
				playMenu();
				y = 125;
				delay(100);
				MainMenu();
			}
			if (curY == 150)
			{ //"achievements"
				delay(100);
				Achievements();
				curY = 150;
				delay(100);
				MainMenu();
			}
			if (curY == 175)
			{ //"Settings"
				delay(100);
				Settings(0);
				curY = 175;
				delay(100);
				MainMenu();
			}
		}
	}
	gfx_End();
	appvar = ti_Open("MCESETT", "w");
	ti_Write(gameSettings, sizeof gameSettings, 1, appvar);
	ti_CloseAll();
	exit(1);
}

void Achievements(void)
{
	int16_t x, y;
	DrawDirtBackground(0);
	gfx_SetColor(181);
	gfx_FillCircle(10, 10, 5);
	gfx_FillCircle(309, 10, 5);
	gfx_FillCircle(10, 229, 5);
	gfx_FillCircle(309, 229, 5);
	gfx_FillRectangle(10, 5, 300, 230);
	gfx_FillRectangle(5, 10, 310, 220);
	gfx_SetTextFGColor(0);
	DrawCenteredText(MenuElements[1], 160, 20);
	gfx_SetTextFGColor(255);
	for (x = 2; x < 18; x++)
	{
		for (y = 2; y < 13; y++)
		{
			gfx_TransparentSprite(BlockTextures[201], x * 16, y * 16);
		}
	}
	gfx_BlitBuffer();

	while (!(os_GetCSC()));
	delay(100);
}

void Settings(bool ingameTrue)
{
	int16_t tab = 0, itemScroll = 0, pos, option, curpX = 10, curpY = 40;
	DrawDirtBackground(0);
	if (ingameTrue == 1) RenderEngine();
	while (!(kb_IsDown(kb_KeyClear)) && !(kb_IsDown(kb_Key2nd) && tab == 3))
	{
		kb_Scan();
		gfx_SetColor(74);
		gfx_FillRectangle(5, 5, 310, 230);
		gfx_SetColor(0);
		gfx_Rectangle(5, 5, 310, 230);
		gfx_Rectangle(6, 6, 308, 228);
		for (y = 40; y < 140; y += 20) {
			gfx_SetColor(181);
			if (y < 120) gfx_FillRectangle(10, y, 80, 16);
			if (tab < 2) gfx_FillRectangle(100, y, 180, 16);
			// selected tab/category
			gfx_SetColor(148);
			gfx_FillRectangle(10, 40 + tab * 20, 80, 16);
			gfx_SetColor(0);
			if (y < 120) {
				gfx_Rectangle(10, y, 80, 16);
				gfx_Rectangle(11, y + 1, 78, 14);
			}
			if (tab < 2) gfx_Rectangle(100, y, 180, 16);
			if (tab < 2) gfx_Rectangle(101, y + 1, 178, 14);
		}
		for (pos = 4; pos < 8; pos++) {
			DrawCenteredText(MenuElements[pos], 50, 44 + ((pos - 4) * 20));
		}
		gfx_SetColor(254);
		gfx_Rectangle(curpX, curpY, 180 - (100 * (curpX == 10)), 16);
		gfx_Rectangle(curpX + 1, curpY + 1, 178 - (100 * (curpX == 10)), 14);
		if (tab == 0) {
			gameSettingsStr[0] = languages[gameSettings[0]];
			gameSettingsStr[1] = SpeedStr[gameSettings[1]];
			gameSettingsStr[2] = togglesOnOff[gameSettings[2]];
			gameSettingsStr[3] = togglesOnOff[gameSettings[3]];
			for (pos = 0; pos < 4; pos++) {
				gfx_SetTextXY(105, 44 + (pos * 20));
				gfx_PrintString(GameOptionStr[pos]);
				gfx_PrintString(": ");
				gfx_PrintString(gameSettingsStr[pos]);
			}
		}
		// about tab
		if (tab == 2) {
			gfx_SetColor(148);
			gfx_FillRectangle(100, 30, 200, 130);
			gfx_FillRectangle(10, 180, 300, 30);
			gfx_SetColor(0);
			gfx_Rectangle(100, 30, 200, 130);
			gfx_Rectangle(101, 31, 198, 128);
			gfx_Rectangle(10, 180, 300, 30);
			gfx_Rectangle(11, 181, 298, 28);
			gfx_SetTextFGColor(0);
			gfx_SetTextXY(15, 185);
			gfx_PrintString("Discord:");
			gfx_SetTextFGColor(26);
			gfx_PrintString("https://discord.gg/xavH5eTJP2");
			gfx_SetTextXY(15, 196);
			gfx_SetTextFGColor(0);
			gfx_PrintString("Github:");
			gfx_SetTextFGColor(26);
			gfx_PrintString("https://github.com/TimmyTurner51");
			gfx_SetTextFGColor(0);
			for (pos = 0; pos < 12; pos++) {
				gfx_PrintStringXY(About[pos], 105, 35 + (pos * 10));
			}
		}
		gfx_BlitBuffer();

		if (kb_IsDown(kb_Key2nd) && curpX == 10) {
			tab = (curpY - 40) / 20;
		}
		if (kb_IsDown(kb_Key2nd) && curpX != 10) {
			option = (curpY - 40) / 20;
			// 4 is the amount of current options
			gameSettings[option + (tab * 4)]++;
			if (gameSettings[option + (tab * 4)] > gameSettingsMaxVals[option + (tab * 4)]) gameSettings[option + (tab * 4)] = 0;
			delay(200);
		}
		if (kb_IsDown(kb_KeyUp) || kb_IsDown(kb_KeyDown) || kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight))
		delay(80);
		if (kb_IsDown(kb_KeyUp) && curpY > 40) curpY -= 20;
		if (kb_IsDown(kb_KeyDown) && curpY < 120 - (20 * (curpX == 10))) curpY += 20;
		if (kb_IsDown(kb_KeyLeft)) {
			curpX = 10;
			curpY = 40 + tab * 20;
		}
		if (kb_IsDown(kb_KeyRight) && tab != 2) curpX = 100;
	}
	delay(100);
	appvar = ti_Open("MCESETT", "w");
	ti_Write(gameSettings, sizeof gameSettings, 1, appvar);
	ti_Close(appvar);
	LoadResourcesScreen();
}

void input(char *string, int size)
{
	const char *charsUpper = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	const char *charsLower = "\0\0\0\0\0\0\0\0\0\0\"wrmh\0\0?[vqlg\0\0:zupkfc\0 ytojeb\0\0xsnida\0\0\0\0\0\0\0\0";
	const char *charsNum = "\0\0\0\0\0\0\0\0\0\0\+-*/\0\0\0?369(\0\0\0\0258)\0\0\00147,\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	uint8_t key, length = strlen(string), scroll = 0, i = 0, chartype = 0;
	// set i to the first occurence of a null character
	for (i = 0; i < length; i++) {
		if (i > 31) scroll++;
		if (string[i] == 0) break;
	}
	gfx_SetColor(181);
	gfx_FillRectangle(0, 216, 320, 24);
	gfx_SetColor(74);
	gfx_Rectangle(0, 216, 320, 24);
	gfx_Rectangle(1, 217, 318, 22);
	gfx_SetColor(181);
	while((key = os_GetCSC()) != sk_Enter) {
		gfx_SetColor(181);
		gfx_FillRectangle(1, 217, 318, 22);
		gfx_SetColor(0);
		gfx_FillRectangle(4, 223, 10, 10);
		gfx_SetTextFGColor(254);
		if (chartype == 0) gfx_PrintStringXY("A", 5, 224);
		if (chartype == 1) gfx_PrintStringXY("a", 5, 224);
		if (chartype == 2) gfx_PrintStringXY("1", 5, 224);
		gfx_SetTextFGColor(0);
		gfx_SetTextXY(18, 224);
		pos = scroll;
		for (pos = scroll; pos < scroll + 32; pos++)
		{
			gfx_SetTextFGColor(0);
			if (pos < i)
			gfx_PrintChar(string[pos]);
		}
		gfx_BlitBuffer();

		if (kb_IsDown(kb_KeyAlpha)) {
			delay(200);
			chartype++;
			if (chartype > 2) chartype = 0;
		} 

		if(charsUpper[key] && i <= size) {
			if (chartype == 0) string[i] = charsUpper[key];
			if (chartype == 1) string[i] = charsLower[key];
			if (chartype == 2) string[i] = charsNum[key];
			string[i + 1] = 0;
			i++;
			if (i > 32) scroll++;
		}
		if (kb_IsDown(kb_KeyDel) && i > 0) {
			delay(80);
			string[i] = 0;
			if (i > 32 && scroll > 0) scroll--;
			i--;
		}
	}
}

void playMenu(void)
{
	gamemode = 0;
	cheats = 0;
	findAppvars("MCCESV");
	tab = 0;
	scroll = 0;
	CursorY = 40;
	kb_Scan();
	while (!(kb_IsDown(kb_KeyClear)))
	{
		kb_Scan();
		DrawDirtBackground(0);
		gfx_SetColor(181);
		gfx_FillRectangle(20, 20, 280, 200);
		gfx_SetColor(0);
		gfx_Rectangle(20, 20, 280, 200);
		gfx_Rectangle(20, 20, 280, 20);
		gfx_SetColor(148);
		gfx_FillRectangle(21 + (tab * 89), 21, 100, 18);
		DrawCenteredText(MenuElements[8], 70, 25);
		DrawCenteredText(MenuElements[9], 160, 25);
		DrawCenteredText(MenuElements[10], 250, 25);
		if (tab == 0)
		{
			gfx_SetColor(148);
			gfx_FillRectangle(21, 40, 278, 25);
			DrawCenteredText(MenuElements[11], 160, 47);
			gfx_SetColor(254);
			gfx_Rectangle(21, CursorY, 278, 25 - ((CursorY != 40) * 8));
			gfx_Rectangle(22, CursorY + 1, 276, 23 - ((CursorY != 40) * 8));
			if (foundCount == 0) {
				gfx_PrintStringXY(MenuElements[12], 90, 120);
			}else{
				y = 0;
				for (i = scroll; i < scroll + 2; i++)
				{
					if (i < foundCount)
						gfx_PrintStringXY(WorldsList[i], 40, 70 + y);
					y += 17;
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
			}
			if (kb_IsDown(kb_Key2nd) && (CursorY > 40))
			{
				world_file = WorldsList[scrollYb];
				appvar = ti_Open(world_file, "r");
				if (!memcmp(ti_GetDataPtr(appvar), "MCCESV", 6))
				{
					int world_offset;
					ti_Seek(6, SEEK_SET, appvar);
					ti_Read(&world_offset, 3, 1, appvar);
					ti_Read(&curX, 3, 1, appvar);
					ti_Read(&curY, 3, 1, appvar);
					ti_Read(&MaxX, 3, 1, appvar);
					ti_Read(&MaxY, 3, 1, appvar);
					ti_Read(&flymode, 3, 1, appvar);
					ti_Read(&scrollX, 3, 1, appvar);
					ti_Read(&scrollY, 3, 1, appvar);
					ti_Read(&playerX, 3, 1, appvar);
					ti_Read(&playerY, 3, 1, appvar);
					ti_Read(&curPos, 3, 1, appvar);
					ti_Read(&timeofday, 3, 1, appvar);
					ti_Read(&hotbarSel, 3, 1, appvar);
					ti_Read(&hotbar, 5, 3, appvar);
					ti_Seek(world_offset, SEEK_SET, appvar);
					zx7_Decompress(&WorldData, ti_GetDataPtr(appvar));
					ti_Close(appvar);
					loaded_world = 1;
					Game();
				}
			}
			if (kb_IsDown(kb_Key2nd) && (CursorY == 40)) {
				genTrees = 1;
				genCaves = 1;
				genFlowers = 1;
				genVillages = 1;
				worldSize = 0;
				worldType = 0;
				CursorY = 60;
				NewWorldScreen();
			}
		}
		if (tab == 1)
		{
			gfx_SetColor(148);
			gfx_FillRectangle(21, 40, 278, 25);
			DrawCenteredText(MenuElements[13], 160, 47);
			gfx_SetColor(254);
			gfx_Rectangle(21, CursorY, 278, 25 - ((CursorY != 40) * 8));
			gfx_Rectangle(22, CursorY + 1, 276, 23 - ((CursorY != 40) * 8));
		}
		if (tab == 2)
		{
			DrawCenteredText(MenuElements[14], 160, 47);
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
	MainMenu();
}

void NewWorldScreen(void) {
	while (!(kb_IsDown(kb_KeyClear)))
	{
		DrawDirtBackground(0);
		for (y = 60; y < 220; y += 20)
		{
			gfx_SetColor(181);
			gfx_FillRectangle(50, y, 220, 16);
			gfx_SetColor(0);
			gfx_Rectangle(50, y, 220, 16);
			gfx_Rectangle(51, y + 1, 218, 14);
		}
		gfx_SetColor(148);
		gfx_FillRectangle(0, 0, 320, 12);
		DrawCenteredText(MenuElements[11], 160, 2);
		DrawCenteredText(NewWorldMenuElements[0], 160 - gfx_GetStringWidth(worldNameStr) / 2, 64);
		gfx_PrintStringXY(worldNameStr, 162 - gfx_GetStringWidth(worldNameStr) / 2 + gfx_GetStringWidth(NewWorldMenuElements[0]) / 2, 64);
		DrawCenteredText(NewWorldMenuElements[1], 160 - gfx_GetStringWidth(seedStr) / 2, 84);
		gfx_PrintStringXY(seedStr, 162 - gfx_GetStringWidth(seedStr) / 2 + gfx_GetStringWidth(NewWorldMenuElements[1]) / 2, 84);
		DrawCenteredText(NewWorldMenuElements[2], 160 - gfx_GetStringWidth(gamemodeStr[gamemode]) / 2, 104);
		gfx_PrintStringXY(gamemodeStr[gamemode], 162 - gfx_GetStringWidth(gamemodeStr[gamemode]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[2]) / 2, 104);
		DrawCenteredText(NewWorldMenuElements[3], 160 - gfx_GetStringWidth(worldSizeStr[worldSize]) / 2, 124);
		gfx_PrintStringXY(worldSizeStr[worldSize], 162 - gfx_GetStringWidth(worldSizeStr[worldSize]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[3]) / 2, 124);
		DrawCenteredText(NewWorldMenuElements[4], 160 - gfx_GetStringWidth(togglesOnOff[cheats]) / 2, 144);
		gfx_PrintStringXY(togglesOnOff[cheats], 162 - gfx_GetStringWidth(togglesOnOff[cheats]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[4]) / 2, 144);
		DrawCenteredText(NewWorldMenuElements[5], 160 - gfx_GetStringWidth(worldTypesStr[worldType]) / 2, 164);
		gfx_PrintStringXY(worldTypesStr[worldType], 162 - gfx_GetStringWidth(worldTypesStr[worldType]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[5]) / 2, 164);
		DrawCenteredText(NewWorldMenuElements[6], 160, 184);
		DrawCenteredText(NewWorldMenuElements[7], 160, 204);
		gfx_SetColor(254);
		gfx_Rectangle(50, CursorY, 220, 16);
		gfx_Rectangle(51, CursorY + 1, 218, 14);
		gfx_BlitBuffer();
		kb_Scan();
		if (kb_IsDown(kb_KeyUp) && (CursorY > 60))
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
			if (CursorY == 60)
			{
				input(worldNameStr, 40);
				delay(100);
			}
			if (CursorY == 80)
			{
				input(seedStr, 40);
				delay(100);
			}
			if (CursorY == 100)
			{
				gamemode++;
				if (gamemode > 2)
					gamemode = 0;
			}
			if (CursorY == 120)
			{
				worldSize++;
				if (worldSize > 2)
					worldSize = 0;
			}
			if (CursorY == 140)
			{
				cheats = (cheats == 0);
			}
			if (CursorY == 160)
			{
				worldType++;
				if (worldType > 2)
				{
					worldType = 0;
				}
			}
			if (CursorY == 180) {
				MoreWorldOptions();
				CursorY = 180;
			}
			if (CursorY == 200)
			{
				if (worldSize == 0)
				{
					MaxX = 16 * 4;
					MaxY = 16 * 4;
				}
				if (worldSize == 1)
				{
					MaxX = 16 * 7;
					MaxY = 16 * 7;
				}
				if (worldSize == 2)
				{
					MaxX = 16 * 9;
					MaxY = 16 * 9;
				}
				// supposed to take the first 8 letters of the worldName string and copy to world_file
				memcpy(world_file, worldNameStr, 8);

				Generator();
				Game();
				delay(200);
				redraw = 1;
				break;
			}
		}
	}
	delay(100);
	kb_Scan();
	playMenu();
}

void MoreWorldOptions(void) {
	CursorY = 60;
	while (!(kb_IsDown(kb_KeyClear)) && !(kb_IsDown(kb_Key2nd) && CursorY == 140))
	{
		DrawDirtBackground(0);
		for (y = 60; y < 160; y += 20)
		{
			gfx_SetColor(181);
			gfx_FillRectangle(50, y, 220, 16);
			gfx_SetColor(0);
			gfx_Rectangle(50, y, 220, 16);
			gfx_Rectangle(51, y + 1, 218, 14);
		}
		gfx_SetColor(148);
		gfx_FillRectangle(0, 0, 320, 12);
		DrawCenteredText(NewWorldMenuElements[6], 160, 2);
		DrawCenteredText(NewWorldMenuElements[8], 160 - gfx_GetStringWidth(togglesOnOff[genTrees]) / 2, 64);
		gfx_PrintStringXY(togglesOnOff[genTrees], 162 - gfx_GetStringWidth(togglesOnOff[genTrees]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[8]) / 2, 64);
		DrawCenteredText(NewWorldMenuElements[9], 160 - gfx_GetStringWidth(togglesOnOff[genFlowers]) / 2, 84);
		gfx_PrintStringXY(togglesOnOff[genFlowers], 162 - gfx_GetStringWidth(togglesOnOff[genFlowers]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[9]) / 2, 84);
		DrawCenteredText(NewWorldMenuElements[10], 160 - gfx_GetStringWidth(togglesOnOff[genVillages]) / 2, 104);
		gfx_PrintStringXY(togglesOnOff[genVillages], 162 - gfx_GetStringWidth(togglesOnOff[genVillages]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[10]) / 2, 104);
		DrawCenteredText(NewWorldMenuElements[11], 160 - gfx_GetStringWidth(togglesOnOff[genCaves]) / 2, 124);
		gfx_PrintStringXY(togglesOnOff[genCaves], 162 - gfx_GetStringWidth(togglesOnOff[genCaves]) / 2 + gfx_GetStringWidth(NewWorldMenuElements[11]) / 2, 124);
		DrawCenteredText(NewWorldMenuElements[12], 160, 144);
		gfx_SetColor(254);
		gfx_Rectangle(50, CursorY, 220, 16);
		gfx_Rectangle(51, CursorY + 1, 218, 14);
		gfx_BlitBuffer();
		kb_Scan();
		if (kb_IsDown(kb_KeyUp) && (CursorY > 60)) CursorY -= 20;
		if (kb_IsDown(kb_KeyDown) && (CursorY < 140)) CursorY += 20;
		if (kb_IsDown(kb_Key2nd)) {
			delay(100);
			if (CursorY == 60) genTrees = (genTrees == 0);
			if (CursorY == 80) genFlowers = (genFlowers == 0);
			if (CursorY == 100) genVillages = (genVillages == 0);
			if (CursorY == 120) genCaves = (genCaves == 0);
		}
	}
	delay(100);
	kb_Scan();
}

void DrawDirtBackground(int16_t scrollVal) {
	for (x = 0; x < 320; x += 16) {
		for (y = 0 - scrollVal; y < 240 + scrollVal; y += 16) {
			gfx_TransparentSprite(BlockTextures[77], x, y);
		}
	}
}

// Compress data via zx7 compression
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
	*((int *)0xE30010) = 0xD40000; // force the lcd to use the first half of VRAM so we can use the second half.
	gfx_SetDrawScreen();
	DrawDirtBackground(0);
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
	main();
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
			break;
	}
}