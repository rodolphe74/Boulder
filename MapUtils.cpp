#include "CaveDecoder.h"
#include "MapUtils.h"
#include <algorithm>
#include <stdio.h>

MapUtils *MapUtils::singleton = nullptr;
uint8_t LEFT_DIRECTION[] = { 0, LEFT, RIGHT, DOWN, UP };
uint8_t RIGHT_DIRECTION[] = { 0, RIGHT, LEFT, UP, DOWN };

void MapUtils::convertCaveData(GameContext *gc)
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
				gc->rockFordX = j;
				gc->rockFordY = i;
				printf("Rockford at %d,%d\n", j, i + 2);
				break;
			case 'P':
				map[i][j] = { OUT, STATIONARY };
				gc->exitX = j;
				gc->exitY = i;
				printf("Exit at %d,%d\n", j, i + 2);
				break;
			case 'q':
				map[i][j] = { FIREFLY, STATIONARY, 0, LEFT };
				break;
			case 'B':
				map[i][j] = { BUTTERFLY, STATIONARY, 0, LEFT };
				break;
			case 'a':
				map[i][j] = { AMOEBA, STATIONARY };
				break;
			case 'm':
				map[i][j] = { MAGIC_WALL, STATIONARY };
				break;
			default:
				printf("Unknown:%c at %d,%d\n", CaveDecoder::caveData[j][i + 2], j, i + 2);
				break;
			}
		}
	}
}

void MapUtils::drawMap(GameContext *gc)
{
	uint16_t hx, hy;
	map::Sprite *sprite;

	for (uint8_t y = 0; y < TILES_DISPLAY_HEIGHT + 2 * TILES_DISPLAY_BUFFER; y++) {
		for (uint8_t x = 0; x < TILES_DISPLAY_WIDTH + 2 * TILES_DISPLAY_BUFFER; x++) {
			hx = x * TILE_SIZE * ZOOM;
			hy = y * TILE_SIZE * ZOOM;
			int startX = std::min(std::max(0, x + gc->countX - TILES_DISPLAY_BUFFER), MAP_WIDTH - 1);
			int startY = std::min(std::max(0, y + gc->countY - TILES_DISPLAY_BUFFER), MAP_HEIGHT - 1);
			/*sprite = matchSprite[map[y + countY][x + countX].type];*/
			uint8_t type = map[startY][startX].type;
			sprite = matchSprite[type];
			if ((type == ROCKFORD && gc->rockfordAnimFlag && sprite->isAnim) || (type != ROCKFORD && sprite->isAnim)) {
				uint8_t currentAnim = matchAnimatedSprite[type].currentAnim;
				uint8_t animCount = matchAnimatedSprite[type].animCount;
				sprite = matchAnimatedSprite[type].anim[currentAnim][gc->countFrames % animCount];
			}
			Rectangle frameSource = { (float)sprite->xsource, (float)sprite->ysource, (float)sprite->width, (float)sprite->height };
			Rectangle frameDest = { (float)(hx - gc->shiftX) - (TILE_SIZE * ZOOM), (float)(hy - gc->shiftY) - (TILE_SIZE * ZOOM),(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
			DrawTexturePro(*(sprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
		}
	}
}

int MapUtils::checkMove(GameContext *gc)
{
	if (gc->currentDirection == LEFT && gc->rockFordX > 0 &&
		(map[gc->rockFordY][gc->rockFordX - 1].type == BIGWALL || map[gc->rockFordY][gc->rockFordX - 1].type == WALL || map[gc->rockFordY][gc->rockFordX - 1].type == ROCK || map[gc->rockFordY][gc->rockFordX - 1].type == MAGIC_WALL)) {
		return 0;
	}

	else if (gc->currentDirection == RIGHT && gc->rockFordX < MAP_WIDTH - 1 &&
		(map[gc->rockFordY][gc->rockFordX + 1].type == BIGWALL || map[gc->rockFordY][gc->rockFordX + 1].type == WALL || map[gc->rockFordY][gc->rockFordX + 1].type == ROCK || map[gc->rockFordY][gc->rockFordX + 1].type == MAGIC_WALL)) {
		return 0;
	}

	else if (gc->currentDirection == UP && gc->rockFordY > 0 &&
		(map[gc->rockFordY - 1][gc->rockFordX].type == BIGWALL || map[gc->rockFordY - 1][gc->rockFordX].type == WALL || map[gc->rockFordY - 1][gc->rockFordX].type == ROCK || map[gc->rockFordY - 1][gc->rockFordX].type == MAGIC_WALL)) {
		return 0;
	}

	else if (gc->currentDirection == DOWN && gc->rockFordY < MAP_HEIGHT - 1 &&
		(map[gc->rockFordY + 1][gc->rockFordX].type == BIGWALL || map[gc->rockFordY + 1][gc->rockFordX].type == WALL || map[gc->rockFordY + 1][gc->rockFordX].type == ROCK || map[gc->rockFordY + 1][gc->rockFordX - 1].type == MAGIC_WALL)) {
		return 0;
	}
	return 1;
}

int MapUtils::checkPush(GameContext *gc)
{
	if (gc->currentDirection == RIGHT && map[gc->rockFordY][gc->rockFordX + 1].type == ROCK && gc->rockFordX <= MAP_WIDTH - 3 && map[gc->rockFordY][gc->rockFordX + 2].type == SPACE) {
		gc->push++;
		return RIGHT;
	}
	else if (gc->currentDirection == LEFT && map[gc->rockFordY][gc->rockFordX - 1].type == ROCK && gc->rockFordX > 1 && map[gc->rockFordY][gc->rockFordX - 2].type == SPACE) {
		gc->push++;
		return LEFT;
	}

	return 0;
}

void MapUtils::moveStone(GameContext *gc)
{
	if (gc->currentDirection == LEFT) {
		map[gc->rockFordY][gc->rockFordX - 2].type = map[gc->rockFordY][gc->rockFordX - 1].type;
		map[gc->rockFordY][gc->rockFordX - 1].type = SPACE;
	}
	else if (gc->currentDirection == RIGHT) {
		map[gc->rockFordY][gc->rockFordX + 2].type = map[gc->rockFordY][gc->rockFordX + 1].type;
		map[gc->rockFordY][gc->rockFordX + 1].type = SPACE;
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
	if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (map[y + 1][x].type == SPACE || map[y + 1][x].type == TRANSITIONAL_SPACE || map[y + 1][x].type == BUTTERFLY || map[y + 1][x].type == FIREFLY || (map[y + 1][x].type == MAGIC_WALL)) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = FALL;
	}

	if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (x > 0 && map[y][x - 1].type == SPACE && map[y + 1][x - 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_LEFT;
	}

	else if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (x < MAP_WIDTH - 1 && map[y][x + 1].type == SPACE && map[y + 1][x + 1].type == SPACE) && (map[y + 1][x].type == ROCK || map[y + 1][x].type == DIAMOND || map[y + 1][x].type == WALL) && map[y][x].falling == STATIONARY) {
		map[y][x].falling = ROLL_RIGHT;
	}

	else if ((map[y][x].type == ROCK || map[y][x].type == DIAMOND) && (map[y + 1][x].type != SPACE && map[y + 1][x].type != TRANSITIONAL_SPACE && map[y + 1][x].type != TRANSITIONAL_ROCKFORD && map[y + 1][x].type != ROCKFORD && map[y + 1][x].type != BUTTERFLY && map[y + 1][x].type != FIREFLY && (map[y + 1][x].type != MAGIC_WALL)) && map[y][x].falling == FALL) {
		map[y][x].falling = STATIONARY;
	}

}

void MapUtils::updateFallingBouldersAndDiamonds(int x, int y, GameContext *gc)
{
	if (map[y][x].mark == 0 && (map[y][x].type == ROCK || map[y][x].type == DIAMOND) && map[y][x].falling == FALL) {

		if (map[y + 1][x].type == ROCKFORD || map[y + 1][x].type == BUTTERFLY || map[y + 1][x].type == FIREFLY) {
			printf("HIT ROCKFORD AT %d,%d\n", y + 1, x);
			map::Explosion *e = new map::Explosion;
			*e = { (uint16_t)x, (uint16_t)(y + 1), map[y + 1][x].type, map[y + 1][x].type == ROCKFORD ? (uint8_t) 128 : (uint8_t) 32 };
			explosions.insert(e);
		}

		if (map[y][x].type == ROCK && map[y + 1][x].type == MAGIC_WALL) {
			// boulder disappears
			map[y][x].type = SPACE;
			map[y][x].falling = STATIONARY;
			// check space under
			if (map[y + 2][x].type == SPACE) {
				map[y + 2][x].type = DIAMOND;
				map[y + 2][x].falling = STATIONARY;
				map[y + 2][x].mark = 1;
			}
			return;
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
	if (singleton == NULL) {
		singleton = new MapUtils();
	}
	return singleton;
}

void MapUtils::releaseInstance()
{
	delete singleton;
}

MapUtils::~MapUtils()
{
	printf("MapUtils destruction\n");
	std::set<map::Explosion *>::iterator it;
	for (it = explosions.begin(); it != explosions.end(); it++) {
		delete(*it);
	}
	UnloadTexture(tiles);
	UnloadRenderTexture(mapCache);
}


void MapUtils::cutTilesSheet(GameContext *gc)
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
	firefly = { 16 * 9, 16 * 8, 16, 16, 1, &tiles };
	butterfly = { 16 * 10, 16 * 8, 16, 16, 1, &tiles };
	amoeba = { 16 * 7, 16 * 8, 16, 16, 1, &tiles };
	magicWall = { 16 * 6, 16 * 8, 16, 16, 0, &tiles };

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
	matchSprite[FIREFLY] = &firefly;
	matchSprite[BUTTERFLY] = &butterfly;
	matchSprite[AMOEBA] = &amoeba;
	matchSprite[MAGIC_WALL] = &magicWall;

	gc->countX = 0;
	gc->countY = 0;
	gc->shiftX = 0;
	gc->shiftY = 0;
	gc->rockFordX = 3;
	gc->rockFordY = 2;
	gc->rockfordShift = 0;
	gc->previousRockFordX = gc->rockFordX;
	gc->previousRockFordY = gc->rockFordY;
	gc->visibleX = 3;
	gc->visibleY = 2;

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

	firefly0 = { 16 * 9, 16 * 8, 16, 16, 0, &tiles };
	firefly1 = { 16 * 9, 16 * 9, 16, 16, 0, &tiles };
	firefly2 = { 16 * 9, 16 * 10, 16, 16, 0, &tiles };
	firefly3 = { 16 * 9, 16 * 11, 16, 16, 0, &tiles };

	butterfly0 = { 16 * 10, 16 * 8, 16, 16, 0, &tiles };
	butterfly1 = { 16 * 10, 16 * 9, 16, 16, 0, &tiles };
	butterfly2 = { 16 * 10, 16 * 10, 16, 16, 0, &tiles };
	butterfly3 = { 16 * 10, 16 * 11, 16, 16, 0, &tiles };

	amoeba0 = { 16 * 7, 16 * 8, 16, 16, 0, &tiles };
	amoeba1 = { 16 * 7, 16 * 9, 16, 16, 0, &tiles };
	amoeba2 = { 16 * 7, 16 * 10, 16, 16, 0, &tiles };
	amoeba3 = { 16 * 7, 16 * 11, 16, 16, 0, &tiles };

	magicWall0 = { 16 * 6, 16 * 8, 16, 16, 0, &tiles };
	magicWall1 = { 16 * 6, 16 * 9, 16, 16, 0, &tiles };
	magicWall2 = { 16 * 6, 16 * 10, 16, 16, 0, &tiles };
	magicWall3 = { 16 * 6, 16 * 11, 16, 16, 0, &tiles };


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

	// firefly
	matchAnimatedSprite[FIREFLY].anim[0][0] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][1] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][2] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][3] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][4] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][5] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][6] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][7] = &firefly0;
	matchAnimatedSprite[FIREFLY].anim[0][8] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][9] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][10] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][11] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][12] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][13] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][14] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][15] = &firefly1;
	matchAnimatedSprite[FIREFLY].anim[0][16] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][17] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][18] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][19] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][20] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][21] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][22] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][23] = &firefly2;
	matchAnimatedSprite[FIREFLY].anim[0][24] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][25] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][26] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][27] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][28] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][29] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][30] = &firefly3;
	matchAnimatedSprite[FIREFLY].anim[0][31] = &firefly3;

	// butterfly
	matchAnimatedSprite[BUTTERFLY].anim[0][0] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][1] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][2] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][3] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][4] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][5] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][6] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][7] = &butterfly0;
	matchAnimatedSprite[BUTTERFLY].anim[0][8] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][9] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][10] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][11] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][12] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][13] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][14] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][15] = &butterfly1;
	matchAnimatedSprite[BUTTERFLY].anim[0][16] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][17] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][18] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][19] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][20] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][21] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][22] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][23] = &butterfly2;
	matchAnimatedSprite[BUTTERFLY].anim[0][24] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][25] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][26] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][27] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][28] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][29] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][30] = &butterfly3;
	matchAnimatedSprite[BUTTERFLY].anim[0][31] = &butterfly3;

	// amoeba
	matchAnimatedSprite[AMOEBA].anim[0][0] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][1] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][2] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][3] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][4] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][5] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][6] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][7] = &amoeba0;
	matchAnimatedSprite[AMOEBA].anim[0][8] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][9] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][10] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][11] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][12] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][13] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][14] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][15] = &amoeba1;
	matchAnimatedSprite[AMOEBA].anim[0][16] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][17] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][18] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][19] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][20] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][21] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][22] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][23] = &amoeba2;
	matchAnimatedSprite[AMOEBA].anim[0][24] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][25] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][26] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][27] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][28] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][29] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][30] = &amoeba3;
	matchAnimatedSprite[AMOEBA].anim[0][31] = &amoeba3;

	// magic wall
	matchAnimatedSprite[MAGIC_WALL].anim[0][0] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][1] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][2] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][3] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][4] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][5] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][6] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][7] = &magicWall0;
	matchAnimatedSprite[MAGIC_WALL].anim[0][8] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][9] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][10] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][11] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][12] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][13] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][14] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][15] = &magicWall1;
	matchAnimatedSprite[MAGIC_WALL].anim[0][16] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][17] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][18] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][19] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][20] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][21] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][22] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][23] = &magicWall2;
	matchAnimatedSprite[MAGIC_WALL].anim[0][24] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][25] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][26] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][27] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][28] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][29] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][30] = &magicWall3;
	matchAnimatedSprite[MAGIC_WALL].anim[0][31] = &magicWall3;

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

	matchAnimatedSprite[FIREFLY].currentAnim = 0;
	matchAnimatedSprite[FIREFLY].animCount = 32;

	matchAnimatedSprite[BUTTERFLY].currentAnim = 0;
	matchAnimatedSprite[BUTTERFLY].animCount = 32;

	matchAnimatedSprite[AMOEBA].currentAnim = 0;
	matchAnimatedSprite[AMOEBA].animCount = 32;

	matchAnimatedSprite[MAGIC_WALL].currentAnim = 0;
	matchAnimatedSprite[MAGIC_WALL].animCount = 32;
}


