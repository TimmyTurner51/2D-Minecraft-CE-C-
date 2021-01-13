void MainMenu(void)
{
	int24_t CursorY, x, i, option, test, scroll, scrollY, redraw, timer, val, tab, worldLength, worldHeight;
	int24_t gamemode, worldSize, cheats;
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