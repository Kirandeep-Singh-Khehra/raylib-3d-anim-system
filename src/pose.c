#ifndef __KIRAN_RAY_POSE__
#define __KIRAN_RAY_POSE__

#include "bone_mask.c"
#include "transform.c"

#include <stdlib.h>
#include <string.h>

typedef Transform *Pose;

Pose InitPose(int boneCount);
Pose CopyPose(Pose pose, int boneCount);
void UnloadPose(Pose pose);

Pose PoseScale(Pose pose, int boneCount, float factor);
Pose PoseInvert(Pose pose, int boneCount);
Pose PoseApply(Pose poseA, Pose poseB, int boneCount);
Pose PoseGenerateAdditivePose(Pose pose, Pose referencePose, int boneCount);
Pose PoseLerp(Pose poseA, Pose poseB, int boneCount, float factor);

Pose PoseOverrideBlend(Pose poseA, Pose poseB, int boneCount, float factor,
                       float *boneMask);
Pose PoseAdditiveBlend(Pose poseA, Pose poseB, int boneCount, float factorA,
                       float factorB, float *boneMask);

Pose PoseToPoseTransform(Pose poseA, Pose poseB, int boneCount);
Matrix *PoseToPoseTransformMatrices(Pose poseA, Pose poseB, int boneCount);
Matrix *PoseToTransformMatrix(Pose pose, int boneCount);

Pose PoseToLocalTransformPose(Pose pose, BoneInfo *bones, int boneCount);
Pose PoseToGlobalTransformPose(Pose pose, BoneInfo *bones, int boneCount);

void UpdateModelMeshFromPose(Model model, Pose pose);

void DrawPose(Pose pose, BoneInfo *bones, int boneCount, Matrix mat,
              Color color);

Pose InitPose(int boneCount) {
  Pose p = malloc(boneCount * sizeof(Transform));

  return p;
}

Pose CopyPose(Pose pose, int boneCount) {
  Pose p = InitPose(boneCount);

  memcpy(p, pose, boneCount * sizeof(Transform));

  return p;
}

Pose PoseScale(Pose pose, int boneCount, float factor) {
  Pose poseResult = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    poseResult[i] = TransformScale(pose[i], factor);
  }

  return pose;
}

Pose PoseGenerateAdditivePose(Pose targetPose, Pose referencePose,
                              int boneCount) {
  Pose pose = InitPose(boneCount);
  for (int i = 0; i < boneCount; i++) {
    pose[i] = RelativeTransform(targetPose[i], referencePose[i]);
  }

  return pose;
}

Pose PoseLerp(Pose poseA, Pose poseB, int boneCount, float factor) {
  Pose pose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    pose[i] = TransformLerp(poseA[i], poseB[i], factor);
  }

  return pose;
}

Pose PoseOverrideBlend(Pose poseA, Pose poseB, int boneCount, float factor,
                       float *boneMask) {
  Pose pose = InitPose(boneCount);

  bool boneMaskGiven = true;
  if (boneMask == NULL) {
    boneMaskGiven = false;
    boneMask = BoneMaskOnes(boneCount);
  }

  for (int i = 0; i < boneCount; i++) {
    pose[i] = TransformLerp(poseA[i], poseB[i], factor * boneMask[i]);
  }

  if (boneMaskGiven == false) {
    free(boneMask);
  }

  return pose;
}

Pose PoseAdditiveBlend(Pose poseA, Pose poseB, int boneCount, float weightA,
                       float weightB, float *boneMask) {
  Pose pose = InitPose(boneCount);

  bool boneMaskGiven = true;
  if (boneMask == NULL) {
    boneMaskGiven = false;
    boneMask = BoneMaskOnes(boneCount);
  }

  for (int i = 0; i < boneCount; i++) {

    Transform in = TransformScale(poseA[i], weightA);
    Transform out = TransformScale(poseB[i], weightB * boneMask[i]);

    pose[i] = TransformApply(in, out);
  }

  if (boneMaskGiven == false) {
    free(boneMask);
  }

  return pose;
}

Pose PoseApply(Pose poseA, Pose poseB, int boneCount) {
  Pose pose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    pose[i] = TransformApply(poseA[i], poseB[i]);
  }

  return pose;
}

Matrix *PoseToMatrices(Pose pose, int boneCount) {
  Matrix *matrices = malloc(boneCount * sizeof(Matrix));

  for (int boneId = 0; boneId < boneCount; boneId++) {
    matrices[boneId] = TransformToMatrix(pose[boneId]);
  }

  return matrices;
}

Pose PoseToPoseTransform(Pose poseA, Pose poseB, int boneCount) {
  Pose resultPose = InitPose(boneCount);

  // Update all bones and boneMatrices of first mesh with bones.
  for (int boneId = 0; boneId < boneCount; boneId++) {
    Transform in = poseA[boneId];
    Transform out = poseB[boneId];

    resultPose[boneId] = TransformToTransformTransform(in, out);
  }

  return resultPose;
}

Matrix *PoseToPoseTransformMatrices(Pose poseA, Pose poseB, int boneCount) {
  Pose transformPose = PoseToPoseTransform(poseA, poseB, boneCount);
  Matrix *boneMatrices = PoseToTransformMatrix(transformPose, boneCount);

  UnloadPose(transformPose);
  return boneMatrices;
}

Matrix *PoseToTransformMatrix(Pose pose, int boneCount) {
  Matrix *boneMatrices = malloc(boneCount * sizeof(Matrix));

  for (int boneId = 0; boneId < boneCount; boneId++) {
    boneMatrices[boneId] = TransformToMatrix(pose[boneId]);
  }

  return boneMatrices;
}

Pose PoseInvert(Pose pose, int boneCount) {
  Pose invPose = InitPose(boneCount);

  // Update all bones and boneMatrices of first mesh with bones.
  for (int boneId = 0; boneId < boneCount; boneId++) {
    invPose[boneId] = TransformInvert(pose[boneId]);
  }

  return invPose;
}

void UpdateModelMeshFromPose(Model model, Pose pose) {
  Matrix *matrices =
      PoseToPoseTransformMatrices(model.bindPose, pose, model.boneCount);

  for (int i = 0; i < model.meshCount; i++) {
    if (model.meshes[i].boneMatrices) {
      memcpy(model.meshes[i].boneMatrices, matrices,
             model.meshes[i].boneCount * sizeof(Matrix));
    }
  }

  free(matrices);
}

Pose PoseToLocalTransformPose(Pose globalPose, BoneInfo *bones, int boneCount) {
  Pose relativePose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    int parentIndex = bones[i].parent;
    if (parentIndex == -1) {
      relativePose[i] = globalPose[i];
    } else {
      Transform parentGlobalTransform = globalPose[parentIndex];

      Vector3 relativeTranslation = Vector3RotateByQuaternion(
          Vector3Subtract(globalPose[i].translation,
                          parentGlobalTransform.translation),
          QuaternionInvert(parentGlobalTransform.rotation));

      Quaternion relativeRotation =
          QuaternionMultiply(QuaternionInvert(parentGlobalTransform.rotation),
                             globalPose[i].rotation);

      Vector3 relativeScale =
          Vector3Divide(globalPose[i].scale, parentGlobalTransform.scale);

      relativePose[i].translation = relativeTranslation;
      relativePose[i].rotation = relativeRotation;
      relativePose[i].scale = relativeScale;
    }
  }

  return relativePose;
}

Pose PoseToGlobalTransformPose(Pose localPose, BoneInfo *bones, int boneCount) {
  Pose globalPose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    int parentIndex = bones[i].parent;

    if (parentIndex == -1) {
      globalPose[i] = localPose[i];
    } else {
      Transform parentGlobalTransform = globalPose[parentIndex];

      Vector3 globalTranslation =
          Vector3Add(parentGlobalTransform.translation,
                     Vector3RotateByQuaternion(localPose[i].translation,
                                               parentGlobalTransform.rotation));

      Quaternion globalRotation = QuaternionMultiply(
          parentGlobalTransform.rotation, localPose[i].rotation);

      Vector3 globalScale =
          Vector3Multiply(parentGlobalTransform.scale, localPose[i].scale);

      globalPose[i].translation = globalTranslation;
      globalPose[i].rotation = globalRotation;
      globalPose[i].scale = globalScale;
    }
  }

  return globalPose;
}

void UnloadPose(Pose pose) {
  if (pose) {
    free(pose);
  }
}

void DrawPose(Pose pose, BoneInfo *bones, int boneCount, Matrix mat,
              Color color) {
  for (int i = 0; i < boneCount; i++) {
    DrawCube(Vector3Transform(pose[i].translation, mat),
             pose[i].scale.x * 0.05f, pose[i].scale.y * 0.05f,
             pose[i].scale.z * 0.05f, color);

    if (bones[i].parent >= 0) {
      DrawLine3D(Vector3Transform(pose[i].translation, mat),
                 Vector3Transform(pose[bones[i].parent].translation, mat),
                 color);
    }
  }
}

#endif

