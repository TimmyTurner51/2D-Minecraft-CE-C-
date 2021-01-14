
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

#include "worldengine.h"
#include "defines.h"
#include "gfx/gfx.h"

void WorldEngine(void)
{
	ti_var_t appvar;
	int24_t redraw, x, y, scrollX, scrollY, playerX, playerY, playerPos, curX, curY, posX, playerPosTest, height, flymode, gamemode, timer, temptimer;

	gfx_SetTransparentColor(252);

	if (appvar = ti_Open(world_file, "r"))
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
		gfx_BlitBuffer();

		//this is an updated generator, and it to be used with the new textures...we now have bedrock too!
		//thank u LogicalJoe!!
		height = 20;

		srand(rtc_Time()); // set the seed

		for (x = 0; x < worldLength; x++)
		{

			if ((worldType != 1) && (randInt(0, 2) == 0))
			{
				height += randInt(-1, 1);
				if (height < 1)
					height += 2;
			}

			for (y = 0; y < height; y++)
			{ //air
				WorldData[x + y * worldLength] = 0;
			}
			if (randInt(1, 10) == 2)  //tree
			{
				WorldData[x + (height - 1 * worldLength)] = OAKLOGS + 1;
				WorldData[x + (height - 2 * worldLength)] = OAKLOGS + 1;
				WorldData[x + (height - 3 * worldLength)] = OAKLOGS + 1;
				WorldData[x + (height - 4 * worldLength)] = OAKLOGS + 1;
				WorldData[x + (height - 5 * worldLength)] = OAKLEAVES + 1;
				WorldData[x - 1 + (height - 4 * worldLength)] = OAKLEAVES + 1;
				WorldData[x + 1 + (height - 4 * worldLength)] = OAKLEAVES + 1;
			}
			WorldData[x + (height * worldLength)] = GRASS + 1;
			for (y = height + 1; y < height + 3; y++)
			{
				WorldData[x + y * worldLength] = DIRT + 1;
			}
			for (y = height + 3; y < worldHeight - (height + 3); y++)
			{
				WorldData[x + y * worldLength] = SMOOTHSTONE + 1;
			}
			WorldData[x + worldHeight * worldLength] = BEDROCK + 1;
		}

		for (x = 91; x < 320 - 92; x++)
		{
			//green progress bar... for looks at this point
			gfx_VertLine(x, 121, 5);
			delay(20);
			gfx_BlitBuffer();
		}

		timeofday = 0;
		flymode = 0;
		timer = 0;

		curX = 16 * 10;
		curY = 16 * 4;
		playerX = 1;
		playerY = 0;
		curPos = (curX / 16) + ((curY / 16) * worldLength) + playerX;
		scrollX = 0;
		scrollY = 0;
		posX = 0;
		hotbar[0] = 0;
		hotbar[1] = 0;
		hotbar[2] = 0;
		hotbar[3] = 0;
		hotbar[4] = 0;
		//0 is the first pos in the hotbar array (it's max size is 5 btw)
		hotbarSel = 0;
		redraw = 1;
		//set selected block to grass block...
		blockSel = 1;
	}

	//draw the world and player sprites, as well as the player cursor... (none of which exist just yet)

	LoadBlocks("MCEDEFT");

	gfx_SetDrawBuffer();

	while (1)
	{
		kb_Scan();

		if (redraw == 1)
		{
			redraw = 0;
			gfx_FillScreen(dayColors[timeofday / 6000]);
			gfx_SetColor(32);
			playerPos = (playerX + (playerY * worldLength));
			for (y = scrollY; y < 241 + scrollY; y += 16)
			{
				for (x = scrollX; x < 321 + scrollX; x += 16)
				{
					if (playerY > 2)
					{
						/* now has basic shadowing */
						if ((WorldData[playerPos] != 0) && (WorldData[playerPos - worldLength] == 0))
						{
							gfx_TransparentSprite(sprites[WorldData[playerPos] - 1], x, y);
						}
						else
						{
							if ((WorldData[playerPos] != 0) && (WorldData[playerPos + worldLength] != 0) && (WorldData[playerPos + 1] != 0) && (WorldData[playerPos - 1] != 0))
								gfx_FillRectangle(x, y, 16, 16);
							if ((WorldData[playerPos] != 0) && (WorldData[playerPos + worldLength] == 0))
								gfx_TransparentSprite(sprites[WorldData[playerPos] - 1], x, y);
							if ((WorldData[playerPos - 1] != 0) && (WorldData[playerPos] == 0))
								gfx_TransparentSprite(sprites[WorldData[playerPos - 1] - 1], x - 16, y);
							if ((WorldData[playerPos + 1] != 0) && (WorldData[playerPos] == 0))
								gfx_TransparentSprite(sprites[WorldData[playerPos + 1] - 1], x + 16, y);
						}
					}
					else
					{
						if (WorldData[playerPos] != 0)
							gfx_TransparentSprite(sprites[WorldData[playerPos] - 1], x, y);
					}

					playerPos++;
				}
				playerPos += worldLength - 21;
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

			gfx_TransparentSprite(Head_1, 16 * 9 + 2, 16 * 5 + 14);
			gfx_TransparentSprite(Body_1, 16 * 9 + 4, 16 * 5 + 22);
			gfx_TransparentSprite(Leg_1, 16 * 9 + 4, 16 * 5 + 33);
		}


		if (kb_IsDown(kb_KeyClear))
			break;

		if (kb_IsDown(kb_KeyYequ))
		{
			hotbarSel = 0;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyWindow))
		{
			hotbarSel = 1;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyZoom))
		{
			hotbarSel = 2;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyTrace))
		{
			hotbarSel = 3;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyGraph))
		{
			hotbarSel = 4;
			redraw = 1;
		}

		if (kb_IsDown(kb_Key2nd) && (WorldData[curPos] == 0))
		{
			delay(100);
			WorldData[curPos] = hotbar[hotbarSel] + 1;
			//check block updates
			/*if ((WorldData[curPos] == GRASS + 1) && (WorldData[curPos + worldLength] != 0))
				WorldData[curPos + worldLength] = DIRT + 1;*/
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyDel))
		{
			delay(100);
			WorldData[curPos] = 0;
			redraw = 1;
		}
		if (kb_IsDown(kb_KeyGraphVar))
		{
			/* inventory */
			creativeInventory();
			redraw = 1;
		}
		curX = curX + (16 * (kb_IsDown(kb_KeyStat) && (curX < 320)));
		curX = curX - (16 * (kb_IsDown(kb_KeyAlpha) && (curX > 0)));
		curPos = curPos + (kb_IsDown(kb_KeyStat) && (curX < 320));
		curPos = curPos - (kb_IsDown(kb_KeyAlpha) && (curX > 0));
		if ((kb_IsDown(kb_KeyAlpha)) || (kb_IsDown(kb_KeyStat)))
		{
			delay(100);
			redraw = 1;
		}
		curY = curY + (16 * (kb_IsDown(kb_KeyApps) && (curY < 240)));
		curY = curY - (16 * (kb_IsDown(kb_KeyMode) && (curY > 0)));
		curPos = curPos + (worldLength * (kb_IsDown(kb_KeyApps) && (curY < 240)));
		curPos = curPos - (worldLength * (kb_IsDown(kb_KeyMode) && (curY > 0)));
		if ((kb_IsDown(kb_KeyApps)) || (kb_IsDown(kb_KeyMode)))
		{
			delay(100);
			redraw = 1;
		}
		

		if ((kb_IsDown(kb_KeyLeft)) && (playerX > 0))
		{
			redraw = 1;
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
			if ((scrollX == 0) && (WorldData[ playerX + 8 + ((playerY + 7) * worldLength) ] != 0) && (WorldData[ playerX + 8 + ((playerY + 6) * worldLength) ]) == 0)
				{
					scrollY = 0;
					playerY--;
					curPos -= worldLength;
					curY += 4;
				}
		}
		if ((kb_IsDown(kb_KeyRight)) && (playerX < worldLength - 10))
		{
			redraw = 1;
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
			if ((scrollX == -16) && (WorldData[ playerX + 10 + ((playerY + 7) * worldLength) ] != 0) && (WorldData[ playerX + 10 + ((playerY + 6) * worldLength) ]) == 0)
				{
					scrollY = 0;
					playerY--;
					curPos -= worldLength;
					curY += 4;
				}
		}

		/*gfx_SetTextFGColor(0);
		gfx_SetTextXY(30, 20);
		gfx_PrintInt(timer, 1);
		gfx_SetTextXY(30, 40);
		gfx_PrintInt(flymode, 1);
		gfx_SetTextFGColor(254);*/

		//if (timer != 0) timer++;
		//if (timer > 8) timer = 0;
		
		//if (kb_IsDown(kb_KeyUp) && (timer < 4)) timer = 0;

		if (kb_IsDown(kb_KeyVars))
		{
			gfx_SetTextFGColor(0);
			flymode = (flymode == 0);
			timer = 2;
			temptimer = 0;
			redraw = 1;
			delay(100);
		}
		if (timer == 2) {
			temptimer++;
			if (flymode == 1)
			gfx_PrintStringXY("Flying mode toggled to On", 4, 4);
			if (flymode == 0)
			gfx_PrintStringXY("Flying mode toggled to Off", 4, 4);
			gfx_BlitBuffer();
			if (temptimer > 100)
			{
				temptimer = 0;
				timer = 0;
				redraw = 1;
			}
		}

		if ((kb_IsDown(kb_KeyUp)) && (playerY > 0) && (WorldData[ playerX + 9 + ((playerY + 6) * worldLength) ] == 0))
		{
			redraw = 1;
			/*timer = 1;
			if (timer > 4)
			{
				if (flymode == 0)
				{
					flymode = 1;
					timer = 0;
				}
				if (flymode == 1)
				{
					flymode = 0;
					timer = 0;
				}
			}*/
				if (scrollY == 0)
				{
					scrollY = -16;
					playerY--;
					curPos -= worldLength;
					curY -= 16;
				}
				scrollY += 4;
				curY += 4;
		}
		if ((kb_IsDown(kb_KeyDown)) && (playerY < worldHeight))
		{
			redraw = 1;
			if (scrollY == -16)
			{
				scrollY = 0;
				playerY++;
				curPos += worldLength;
				curY += 16;
			}
			if (flymode == 0)
			{
				scrollY -= 2;
				curY -= 2;
			}
		}

		//gravity...
		if ((flymode == 0) && !kb_IsDown(kb_KeyUp) && WorldData[ playerX + 9 + ((playerY + 8) * worldLength) ] == 0)
		{
			redraw = 1;
			if (scrollY == -16)
			{
				scrollY = 0;
				playerY++;
				curPos += worldLength;
				curY += 16;
			}
			scrollY -= 4;
			curY -= 4;
		}

		if (timeofday % 6000) redraw = 1;
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

