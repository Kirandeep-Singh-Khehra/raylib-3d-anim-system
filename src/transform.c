#ifndef __KIRAN_RAY_TRANSFORM__
#define __KIRAN_RAY_TRANSFORM__

#include <raylib.h>
#include <raymath.h>

Transform TransformScale(Transform transformA, float factor);
Transform TransformLerp(Transform transformA, Transform transformB,
                        float factor);
Transform TransformApply(Transform transformA, Transform transformB);
Transform TransformToTransformTransform(Transform in, Transform out);
Transform TransformInvert(Transform transform);
Matrix TransformToMatrix(Transform transform);

Transform TransformScale(Transform transform, float factor) {
  Transform transformResult = {0};

  transformResult.translation = Vector3Scale(transform.translation, factor);
  transformResult.rotation =
      QuaternionSlerp(QuaternionIdentity(), transform.rotation, factor);
  transformResult.scale.x = pow(transform.scale.x, factor);
  transformResult.scale.y = pow(transform.scale.y, factor);
  transformResult.scale.z = pow(transform.scale.z, factor);

  return transformResult;
}

Transform TransformLerp(Transform transformA, Transform transformB,
                        float amount) {
  Transform transform = {0};

  transform.translation =
      Vector3Lerp(transformA.translation, transformB.translation, amount);
  transform.rotation =
      QuaternionSlerp(transformA.rotation, transformB.rotation, amount);
  transform.scale = Vector3Lerp(transformA.scale, transformB.scale, amount);

  return transform;
}

Transform TransformApply(Transform transformA, Transform transformB) {
  Transform transform = {0};

  transform.translation =
      Vector3Add(transformA.translation, transformB.translation);
  transform.rotation =
      QuaternionMultiply(transformA.rotation, transformB.rotation);
  transform.scale = Vector3Multiply(transformA.scale, transformB.scale);

  return transform;
}

Transform TransformSubtract(Transform transformA, Transform transformB) {
  Transform transform = {0};

  transform.translation =
      Vector3Subtract(transformA.translation, transformB.translation);
  transform.rotation =
      QuaternionMultiply(transformB.rotation, QuaternionInvert(transformA.rotation));
  transform.scale = Vector3Divide(transformA.scale, transformB.scale);

  return transform;
}

Transform TransformInvert(Transform transform) {
  Transform invTransform = {0};

  invTransform.translation =
      Vector3RotateByQuaternion(Vector3Negate(transform.translation),
                                QuaternionInvert(transform.rotation));
  invTransform.rotation = QuaternionInvert(transform.rotation);
  invTransform.scale =
      Vector3Divide((Vector3){1.0f, 1.0f, 1.0f}, transform.scale);

  return invTransform;
}

Transform TransformToTransformTransform(Transform in, Transform out) {
  Transform result = {0};

  Transform inv = TransformInvert(in);

  result.translation =
      Vector3Add(Vector3RotateByQuaternion(
                     Vector3Multiply(out.scale, inv.translation), out.rotation),
                 out.translation);
  result.rotation = QuaternionMultiply(out.rotation, inv.rotation);
  result.scale = Vector3Multiply(out.scale, inv.scale);

  return result;
}

Transform RelativeTransform(Transform first, Transform second) {
  Transform out = {0};

  Quaternion invSecondRotation = QuaternionInvert(second.rotation);
  Vector3 invScale = Vector3Invert(second.scale);

  Vector3 catchePosition = Vector3Subtract(first.translation, second.translation);
  catchePosition = Vector3RotateByQuaternion(catchePosition, invSecondRotation);
  catchePosition = Vector3Multiply(catchePosition, invScale);

  out.translation = catchePosition;
  out.rotation = QuaternionMultiply(invSecondRotation, first.rotation);
  out.scale = Vector3Multiply(first.scale, invScale);

  return out;
}

Matrix TransformToMatrix(Transform transform) {
  Matrix boneMatrix = MatrixMultiply(
      MatrixMultiply(QuaternionToMatrix(transform.rotation),
                     MatrixTranslate(transform.translation.x,
                                     transform.translation.y,
                                     transform.translation.z)),
      MatrixScale(transform.scale.x, transform.scale.y, transform.scale.z));

  return boneMatrix;
}

#endif

