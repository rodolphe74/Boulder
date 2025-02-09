#pragma once
#include <stdint.h>
#include <set>
#include "raylib.h"

//#define MAX(x, y) (((x) > (y)) ? (x) : (y))
//#define MIN(x, y) (((x) < (y)) ? (x) : (y))
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

#define SPRITES_COUNT 10
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

	static Texture2D tiles;
	static RenderTexture2D mapCache;
	static MapUtils *singleton;
	MapUtils();

public:
	static MapUtils *getInstance();
	~MapUtils();

	static map::Sprite bigWall, wall, grass, space, diamond, rock, rockFord, explode;
	static map::Sprite waitRockford0, waitRockford1, waitRockford2, waitRockford3;
	static map::Sprite upRockford0, upRockford1, upRockford2, upRockford3;
	static map::Sprite downRockford0, downRockford1, downRockford2, downRockford3;
	static map::Sprite endRockford0, endRockford1;
	static map::Sprite diamond0, diamond1, diamond2, diamond3;
	static map::Sprite rock0, rock1, rock2, rock3;
	static map::Sprite grass0, grass1, grass2, grass3;
	static map::Sprite space0, space1, space2, space3;


	static map::Sprite leftRockford0, leftRockford1, leftRockford2;
	static map::Sprite rightRockford0, rightRockford1, rightRockford2;
	static map::Sprite explode0, explode1;
	static map::Object map[MAP_HEIGHT][MAP_WIDTH];
	static map::Object previousMap[MAP_HEIGHT][MAP_WIDTH];
	static map::Sprite *matchSprite[SPRITES_COUNT];
	static map::MatchAnimatedSprite matchAnimatedSprite[SPRITES_COUNT];
	static std::set<map::Explosion *> explosions;


	void cutTilesSheet();
	void convertCaveData();
	void drawMap();
	int checkMove();
	int checkPush();
	void moveStone();
	int getType(int x, int y);
	int isEmpty(int x, int y);
	void unmarkBouldersAndDiamonds();
	int canShift(int x, int y);
	void move(int x, int y, int direction);
	void scanBouldersAndDiamonds(int x, int y);
	void updateFallingBouldersAndDiamonds(int x, int y);
};
