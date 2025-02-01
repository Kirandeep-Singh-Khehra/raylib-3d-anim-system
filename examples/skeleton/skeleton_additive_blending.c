// #define DISABLE_CURSOR
#include "../common/boilerplate_main.h"
#include <raygui.h>
#include <raymath.h>

#include "../../src/skeleton.h"

Model model;
ModelAnimation *anims;
ModelAnimation *motionAnims;
int motionAnimFrameCounter = 0;
Skeleton skeleton;

int animCount = 0;
int animIndex = 0;

int motionAnimCount = 0;

float aimLeftWeight = 0.0f;
float aimUpWeight = 0.0f;

float modelRotationAngle = 0.0f;
bool playLowerBodyAnimation = false;

float idleToRunWeight = 0.0f; // Idle(0.0f) <--> Walk(1.0f) <--> Run(2.0f)

BoneMask lowerBodyMask;

void OnStart() {
  model = LoadModel("resources/models/bot.glb");
  skeleton = LoadSkeletonFromModel(model);
  anims = LoadModelAnimations("resources/models/bot.aim.glb", &animCount);
  motionAnims = LoadModelAnimations("resources/models/bot.glb", &motionAnimCount);

  for (int i = 0; i < model.materialCount; i++) {
    model.materials[i].shader = skinningShader;
  }

  model.transform = MatrixIdentity();
  model.transform = MatrixMultiply(model.transform, MatrixRotateY(PI * 0.75f));
  model.transform = MatrixMultiply(model.transform, MatrixScale(0.01f, 0.01f, 0.01f));
  model.transform = MatrixMultiply(model.transform, MatrixTranslate(camera.target.x, camera.target.y, camera.target.z));

  camera.position = (Vector3){0.0f, 2.0f, -2.5f};
  camera.target = (Vector3){1.0f, 0.7f, 0.0f};

  lowerBodyMask = BoneMaskZeros(skeleton.boneCount);
  MaskChildBonesByParentRegex(lowerBodyMask, skeleton.bones, "Leg", 1.0f, skeleton.boneCount);
  MaskBonesByRegex(lowerBodyMask, skeleton.bones, "Leg", 1.0f, skeleton.boneCount);
  lowerBodyMask[0] = 1.0f;
}

void OnUpdate() {
  float idleToWalkWeight = Clamp(idleToRunWeight, 0.0f, 1.0f);
  float walktoRunWeight = Clamp(idleToRunWeight, 1.0f, 2.0f) - 1.0f;

  UpdateSkeletonModelAnimation(skeleton, anims[0], 0);
  UpdateSkeletonModelAnimationPoseAdditiveLayer(skeleton, anims[1], 0, anims[0].framePoses[0], aimLeftWeight, USE_LOCAL_POSE, NULL);
  UpdateSkeletonModelAnimationPoseAdditiveLayer(skeleton, anims[2], 0, anims[0].framePoses[0], aimUpWeight, USE_LOCAL_POSE, NULL);
  if (playLowerBodyAnimation) {
  UpdateSkeletonModelAnimationPoseOverrideLayer(skeleton, motionAnims[6/*WALK*/], motionAnimFrameCounter ++, idleToWalkWeight, USE_LOCAL_POSE, lowerBodyMask);
  UpdateSkeletonModelAnimationPoseOverrideLayer(skeleton, motionAnims[1/*RUN */], motionAnimFrameCounter ++, walktoRunWeight , USE_LOCAL_POSE, lowerBodyMask);
}
  UpdateModelMeshFromPose(model, skeleton.pose);
}

void OnDraw() {
  Matrix transform = model.transform;

  transform = MatrixMultiply(transform, MatrixRotateY(modelRotationAngle));
  for(int i = 0; i < model.meshCount; i++) {
    DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], transform);
  }
}

void OnDrawHUD() {
  GuiPanel((Rectangle){ 32, 24, 300, 400 }, "Additive Animation Demo");

  GuiLabel((Rectangle){ 70, 48, 120, 24 }, "Aim Horizontally");
  GuiSlider((Rectangle){ 70, 75, 224, 16 }, "Right", "Left", &aimLeftWeight, -1.0f, 1.0f);
  
  GuiLabel((Rectangle){ 70, 96, 120, 24 }, "Aim Vertically");
  GuiSlider((Rectangle){ 70, 123, 224, 16 }, "Down", "Up", &aimUpWeight, -1.0f, 1.0f);

  GuiLabel((Rectangle){ 70, 144, 120, 24 }, "Rotation Angle");
  GuiSlider((Rectangle){ 70, 171, 224, 16 }, "-PI", "PI", &modelRotationAngle, -PI, PI);

  GuiCheckBox((Rectangle){ 70, 200, 16, 16 }, "Play Lower Body Animation", &playLowerBodyAnimation);

  GuiLabel((Rectangle){ 70, 224, 200, 24 }, "Animation selector Idle<-->Walk<-->Run");
  GuiSlider((Rectangle){ 70, 251, 224, 16 }, "Idle", "Run", &idleToRunWeight, 0.0f, 2.0f);
}

void OnEnd() {
  UnloadModel(model);
  UnloadSkeleton(skeleton);
  UnloadModelAnimations(anims, animCount);
  UnloadModelAnimations(motionAnims, motionAnimCount);

  UnloadBoneMask(lowerBodyMask);
}

