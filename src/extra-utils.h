#ifndef __EXTRA_UTILS__
#define __EXTRA_UTILS__

#include "pose.h"

void ModelAnimationToLocalPose(ModelAnimation *anims, int animCount) {
  Pose tempPose;
  for (int animId = 0; animId < animCount; animId ++) {
    for (int frameId = 0; frameId < anims[animId].frameCount; frameId++) {
      tempPose = anims[animId].framePoses[frameId];
      anims[animId].framePoses[frameId] = PoseToLocalTransformPose(tempPose, anims[animId].bones, anims[animId].boneCount);
      UnloadPose(tempPose);
    }
  }
}

#endif
