#pragma once

typedef unsigned char UBYTE;
typedef short objectType;
typedef UBYTE CAVE[40][24];

class CaveDecoder
{
private:


	/* Creature code conversion table */
	/* Converts the C64 BoulderDash codes into the codes used by Jeff Bevis's Amiga implementation of BoulderDash. */
	static UBYTE creatureCode[64];

	/* Human-readable names for the 64 different C64 object codes */
	const static char *objects[];

	/* DrawLine data */
	/* When drawing lines, you can draw in all eight directions. This table gives the offsets needed to move in each of the 8 directions. */
	static int ldx[8];
	static int ldy[8];

	/* Human-readable versions of the 8 directions */
	const static char *directions[];

	/* C64 color names */
	const static char *colors[];
	const static char *caveNames[];
	const static char *caveDescriptions[];

	void StoreObject(int x, int y, objectType anObject);
	void DrawLine(objectType anObject, int x, int y, int aLength, int aDirection);
	void DrawFilledRect(objectType anObject, int x, int y, int aWidth, int aHeight, objectType aFillObject);
	void DrawRect(objectType anObject, int x, int y, int aWidth, int aHeight);
	void NextRandom(int *RandSeed1, int *RandSeed2);


public:
	static const UBYTE cave1[];
	static const UBYTE cave2[];
	static const UBYTE cave3[];
	static const UBYTE cave4[];
	static const UBYTE cave5[];
	static const UBYTE cave6[];
	static const UBYTE cave7[];
	static const UBYTE cave8[];
	static const UBYTE cave9[];
	static const UBYTE cave10[];
	static const UBYTE cave11[];
	static const UBYTE cave12[];
	static const UBYTE cave13[];
	static const UBYTE cave14[];
	static const UBYTE cave15[];
	static const UBYTE cave16[];
	static const UBYTE cave17[];
	static const UBYTE cave18[];
	static const UBYTE cave19[];
	static const UBYTE cave20[];
	static CAVE caveData;
	void DecodeCave(const UBYTE *aCaveData);
};

