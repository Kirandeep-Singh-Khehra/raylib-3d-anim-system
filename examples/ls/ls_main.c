/******************************************************************\
 Simple example to show simple demo of animation system

 This example provides basic usage of `Pose` system to work
   with bone data of 3D models in raylib. It have a simple model 
   picked from Mixamo(https://www.mixamo.com/) along with some basic
   locomotion animations(listed in `enum ANIM`).

 By default raylib bone data in global transforms(i.e. relative to origin).
    So, here `PoseToLocalTransformPose()` function converts a pose from 
    global transforms to local transforms(i.e. relative to parent bone).
    And its reversal is done using `PoseToGlobalTransformPose()`.

 Authors:
  - Kirandeep Singh (@Kirandeep-Singh-Khehra)

 This system is built as drop in for raylib (https://github.com/raysan5/raylib/)
\******************************************************************/
#define DISABLE_CURSOR
#include "boilerplate_main.h"
#include "player.h"
#include <raylib.h>

#define MODEL_FILE_NAME "resources/models/bot.glb"

Player player;

void OnStart() {
  player = CreatePlayer();

  camera.position.y = 3.0f;
  camera.position.z = -3.0f;
}

void OnUpdate() {
  UpdatePlayer(&player, &camera);
}

void OnDraw() {
  DrawPlayer(player);
  DrawGrid(20, 1);
}

void OnDrawHUD() {
  DrawText("Use WASD to move", 10, 10, 20, BLACK);
  DrawText("Use TFGH to move aim", 10, 30, 20, BLACK);
  DrawText("P to toggle aim animation", 10, 50, 20, BLACK);
  DrawText("L to run", 10, 70, 20, BLACK);
  DrawText("SPACE to jump", 10, 90, 20, BLACK);
}

void OnEnd() {
}

