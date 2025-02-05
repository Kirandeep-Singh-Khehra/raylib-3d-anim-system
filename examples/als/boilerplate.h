#ifndef _BOILERPLATE_
#define _BOILERPLATE_

#include "raylib.h"
#include <raymath.h>

//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#include <stdio.h>
#include <assert.h>

#define RLIGHTS_IMPLEMENTATION
#include "../../src/external/rlights.h"

#include "kcamera.c"
#include "kutils.c"

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  450
#define WINDOW_TITLE  "RayLib 3D Animation System"

Shader skinningShader;
KCamera camera = {0};

void OnStart();
void OnUpdate();
void OnDraw();
void OnDrawHUD();
void OnEnd();

void UpdateDrawFrame();

int main() {
  int screenWidth = SCREEN_WIDTH;
  int screenHeight = SCREEN_HEIGHT;

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, WINDOW_TITLE);

  skinningShader = LoadShader("resources/shaders/skinning-lighting.vs.glsl",
                              "resources/shaders/skinning-lighting.fs.glsl");

  // Get some required shader locations
  skinningShader.locs[SHADER_LOC_VECTOR_VIEW] =
      GetShaderLocation(skinningShader, "viewPos");

  // Ambient light level (some basic lighting)
  int ambientLoc = GetShaderLocation(skinningShader, "ambient");
  SetShaderValue(skinningShader, ambientLoc, (float[4]){0.4f, 0.4f, 0.4f, 1.0f},
                 SHADER_UNIFORM_VEC4);

  Light lights[MAX_LIGHTS] = {0};
  lights[0] = CreateLight(LIGHT_POINT, (Vector3){-2, 1, -2}, Vector3Zero(),
                          WHITE, skinningShader);

  KCameraInit(&camera);

  OnStart();

#ifdef DISABLE_CURSOR
  DisableCursor();
#endif

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif

  UnloadShader(skinningShader);
  
  OnEnd();

  CloseWindow();

  return 0;
} /* main() */

void UpdateDrawFrame() {
    KUpdateCamera(&camera);

    OnUpdate();

    BeginDrawing();
    {
      ClearBackground(GRAY);

      BeginMode3D(camera.baseCamera);
      {
        OnDraw();
      }
      EndMode3D();

      OnDrawHUD();
    }
    EndDrawing();
  }

#endif
