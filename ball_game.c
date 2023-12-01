#include "include/raylib.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define max_lifes      5
#define max_enemys     5
#define player_size    20.0f
#define SPEED          40.0f
#define player_SPEED   7.0f
#define max_shoots     1
#define B_SPEED        10.0f

typedef struct Player {
	int life;
	float rotation;
	Vector3 collider;
	Vector2 speed;
	Vector2 position;
} Player;
typedef struct Enemy {
	int radius;
	Vector2 position;
	Vector2 speed;
	bool active;
} Enemy;
typedef struct Shoot {
	int radius;
	Vector2 position;
	Vector2 speed;
	bool active;
} Shoot;

static const int ScreenWidth = 850;
static const int ScreenHeight = 450;

static bool pause = false;
static bool gameover = false;

static float h = 0.0f;
static float sh = 0.0f;
static float gravity = 0.0f;
static int FrameCounter = 0;

static Player player = { 0 };
static Enemy enemy[max_enemys] = { 0 };
static Shoot shoot[max_shoots] = { 0 };

static void InitGame();
static void UpdateGame();
static void DrawGame();
static void Unload();
static void UpdateDrawFrame();

int main(void) {
	InitWindow(ScreenWidth, ScreenHeight, "hello");
	
	InitGame();

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		UpdateDrawFrame();	
	}
	Unload();
	CloseWindow();
	return 0;
}

static void InitGame(void) {

	gravity = 0.35f;
	FrameCounter = 0;
	bool correctRange = false;
	int posx, posy;
	int velx = 0;
	int vely = 0;
	int sx = 0;
	int sy = 0;
	sh = (player_size/2)/tanf(20*DEG2RAD);

	player.collider = (Vector3){player.position.x + sin(player.rotation*DEG2RAD)*(sh/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(sh/2.5f), 12};
	player.speed = (Vector2) {0, 0};
	player.rotation = 0;
	player.position = (Vector2) { GetScreenWidth()/2 -30, GetScreenHeight()/2 -30 };
	player.life = max_lifes;

	for (int i = 0; i < max_enemys; i++) {
        	enemy[i].radius = 40.0f;

        	posx = GetRandomValue(0 + enemy[i].radius, ScreenWidth - enemy[i].radius);
		while (!correctRange) {
          		if (posx > ScreenWidth/2 - 150 && posx < ScreenWidth/2 + 150) posx = GetRandomValue(0 + enemy[i].radius, ScreenWidth - enemy[i].radius);
           		else correctRange = true;
       		}
		correctRange = false;
        	posy = GetRandomValue(0 + enemy[i].radius, ScreenHeight/2);
		while (!correctRange) {
			if (posy > ScreenHeight/2 - 150 && posy < ScreenHeight/2 + 150) posy = GetRandomValue(0+ enemy[i].radius, ScreenHeight - enemy[i].radius);
			else correctRange = true;
		}
        	enemy[i].position = (Vector2){ posx, posy };
		correctRange = false;
		velx = GetRandomValue(-SPEED, SPEED);
		vely = GetRandomValue(-SPEED, SPEED);
		while (!correctRange) {
        		if ((velx == 0) || (vely == 0)) {
           			velx = GetRandomValue(-SPEED, SPEED);
            			vely = GetRandomValue(-SPEED, SPEED);
        		}
			else correctRange = true;
		}

        	enemy[i].speed = (Vector2){ velx, vely };
        	enemy[i].active = false;
	}
	for (int i = 0; i < max_shoots; i++) {
		shoot[i].active = false;
		shoot[i].position = (Vector2) { player.position.x, player.position.y };
		shoot[i].radius = 2.5;
		sx = GetRandomValue(-B_SPEED, B_SPEED);
		sy = GetRandomValue(-B_SPEED, B_SPEED);
		while (!correctRange) {
			if ((sx == 0) || (sy == 0)) {
				sx = GetRandomValue(-B_SPEED, B_SPEED);
				sy = GetRandomValue(-B_SPEED, B_SPEED);
			} else {
				correctRange = true;
			}
		}
		shoot[i].speed = (Vector2) { sx, sy };
	}
}

static void UpdateGame(void) {
	if(!gameover) {
		if(IsKeyPressed(KEY_P)) pause = !pause;
		if(!pause) {

			if(IsKeyDown(KEY_W)) {
			       	player.position.y -= player_SPEED;
			} else if (IsKeyDown(KEY_S)) {
				player.position.y += player_SPEED;
			} else if (IsKeyDown(KEY_D)) {
			       	player.position.x += player_SPEED;
			} else if (IsKeyDown(KEY_A)) {
				player.position.x -= player_SPEED;
			}

	                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_DOWN)) player.rotation -= 5;
            		if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_UP)) player.rotation += 5;

	           	player.speed.x = sin(player.rotation*DEG2RAD)*SPEED;
        		player.speed.y = cos(player.rotation*DEG2RAD)*SPEED;

			if (player.position.x > ScreenWidth + h) player.position.x = -(h);
           		else if (player.position.x < -(h)) player.position.x = ScreenWidth + h;
            		if (player.position.y > (ScreenHeight + h)) player.position.y = -(h);
            		else if (player.position.y < -(h)) player.position.y = ScreenHeight + h;

			player.collider = (Vector3) { player.position.x + sin(player.rotation*DEG2RAD)*(sh/2.5f), player.position.y - cos(player.rotation*DEG2RAD)*(sh/2.5f), 12 };

			for (int i = 0; i < max_enemys; i++) {
				if(IsKeyPressed(KEY_SPACE)) enemy[i].active = true;
				if(enemy[i].active) {
					FrameCounter++;
					enemy[i].position.x += enemy[i].speed.x;
					enemy[i].position.y += enemy[i].speed.y;

					if (enemy[i].position.x + enemy[i].radius >= ScreenWidth || enemy[i].position.x - enemy[i].radius <= 0) enemy[i].speed.x *= -1;
                  			if (enemy[i].position.y - enemy[i].radius <= 0) enemy[i].speed.y *= -1;
                    			if (enemy[i].position.y + enemy[i].radius >= ScreenHeight) {
						enemy[i].speed.y *=  -1;
						enemy[i].position.y = ScreenHeight - enemy[i].radius;
					}
					enemy[i].speed.y += gravity + 0.6f;
					if (CheckCollisionCircles((Vector2){player.collider.x, player.collider.y}, player.collider.z, enemy[i].position, enemy[i].radius)) {
						enemy[i].active = false;
						player.life--;
					}
					for (int p = 0; p < max_shoots; p++) {
						if (shoot[p].active) {
							if (CheckCollisionCircles((Vector2)shoot[p].position, shoot[p].radius, enemy[i].position, enemy[i].radius)) {
								enemy[i].active = false;
								shoot[p].active = false;
								player.life++;
							}
						}
					}
				}
			}
			for (int i = 0; i < max_enemys; i++) {
				if (IsKeyPressed(KEY_Q)) shoot[i].active = true;
				if (IsKeyUp(KEY_Q)) shoot[i].active = false;
				if (shoot[i].active) {
					shoot[i].position.x += shoot[i].speed.x;
					shoot[i].position.y += shoot[i].speed.y;
				}
			}
			if(player.life <= 0) {
				gameover = true;
			}
		 
		} 
	} else {
		if(IsKeyPressed(KEY_ENTER)) {
			InitGame();
			gameover = false;
		}
	}
}

static void DrawGame(void) {
	BeginDrawing();
		ClearBackground(RAYWHITE);
		if(!gameover) {
			Vector2 v1 = { player.position.x + sinf(player.rotation*DEG2RAD)*(sh), player.position.y - cosf(player.rotation*DEG2RAD)*(sh) };
            		Vector2 v2 = { player.position.x - cosf(player.rotation*DEG2RAD)*(player_size/2), player.position.y - sinf(player.rotation*DEG2RAD)*(player_size/2) };
            		Vector2 v3 = { player.position.x + cosf(player.rotation*DEG2RAD)*(player_size/2), player.position.y + sinf(player.rotation*DEG2RAD)*(player_size/2) };
            		DrawTriangle(v1, v2, v3, GREEN);
			DrawFPS(770, 10);
			for (int i = 0; i < player.life; i++) DrawRectangle(20 + 40*i, ScreenHeight - 30, 35, 10, GREEN);
			for (int i = 0; i < max_enemys; i++) DrawCircleV(enemy[i].position, enemy[i].radius, RED);
			for (int i = 0; i < max_shoots; i++) {
				if (shoot[i].active) {
					DrawCircleV(shoot[i].position, shoot[i].radius, GREEN);
				}
			}
			DrawText("Press space to start", 10, 10, 20, BLACK);
			DrawText(TextFormat("TIME: %.02f", (float)FrameCounter/60), 400, 10, 20, BLACK);
		    	if (pause) DrawText("PAUSED", ScreenWidth/2 - MeasureText("PAUSED", 40)/2, ScreenHeight/2 - 40, 40, BLACK);
		} else {
			DrawText("           Game Over\n Press Enter to play again", ScreenWidth/2 - MeasureText("Game Over\n Press Enter to play again", 40)/2, ScreenHeight/2 - 40, 40, BLACK);
		}
	EndDrawing();
}

static void Unload(void) {
}

static void UpdateDrawFrame(void) {
	UpdateGame();
	DrawGame();
}
