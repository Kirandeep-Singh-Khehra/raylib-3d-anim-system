/******************************************************************\
 Simple player implementation to show the capability of animation system

 This example provides usage of `Pose` system to work 
  with bone data of 3D models in raylib. It have a simple model 
   picked from Mixamo(https://www.mixamo.com/) along with some basic
   locomotion animations(listed in `enum ANIM`).

 Authors:
  - Kirandeep Singh (@Kirandeep-Singh-Khehra)

 This system is built as drop in for raylib (https://github.com/raysan5/raylib/)
\******************************************************************/
#include "skeleton.h"
#include "boilerplate_main.h"
#include "extra-utils.h"
#include "player_anim.h"

#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define PLAYER_MODEL_FILE_NAME "resources/models/bot.glb"
#define PLAYER_ANIMATION_FILE_NAME "resources/models/bot.glb"

/* Animations in bot.glb */
enum MotionAnim {
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

/* States of player */
typedef enum PlayerState {
  STATE_WALKING,
  STATE_IN_JUMP,

  PLAYER_STATES_COUNT
} PlayerState;

/* Aiming animation states */
typedef enum PlayerArmingState {
  PLAYER_DISARMED,
  PLAYER_ARMING,
  PLAYER_ARMED,
  PLAYER_DISARMING
} PlayerArmingState;

typedef struct Player {
  Model model;
  Pose pose;

  Vector3 position;
  Vector3 velocity;

  /* Direction of animation calculted using input keys */
  Vector2 animationDirection;

  /* Bone masks for split body animation */
  BoneMask lowerBodyMask;
  BoneMask upperBodyMask;

  /* Walking + Running animations */
  ModelAnimation *motionAnims;
  int motionAnimCount;
  int motionAnimFrame;

  /* Jump or falling animation */
  ModelAnimation *fallingAnims;
  int fallingAnimCount;
  int fallingAnimFrame;

  /* Animation to draw rifle, run in reverse to put weapon back in */
  ModelAnimation *drawRifleAnims;
  int drawRifleAnimCount;
  int drawRifleAnimFrame;

  /* Poses for aiming */
  ModelAnimation *aimAnims;
  int aimAnimCount;

  Vector2 aimDir; /* Aiming direction on xz axis (or stores left-right and up down blend)*/
  int armingFrame; /* State varable to store current frame number or arming/drawing animation */

  /* Poses to control aiming direction */
  Pose aimIdle,
       additiveAimLeft,
       additiveAimUp;

  /* Player state about weapon and arming */
  PlayerArmingState armingState;

  /* Player's motion state */
  PlayerState state;

  /* To easily get omni-directional motion animation pose(see `player_anim.h`) */
  AnimModelDisc walkDisc;

  /* Stores blend between walking and running animations */
  float walkToRunBlend;
} Player;

Player CreatePlayer() {
  Player player = {0};

  player.model = LoadModel(PLAYER_MODEL_FILE_NAME);
  player.pose = CopyPose(player.model.bindPose, player.model.boneCount);

  for (int i = 0; i < player.model.materialCount; i++) {
    player.model.materials[i].shader = skinningShader;
  }

  // Convert all animation frame poses to local pose
  ModelAnimationToLocalPose(player.motionAnims, player.motionAnimCount);

  player.lowerBodyMask = BoneMaskZeros(player.model.boneCount);
  MaskChildBonesByParentRegex(player.lowerBodyMask, player.model.bones, "Leg", 1.0f, player.model.boneCount);
  MaskBonesByRegex(player.lowerBodyMask, player.model.bones, "Leg", 1.0f, player.model.boneCount);
  player.lowerBodyMask[0] = 1.0f;

  player.upperBodyMask = CopyBoneMask(player.lowerBodyMask, player.model.boneCount);
  BoneMaskInvert(player.upperBodyMask, player.model.boneCount);

  player.motionAnims = LoadModelAnimations("resources/models/bot.glb", &player.motionAnimCount);
  ModelAnimationToLocalPose(player.motionAnims, player.motionAnimCount);

  player.fallingAnims = LoadModelAnimations("resources/models/bot.falling.idle.glb", &player.fallingAnimCount);
  ModelAnimationToLocalPose(player.fallingAnims, player.fallingAnimCount);

  player.drawRifleAnims = LoadModelAnimations("resources/models/bot.aim.draw.rifle.glb", &player.drawRifleAnimCount);
  ModelAnimationToLocalPose(player.drawRifleAnims, player.drawRifleAnimCount);

  player.aimAnims = LoadModelAnimations("resources/models/bot.aim.glb", &player.aimAnimCount);
  ModelAnimationToLocalPose(player.aimAnims, player.aimAnimCount);

  // Limit animation. Animation ends at holding weapon and we don't need it to end on hold position and then snap to aim.
  player.drawRifleAnims[0].frameCount = 60;

  player.aimIdle = player.aimAnims[0].framePoses[0];
  player.additiveAimLeft = PoseGenerateAdditivePose(player.aimAnims[1].framePoses[0], player.aimIdle, player.model.boneCount);
  player.additiveAimUp   = PoseGenerateAdditivePose(player.aimAnims[2].framePoses[0], player.aimIdle, player.model.boneCount);

  // It might be little confusing see `player_anim.h` for implementation.
  AnimModelDisc *playerRunDisc = new_(AnimModelDisc);
  *playerRunDisc = (AnimModelDisc) {
    .boneCount = player.model.boneCount,
    .idle = player.motionAnims[IDLE],
    .up = player.motionAnims[RUN],
    .down = player.motionAnims[RUN_BACK],
    .left = player.motionAnims[RUN_LEFT],
    .right = player.motionAnims[RUN_RIGHT],
    .invert_LR_on_D = true,
    .superimposedDisc = NULL,
    .frame = 0,
  };

  player.walkDisc = (AnimModelDisc) {
    .boneCount = player.model.boneCount,
    .idle = player.motionAnims[IDLE],
    .up = player.motionAnims[WALK],
    .down = player.motionAnims[WALK_BACK],
    .left = player.motionAnims[WALK_LEFT],
    .right = player.motionAnims[WALK_RIGHT],
    .invert_LR_on_D = true,
    .superimposedDisc = playerRunDisc,
    .frame = 0,
  };

  return player;
}

void UpdatePlayer(Player *player, Camera *camera) {
  /* Input control coordinates to player velocity scale */
  float animToVelScale = 0.03f;
  float gravity = 0.01f;

  player->animationDirection = Vector2MoveTowards(player->animationDirection
            , Vector2Normalize((Vector2){
                  /* If dont want to change direction while jumping */
                  // .x = (player->state == STATE_WALKING) ? (IsKeyDown(KEY_A) - IsKeyDown(KEY_D)) : 0,
                  // .y = (player->state == STATE_WALKING) ? (IsKeyDown(KEY_W) - IsKeyDown(KEY_S)) : 0,

                  /* If you want to change direction while jumping */
                  .x = IsKeyDown(KEY_A) - IsKeyDown(KEY_D),
                  .y = IsKeyDown(KEY_W) - IsKeyDown(KEY_S),
              })
            , 0.03f);

  bool isRunning = IsKeyDown(KEY_L);
  player->walkToRunBlend = (player->walkToRunBlend + ((isRunning)? 0.1f : -0.1f));
  player->walkToRunBlend = Clamp(player->walkToRunBlend, 0.0f, 1.0f);

  animToVelScale = animToVelScale + player->walkToRunBlend * 0.04f; // Increase scale bcs runnig is faster than walking

  // ANIMATION PROCESSING //
  Pose playerNewPose defer(UnloadPosePtr); // Temporary pose to processed and updated at each step and applied in end.

  // WALKING+RUNNIG OR JUMPING := state machine //
  if (player->state == STATE_WALKING) {
    // Get walking running pose from disc
    playerNewPose = AnimModelDiscGetPose(&player->walkDisc, player->animationDirection.y, -player->animationDirection.x, player->walkToRunBlend);

    // Process Trasitions
    if (IsKeyDown(KEY_SPACE)) { // to jump
      player->velocity.y = 0.2f;
      player->walkDisc.lagFactor = 0.2f;
      player->state = STATE_IN_JUMP;
    }
  } else if (player->state == STATE_IN_JUMP) {
    playerNewPose = CopyPose(GetAnimPose(player->fallingAnims[0], 0), player->model.boneCount);
    
    // Process transitions
    if (player->velocity.y < 0 && player->position.y < 1.0f) { // to walking 
      // Trigger early to make player lerp to walking pose in air

      player->walkDisc.lagFactor = 0.1f;
      player->state = STATE_WALKING;
    }
  }

  // Arming := new layer of another state machine on top of walking //
  if (IsKeyPressed(KEY_P)) {
    player->walkDisc.lagFactor = 0.02f;
    switch (player->armingState) {
      case PLAYER_ARMED:
        player->armingState = PLAYER_DISARMING;
        player->armingFrame = player->drawRifleAnims[0].frameCount - 1;
        break;
      case PLAYER_DISARMED:
        player->armingState = PLAYER_ARMING;
        player->armingFrame = 0;
        break;
    }
  }

  // AIMING := state machine processing //
  if (player->armingState == PLAYER_ARMED) {

    // Get aim direction
    player->aimDir.x += (IsKeyDown(KEY_F) - IsKeyDown(KEY_H)) * 0.02f;
    player->aimDir.y += (IsKeyDown(KEY_T) - IsKeyDown(KEY_G)) * 0.02f;

    player->aimDir.x = Clamp(player->aimDir.x, -0.8f, 0.8f);
    player->aimDir.y = Clamp(player->aimDir.y, -0.8f, 0.8f);

    // Process aim pose //
    // Pick aimIdle(aiming to front) and add additiveAimLeft pose (scaled by player->aimDir.x) on top of it to make player aim left or right as per input
    Pose afterAimLR defer(UnloadPosePtr) = PoseAdditiveBlend(player->aimIdle, player->additiveAimLeft, player->model.boneCount, 1.0f, player->aimDir.x, NULL);
    // Now add aim up down pose to previous pose
    Pose afterAimUD defer(UnloadPosePtr) = PoseAdditiveBlend(afterAimLR, player->additiveAimUp, player->model.boneCount, 1.0f, player->aimDir.y, NULL);
    // New apply previous pose to only upper half of body
    Pose afterLower defer(UnloadPosePtr) = PoseOverrideBlend(afterAimUD, playerNewPose, player->model.boneCount, 1.0, player->lowerBodyMask);

    // apply afterLower
    UnloadPose(playerNewPose);
    playerNewPose = CopyPose(afterLower, player->model.boneCount);
  } else if (player->armingState != PLAYER_DISARMED) { // Either arming or disarming
    // Update frame number
    player->armingFrame += (player->armingState == PLAYER_ARMING)? 1:-1;

    // Apply pose to upper half
    Pose armingPose = PoseOverrideBlend(playerNewPose, GetAnimPose(player->drawRifleAnims[0], player->armingFrame), player->model.boneCount, 1.0f, player->upperBodyMask);

    // Update pose
    UnloadPose(playerNewPose);
    playerNewPose = armingPose;

    // Process transitions //
    if (player->armingFrame == 0 || player->armingFrame == player->drawRifleAnims[0].frameCount - 1) {
      player->walkDisc.lagFactor = 0.2f;
      player->armingState = (player->armingState == PLAYER_ARMING)? PLAYER_ARMED : PLAYER_DISARMED;
    }
  }

  // Update Pose //
  // Convert player's current pose to local pose
  Pose playerLocalPose defer(UnloadPosePtr) = PoseToLocalTransformPose(player->pose, player->model.bones, player->model.boneCount);
  // Lerp current pose to new pose by lag factor.
  Pose laggedPose defer(UnloadPosePtr) = PoseLerp(playerLocalPose, playerNewPose, player->model.boneCount, player->walkDisc.lagFactor);

  // Apply pose
  UnloadPose(player->pose);
  player->pose = PoseToGlobalTransformPose(laggedPose, player->model.bones, player->model.boneCount);
  UpdateModelMeshFromPose(player->model, player->pose);

  // Update Player Position //

  // Basic kinematics and camera following logic
  player->velocity = (Vector3) { player->animationDirection.x, player->velocity.y, player->animationDirection.y };
  player->velocity.x *= animToVelScale;
  player->velocity.z *= animToVelScale;
  player->velocity.y -= gravity;

  player->position = Vector3Add(player->position, player->velocity);
  player->position.y = max(player->position.y, 0);

  camera->position = Vector3Add(camera->position, player->velocity);
  camera->position.y = player->position.y + 1.8f;
  camera->target = player->position;
  camera->target.y += 0.8f;
}

void DrawPlayer(Player player) {
  DrawModel(player.model, player.position, 0.01f, WHITE);
}

