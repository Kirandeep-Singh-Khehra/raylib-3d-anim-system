#include "raylib.h"
#include <raymath.h>
#include <stdlib.h>
#include <math.h>

typedef struct AnimEngine {
  int animsCount;
  float *animFrameCounters;
  ModelAnimation *anims;
} AnimEngine;

AnimEngine CreateAnimEngine(const char *fileName) {
  AnimEngine animEng = {0};
  animEng.anims = LoadModelAnimations(fileName, &animEng.animsCount);

  animEng.animFrameCounters = malloc(animEng.animsCount * sizeof(int));
  for (int i = 0; i < animEng.animsCount; i++) {
    animEng.animFrameCounters[i] = 0;
  }

  return animEng;
};

void SyncAnimationLoop(float* frameA, float* frameB, int maxFrameA, int maxFrameB, float factor, float dt) {
    float lerpFrame = Lerp(maxFrameA, maxFrameB, factor);

    float adjustedFrameTimeA = maxFrameA / lerpFrame;
    float adjustedFrameTimeB = maxFrameB / lerpFrame;

    *frameA = fmodf(*frameA + adjustedFrameTimeA, maxFrameA);
    *frameB = fmodf(*frameB + adjustedFrameTimeB, maxFrameB);
}

void SyncAnimationLoop2(float* frameA, float* frameB, int maxFrameA, int maxFrameB, int frame, float factor, float dt) {
    float lerpFrame = Lerp(maxFrameA, maxFrameB, factor);
    frame = (frame % (int)lerpFrame);

    float frameRatio = (lerpFrame / frame);

    *frameA = maxFrameA / frameRatio;
    *frameB = maxFrameB / frameRatio;
}

