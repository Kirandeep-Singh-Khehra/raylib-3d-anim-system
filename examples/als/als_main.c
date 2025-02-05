/******************************************************************\
 Simple example of using layered animation system for raylib
 
 This example provides basic usage of `Skeleton` system to work 
   with animations in raylib. It have a simple model picked from 
   Mixamo(https://www.mixamo.com/) along with some basic 
   locomotion animations(listed in `enum ANIM`).

 Use the following macros to see the layers in action.
  - #define THREE_LAYER_IMPL : This directive will use three layer
      implementation. Which first binds an animation frame of IDLE
      animation then it adds one override layer(interpolation layer)
      of RUN or RUN_BACK (depending on input). Followed by
      another override layer of RUN_LEFT or RUN_RIGHT(depending on
      input).
  - #define TWO_LAYER_IMPL : This directive will use two layer
      implementation. Which will first bind a blended animation pose
      consisting of forward and sideways animation to skeleton.
      Followed by idle pose.

 Authors:
  - Kirandeep Singh (@Kirandeep-Singh-Khehra)

 This system is built as drop in for raylib (https://github.com/raysan5/raylib/)
\******************************************************************/

#define DISABLE_CURSOR
#include "boilerplate.h"

#include "../../src/skeleton.h"
#include "player.c"
#include "map.c"

// Use any one of below
// #define THREE_LAYER_IMPL
#define TWO_LAYER_IMPL

Player player;
Map map;

float *fullBodyMask;

void OnStart() {
  player = CreatePlayer();
  map = CreateMap();

  camera.baseCamera.position.y = 3.0f;
  camera.baseCamera.position.z = -3.0f;

  camera.baseCamera.target.y = 2.0f;
}

void OnUpdate() {
  /********** PROCESS INPUT **********/
  // routine_bind_camera_to_player_and_update_player_y(&player, &map, &camera);

  UpdatePlayerMotion(&player, map, &camera);

  camera.focusPoint = player.kine.position;
  camera.focusPoint.y = 1.0f;
  /********** UPDATE ANIMATION **********/


}

void OnDraw() {
  DrawMap(map);
  DrawPlayer(player);
  DrawCube(player.kine.velocity, 0.1f, 0.1f, 0.1f, RED);
  DrawGrid(20, 1);
}

void OnDrawHUD() {
  DrawText("Use T/F/G/H to move character", 10.0f, 10.0f, 20.0f, GREEN);
}

void OnEnd() {
  UnloadModelAnimations(player.animEng.anims, player.animEng.animsCount);
  UnloadModel(player.model);
  // UnloadSkeleton(skeleton);

  free(fullBodyMask);
}

