////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE (C)
//  Authors: TimmyTurner51 and LogicalJoe
//  License: ??
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

void LoadBlocks(void);
	uint24_t numSprites;
	uint24_t spriteSize;
	uint24_t sprites;
	uint24_t Pointer;
	uint24_t data;
	ti_var_t appvar;
	
	
	
	
	
	
	
	void LoadBlocks() {
	numSprites = 64;
	spriteSize = (16 * 16) + 2;
	appvar = ti_Open("pack", "r");
	Pointer = ti_GetDataPtr(appvar);
	ti_CloseAll;
	data = numSprites * 3;
	sprites = ti_mAlloc(N);
	x = 0;
	While (x = data) {
		{sprites + x} = Pointer;
		Pointer = Pointer + spriteSize;
		x += 3;
		}
}
