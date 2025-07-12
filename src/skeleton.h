#ifndef __KIRAN_RAY_SKELETON__
#define __KIRAN_RAY_SKELETON__

#include "pose.h"

#define USE_LOCAL_POSE (1 << 0)

typedef struct Skeleton {
  int boneCount;         // Number of bones
  BoneInfo *bones;       // Bones information (skeleton)
  Pose bindPose;         // Bones base transformation (pose)

  Matrix *boneMatrices;  // Bones animated transformation matrices (not used yet)

  Pose pose;      // Current pose
} Skeleton;

Skeleton LoadSkeletonFromModel(Model model);
void UpdateModelBonesFromPose(Model model, Pose pose);
void UnloadSkeleton(Skeleton skeleton);

void UpdateSkeletonPose(Skeleton skeleton, Pose pose);
void UpdateSkeletonPoseWithMask(Skeleton skeleton, Pose pose, float *boneMask);
void UpdateSkeletonModelAnimation(Skeleton skeleton, ModelAnimation anim, int frame);
void UpdateSkeletonModelAnimationLerp(Skeleton skeleton, ModelAnimation  animA, int frameA, ModelAnimation animB, int frameB, float blendFactor, int flags);
void UpdateSkeletonModelAnimationPoseOverrideLayer(Skeleton skeleton, ModelAnimation anim, int frame, float factor, int flags, float *boneMask);
void UpdateSkeletonModelAnimationPoseAdditiveLayer(Skeleton skeleton, ModelAnimation anim, int frame, Pose referencePose, float factor, int flags, float *boneMask);

Skeleton LoadSkeletonFromModel(Model model) {
  Skeleton skeleton = {0};

  skeleton.boneCount = model.boneCount;

  skeleton.bones = calloc(skeleton.boneCount, sizeof(BoneInfo));
  skeleton.bindPose = calloc(skeleton.boneCount, sizeof(Transform));
  skeleton.boneMatrices = calloc(skeleton.boneCount, sizeof(Matrix));
  skeleton.pose = calloc(skeleton.boneCount, sizeof(Transform));

  // Deep copy
  for (int i = 0; i < skeleton.boneCount; i++) {
    memcpy(skeleton.bones[i].name, model.bones[i].name, 32);
    skeleton.bones[i].parent = model.bones[i].parent;

    skeleton.bindPose[i] = model.bindPose[i];
    skeleton.boneMatrices[i] = model.meshes[0].boneMatrices[i];
  }

  return skeleton;
}

void UpdateSkeletonPose(Skeleton skeleton, Pose pose) {
  memcpy(skeleton.pose, pose, skeleton.boneCount * sizeof(Transform));
}

void UpdateSkeletonPoseWithMask(Skeleton skeleton, Pose pose, float *boneMask) {
  for (int i = 0; i < skeleton.boneCount; i++) {
    if (boneMask[i] != 0.0f) {
      skeleton.pose[i] = pose[i];
    }
  }
}

void UpdateSkeletonModelAnimation(Skeleton skeleton, ModelAnimation anim,
                                  int frame) {
  if ((anim.frameCount > 0) && (anim.bones != NULL) &&
      (anim.framePoses != NULL)) {
    if (frame >= anim.frameCount)
      frame = frame % anim.frameCount;

    UpdateSkeletonPose(skeleton, anim.framePoses[frame]);
  }
}

void UpdateSkeletonModelAnimationLerp(Skeleton skeleton, ModelAnimation animA, int frameA,
                            ModelAnimation animB, int frameB,
                            float blendFactor, int flags) {
  if ((animA.frameCount > 0) && (animA.bones != NULL) &&
      (animA.framePoses != NULL) && (animB.frameCount > 0) &&
      (animB.bones != NULL) && (animB.framePoses != NULL) &&
      (blendFactor >= 0.0f) && (blendFactor <= 1.0f)) {
    frameA = frameA % animA.frameCount;
    frameB = frameB % animB.frameCount;

    Pose pose;
    if (flags & USE_LOCAL_POSE) {
      Pose localAnimAPose = PoseToLocalTransformPose(animA.framePoses[frameA], skeleton.bones, skeleton.boneCount);
      Pose localAnimBPose = PoseToLocalTransformPose(animB.framePoses[frameB], skeleton.bones, skeleton.boneCount);

      Pose lerpPose = PoseLerp(localAnimAPose, localAnimBPose, skeleton.boneCount, blendFactor);
      pose = PoseToGlobalTransformPose(lerpPose, skeleton.bones, skeleton.boneCount);

      UnloadPose(localAnimAPose);
      UnloadPose(localAnimBPose);
      UnloadPose(lerpPose);
    } else {
      pose = PoseLerp(animA.framePoses[frameA], animB.framePoses[frameB], skeleton.boneCount, blendFactor);
    }
    UpdateSkeletonPose(skeleton, pose);
    free(pose);
  }
}

void UpdateSkeletonModelAnimationPoseOverrideLayer(Skeleton skeleton, ModelAnimation anim,
                                     int frame, float factor, int flags,
                                     float *boneMask) {
  if ((anim.frameCount > 0) && (anim.bones != NULL) &&
      (anim.framePoses != NULL) && (factor != 0.0f)) {
    frame = frame % anim.frameCount;

    Pose pose;
    if (flags & USE_LOCAL_POSE) {
      Pose localSkeletonPose = PoseToLocalTransformPose(skeleton.pose, skeleton.bones, skeleton.boneCount);
      Pose localAnimationPose = PoseToLocalTransformPose(anim.framePoses[frame], skeleton.bones, skeleton.boneCount);

      Pose lerpPose = PoseOverrideBlend(localSkeletonPose, localAnimationPose, skeleton.boneCount, factor, boneMask);

      pose = PoseToGlobalTransformPose(lerpPose, skeleton.bones, skeleton.boneCount);

      UnloadPose(lerpPose);
      UnloadPose(localSkeletonPose);
      UnloadPose(localAnimationPose);
    } else {
      pose = PoseOverrideBlend(skeleton.pose, anim.framePoses[frame], skeleton.boneCount, factor, boneMask);
    }

    UpdateSkeletonPose(skeleton, pose);
    UnloadPose(pose);
  }
}

void UpdateSkeletonModelAnimationPoseAdditiveLayer(Skeleton skeleton, ModelAnimation anim, int frame, Pose referencePose, float factor, int flags, float *boneMask) {
  if ((anim.frameCount > 0) && (anim.bones != NULL) &&
      (anim.framePoses != NULL) && (factor != 0.0f)) {
    frame = frame % anim.frameCount;

    Pose pose;
    if (flags & USE_LOCAL_POSE) {
      Pose localSkeletonPose = PoseToLocalTransformPose(skeleton.pose, skeleton.bones, skeleton.boneCount);
      Pose localAnimationPose = PoseToLocalTransformPose(anim.framePoses[frame], skeleton.bones, skeleton.boneCount);
      Pose localReferencePose = PoseToLocalTransformPose(referencePose, skeleton.bones, skeleton.boneCount);

      Pose localAdditivePose = PoseGenerateAdditivePose(localAnimationPose, localReferencePose, skeleton.boneCount);

      Pose combinedPose = PoseAdditiveBlend(localSkeletonPose, localAdditivePose, skeleton.boneCount, 1.0f, factor, boneMask);

      pose = PoseToGlobalTransformPose(combinedPose, skeleton.bones, skeleton.boneCount);

      UnloadPose(combinedPose);
      UnloadPose(localAdditivePose);
      UnloadPose(localReferencePose);
      UnloadPose(localAnimationPose);
      UnloadPose(localSkeletonPose);
    } else {
      Pose additivePose = PoseGenerateAdditivePose(anim.framePoses[frame], referencePose, skeleton.boneCount);

      pose = PoseAdditiveBlend(skeleton.pose, additivePose, skeleton.boneCount, 1.0f, factor, boneMask);

      UnloadPose(additivePose);
    }

    UpdateSkeletonPose(skeleton, pose);
    UnloadPose(pose);
  }
}

void UnloadSkeleton(Skeleton skeleton) {
  UnloadPose(skeleton.pose);
  UnloadPose(skeleton.bindPose);

  free(skeleton.bones);
  free(skeleton.boneMatrices);
}

#endif

