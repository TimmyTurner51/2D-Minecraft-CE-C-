////////////////////////////////////////////////////////////////////////////////
//  2D Minecraft CE (C)
//  Authors: TimmyTurner51 and LogicalJoe
//  License: ??
//  Description: A 2D Minecraft clone made for the TI-84 Plus CE, written in C.
////////////////////////////////////////////////////////////////////////////////

[i]OPEN THE APPVAR
64->NSPRITES
16*16+2->SPRITESIZE
Open(Str0,"r"->D
GetDataPtr(D->PTR
Close(D
NSPRITES*3->N
Alloc(N->SPRITES
0->A
Repeat A=N
	PTR->{SPRITES+A
	PTR+SPRITESIZE->PTR
	A+3->A
End

