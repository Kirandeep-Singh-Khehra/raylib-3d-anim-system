/******************************************************************\
 Simple example to show simple implementation of forward kinematics

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

#include "skeleton.h"
#include "../common/boilerplate_main.h"

#define MODEL_FILE_NAME "resources/models/bot.glb"

Model model;
Skeleton skeleton;

void OnStart() {
  model = LoadModel(MODEL_FILE_NAME);
  for (int i = 1; i < model.materialCount; i++) {
    model.materials[i].shader = skinningShader;
  }

  skeleton = LoadSkeletonFromModel(model);
  skeleton.pose = CopyPose(model.bindPose, model.boneCount);

  model.transform = MatrixIdentity();
  model.transform = MatrixMultiply(MatrixRotateX(PI / 2), model.transform);
  model.transform =
      MatrixMultiply(MatrixScale(0.01f, 0.01f, 0.01f), model.transform);
  model.transform = MatrixMultiply(
      MatrixTranslate(camera.target.x, camera.target.y, camera.target.z),
      model.transform);
}

void OnUpdate() {
  /********** PROCESS INPUT **********/

  /********** UPDATE ANIMATION **********/

  ///// MODIFY POSE BELOW /////

  // // Uncomment the following line to reset pose every time. But
  // // make sure to replace IsKeyPressed with IsKeyDown in following if-else
  // skeleton.pose = CopyPose(model.bindPose, model.boneCount);

  Pose relPose = InitPose(skeleton.boneCount);
  Pose globalPose = InitPose(skeleton.boneCount);

  relPose = PoseToLocalTransformPose(skeleton.pose, skeleton.bones,
                                     skeleton.boneCount);

  if (IsKeyPressed(KEY_X)) {
    relPose[8].rotation =
        QuaternionMultiply(QuaternionFromMatrix(MatrixRotateX(90.0f * DEG2RAD)),
                           relPose[8].rotation);
  } else if (IsKeyPressed(KEY_Y)) {
    relPose[8].rotation =
        QuaternionMultiply(QuaternionFromMatrix(MatrixRotateY(90.0f * DEG2RAD)),
                           relPose[8].rotation);
  } else if (IsKeyPressed(KEY_Z)) {
    relPose[8].rotation =
        QuaternionMultiply(QuaternionFromMatrix(MatrixRotateZ(90.0f * DEG2RAD)),
                           relPose[8].rotation);
  }

  globalPose =
      PoseToGlobalTransformPose(relPose, skeleton.bones, skeleton.boneCount);

  UnloadPose(skeleton.pose);
  skeleton.pose = globalPose;
  globalPose = NULL;
  UnloadPose(relPose);

  ///// MODIFY POSE ABOVE /////

  UpdateModelMeshFromPose(model, skeleton.pose);
}

void OnDraw() {
  for (int i = 0; i < model.meshCount; i++) {
    DrawMesh(model.meshes[i], model.materials[i + 1], model.transform);
  }
  DrawGrid(20, 1);
}

void OnDrawHUD() {
  DrawText("Use X/Y/Z to rotate left arm on respective axis", 10.0f, 10.0f,
           20.0f, GREEN);
}

void OnEnd() {
  UnloadModel(model);
  UnloadSkeleton(skeleton);
}

