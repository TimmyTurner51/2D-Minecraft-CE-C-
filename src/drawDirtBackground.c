
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
