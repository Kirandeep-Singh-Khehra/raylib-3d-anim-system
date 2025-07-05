#ifndef _BOILERPLATE_HEAD_
#define _BOILERPLATE_HEAD_

#include <raylib.h>
#include <raymath.h>

#include <stdio.h>
#include <assert.h>

#define RLIGHTS_IMPLEMENTATION
#include "external/rlights.h"

#define min(x, y) ((x < y) ? x : y)
#define max(x, y) ((x > y) ? x : y)
#define abs_(x) ((x < 0) ? -x : x)
#define sign(x) ((x < 0) ? -1 : 1)
#define clamp(x, a, b) ((x < a)? a : (x > b)? b : x)
#define new_(x) malloc(sizeof(x));
#define new_n(n, x) malloc(n * sizeof(x));

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  450
#define WINDOW_TITLE  "RayLib 3D Animation System"

Shader skinningShader;
Camera camera;

void OnStart();
void OnUpdate();
void OnDraw();
void OnDrawHUD();
void OnEnd();

#endif
