#ifndef _K_UTILS_
#define _K_UTILS_

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define sign(x) ((x < 0)?-1 : 1)
#define max(a, b) ((a > b)? a : b)
#define min(a, b) ((a < b)? a : b)

#define new(_s) (_s*)malloc(sizeof(_s))
#define newN(_s, _n) (_s*)malloc(sizeof(_s) * _n)

#define abs_(x) ((x < 0) ? -x : x)
#define clamp(x, a, b) ((x < a)? a : (x > b)? b : x)

#define rmod(x, y) (((x) % (y)) + (y)) % (y)

#define GetAnimPose(anim, frame) (anim.framePoses[rmod(frame, anim.frameCount)])

void printVector(char *tag, Vector3 v) {
  printf("%s(%f, %f, %f)\n", tag, v.x, v.y, v.z);
}

float clampFloat(float num, float min, float max) {
  if (num > max)
    return max;
  if (num < min)
    return min;
  return num;
}

float bilinear_interpolation(float x, float y, float q11, float q12, float q21, float q22) {
    float q11_21 = q11 + x * (q21 - q11);
    float q12_22 = q12 + x * (q22 - q12);

    return q11_21 + y * (q12_22 - q11_21);
}

// Shortest angle between 2 angles
float AngleDiff(float a1, float a2) {
  float min = -PI;
  float max = PI;
  
  float da = fmodf((a2 - a1), (max - min));
  return fmodf(2 * da, (max - min)) - da;
}

// Linear interpolation (lerp) on angles in the range [-π, π]
float LerpAngle(float start, float end, float t) {
    return start + AngleDiff(start, end) * t;
}

float Vector3ManhattanDistance(Vector3 v1, Vector3 v2) {
  Vector3 result = Vector3Subtract(v2, v1);
  return fabsf(result.x) + fabsf(result.y) + fabsf(result.z);
}

float Vector2ManhattanDistance(Vector2 v1, Vector2 v2) {
  Vector2 result = Vector2Subtract(v2, v1);
  return fabsf(result.x) + fabsf(result.y);
}



#endif
