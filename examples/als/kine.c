#ifndef KINEOBJECT
#define KINEOBJECT

#include <raylib.h>
#include <raymath.h>
#include "kutils.c"

typedef struct KineObject {
  Vector3 position;
  Vector3 rotation;
  Vector3 scale;

  Vector3 velocity;
  Vector3 force;

  float mass;
} KineObject;

KineObject KineIdentity() {
  KineObject kine = {0};

  kine.position = Vector3Zero();
  kine.rotation = Vector3Zero();
  kine.scale = Vector3One();
  
  kine.velocity = Vector3Zero();
  kine.force = Vector3Zero();

  kine.mass = 10.0f;
  return kine;
}

void UpdateKineObject(KineObject *object) {
  Vector3 linear_acceleration = {
    .x = object->force.x / object->mass,
    .y = object->force.y / object->mass,
    .z = object->force.z / object->mass
  };

  float delta_time = GetFrameTime();

  // s = ut + (1/2)at^2
  object->position.x += object->velocity.x * delta_time + 0.5 * linear_acceleration.x * delta_time * delta_time;
  object->position.y += object->velocity.y * delta_time + 0.5 * linear_acceleration.y * delta_time * delta_time;
  object->position.z += object->velocity.z * delta_time + 0.5 * linear_acceleration.z * delta_time * delta_time;

  // v = u + at
  object->velocity.x += linear_acceleration.x * delta_time;
  object->velocity.y += linear_acceleration.y * delta_time;
  object->velocity.z += linear_acceleration.z * delta_time;

  // MatrixMultiply(MatrixRotateY(object->rotation.y), MatrixMultiply(MatrixRotateY(object->rotation.x), MatrixRotateY(object->rotation.z)));
}

#endif
