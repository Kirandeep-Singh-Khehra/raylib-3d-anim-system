#ifndef __KIRAN_RAY_BONE_MASK__
#define __KIRAN_RAY_BONE_MASK__

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>

typedef float* BoneMask;

BoneMask BoneMaskZeros(int boneCount);
BoneMask BoneMaskOnes(int boneCount);
BoneMask BoneMaskHalf(int boneCount);

void MaskBoneChild(BoneMask mask, BoneInfo *bones, char * name, float value, int boneCount);
void MaskBonesByRegex(BoneMask mask, BoneInfo *bones, char *pattern, float value, int boneCount);
void MaskChildBonesByParentRegex(BoneMask mask, BoneInfo *bones, char *pattern, float value, int boneCount);

void UnloadBoneMask(BoneMask mask);

BoneMask BoneMaskZeros(int boneCount) {
  BoneMask mask = malloc(boneCount * sizeof(float));

  for (int i = 0; i < boneCount; i++) {
    mask [i] = 0.0f;
  }

  return mask;
}

BoneMask BoneMaskOnes(int boneCount) {
  BoneMask mask = malloc(boneCount * sizeof(float));

  for (int i = 0; i < boneCount; i++) {
    mask [i] = 1.0f;
  }

  return mask;
}

BoneMask BoneMaskHalf(int boneCount) {
  BoneMask mask = malloc(boneCount * sizeof(float));

  for (int i = 0; i < boneCount; i++) {
    mask [i] = 0.5f;
  }

  return mask;
}

void MaskBoneChild(BoneMask mask, BoneInfo *bones, char * name, float value, int boneCount) {
    for (int boneId = 0; boneId < boneCount; boneId++) {
        int currentBoneId = boneId;
        while (bones[currentBoneId].parent != -1) {
            if (strcmp(bones[bones[currentBoneId].parent].name, name) == 0) {
                mask[boneId] = value;
                break;
            }
            currentBoneId = bones[currentBoneId].parent;
        }
    }
}

void MaskBonesByRegex(BoneMask mask, BoneInfo *bones, char *pattern, float value, int boneCount) {
    regex_t regex;
    int ret;

    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        printf("Could not compile regex\n");
        free(mask);
        return;
    }

    for (int i = 0; i < boneCount; i++) {
        ret = regexec(&regex, bones[i].name, 0, NULL, 0);
        if (ret == 0) {
            mask[i] = value;
            printf("Bone \"%s\" matches the pattern.\n", bones[i].name);
        }
    }

    regfree(&regex);
}

void MaskChildBonesByParentRegex(BoneMask mask, BoneInfo *bones, char *pattern, float value, int boneCount) {
    regex_t regex;
    int ret;

    for (int i = 0; i < boneCount; i++) {
        mask[i] = 0.0f;
    }

    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        printf("Could not compile regex\n");
        free(mask);
        return;
    }

    for (int i = 0; i < boneCount; i++) {
        int currentBoneId = i;
        while (bones[currentBoneId].parent != -1) {
            int parentBoneId = bones[currentBoneId].parent;

            ret = regexec(&regex, bones[parentBoneId].name, 0, NULL, 0);
            if (ret == 0) {
                mask[currentBoneId] = value;
                printf("Bone \"%s\" (ID: %d) is a child of bone \"%s\" (ID: %d) which matches the pattern.\n",
                       bones[currentBoneId].name, currentBoneId, bones[parentBoneId].name, parentBoneId);
                break;
            }
            currentBoneId = parentBoneId;
        }
    }

    regfree(&regex);
}

void UnloadBoneMask(BoneMask mask) {
  if(mask) {
    free(mask);
  }
}

#endif
