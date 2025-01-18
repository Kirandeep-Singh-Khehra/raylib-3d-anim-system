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

#include "../common/boilerplate_main.h"

#include "../../src/skeleton.c"

// Use any one of below
#define THREE_LAYER_IMPL
// #define TWO_LAYER_IMPL

enum Anim {
  IDLE,
  RUN,
  RUN_BACK,
  RUN_LEFT,
  RUN_RIGHT,
  T_POSE,
  WALK,
  WALK_BACK,
  WALK_LEFT,
  WALK_RIGHT
};

#define MODEL_FILE_NAME "resources/models/bot.glb"
#define ANIMATION_FILE_NAME "resources/models/bot.glb"

Model model;
Skeleton skeleton;

int animsCount = 0;
int animFrameCounter = 0;
int idleAnimFrameCounter = 0;
ModelAnimation *anims;

Vector2 inputDirection = {0.0f};
Vector2 velocity = {0.0f};

enum Anim indexX = IDLE;
enum Anim indexY = IDLE;

void OnStart() {
  model = LoadModel(MODEL_FILE_NAME);
  for (int i = 1; i < model.materialCount; i++) {
    model.materials[i].shader = skinningShader;
  }

  skeleton = LoadSkeletonFromModel(model);
  skeleton.pose = CopyPose(model.bindPose, model.boneCount);

  anims = LoadModelAnimations(ANIMATION_FILE_NAME, &animsCount);

  model.transform = MatrixScale(0.01f, 0.01f, 0.01f);

  camera.position.y = 3.0f;
  camera.position.z = -3.0f;
}

void OnUpdate() {
  /********** PROCESS INPUT **********/
  inputDirection =
      Vector2Normalize((Vector2){IsKeyDown(KEY_T) - IsKeyDown(KEY_G),
                                 IsKeyDown(KEY_F) - IsKeyDown(KEY_H)});

  velocity.x += 0.0125f * (inputDirection.x - velocity.x);
  velocity.y += 0.0125f * (inputDirection.y - velocity.y);

  if (Vector2Length(velocity) == 0 && Vector2Length(inputDirection) == 0) {
    if (Vector2Length(velocity) < 0.0001f) {
      velocity.x -= 0.5f * sign(inputDirection.x - velocity.x);
      velocity.y -= 0.5f * sign(inputDirection.y - velocity.y);
    } else if (Vector2Length(velocity) < 0.0125f) {
      velocity = (Vector2){0.0f, 0.0f};
    }
  }

  indexX = 0;
  if (velocity.x) {
    indexX = (velocity.x < 0) ? RUN_BACK : RUN;
  }

  indexY = 0;
  if (velocity.y) {
    indexY = (velocity.y < 0) ? RUN_RIGHT : RUN_LEFT;
  }

  float weightIdle = (1.0f - Vector2Length(velocity));
  float weightX = clamp(fabs(velocity.x), 0.0f, 1.0f) /
                  (weightIdle + clamp(fabs(velocity.x), 0.0f, 1.0f) +
                   clamp(fabs(velocity.y), 0.0f, 1.0f));
  float weightY = clamp(fabs(velocity.y), 0.0f, 1.0f) /
                  (weightIdle + clamp(fabs(velocity.x), 0.0f, 1.0f) +
                   clamp(fabs(velocity.y), 0.0f, 1.0f));

  /********** UPDATE ANIMATION **********/

  animFrameCounter++;
  animFrameCounter %= min(anims[indexX].frameCount, anims[indexY].frameCount);
  idleAnimFrameCounter++;

#ifdef THREE_LAYER_IMPL
  UpdateSkeletonModelAnimation(skeleton, anims[IDLE], idleAnimFrameCounter);
  UpdateSkeletonModelAnimationPoseOverrideLayer(
      skeleton, anims[indexX], animFrameCounter, weightX, USE_LOCAL_POSE, NULL);
  UpdateSkeletonModelAnimationPoseOverrideLayer(
      skeleton, anims[indexY], animFrameCounter, weightY, USE_LOCAL_POSE, NULL);
#endif
#ifdef TWO_LAYER_IMPL
  UpdateSkeletonModelAnimationLerp(skeleton, anims[indexX], animFrameCounter,
                                   anims[indexY], animFrameCounter, weightY, USE_LOCAL_POSE);
  UpdateSkeletonModelAnimationPoseOverrideLayer(
      skeleton, anims[IDLE], idleAnimFrameCounter,
      clamp(1.0f - Vector2Length(velocity), 0.0f, 1.0f), USE_LOCAL_POSE, NULL);
#endif

  UpdateModelMeshFromPose(model, skeleton.pose);
}

void OnDraw() {
  for (int i = 0; i < model.meshCount; i++) {
    DrawMesh(model.meshes[i], model.materials[i + 1], model.transform);
  }
  DrawCube((Vector3){velocity.y, 0.0, velocity.x}, 0.1f, 0.1f, 0.1f, RED);
  DrawCube((Vector3){inputDirection.y, 0.0, inputDirection.x}, 0.1f, 0.1f, 0.1f,
           BLUE);
  DrawGrid(20, 1);
}

void OnDrawHUD() {
  DrawText("Use T/F/G/H to move character", 10.0f, 10.0f, 20.0f, GREEN);
}

void OnEnd() {
  UnloadModelAnimations(anims, animsCount);
  UnloadModel(model);
  UnloadSkeleton(skeleton);
}

