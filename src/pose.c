#ifndef __KIRAN_RAY_POSE__
#define __KIRAN_RAY_POSE__

#include "transform.c"

#include <stdlib.h>
#include <string.h>

typedef Transform *Pose;

Pose InitPose(int boneCount);
Pose CopyPose(Pose pose, int boneCount);
void UnloadPose(Pose pose);

Pose PoseScale(Pose pose, int boneCount, float factor);
Pose PoseInvert(Pose pose, int boneCount);
Pose PoseAdd(Pose poseA, Pose poseB, int boneCount);
Pose PoseLerp(Pose poseA, Pose poseB, int boneCount, float factor);

Pose PoseToPoseTransform(Pose poseA, Pose poseB, int boneCount);
Matrix *PoseToPoseTransformMatrices(Pose poseA, Pose poseB, int boneCount);
Matrix *PoseToTransformMatrix(Pose pose, int boneCount);

Pose PoseToLocalTransformPose(Pose pose, BoneInfo *bones, int boneCount);
Pose PoseToGlobalTransformPose(Pose pose, BoneInfo *bones, int boneCount);

void UpdateModelMeshFromPose(Model model, Pose pose);

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

Pose PoseLerp(Pose poseA, Pose poseB, int boneCount, float factor) {
  Pose pose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    pose[i] = TransformLerp(poseA[i], poseB[i], factor);
  }

  return pose;
}

Pose PoseAdd(Pose poseA, Pose poseB, int boneCount) {
  Pose pose = InitPose(boneCount);

  for (int i = 0; i < boneCount; i++) {
    pose[i] = TransformAdd(poseA[i], poseB[i]);
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
                Vector3Subtract(globalPose[i].translation, parentGlobalTransform.translation),
                QuaternionInvert(parentGlobalTransform.rotation)
            );

            Quaternion relativeRotation = QuaternionMultiply(
                QuaternionInvert(parentGlobalTransform.rotation),
                globalPose[i].rotation
            );

            Vector3 relativeScale = Vector3Divide(globalPose[i].scale, parentGlobalTransform.scale);

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

            Vector3 globalTranslation = Vector3Add(
                parentGlobalTransform.translation,
                Vector3RotateByQuaternion(localPose[i].translation, parentGlobalTransform.rotation)
            );

            Quaternion globalRotation = QuaternionMultiply(
                parentGlobalTransform.rotation,
                localPose[i].rotation
            );

            Vector3 globalScale = Vector3Multiply(parentGlobalTransform.scale, localPose[i].scale);

            globalPose[i].translation = globalTranslation;
            globalPose[i].rotation = globalRotation;
            globalPose[i].scale = globalScale;
        }
    }

    return globalPose;
}

void UnloadPose(Pose pose) { free(pose); }

#endif

