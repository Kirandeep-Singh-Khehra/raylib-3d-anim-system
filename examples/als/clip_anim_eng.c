#include <raylib.h>
#include <raymath.h>

#include "../../src/skeleton.h"

typedef struct AnimEngine AnimEngine;

typedef Pose (*AnimClip)(AnimEngine engine, Pose pose, ...);

typedef enum AnimClipState {
  LOOP,
  FADE_OUT,
  FADE_IN
} AnimClipState; 

typedef struct AnimStack {
  int clipCount;
  AnimClip *clips;
  AnimClipState *clipStates;
} AnimStack;

typedef struct AnimEngine {
  int animsCount;
  float *animFrameCounters;
  ModelAnimation *anims;

  AnimStack stack;
} AnimEngine;

void AnimEngineAddAnimClip(AnimEngine *engine, AnimClip clip) {
  engine->stack.clips = realloc(engine->stack.clips, sizeof(AnimClip) * (engine->stack.clipCount + 1));
  engine->stack.clipStates = realloc(engine->stack.clipStates, sizeof(AnimClipState) * (engine->stack.clipCount + 1));

  engine->stack.clips[engine->stack.clipCount] = clip;
  engine->stack.clipCount ++;
}

Pose AnimEnginePlayAnimStack(AnimEngine engine, Pose basePose, int boneCount) {
  Pose prevPose = CopyPose(basePose, boneCount);
  Pose nextPose;
  for (int i = 0; i < engine.stack.clipCount; i++) {
    nextPose = engine.stack.clips[i](engine, prevPose);
    UnloadPose(prevPose);

    /* Prepare for next iteration */
    prevPose = nextPose;
    nextPose = NULL;
  }
  return prevPose;
}

