/****************************************************************\
Disc based animation system implementation to process 
omnidirectional poses with ease.

This is same as 2D blending.
It consists of 2D grapth with an animation on each end axis. 

          y-axis
            ^ [UP]
            |
            |
            |
            |
<---------[IDLE]---------> x-axis
[LEFT]      |       [RIGHT]
            |
            |
            |
            v [DOWN]

Here UP, DOWN, LEFT, RIGHT and IDLE are animation. This system
takes in x and y on this graph and provides a blended pose from
each animation.

It also uses a superimposed disc. which is just another
disk(same as above graph). And the pose from this new disc is blended
to previous disc.

Usage example to help understad:
  Have one disc to find walking animation pose
  Have another one disc to find running animation pose
  Blend them both as per need

 Authors:
  - Kirandeep Singh (@Kirandeep-Singh-Khehra)

\****************************************************************/
#include <raylib.h>
#include <stdbool.h>

#include "../common/utils.h"
#include "skeleton.h"
#include "pose.h"


typedef struct AnimModelDisc {
  /* Bone count of model/pose */
  int boneCount;

  int frame;

  //  POSES  //
  ModelAnimation idle;

  ModelAnimation up;
  ModelAnimation down;
  ModelAnimation left;
  ModelAnimation right;

  /* Inverts left right animation when y is -ve */
  bool invert_LR_on_D;

  /* Stores previous inputs */
  float prev_ud;
  float prev_lr;

  /* Stores lag factor := how much the animation will lag behind (It will catch up automatically).
  Useful for smooth transitions whenever next pose is not seamless. */
  float lagFactor;

  /* Another disc to use and blend */
  struct AnimModelDisc *superimposedDisc; // eg: This disc contain running animation data when base disc contains walking.
} AnimModelDisc;

Pose AnimModelDiscGetPose(AnimModelDisc *disc, /* UP-DOWN input */ float ud, /* LEFT-RIGHT input */ float lr, /* How must to superimpose the superimposeDisc */float superimposeFactor) {
  // Sync all frames (Intentionally done bcs mixamo's walk back anim have one pose extra than others)
  int frame = rmod(disc->frame ++, disc->up.frameCount);

  // Get poses
  Pose idle = GetAnimPose(disc->idle, frame);
  Pose up = GetAnimPose(disc->up, frame);
  Pose down = GetAnimPose(disc->down, frame);
  Pose left = GetAnimPose(disc->left, ((disc->invert_LR_on_D && ud<0)?-1:1) * frame);
  Pose right = GetAnimPose(disc->right, ((disc->invert_LR_on_D && ud < 0)?-1:1) * frame);
  //                                         ^~~~ Plays animation in reverse if invertion is needed(required for mixamo anims)
  if (disc->invert_LR_on_D && ud < 0) {
    // Swap LR poses (required for mixamo's anims)
    Pose temp = NULL;
    temp = left;
    left = right;
    right = temp;
  }

  Pose poseUD = (ud >= 0)? up: down;
  Pose poseLR = (lr >= 0)? right: left;

  // If y switches to -ve to +ve or vice-versa
  if (sign(disc->prev_ud) != sign(ud)) {
    disc->lagFactor = 0.02f;
  }

  // Update lag factor to catch up automatically
  if (disc->lagFactor < 1.0f) {
    disc->lagFactor += 0.01f;
  } else {
    disc->lagFactor = 1.0f;
  }

  disc->prev_ud = ud;
  disc->prev_lr = lr;

  // Calculates weights to LERP left-right motion with front-back
  float weightY = Clamp(fabs(lr), 0.0f, 1.0f) /
                (Clamp(fabs(ud), 0.0001f, 1.0f) +
                 Clamp(fabs(lr), 0.0001f, 1.0f));

  // lerp the left-right motion with front-back
  Pose baseLerpPose defer(UnloadPosePtr) = PoseLerp(poseUD, poseLR, disc->boneCount, weightY);

  // Get pose from superimposed disc
  Pose superimposedPose defer(UnloadPosePtr) = NULL;
  if (disc->superimposedDisc && superimposeFactor) {
    superimposedPose = AnimModelDiscGetPose(disc->superimposedDisc, ud, lr, superimposeFactor);
  }

  Pose finalMotionPose defer(UnloadPosePtr) = NULL;
  if (superimposedPose) {
    finalMotionPose = PoseLerp(baseLerpPose, superimposedPose, disc->boneCount, superimposeFactor);
  } else {
    finalMotionPose = CopyPose(baseLerpPose, disc->boneCount);
  }

  float idleToMotionBlend = sqrt(ud*ud + lr*lr);

  // Finally lerp the motion pose calculated above to idle pose
  Pose finalPose = PoseLerp(idle, finalMotionPose, disc->boneCount, idleToMotionBlend);
  return finalPose;
}

