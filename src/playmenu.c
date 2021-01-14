
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

#include "playmenu.h"
#include "defines.h"

void playMenu(void)
{

	int24_t CursorY, x, i, option, test, scroll, scrollY, redraw, timer, val, tab;
	int24_t worldLength, worldHeight, gamemode, worldSize, cheats, key, scrollYb;
	char *chars = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
	char *gamemodeStr[3] = {"Survival", "Creative", "Hardcore"};
	char *worldSizeStr[4] = {"Small", "Medium", "Large", "Infinite"};
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
				worldSize = 0;
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
						}
						gfx_PrintStringXY("World Name:", 74, 84);
						gfx_PrintStringXY(&worldNameStr, 150, 84);
						gfx_PrintStringXY("Seed:", 74, 104);
						gfx_PrintStringXY(&seedStr, 114, 104);
						gfx_PrintStringXY("Gamemode:", 74, 124);
						gfx_PrintStringXY(gamemodeStr[gamemode], 144, 124);
						gfx_PrintStringXY("World Size:", 74, 144);
						gfx_PrintStringXY(worldSizeStr[worldSize], 154, 144);
						gfx_PrintStringXY("Cheats:", 74, 164);
						gfx_PrintStringXY(cheatsStr[cheats], 144, 164);
						gfx_PrintStringXY("World Type:", 74, 184);
						gfx_PrintStringXY(worldTypesStr[worldType], 150, 184);
						gfx_PrintStringXY("Generate", 74, 204);
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
							worldSize++;
							if (worldSize > 3)
							{
								worldSize = 0;
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

							if (worldSize == 0)
							{
								worldLength = 100;
								worldHeight = 100;
							}
							if (worldSize == 1)
							{
								worldLength = 200;
								worldHeight = 200;
							}
							if (worldSize == 2)
							{
								worldLength = 500;
								worldHeight = 200;
							}
							if (worldSize == 3)
							{
								//infinite...well, not really... :(
								worldLength = 10000;
								worldHeight = 300;
							}

							//supposed to take the first 8 letters of the worldName string and copy to world_file
							world_file = "        ";
							memcpy(world_file, worldNameStr, 8);

							dbg_sprintf(dbgout, "%s", world_file);

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
