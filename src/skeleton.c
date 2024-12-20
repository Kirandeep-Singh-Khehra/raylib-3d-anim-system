#ifndef __KIRAN_RAY_SKELETON__
#define __KIRAN_RAY_SKELETON__

#include "pose.c"
#include <stdio.h>

/*
  Defined to implement bone hierarchy (present to check 
    if any modification is needed for BoneInfo 
    but is not implemented yet.)

  Thinking to sort bones according to parent(Prefix tree) to get workable structure for bone hierarchy
   - FK(Forward kinematics) Using tree data structure O(n) (n is no. of bones) and theta(log(n))
   - FK using sorting O(n) and theta(n) <- Average case can be improved using weighed/right-skewed binary
      search followed double pointers. 
*/
typedef struct KBoneInfo {
    char name[32];          // Bone name
    int parent;             // Bone parent
} KBoneInfo;

typedef struct Skeleton {
  /* Extracted from Model */
  int boneCount;         // Number of bones
  BoneInfo *bones;       // Bones information (skeleton)
  Pose bindPose;   // Bones base transformation (pose)

  /* Extracted from Mesh */
  Matrix *boneMatrices;  // Bones animated transformation matrices (not used yet)

  /* Kiran defined */
  Pose pose;      // Current pose
} Skeleton;

Skeleton LoadSkeletonFromModel(Model model);
// void UpdateSkeletonBoneMatrices(Skeleton skeleton);
// void UpdateModelBonesFromSkeleton(Model model, Skeleton skeleton); // Works same as below one(UpdateModelMeshFromPose())
void UpdateModelBonesFromPose(Model model, Pose pose);
void UnloadSkeleton(Skeleton skeleton);

void UpdateSkeletonPose(Skeleton skeleton, Pose pose);
void UpdateSkeletonModelAnimation(Skeleton skeleton, ModelAnimation anim, int frame);
void UpdateSkeletonModelAnimationPoseLerp(Skeleton skeleton, ModelAnimation  animA, int frameA, ModelAnimation animB, int frameB, float blendFactor);
void UpdateSkeletonModelAnimationPoseOverrideLayer(Skeleton skeleton, ModelAnimation anim, int frame, float factor, int flags, float *boneMask);
void UpdateSkeletonModelAnimationPoseAdditiveLayer(Skeleton skeleton, ModelAnimation anim, int frame, float factor, int flags, float *boneMask);

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
                            float blendFactor) {
  if ((animA.frameCount > 0) && (animA.bones != NULL) &&
      (animA.framePoses != NULL) && (animB.frameCount > 0) &&
      (animB.bones != NULL) && (animB.framePoses != NULL) &&
      (blendFactor >= 0.0f) && (blendFactor <= 1.0f)) {
    frameA = frameA % animA.frameCount;
    frameB = frameB % animB.frameCount;

    Pose pose = PoseLerp(animA.framePoses[frameA], animB.framePoses[frameB], skeleton.boneCount, blendFactor);

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

    Pose pose = PoseLerp(skeleton.pose, anim.framePoses[frame], skeleton.boneCount, factor);

    UpdateSkeletonPose(skeleton, pose);
    free(pose);
  }
}

void UnloadSkeleton(Skeleton skeleton) {
  UnloadPose(skeleton.pose);
  UnloadPose(skeleton.bindPose);

  free(skeleton.bones);
  free(skeleton.boneMatrices);
}


#endif

