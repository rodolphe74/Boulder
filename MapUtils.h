#pragma once
#include "GameContext.h"
#include "raylib.h"
#include <set>
#include <stdint.h>

#define MAP_WIDTH 40
#define MAP_HEIGHT 22
#define TILES_DISPLAY_WIDTH 21
#define TILES_DISPLAY_HEIGHT 15
#define TILE_SIZE 16
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1008
#define SCROLL_BORDER 5
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define SHIFT 5
#define SPEED 1
#define ZOOM 3
#define FRAMES_BEFORE_PUSH 32

#define STATIONARY 0
#define FALL 1
#define ROLL_LEFT 2
#define ROLL_RIGHT 3

#define SPRITES_COUNT 20
#define BIGWALL 0
#define WALL 1
#define GRASS 2
#define SPACE 3
#define DIAMOND 4
#define ROCK 5
#define ROCKFORD 6
#define TRANSITIONAL_SPACE 7
#define TRANSITIONAL_ROCKFORD 8
#define EXPLODE 9
#define OUT 10
#define WIN_ROCKFORD 11
#define FIREFLY 12
#define UNKNOWN 13
#define BUTTERFLY 14
#define AMOEBA 15

#define MAX_AMOEBAS 200

extern uint8_t LEFT_DIRECTION[];
extern uint8_t RIGHT_DIRECTION[];

namespace map {
	struct Sprite {
		uint16_t xsource, ysource;
		uint16_t width, height;
		uint8_t isAnim;
		Texture2D *bitmap;
	};
	typedef struct Sprite Sprite;

	struct Object {
		uint8_t type;
		uint8_t falling;
		uint8_t mark;
		uint8_t direction;
	};
	typedef struct Object Object;

	struct MatchAnimatedSprite {
		Sprite *anim[8][32];
		uint8_t currentAnim;
		uint8_t animCount;
	};
	typedef struct MatchAnimatedSprite MatchAnimatedSprite;

	struct Explosion {
		uint16_t x, y;
		uint8_t type;
		uint8_t count;
		bool operator <(const Explosion &pt) const
		{
			//return (x < pt.x) || ((!(pt.x < x)) && (y < pt.y));
			return ((void *)this < (void *)&pt);
		}
	};
	typedef struct Explosion Explosion;
}

class MapUtils
{
private:

	Texture2D tiles;
	RenderTexture2D mapCache;
	static MapUtils *singleton;
	MapUtils();
	~MapUtils();

public:
	static MapUtils *getInstance();
	static void releaseInstance();

	map::Sprite bigWall, wall, grass, space, diamond, rock, rockFord, explode, preOut, winRockford, firefly, butterfly, amoeba;
	map::Sprite waitRockford0, waitRockford1, waitRockford2, waitRockford3;
	map::Sprite upRockford0, upRockford1, upRockford2, upRockford3;
	map::Sprite downRockford0, downRockford1, downRockford2, downRockford3;
	map::Sprite endRockford0, endRockford1;
	map::Sprite winRockford0, winRockford1;
	map::Sprite diamond0, diamond1, diamond2, diamond3;
	map::Sprite rock0, rock1, rock2, rock3;
	map::Sprite grass0, grass1, grass2, grass3;
	map::Sprite space0, space1, space2, space3;
	map::Sprite out0, out1;
	map::Sprite leftRockford0, leftRockford1, leftRockford2;
	map::Sprite rightRockford0, rightRockford1, rightRockford2;
	map::Sprite explode0, explode1;
	map::Sprite firefly0, firefly1, firefly2, firefly3;
	map::Sprite butterfly0, butterfly1, butterfly2, butterfly3;
	map::Sprite amoeba0, amoeba1, amoeba2, amoeba3;

	map::Object map[MAP_HEIGHT][MAP_WIDTH];
	map::Object previousMap[MAP_HEIGHT][MAP_WIDTH];
	map::Sprite *matchSprite[SPRITES_COUNT];
	map::MatchAnimatedSprite matchAnimatedSprite[SPRITES_COUNT];
	std::set<map::Explosion *> explosions;

	void cutTilesSheet(GameContext *gc);
	void convertCaveData(GameContext *gc);
	void drawMap(GameContext *gc);
	int checkMove(GameContext *gc);
	int checkPush(GameContext *gc);
	void moveStone(GameContext *gc);
	int getType(int x, int y);
	int isEmpty(int x, int y);
	void unmarkBouldersAndDiamonds();
	int canShift(int x, int y);
	void move(int x, int y, int direction);
	void scanBouldersAndDiamonds(int x, int y);
	void updateFallingBouldersAndDiamonds(int x, int y, GameContext *gc);
};
