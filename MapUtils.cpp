#include "CaveDecoder.h"
#include "Globals.h"
#include "MapUtils.h"
#include <algorithm>
#include <stdio.h>

MapUtils *MapUtils::singleton = NULL;
map::Object MapUtils::map[MAP_HEIGHT][MAP_WIDTH];
map::Object MapUtils::previousMap[MAP_HEIGHT][MAP_WIDTH];
map::Sprite MapUtils::bigWall, MapUtils::wall, MapUtils::grass, MapUtils::space, MapUtils::diamond, MapUtils::rock, MapUtils::rockFord;

map::Sprite MapUtils::waitRockford0, MapUtils::waitRockford1, MapUtils::waitRockford2, MapUtils::waitRockford3;
map::Sprite MapUtils::upRockford0, MapUtils::upRockford1, MapUtils::upRockford2, MapUtils::upRockford3;
map::Sprite MapUtils::downRockford0, MapUtils::downRockford1, MapUtils::downRockford2, MapUtils::downRockford3;
map::Sprite MapUtils::leftRockford0, MapUtils::leftRockford1, MapUtils::leftRockford2;
map::Sprite MapUtils::rightRockford0, MapUtils::rightRockford1, MapUtils::rightRockford2;

map::Sprite *MapUtils::matchSprite[9];
map::MatchAnimatedSprite MapUtils::matchAnimatedSprite[7];
Texture2D MapUtils::tiles;
RenderTexture2D MapUtils::mapCache;

void MapUtils::convertCaveData()
{
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			switch (CaveDecoder::caveData[j][i + 2]) {
			case 'W':
				map[i][j] = { BIGWALL, STATIONARY };
				break;
			case 'w':
				map[i][j] = { WALL, STATIONARY };
				break;
			case '.':
				map[i][j] = { GRASS, STATIONARY };
				break;
			case ' ':
				map[i][j] = { SPACE, STATIONARY };
				break;
			case 'd':
				map[i][j] = { DIAMOND, STATIONARY };
				break;
			case 'r':
				map[i][j] = { ROCK, STATIONARY };
				break;
			case 'X':
				map[i][j] = { ROCKFORD, STATIONARY };
				break;
			default:
				break;
			}
		}
	}
}

void MapUtils::drawMap()
{
	uint16_t hx, hy;
	map::Sprite *sprite;

	for (uint8_t y = 0; y < TILES_DISPLAY_HEIGHT + 2 * TILES_DISPLAY_BUFFER; y++) {
		for (uint8_t x = 0; x < TILES_DISPLAY_WIDTH + 2 * TILES_DISPLAY_BUFFER; x++) {
			hx = x * TILE_SIZE * ZOOM;
			hy = y * TILE_SIZE * ZOOM;
			int startX = std::min(std::max(0, x + countX - TILES_DISPLAY_BUFFER), MAP_WIDTH);
			int startY = std::min(std::max(0, y + countY - TILES_DISPLAY_BUFFER), MAP_HEIGHT);
			/*sprite = matchSprite[map[y + countY][x + countX].type];*/
			sprite = matchSprite[map[startY][startX].type];
			Rectangle frameSource = { (float)sprite->xsource, (float)sprite->ysource, (float)sprite->width, (float)sprite->height };
			Rectangle frameDest = { (float)(hx - shiftX) - (TILE_SIZE * ZOOM), (float)(hy - shiftY) - (TILE_SIZE * ZOOM),(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
			DrawTexturePro(*(sprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
		}
	}
}

int MapUtils::checkMove()
{
	if (currentDirection == LEFT && rockFordX > 0 &&
		(map[rockFordY][rockFordX - 1].type == BIGWALL || map[rockFordY][rockFordX - 1].type == WALL || map[rockFordY][rockFordX - 1].type == ROCK)) {
		return 0;
	}

	else if (currentDirection == RIGHT && rockFordX < MAP_WIDTH - 1 &&
		(map[rockFordY][rockFordX + 1].type == BIGWALL || map[rockFordY][rockFordX + 1].type == WALL || map[rockFordY][rockFordX + 1].type == ROCK)) {
		return 0;
	}

	else if (currentDirection == UP && rockFordY > 0 &&
		(map[rockFordY - 1][rockFordX].type == BIGWALL || map[rockFordY - 1][rockFordX].type == WALL || map[rockFordY - 1][rockFordX].type == ROCK)) {
		return 0;
	}

	else if (currentDirection == DOWN && rockFordY < MAP_HEIGHT - 1 &&
		(map[rockFordY + 1][rockFordX].type == BIGWALL || map[rockFordY + 1][rockFordX].type == WALL || map[rockFordY + 1][rockFordX].type == ROCK)) {
		return 0;
	}
	return 1;
}

int MapUtils::checkPush()
{
	if (currentDirection == RIGHT && map[rockFordY][rockFordX + 1].type == ROCK && rockFordX <= MAP_WIDTH - 3 && map[rockFordY][rockFordX + 2].type == SPACE) {
		push++;
		return RIGHT;
	}
	else if (currentDirection == LEFT && map[rockFordY][rockFordX - 1].type == ROCK && rockFordX > 1 && map[rockFordY][rockFordX - 2].type == SPACE) {
		push++;
		return LEFT;
	}

	return 0;
}

void MapUtils::moveStone()
{
	if (currentDirection == LEFT) {
		map[rockFordY][rockFordX - 2].type = map[rockFordY][rockFordX - 1].type;
		map[rockFordY][rockFordX - 1].type = SPACE;
	}
	else if (currentDirection == RIGHT) {
		map[rockFordY][rockFordX + 2].type = map[rockFordY][rockFordX + 1].type;
		map[rockFordY][rockFordX + 1].type = SPACE;
	}
}

int MapUtils::getType(int x, int y)
{
	return map[y][x].type;
}

int MapUtils::isEmpty(int x, int y)
{
	if (y < TILES_DISPLAY_HEIGHT - 1 && map[y + 1][x].type == SPACE) {
		return 1;
	}
	return 0;
}

void MapUtils::unmarkRocks()
{
	for (int y = 0; y < MAP_HEIGHT - 1; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x].type == ROCK) {
				map[y][x].mark = 0;
			}
		}
	}
}

int MapUtils::canShift(int x, int y)
{
	if ((x > 0 && map[y][x - 1].type == SPACE) || (x < TILES_DISPLAY_WIDTH - 1 && map[y][x + 1].type == SPACE)) {
		return 1;
	}
	return 0;
}

void MapUtils::move(int x, int y, int direction)
{
	map[y][x].type = SPACE;
	switch (direction) {
	case LEFT:
		map[y][x - 1] = map[y][x];
		break;
	case RIGHT:
		map[y][x + 1] = map[y][x];
		break;
	case UP:
		map[y - 1][x] = map[y][x];
		break;
	case DOWN:
		map[y + 1][x] = map[y][x];
		break;
	default:
		break;
	}
}



void MapUtils::scanBoulders(int x, int y)
{
	if (map[y][x].type == ROCK && (map[y + 1][x].type == SPACE || map[y + 1][x].type == TRANSITIONAL_SPACE) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = FALL;
	}

	if (map[y][x].type == ROCK &&  (x > 0 && map[y][x - 1].type == SPACE && map[y + 1][x - 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_LEFT;
	}

	else if (map[y][x].type == ROCK && (x < MAP_WIDTH - 1 && map[y][x + 1].type == SPACE && map[y + 1][x + 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_RIGHT;
	}

	else if (map[y][x].type == ROCK && (map[y + 1][x].type != SPACE && map[y + 1][x].type != TRANSITIONAL_SPACE && map[y + 1][x].type != TRANSITIONAL_ROCKFORD && map[y + 1][x].type != ROCKFORD) && map[y][x].falling == FALL) {
		map[y][x].falling = STATIONARY;
	}

}

void MapUtils::updateFallingBoulders(int x, int y)
{
	if (map[y][x].mark == 0 && map[y][x].type == ROCK && map[y][x].falling == FALL) {

		if (map[y + 1][x].type == ROCKFORD) {
			printf("HIT ROCKFORD AT %d,%d\n", y + 1, x);
		}

		map[y + 1][x].type = map[y][x].type;
		map[y + 1][x].falling = FALL;
		map[y + 1][x].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}

	else if (map[y][x].mark == 0 && map[y][x].type == ROCK && map[y][x].falling == ROLL_LEFT) {
		map[y][x - 1].type = map[y][x].type;
		map[y][x - 1].falling = FALL;
		map[y][x - 1].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}

	else if (map[y][x].mark == 0 && map[y][x].type == ROCK && map[y][x].falling == ROLL_RIGHT) {
		map[y][x + 1].type = map[y][x].type;
		map[y][x + 1].falling = FALL;
		map[y][x + 1].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}
}


MapUtils::MapUtils()
{
	tiles = LoadTexture("Resources/tileset.png");
	mapCache = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	for (int y = 0; y < TILES_DISPLAY_HEIGHT; y++) {
		for (int x = 0; x < TILES_DISPLAY_WIDTH; x++) {
			map[y][x].falling = STATIONARY;
		}
	}
}

MapUtils *MapUtils::getInstance()
{
	if (singleton == NULL) {
		singleton = new MapUtils();
	}
	return singleton;
}

MapUtils::~MapUtils()
{
	UnloadTexture(tiles);
	UnloadRenderTexture(mapCache);
}


void MapUtils::cutTilesSheet()
{
	bigWall = { 16 * 0, 16 * 8, 16, 16, 0, &tiles };
	wall = { 16 * 0, 16 * 8, 16, 16, 0, &tiles };
	grass = { 16 * 1, 16 * 8, 16, 16, 0, &tiles };
	space = { 16 * 2, 16 * 8, 16, 16, 0, &tiles };
	diamond = { 16 * 4, 16 * 8, 16, 16, 0, &tiles };
	rock = { 16 * 3, 16 * 8, 16, 16, 0, &tiles };
	rockFord = { 16 * 0, 16 * 0, 16, 16, 1, &tiles };

	matchSprite[BIGWALL] = &bigWall;
	matchSprite[WALL] = &wall;
	matchSprite[GRASS] = &grass;
	matchSprite[SPACE] = &space;
	matchSprite[DIAMOND] = &diamond;
	matchSprite[ROCK] = &rock;
	matchSprite[ROCKFORD] = &rockFord;
	matchSprite[TRANSITIONAL_SPACE] = &space;
	matchSprite[TRANSITIONAL_ROCKFORD] = &space;


	countX = 0;
	countY = 0;
	shiftX = 0;
	shiftY = 0;
	rockFordX = 3;
	rockFordY = 2;
	rockfordShift = 0;
	previousRockFordX = rockFordX;
	previousRockFordY = rockFordY;
	visibleX = 3;
	visibleY = 2;

	// animated sprites
	waitRockford0 = { 16 * 0, 16 * 0, 16, 16, 0, &tiles };
	waitRockford1 = { 16 * 1, 16 * 0, 16, 16, 0, &tiles };
	waitRockford2 = { 16 * 2, 16 * 0, 16, 16, 0, &tiles };
	waitRockford3 = { 16 * 3, 16 * 0, 16, 16, 0, &tiles };

	upRockford0 = { 16 * 0, 16 * 2, 16, 16, 0, &tiles };
	upRockford1 = { 16 * 1, 16 * 2, 16, 16, 0, &tiles };
	upRockford2 = { 16 * 2, 16 * 2, 16, 16, 0, &tiles };
	upRockford3 = { 16 * 3, 16 * 2, 16, 16, 0, &tiles };

	downRockford0 = { 16 * 0, 16 * 4, 16, 16, 0, &tiles };
	downRockford1 = { 16 * 1, 16 * 4, 16, 16, 0, &tiles };
	downRockford2 = { 16 * 2, 16 * 4, 16, 16, 0, &tiles };
	downRockford3 = { 16 * 3, 16 * 4, 16, 16, 0, &tiles };

	leftRockford0 = { 16 * 0, 16 * 1, 16, 16, 0, &tiles };
	leftRockford1 = { 16 * 1, 16 * 1, 16, 16, 0, &tiles };
	leftRockford2 = { 16 * 2, 16 * 1, 16, 16, 0, &tiles };

	rightRockford0 = { 16 * 0, 16 * 3, 16, 16, 0, &tiles };
	rightRockford1 = { 16 * 1, 16 * 3, 16, 16, 0, &tiles };
	rightRockford2 = { 16 * 2, 16 * 3, 16, 16, 0, &tiles };

	// restless
	matchAnimatedSprite[ROCKFORD].anim[0][0] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[0][1] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[0][2] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[0][3] = &waitRockford1;
	matchAnimatedSprite[ROCKFORD].anim[0][4] = &waitRockford1;
	matchAnimatedSprite[ROCKFORD].anim[0][5] = &waitRockford1;
	matchAnimatedSprite[ROCKFORD].anim[0][6] = &waitRockford2;
	matchAnimatedSprite[ROCKFORD].anim[0][7] = &waitRockford2;
	matchAnimatedSprite[ROCKFORD].anim[0][8] = &waitRockford2;
	matchAnimatedSprite[ROCKFORD].anim[0][9] = &waitRockford3;
	matchAnimatedSprite[ROCKFORD].anim[0][10] = &waitRockford3;
	matchAnimatedSprite[ROCKFORD].anim[0][11] = &waitRockford3;

	matchAnimatedSprite[ROCKFORD].anim[1][0] = &upRockford0;
	matchAnimatedSprite[ROCKFORD].anim[1][1] = &upRockford0;
	matchAnimatedSprite[ROCKFORD].anim[1][2] = &upRockford0;
	matchAnimatedSprite[ROCKFORD].anim[1][3] = &upRockford1;
	matchAnimatedSprite[ROCKFORD].anim[1][4] = &upRockford1;
	matchAnimatedSprite[ROCKFORD].anim[1][5] = &upRockford1;
	matchAnimatedSprite[ROCKFORD].anim[1][6] = &upRockford2;
	matchAnimatedSprite[ROCKFORD].anim[1][7] = &upRockford2;
	matchAnimatedSprite[ROCKFORD].anim[1][8] = &upRockford2;
	matchAnimatedSprite[ROCKFORD].anim[1][9] = &upRockford3;
	matchAnimatedSprite[ROCKFORD].anim[1][10] = &upRockford3;
	matchAnimatedSprite[ROCKFORD].anim[1][11] = &upRockford3;

	matchAnimatedSprite[ROCKFORD].anim[2][0] = &downRockford0;
	matchAnimatedSprite[ROCKFORD].anim[2][1] = &downRockford0;
	matchAnimatedSprite[ROCKFORD].anim[2][2] = &downRockford0;
	matchAnimatedSprite[ROCKFORD].anim[2][3] = &downRockford1;
	matchAnimatedSprite[ROCKFORD].anim[2][4] = &downRockford1;
	matchAnimatedSprite[ROCKFORD].anim[2][5] = &downRockford1;
	matchAnimatedSprite[ROCKFORD].anim[2][6] = &downRockford2;
	matchAnimatedSprite[ROCKFORD].anim[2][7] = &downRockford2;
	matchAnimatedSprite[ROCKFORD].anim[2][8] = &downRockford2;
	matchAnimatedSprite[ROCKFORD].anim[2][9] = &downRockford3;
	matchAnimatedSprite[ROCKFORD].anim[2][10] = &downRockford3;
	matchAnimatedSprite[ROCKFORD].anim[2][11] = &downRockford3;

	matchAnimatedSprite[ROCKFORD].anim[3][0] = &leftRockford0;
	matchAnimatedSprite[ROCKFORD].anim[3][1] = &leftRockford0;
	matchAnimatedSprite[ROCKFORD].anim[3][2] = &leftRockford0;
	matchAnimatedSprite[ROCKFORD].anim[3][3] = &leftRockford0;
	matchAnimatedSprite[ROCKFORD].anim[3][4] = &leftRockford1;
	matchAnimatedSprite[ROCKFORD].anim[3][5] = &leftRockford1;
	matchAnimatedSprite[ROCKFORD].anim[3][6] = &leftRockford1;
	matchAnimatedSprite[ROCKFORD].anim[3][7] = &leftRockford1;
	matchAnimatedSprite[ROCKFORD].anim[3][8] = &leftRockford2;
	matchAnimatedSprite[ROCKFORD].anim[3][9] = &leftRockford2;
	matchAnimatedSprite[ROCKFORD].anim[3][10] = &leftRockford2;
	matchAnimatedSprite[ROCKFORD].anim[3][11] = &leftRockford2;

	matchAnimatedSprite[ROCKFORD].anim[4][0] = &rightRockford0;
	matchAnimatedSprite[ROCKFORD].anim[4][1] = &rightRockford0;
	matchAnimatedSprite[ROCKFORD].anim[4][2] = &rightRockford0;
	matchAnimatedSprite[ROCKFORD].anim[4][3] = &rightRockford0;
	matchAnimatedSprite[ROCKFORD].anim[4][4] = &rightRockford1;
	matchAnimatedSprite[ROCKFORD].anim[4][5] = &rightRockford1;
	matchAnimatedSprite[ROCKFORD].anim[4][6] = &rightRockford1;
	matchAnimatedSprite[ROCKFORD].anim[4][7] = &rightRockford1;
	matchAnimatedSprite[ROCKFORD].anim[4][8] = &rightRockford2;
	matchAnimatedSprite[ROCKFORD].anim[4][9] = &rightRockford2;
	matchAnimatedSprite[ROCKFORD].anim[4][10] = &rightRockford2;
	matchAnimatedSprite[ROCKFORD].anim[4][11] = &rightRockford2;

	// default
	matchAnimatedSprite[ROCKFORD].anim[5][0] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][1] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][2] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][3] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][4] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][5] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][6] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][7] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][8] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][9] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][10] = &waitRockford0;
	matchAnimatedSprite[ROCKFORD].anim[5][11] = &waitRockford0;

	matchAnimatedSprite[ROCKFORD].currentAnim = 5;

}


