#include "CaveDecoder.h"
#include "Globals.h"
#include "MapUtils.h"
#include <algorithm>
#include <stdio.h>

std::unique_ptr<MapUtils> MapUtils::singleton = NULL;

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
				rockFordX = j;
				rockFordY = i;
				printf("Rockford at %d,%d\n", j, i + 2);
				break;
			case 'P':
				map[i][j] = { OUT, STATIONARY };
				exitX = j;
				exitY = i;
				printf("Exit at %d,%d\n", j, i + 2);
				break;
			default:
				printf("Unknown:%c at %d,%d\n", CaveDecoder::caveData[j][i + 2], j, i + 2);
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
			int startX = std::min(std::max(0, x + countX - TILES_DISPLAY_BUFFER), MAP_WIDTH - 1);
			int startY = std::min(std::max(0, y + countY - TILES_DISPLAY_BUFFER), MAP_HEIGHT - 1);
			/*sprite = matchSprite[map[y + countY][x + countX].type];*/
			uint8_t type = map[startY][startX].type;
			sprite = matchSprite[type];
			if ((type == ROCKFORD && rockfordAnimFlag && sprite->isAnim) || (type != ROCKFORD && sprite->isAnim)) {
				uint8_t currentAnim = mapUtils->matchAnimatedSprite[type].currentAnim;
				uint8_t animCount = mapUtils->matchAnimatedSprite[type].animCount;
				sprite = matchAnimatedSprite[type].anim[currentAnim][countFrames % animCount];
			}
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

void MapUtils::unmarkBouldersAndDiamonds()
{
	for (int y = 0; y < MAP_HEIGHT - 1; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x].type == ROCK || map[y][x].type == DIAMOND) {
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



void MapUtils::scanBouldersAndDiamonds(int x, int y)
{
	if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (map[y + 1][x].type == SPACE || map[y + 1][x].type == TRANSITIONAL_SPACE) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = FALL;
	}

	if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (x > 0 && map[y][x - 1].type == SPACE && map[y + 1][x - 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_LEFT;
	}

	else if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (x < MAP_WIDTH - 1 && map[y][x + 1].type == SPACE && map[y + 1][x + 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_RIGHT;
	}

	else if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (map[y + 1][x].type != SPACE && map[y + 1][x].type != TRANSITIONAL_SPACE && map[y + 1][x].type != TRANSITIONAL_ROCKFORD && map[y + 1][x].type != ROCKFORD) && map[y][x].falling == FALL) {
		map[y][x].falling = STATIONARY;
	}

}

void MapUtils::updateFallingBouldersAndDiamonds(int x, int y)
{
	if (map[y][x].mark == 0 && (map[y][x].type == ROCK || map[y][x].type == DIAMOND) && map[y][x].falling == FALL) {

		if (map[y + 1][x].type == ROCKFORD) {
			printf("HIT ROCKFORD AT %d,%d\n", y + 1, x);
			map::Explosion *e = new map::Explosion;
			*e = { (uint16_t)(x - countX), (uint16_t)(y + 1 - countY), ROCKFORD, 128 };
			explosions.insert(e);
		}

		map[y + 1][x].type = map[y][x].type;
		map[y + 1][x].falling = FALL;
		map[y + 1][x].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}

	else if (map[y][x].mark == 0 && (map[y][x].type == ROCK || map[y][x].type == DIAMOND) && map[y][x].falling == ROLL_LEFT) {
		map[y][x - 1].type = map[y][x].type;
		map[y][x - 1].falling = FALL;
		map[y][x - 1].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}

	else if (map[y][x].mark == 0 && (map[y][x].type == ROCK || map[y][x].type == DIAMOND) && map[y][x].falling == ROLL_RIGHT) {
		map[y][x + 1].type = map[y][x].type;
		map[y][x + 1].falling = FALL;
		map[y][x + 1].mark = 1;
		map[y][x].type = SPACE;
		map[y][x].falling = STATIONARY;
	}
}


MapUtils::MapUtils()
{
	tiles = LoadTexture("Resources/tileset_1.png");
	mapCache = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
	for (int y = 0; y < TILES_DISPLAY_HEIGHT; y++) {
		for (int x = 0; x < TILES_DISPLAY_WIDTH; x++) {
			map[y][x].falling = STATIONARY;
		}
	}
}

MapUtils *MapUtils::getInstance()
{
	if (singleton.get() == NULL) {
		singleton = std::make_unique<MapUtils>();
	}
	return singleton.get();
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
	grass = { 16 * 1, 16 * 8, 16, 16, 1, &tiles };
	space = { 16 * 2, 16 * 8, 16, 16, 1, &tiles };
	diamond = { 16 * 4, 16 * 8, 16, 16, 1, &tiles };
	rock = { 16 * 3, 16 * 8, 16, 16, 1, &tiles };
	rockFord = { 16 * 0, 16 * 0, 16, 16, 1, &tiles };
	winRockford = { 16 * 2, 16 * 0, 16, 16, 1, &tiles };
	explode = { 16 * 11, 16 * 8, 16, 16, 1, &tiles };
	preOut = { 16 * 0, 16 * 8, 16, 16, 0, &tiles };

	matchSprite[BIGWALL] = &bigWall;
	matchSprite[WALL] = &wall;
	matchSprite[GRASS] = &grass;
	matchSprite[SPACE] = &space;
	matchSprite[DIAMOND] = &diamond;
	matchSprite[ROCK] = &rock;
	matchSprite[ROCKFORD] = &rockFord;
	matchSprite[TRANSITIONAL_SPACE] = &space;
	matchSprite[TRANSITIONAL_ROCKFORD] = &space;
	matchSprite[EXPLODE] = &explode;
	matchSprite[OUT] = &preOut;
	matchSprite[WIN_ROCKFORD] = &winRockford;

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

	endRockford0 = { 16 * 4, 16 * 5, 16, 16, 0, &tiles };
	endRockford1 = { 16 * 5, 16 * 5, 16, 16, 0, &tiles };

	explode0 = { 16 * 11, 16 * 8, 16, 16, 0, &tiles };
	explode1 = { 16 * 11, 16 * 9, 16, 16, 0, &tiles };

	diamond0 = { 16 * 4, 16 * 8, 16, 16, 0, &tiles };
	diamond1 = { 16 * 4, 16 * 9, 16, 16, 0, &tiles };
	diamond2 = { 16 * 4, 16 * 10, 16, 16, 0, &tiles };
	diamond3 = { 16 * 4, 16 * 11, 16, 16, 0, &tiles };

	rock0 = { 16 * 3, 16 * 8, 16, 16, 0, &tiles };
	rock1 = { 16 * 3, 16 * 9, 16, 16, 0, &tiles };
	rock2 = { 16 * 3, 16 * 10, 16, 16, 0, &tiles };
	rock3 = { 16 * 3, 16 * 11, 16, 16, 0, &tiles };

	grass0 = { 16 * 1, 16 * 8, 16, 16, 0, &tiles };
	grass1 = { 16 * 1, 16 * 9, 16, 16, 0, &tiles };
	grass2 = { 16 * 1, 16 * 10, 16, 16, 0, &tiles };
	grass3 = { 16 * 1, 16 * 11, 16, 16, 0, &tiles };

	space0 = { 16 * 2, 16 * 8, 16, 16, 0, &tiles };
	space1 = { 16 * 2, 16 * 9, 16, 16, 0, &tiles };
	space2 = { 16 * 2, 16 * 10, 16, 16, 0, &tiles };
	space3 = { 16 * 2, 16 * 11, 16, 16, 0, &tiles };

	out0 = { 16 * 0, 16 * 8, 16, 16, 0, &tiles };
	out1 = { 16 * 0, 16 * 9, 16, 16, 0, &tiles };

	winRockford0 = { 16 * 2, 16 * 0, 16, 16, 0, &tiles };
	winRockford1 = { 16 * 3, 16 * 0, 16, 16, 0, &tiles };


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

	// end
	matchAnimatedSprite[ROCKFORD].anim[6][0] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][1] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][2] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][3] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][4] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][5] = &endRockford0;
	matchAnimatedSprite[ROCKFORD].anim[6][6] = &endRockford1;
	matchAnimatedSprite[ROCKFORD].anim[6][7] = &endRockford1;
	matchAnimatedSprite[ROCKFORD].anim[6][8] = &endRockford1;
	matchAnimatedSprite[ROCKFORD].anim[6][9] = &endRockford1;
	matchAnimatedSprite[ROCKFORD].anim[6][10] = &endRockford1;
	matchAnimatedSprite[ROCKFORD].anim[6][11] = &endRockford1;

	// explode
	matchAnimatedSprite[EXPLODE].anim[0][0] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][1] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][2] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][3] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][4] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][5] = &explode0;
	matchAnimatedSprite[EXPLODE].anim[0][6] = &explode1;
	matchAnimatedSprite[EXPLODE].anim[0][7] = &explode1;
	matchAnimatedSprite[EXPLODE].anim[0][8] = &explode1;
	matchAnimatedSprite[EXPLODE].anim[0][9] = &explode1;
	matchAnimatedSprite[EXPLODE].anim[0][10] = &explode1;
	matchAnimatedSprite[EXPLODE].anim[0][11] = &explode1;

	// diamond
	matchAnimatedSprite[DIAMOND].anim[0][0] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][1] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][2] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][3] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][4] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][5] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][6] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][7] = &diamond0;
	matchAnimatedSprite[DIAMOND].anim[0][8] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][9] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][10] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][11] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][12] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][13] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][14] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][15] = &diamond1;
	matchAnimatedSprite[DIAMOND].anim[0][16] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][17] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][18] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][19] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][20] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][21] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][22] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][23] = &diamond2;
	matchAnimatedSprite[DIAMOND].anim[0][24] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][25] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][26] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][27] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][28] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][29] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][30] = &diamond3;
	matchAnimatedSprite[DIAMOND].anim[0][31] = &diamond3;

	// diamond
	matchAnimatedSprite[ROCK].anim[0][0] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][1] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][2] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][3] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][4] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][5] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][6] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][7] = &rock0;
	matchAnimatedSprite[ROCK].anim[0][8] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][9] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][10] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][11] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][12] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][13] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][14] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][15] = &rock1;
	matchAnimatedSprite[ROCK].anim[0][16] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][17] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][18] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][19] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][20] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][21] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][22] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][23] = &rock2;
	matchAnimatedSprite[ROCK].anim[0][24] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][25] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][26] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][27] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][28] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][29] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][30] = &rock3;
	matchAnimatedSprite[ROCK].anim[0][31] = &rock3;

	// grass
	matchAnimatedSprite[GRASS].anim[0][0] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][1] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][2] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][3] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][4] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][5] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][6] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][7] = &grass0;
	matchAnimatedSprite[GRASS].anim[0][8] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][9] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][10] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][11] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][12] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][13] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][14] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][15] = &grass1;
	matchAnimatedSprite[GRASS].anim[0][16] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][17] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][18] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][19] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][20] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][21] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][22] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][23] = &grass2;
	matchAnimatedSprite[GRASS].anim[0][24] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][25] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][26] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][27] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][28] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][29] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][30] = &grass3;
	matchAnimatedSprite[GRASS].anim[0][31] = &grass3;

	// space
	matchAnimatedSprite[SPACE].anim[0][0] = &space0;
	matchAnimatedSprite[SPACE].anim[0][1] = &space0;
	matchAnimatedSprite[SPACE].anim[0][2] = &space0;
	matchAnimatedSprite[SPACE].anim[0][3] = &space0;
	matchAnimatedSprite[SPACE].anim[0][4] = &space0;
	matchAnimatedSprite[SPACE].anim[0][5] = &space0;
	matchAnimatedSprite[SPACE].anim[0][6] = &space0;
	matchAnimatedSprite[SPACE].anim[0][7] = &space0;
	matchAnimatedSprite[SPACE].anim[0][8] = &space1;
	matchAnimatedSprite[SPACE].anim[0][9] = &space1;
	matchAnimatedSprite[SPACE].anim[0][10] = &space1;
	matchAnimatedSprite[SPACE].anim[0][11] = &space1;
	matchAnimatedSprite[SPACE].anim[0][12] = &space1;
	matchAnimatedSprite[SPACE].anim[0][13] = &space1;
	matchAnimatedSprite[SPACE].anim[0][14] = &space1;
	matchAnimatedSprite[SPACE].anim[0][15] = &space1;
	matchAnimatedSprite[SPACE].anim[0][16] = &space2;
	matchAnimatedSprite[SPACE].anim[0][17] = &space2;
	matchAnimatedSprite[SPACE].anim[0][18] = &space2;
	matchAnimatedSprite[SPACE].anim[0][19] = &space2;
	matchAnimatedSprite[SPACE].anim[0][20] = &space2;
	matchAnimatedSprite[SPACE].anim[0][21] = &space2;
	matchAnimatedSprite[SPACE].anim[0][22] = &space2;
	matchAnimatedSprite[SPACE].anim[0][23] = &space2;
	matchAnimatedSprite[SPACE].anim[0][24] = &space3;
	matchAnimatedSprite[SPACE].anim[0][25] = &space3;
	matchAnimatedSprite[SPACE].anim[0][26] = &space3;
	matchAnimatedSprite[SPACE].anim[0][27] = &space3;
	matchAnimatedSprite[SPACE].anim[0][28] = &space3;
	matchAnimatedSprite[SPACE].anim[0][29] = &space3;
	matchAnimatedSprite[SPACE].anim[0][30] = &space3;
	matchAnimatedSprite[SPACE].anim[0][31] = &space3;

	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][0] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][1] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][2] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][3] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][4] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][5] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][6] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][7] = &space0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][8] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][9] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][10] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][11] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][12] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][13] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][14] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][15] = &space1;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][16] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][17] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][18] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][19] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][20] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][21] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][22] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][23] = &space2;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][24] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][25] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][26] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][27] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][28] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][29] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][30] = &space3;
	matchAnimatedSprite[TRANSITIONAL_SPACE].anim[0][31] = &space3;

	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][0] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][1] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][2] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][3] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][4] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][5] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][6] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][7] = &space0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][8] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][9] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][10] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][11] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][12] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][13] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][14] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][15] = &space1;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][16] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][17] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][18] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][19] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][20] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][21] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][22] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][23] = &space2;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][24] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][25] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][26] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][27] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][28] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][29] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][30] = &space3;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].anim[0][31] = &space3;

	// out
	matchAnimatedSprite[OUT].anim[0][0] = &out0;
	matchAnimatedSprite[OUT].anim[0][1] = &out0;
	matchAnimatedSprite[OUT].anim[0][2] = &out0;
	matchAnimatedSprite[OUT].anim[0][3] = &out0;
	matchAnimatedSprite[OUT].anim[0][4] = &out0;
	matchAnimatedSprite[OUT].anim[0][5] = &out0;
	matchAnimatedSprite[OUT].anim[0][6] = &out0;
	matchAnimatedSprite[OUT].anim[0][7] = &out0;
	matchAnimatedSprite[OUT].anim[0][8] = &out0;
	matchAnimatedSprite[OUT].anim[0][9] = &out0;
	matchAnimatedSprite[OUT].anim[0][10] = &out0;
	matchAnimatedSprite[OUT].anim[0][11] = &out0;
	matchAnimatedSprite[OUT].anim[0][12] = &out0;
	matchAnimatedSprite[OUT].anim[0][13] = &out0;
	matchAnimatedSprite[OUT].anim[0][14] = &out0;
	matchAnimatedSprite[OUT].anim[0][15] = &out0;
	matchAnimatedSprite[OUT].anim[0][16] = &out1;
	matchAnimatedSprite[OUT].anim[0][17] = &out1;
	matchAnimatedSprite[OUT].anim[0][18] = &out1;
	matchAnimatedSprite[OUT].anim[0][19] = &out1;
	matchAnimatedSprite[OUT].anim[0][20] = &out1;
	matchAnimatedSprite[OUT].anim[0][21] = &out1;
	matchAnimatedSprite[OUT].anim[0][22] = &out1;
	matchAnimatedSprite[OUT].anim[0][23] = &out1;
	matchAnimatedSprite[OUT].anim[0][24] = &out1;
	matchAnimatedSprite[OUT].anim[0][25] = &out1;
	matchAnimatedSprite[OUT].anim[0][26] = &out1;
	matchAnimatedSprite[OUT].anim[0][27] = &out1;
	matchAnimatedSprite[OUT].anim[0][28] = &out1;
	matchAnimatedSprite[OUT].anim[0][29] = &out1;
	matchAnimatedSprite[OUT].anim[0][30] = &out1;
	matchAnimatedSprite[OUT].anim[0][31] = &out1;

	// Rockford win
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][0] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][1] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][2] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][3] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][4] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][5] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][6] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][7] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][8] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][9] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][10] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][11] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][12] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][13] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][14] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][15] = &winRockford0;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][16] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][17] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][18] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][19] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][20] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][21] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][22] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][23] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][24] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][25] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][26] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][27] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][28] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][29] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][30] = &winRockford1;
	matchAnimatedSprite[WIN_ROCKFORD].anim[0][31] = &winRockford1;

	// init
	matchAnimatedSprite[ROCKFORD].currentAnim = 5;
	matchAnimatedSprite[ROCKFORD].animCount = 12;

	matchAnimatedSprite[EXPLODE].currentAnim = 0;
	matchAnimatedSprite[EXPLODE].animCount = 12;

	matchAnimatedSprite[DIAMOND].currentAnim = 0;
	matchAnimatedSprite[DIAMOND].animCount = 32;

	matchAnimatedSprite[ROCK].currentAnim = 0;
	matchAnimatedSprite[ROCK].animCount = 32;

	matchAnimatedSprite[GRASS].currentAnim = 0;
	matchAnimatedSprite[GRASS].animCount = 32;

	matchAnimatedSprite[SPACE].currentAnim = 0;
	matchAnimatedSprite[SPACE].animCount = 32;

	matchAnimatedSprite[TRANSITIONAL_SPACE].currentAnim = 0;
	matchAnimatedSprite[TRANSITIONAL_SPACE].animCount = 32;

	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].currentAnim = 0;
	matchAnimatedSprite[TRANSITIONAL_ROCKFORD].animCount = 32;

	matchAnimatedSprite[OUT].currentAnim = 0;
	matchAnimatedSprite[OUT].animCount = 32;

	matchAnimatedSprite[WIN_ROCKFORD].currentAnim = 0;
	matchAnimatedSprite[WIN_ROCKFORD].animCount = 32;
}


