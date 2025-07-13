#ifndef _BOILERPLATE_HEAD_
#define _BOILERPLATE_HEAD_

#include <raylib.h>
#include <raymath.h>

#include <stdio.h>
#include <assert.h>

#define RLIGHTS_IMPLEMENTATION
#include "external/rlights.h"

#include "utils.h"

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
