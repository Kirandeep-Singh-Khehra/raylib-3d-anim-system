#ifndef _KCAMERA_
#define _KCAMERA_

#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "kutils.c"

typedef struct {
  Camera baseCamera;

  Vector3 focusPoint;

  float heightBase;
  float heightBaseOffset;
  float radius;

  float MIN_CAMERA_RADIUS;
  float MAX_CAMERA_RADIUS;

  float MIN_CAMERA_HEIGHT;
  float MAX_CAMERA_HEIGHT;

  float translationSmoothness;
  float narrowingRatio;

  float mouseSensitivity;
  float scrollSensitivity;

} KCamera;

// Helpers
Vector3 KGetCameraFront(KCamera);
Vector3 KGetCameraRight(KCamera);
Vector3 KGetCameraTrueUp(KCamera);

// Modification functions
void KCameraInit(KCamera *);
void KUpdateCamera(KCamera *);

void KUpdateCameraRadiusAndBase(KCamera *);

void KMoveCameraFront(KCamera *, float);
void KMoveCameraRight(KCamera *, float);
void KMoveCameraUp(KCamera *, float);
void KMoveCameraTrueUp(KCamera *, float);

// Implementation
void KCameraInit(KCamera *camera) {
  camera->baseCamera = (Camera){0};
  camera->baseCamera.position = (Vector3){0, 100.0f, -10.0f};
  camera->baseCamera.target = (Vector3){0.0f, 6.5f, 0.0f};
  camera->baseCamera.target.y += 6.5f;
  camera->baseCamera.up = (Vector3){0.0f, 1.0f, 0.0f};
  camera->baseCamera.fovy = 45.0f;
  camera->baseCamera.projection = CAMERA_PERSPECTIVE;

  // camera->heightBase = 4.0f;
  camera->heightBase = 0.0f;
  camera->heightBaseOffset = 0.0f;
  camera->radius = 4.0f;

  camera->MIN_CAMERA_RADIUS = 1.0f;
  camera->MAX_CAMERA_RADIUS = 10.0f;

  camera->MIN_CAMERA_HEIGHT = -4.0f;
  camera->MAX_CAMERA_HEIGHT = 4.0f;

  camera->translationSmoothness = 0.125f;
  camera->narrowingRatio = 0.25f;

  camera->mouseSensitivity = 0.02f;
  camera->scrollSensitivity = 0.25f;
}

void KUpdateCamera(KCamera *camera) {
  static Vector3 previousFocusPoint = (Vector3) {0.0f, 0.0f, 0.0f};

  { // Update camera position by interpolation //
    camera->baseCamera.target = camera->focusPoint;
    // camera->heightBase = camera->focusPoint.y + 10.0f;
    
    Vector2 cameraToCameraFocusPointVector = Vector2Subtract(
      (Vector2) {camera->baseCamera.position.x, camera->baseCamera.position.z}, 
      (Vector2) {camera->focusPoint.x         , camera->focusPoint.z         });
    
    float cameraToCameraFocusPointDistance = Vector2Length(cameraToCameraFocusPointVector);
    float cameraOffBy = cameraToCameraFocusPointDistance - camera->radius;

    Vector2 cameraDelta =
        Vector2Scale(cameraToCameraFocusPointVector,
                     (cameraOffBy / cameraToCameraFocusPointDistance) *
                         camera->translationSmoothness);

    camera->baseCamera.position.x -= cameraDelta.x;
    camera->baseCamera.position.z -= cameraDelta.y;
  }

  { // Update horizontal movement with keyboard
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
      camera->baseCamera.position = Vector3Add(camera->baseCamera.position, Vector3Subtract(camera->focusPoint, previousFocusPoint));
    }
  }
  
  { // Update camera radius and rotational(not as it is) position with mouse
    Vector2 mouseDelta = GetMouseDelta();
    KMoveCameraRight(camera, mouseDelta.x * camera->mouseSensitivity);
    KMoveCameraTrueUp(camera, -mouseDelta.y * camera->mouseSensitivity);
  }

  { // Update Camera Radius with scroll //
    //   Update radius
    camera->radius = clampFloat(
        camera->radius - GetMouseWheelMove() * camera->scrollSensitivity,
        camera->MIN_CAMERA_RADIUS, camera->MAX_CAMERA_RADIUS);
    //   Update height(lower when close)
    camera->baseCamera.position.y =
        camera->focusPoint.y + camera->heightBase + camera->heightBaseOffset -
        (camera->MAX_CAMERA_RADIUS - camera->radius) * camera->narrowingRatio;
  }

  UpdateCamera(&(camera->baseCamera), CAMERA_CUSTOM);
  previousFocusPoint = camera->focusPoint;
}

void KUpdateCameraRadiusAndBase(KCamera *camera) {
  /* camera->radius = clampFloat(Vector2Distance(
    (Vector2) { camera->baseCamera.position.x, camera->baseCamera.position.z},
    (Vector2) { camera->focusPoint.x,          camera->focusPoint.z         })
    , camera->MAX_CAMERA_RADIUS, camera->MIN_CAMERA_RADIUS);

  camera->heightBaseOffset = camera->baseCamera.position.y - (camera->focusPoint.y + camera->heightBase);
  */
}

void KMoveCameraFront(KCamera *camera, float distance) {
  camera->baseCamera.position =
      Vector3Add(camera->baseCamera.position,
                 Vector3Scale(KGetCameraFront(*camera), distance));
}

void KMoveCameraRight(KCamera *camera, float distance) {
  camera->baseCamera.position =
      Vector3Add(camera->baseCamera.position,
                 Vector3Scale(KGetCameraRight(*camera), distance));
}

void KMoveCameraUp(KCamera *camera, float distance) {
  camera->baseCamera.position =
      Vector3Add(camera->baseCamera.position,
                 Vector3Scale(camera->baseCamera.up, distance));
}

void KMoveCameraTrueUp(KCamera *camera, float distance) {
  camera->baseCamera.position =
      Vector3Add(camera->baseCamera.position,
                 Vector3Scale(KGetCameraTrueUp(*camera), distance));

  if (distance != 0) {
    Vector3 delta = Vector3Scale(KGetCameraTrueUp(*camera), distance);
    float angle = Vector3Angle(delta, (Vector3){delta.x, 0, delta.z});
    float length = Vector2Length((Vector2){delta.x, delta.z});

    camera->radius = clampFloat(camera->radius + (sign((camera->baseCamera.position.y - camera->focusPoint.x) * distance) * length), camera->MIN_CAMERA_RADIUS, camera->MAX_CAMERA_RADIUS);
    camera->heightBaseOffset = clampFloat(camera->heightBaseOffset - distance * cos(Vector2Length((Vector2){delta.x, delta.z})), camera->MIN_CAMERA_HEIGHT, camera->MAX_CAMERA_HEIGHT);
  }
}

Vector3 KGetCameraFront(KCamera camera) {
  return Vector3Normalize(
      Vector3Subtract(camera.baseCamera.target, camera.baseCamera.position));
}

Vector3 KGetCameraRight(KCamera camera) {
  return Vector3Normalize(
      Vector3CrossProduct(camera.baseCamera.up, KGetCameraFront(camera)));
}

Vector3 KGetCameraTrueUp(KCamera camera) {
  return Vector3Normalize(
      Vector3CrossProduct(KGetCameraRight(camera), KGetCameraFront(camera)));
}

#endif
