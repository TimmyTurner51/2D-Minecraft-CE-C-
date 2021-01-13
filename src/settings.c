
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
