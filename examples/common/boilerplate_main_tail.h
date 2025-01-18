#ifndef _BOILERPLATE_TAIL_
#define _BOILERPLATE_TAIL_

#include "boilerplate_main_head.h"

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

  camera.position = (Vector3){0.0f, 3.0f, -3.0f};
  camera.target = (Vector3){0.0f, 0.0f, 0.0f};
  camera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  OnStart();

#ifdef DISABLE_CURSOR
  DisableCursor();
#endif
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    CameraMode mode = CAMERA_CUSTOM;
#ifdef CAMERA_MODE
    mode = CAMERA_MODE;
#endif
    UpdateCamera(&camera, mode);

    OnUpdate();

    BeginDrawing();
    {
      ClearBackground(GRAY);

      BeginMode3D(camera);
      {
        OnDraw();
      }
      EndMode3D();

      OnDrawHUD();
    }
    EndDrawing();
  } /* while */

  UnloadShader(skinningShader);
  
  OnEnd();

  CloseWindow();

  return 0;
} /* main() */

#endif

