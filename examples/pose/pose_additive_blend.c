/******************************************************************\
 Simple example of using layered animation system for raylib
 
 This example provides basic usage of `Pose` system to convert
  global pose tranformations to local pose transformations and get
  better results and more option with animations in raylib. 
  This example specificly shows additive animation blending between
  local transform frames. It have a simple model picked from 
   Mixamo(https://www.mixamo.com/) along with some basic 
   locomotion animations(listed in `enum ANIM`).

 Authors:
  - Kirandeep Singh (@Kirandeep-Singh-Khehra)

 This system is built as drop in for raylib (https://github.com/raysan5/raylib/)
\******************************************************************/

#include "../common/boilerplate_main.h"
#include "../../src/skeleton.c"

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
#define ANIMATION_CROUCH_FILE_NAME "resources/models/crouch.glb"

Model model;
Skeleton skeleton;

int animsCount = 0;
int animFrameCounter = 0;
int idleAnimFrameCounter = 0;
ModelAnimation *anims;

int crouchAnimIndex = 0;
int crouchAnimCount = 0;
ModelAnimation *crouchAnim;

float additiveLayerWeight = 0.0f;

void OnStart() {
  model = LoadModel(MODEL_FILE_NAME);
  for (int i = 1; i < model.materialCount; i++) {
    model.materials[i].shader = skinningShader;
  }

  skeleton = LoadSkeletonFromModel(model);
  skeleton.pose = CopyPose(model.bindPose, model.boneCount);

  anims = LoadModelAnimations(ANIMATION_FILE_NAME, &animsCount);
  crouchAnim = LoadModelAnimations(ANIMATION_CROUCH_FILE_NAME, &crouchAnimCount);

  model.transform = MatrixIdentity();
  model.transform = MatrixMultiply(MatrixRotateX(PI / 2), model.transform);
  model.transform =
      MatrixMultiply(MatrixScale(0.01f, 0.01f, 0.01f), model.transform);
  model.transform = MatrixMultiply(
      MatrixTranslate(camera.target.x, camera.target.y, camera.target.z),
      model.transform);

  camera.position.y = 3.0f;
  camera.position.z = -3.0f;
}

void OnUpdate() {
  /********** PROCESS INPUT **********/

  additiveLayerWeight += 0.125 * (IsKeyDown(KEY_T) - additiveLayerWeight);
  
  /********** UPDATE ANIMATION **********/

  /// UPDATE POSE BELOW ///
  Pose idlePose = PoseToLocalTransformPose(anims[IDLE].framePoses[0], skeleton.bones, skeleton.boneCount);
  Pose crouchPose = PoseToLocalTransformPose(crouchAnim[0].framePoses[0], skeleton.bones, skeleton.boneCount);
  Pose crouchPoseAdditive = PoseGenerateAdditivePose(crouchPose, idlePose, skeleton.boneCount);

  // Note: Weight is doubled
  Pose resultPose = PoseAdditiveBlend(idlePose, crouchPoseAdditive, skeleton.boneCount, 1.0f, 2.0 * clamp(abs_(additiveLayerWeight), 0.0f, 1.0f));

  UnloadPose(idlePose);
  UnloadPose(crouchPoseAdditive);
  UnloadPose(skeleton.pose);

  skeleton.pose = PoseToGlobalTransformPose(resultPose, skeleton.bones, skeleton.boneCount);
  resultPose = NULL;

  if (IsKeyDown(KEY_I)) {
    skeleton.pose = anims[IDLE].framePoses[0];
  } else if (IsKeyDown(KEY_C)) {
    skeleton.pose = crouchAnim[0].framePoses[0];
  }

  /// UPDATE POSE ABOVE ///
  UpdateModelMeshFromPose(model, skeleton.pose);
}

void OnDraw() {
  for (int i = 0; i < model.meshCount; i++) {
    DrawMesh(model.meshes[i], model.materials[i + 1], model.transform);
  }
  DrawPose(skeleton.pose, skeleton.bones, skeleton.boneCount, MatrixMultiply(model.transform, MatrixTranslate(2.0f, 0.0f, 0.0f)), RED);
  DrawCube((Vector3){0.0f, 0.0f, additiveLayerWeight}, 0.1f, 0.1f, 0.1f, RED);
  DrawGrid(20, 1);
}

void OnDrawHUD() {
  DrawText("Use T to move weight to 2.0f of 'crouch' pose over 'idle' pose", 10.0f, 10.0f, 20.0f, GREEN);
  DrawText("Press and hold I to show `idle` pose and C to show 'crouch' pose", 10.0f, 30.0f, 20.0f, GREEN);
}

void OnEnd() {
  UnloadModelAnimations(anims, animsCount);
  UnloadModel(model);
  UnloadSkeleton(skeleton);
}

