#include "../../src/skeleton.h"
#include "anim_eng.c"
#include "kine.c"
#include "map.c"
#include "boilerplate.h"
#include "../../src/extra-utils.c"

#include <raylib.h>
#include <raymath.h>

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

#define PLAYER_MODEL_FILE_NAME "resources/models/bot2.glb"
#define PLAYER_ANIMATION_FILE_NAME "resources/models/bot2.glb"

typedef struct Player {
  Model model;
  Pose pose;
  Shader shader;

  KineObject kine;

  AnimEngine animEng;
} Player;

Player CreatePlayer() {
  Player player = {0};
  player.model = LoadModel(PLAYER_MODEL_FILE_NAME);
  player.pose = CopyPose(player.model.bindPose, player.model.boneCount);

  player.kine = KineIdentity();
  player.kine.force.y = -10.0f;
  player.animEng = CreateAnimEngine(PLAYER_ANIMATION_FILE_NAME);
  player.shader = skinningShader;
  for (int i = 0; i < player.model.materialCount; i++) {
    player.model.materials[i].shader = skinningShader;
  }

  player.model.transform = MatrixScale(0.01f, 0.01f, 0.01f);

  ModelAnimationToLocalPose(player.animEng.anims, player.animEng.animsCount);
  return player;
}

void UpdatePlayerMotion(Player *player, Map map, KCamera *camera) {
  static Vector2 animationDirection;// = { player->kine.velocity.z, player->kine.velocity.x };
  bool isSprinting = IsKeyDown(KEY_P);
  animationDirection = Vector2MoveTowards(animationDirection
              , Vector2Normalize((Vector2){
                    .x = IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
                    .y = IsKeyDown(KEY_A) - IsKeyDown(KEY_D)
                })
              , 0.03f);

  int indexX = (animationDirection.x < 0) ? RUN_BACK : RUN;
  int indexY = (animationDirection.y < 0) ? RUN_RIGHT : RUN_LEFT;

  if (!isSprinting) {
    indexX += 5;
    indexY += 5;
  }

  float weightX = clamp(fabs(animationDirection.x), 0.0f, 1.0f) /
                  (clamp(fabs(animationDirection.x), 0.0001f, 1.0f) +
                   clamp(fabs(animationDirection.y), 0.0001f, 1.0f));
  float weightY = clamp(fabs(animationDirection.y), 0.0f, 1.0f) /
                  (clamp(fabs(animationDirection.x), 0.0001f, 1.0f) +
                   clamp(fabs(animationDirection.y), 0.0001f, 1.0f));

  Pose poseA, poseB;
  float blendFactor;

  float speed = Vector2Length(animationDirection);

  player->animEng.animFrameCounters[IDLE] += 1.0f;

  if (indexX == WALK || indexX == RUN) {
    poseA = GetAnimPose(player->animEng.anims[IDLE], (int)player->animEng.animFrameCounters[IDLE]);

    int frame = (int)player->animEng.animFrameCounters[IDLE] % min(player->animEng.anims[indexX].frameCount, player->animEng.anims[indexY].frameCount);

    Pose moveX = GetAnimPose(player->animEng.anims[indexX], frame);
    Pose moveY = GetAnimPose(player->animEng.anims[indexY], frame);
    poseB = PoseLerp(moveX, moveY, player->model.boneCount, weightY);

    blendFactor = speed;
  } else {
    poseA = GetAnimPose(player->animEng.anims[IDLE], (int)player->animEng.animFrameCounters[IDLE]);
    int frame = (int)player->animEng.animFrameCounters[IDLE] % min(player->animEng.anims[indexX].frameCount, player->animEng.anims[indexY].frameCount);

    int indexY_inv = indexY;
    if (indexX == RUN_BACK)
      indexY_inv = (indexY == RUN_LEFT)? RUN_RIGHT : RUN_LEFT;
    if (indexX == WALK_BACK)
      indexY_inv = (indexY == WALK_LEFT)? WALK_RIGHT : WALK_LEFT;

    Pose moveX = GetAnimPose(player->animEng.anims[indexX    ],  frame);
    Pose moveY = GetAnimPose(player->animEng.anims[indexY_inv], -frame);
    poseB = PoseLerp(moveX, moveY, player->model.boneCount, weightY);

    if (animationDirection.x > -0.5f && animationDirection.x < 0.0f) {
      moveY = GetAnimPose(player->animEng.anims[indexY], frame);
      Pose backWalkPose = poseB;

      poseB = PoseLerp(moveY, backWalkPose, player->model.boneCount, animationDirection.x * -2.0f);

      UnloadPose(backWalkPose);
      UnloadPose(moveY);
    }

    blendFactor = speed;

  }

  Pose moveLocalPose = PoseLerp(poseA, poseB, player->model.boneCount, blendFactor);
  UnloadPose(player->pose);
  player->pose = PoseToGlobalTransformPose(moveLocalPose, player->model.bones, player->model.boneCount);
  UnloadPose(moveLocalPose);
  UnloadPose(poseB);
  // UnloadPose(poseA);

  UpdateModelMeshFromPose(player->model, player->pose);

  if (speed) {
    float angleBetween = Vector2Angle((Vector2){camera->baseCamera.target.x - camera->baseCamera.position.x,
                                   camera->baseCamera.target.z - camera->baseCamera.position.z},
                        (Vector2){0, 1});;
    player->kine.rotation.y = LerpAngle(player->kine.rotation.y, angleBetween, 0.3f);

    Vector2 rotatedAnimationDirection = Vector2Rotate(animationDirection, angleBetween);

    float charSpeedScale = (!isSprinting)? 2.0f: 4.5f;
    player->kine.velocity.x = rotatedAnimationDirection.y * charSpeedScale;
    player->kine.velocity.z = rotatedAnimationDirection.x * charSpeedScale;
  }

  Vector3 playerPositionTemp = player->kine.position;
  // Update player position below otherwise camera moves upards //
  UpdateKineObject(&player->kine);
  player->kine.position.y = max(player->kine.position.y, MapGetAt(map, player->kine.position.x, player->kine.position.z));
  // Update player position above otherwise camera moves upards //
  playerPositionTemp = Vector3Subtract(player->kine.position, playerPositionTemp);

  camera->baseCamera.target = player->kine.position;
  camera->baseCamera.target.y += 1.75f;

  camera->heightBase += playerPositionTemp.y;

}

void DrawPlayer(Player player) {
  Matrix transformMatrix = TransformToMatrix((Transform){
    .translation = player.kine.position,
    .rotation = QuaternionFromEuler(player.kine.rotation.x, player.kine.rotation.y, player.kine.rotation.z),
    .scale = player.kine.scale,
  });

  transformMatrix = MatrixMultiply(player.model.transform, transformMatrix);
  for(int i = 0; i < player.model.meshCount; i++) {
    DrawMesh(player.model.meshes[i], player.model.materials[player.model.meshMaterial[i]], transformMatrix);
  }
}

