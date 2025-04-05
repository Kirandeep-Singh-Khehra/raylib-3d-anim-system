#include <raylib.h>
#include <raymath.h>

#include "../../src/skeleton.h"

typedef struct AnimEngine AnimEngine;

typedef Pose (*AnimClip)(AnimEngine engine, Pose pose, ...);

typedef enum AnimClipState {
  ENTERING,
  RUNNING,
  EXITING
} AnimClipState;

typedef struct AnimStack {
  int clipCount;
  AnimClip *clips;
  AnimClipState *clipStates;
  float *weights;
} AnimStack;

typedef struct AnimEngine {
  int animCount;
  float *animFrameCounters;
  ModelAnimation *anims;

  AnimStack stack;
} AnimEngine;

AnimEngine CreateAnimEngine(char *filename) {
  AnimEngine engine;
  engine.anims = LoadModelAnimations(filename, &engine.animCount);
  engine.animFrameCounters = malloc(sizeof(float) * engine.animCount);

  engine.stack.clipCount = 0;

  return engine;
}

void AnimEngineAddAnimClip(AnimEngine *engine, AnimClip clip) {
  if (engine->stack.clipCount == 0) {
    engine->stack.clips = malloc(sizeof(AnimClip));
    engine->stack.clipStates = malloc(sizeof(AnimClipState));
    engine->stack.weights = malloc(sizeof(float));
  } else {
    engine->stack.clips = realloc(engine->stack.clips, sizeof(AnimClip) * (engine->stack.clipCount + 1));
    engine->stack.clipStates = realloc(engine->stack.clipStates, sizeof(AnimClipState) * (engine->stack.clipCount + 1));
    engine->stack.weights = realloc(engine->stack.weights, sizeof(float) * (engine->stack.clipCount + 1));
  }

  engine->stack.clips[engine->stack.clipCount] = clip;
  engine->stack.clipStates[engine->stack.clipCount] = ENTERING;
  engine->stack.clipCount ++;
}

Pose AnimEnginePlayAnimStack(AnimEngine engine, Pose basePose) {
  int boneCount = engine.anims[0].boneCount;
  Pose prevPose = CopyPose(basePose, boneCount);
  Pose nextPose;
  Pose finalPose;
  for (int clipId = 0; clipId < engine.stack.clipCount; clipId++) {
    nextPose = engine.stack.clips[clipId](engine, prevPose);
    finalPose = PoseLerp(prevPose, nextPose, boneCount, engine.stack.weights[clipId]);
    UnloadPose(prevPose);
    UnloadPose(nextPose);

    /* Prepare for next iteration */
    prevPose = finalPose;
    nextPose = NULL;
    finalPose = NULL;
  }
  return prevPose;
}

