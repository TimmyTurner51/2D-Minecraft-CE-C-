
int natureBlocks[13] = {GRASS, DIRT, SMOOTHSTONE, COBBLESTONE, SAND, GRAVEL, OAKLOGS, OAKLEAVES, BEDROCK, COALORE, IRONORE, GOLDORE, LAPIZORE};
int buildingBlocks[3] = {OAKPLANK, GLASS, SPONGE};
int redstoning[3] = {REDSTONEDUSTOFF, NOTEBLOCK, REGULARPISTONRIGHTOFF};
int toolsEtc[1] = { BEDBACK };

int typesvalues[4] = {13, 3, 3, 1};

void creativeInventory(void)
{
	int24_t tab, scroll, redraw, selX, selY, posB, oldBlock, newBlock, timer, selXb, selYb, selPos, i, maxSize;
	char *names[4] = { "Nature", "Building", "Redstone", "Tools" };
	gfx_FillScreen(dayColors[timeofday / 6000]);
	gfx_SetTextFGColor(0);
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
			gfx_SetColor(254);
			gfx_FillRectangle(selX + 6, selY + 6, 3, 3);
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

		if (selY == 211)
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

		if (kb_IsDown(kb_KeyUp) && (selY == 211))
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
			if (selY < 211)
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
				selY = 211;
				posB = 0;
				redraw = 1;
				delay(80);
			}
		}
	}

	redraw = 1;
	delay(200);
}
