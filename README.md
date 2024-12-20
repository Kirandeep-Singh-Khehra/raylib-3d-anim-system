# RayLib 3D AnimSystem
`Skeleton` and `Pose` based 3D Animation System for RayLib

# Example
Basic usage example is present in [`examples/skeleton/skeleton_animation_blending.c`](https://github.com/Kirandeep-Singh-Khehra/raylib-3d-anim-system/blob/main/examples/skeleton/skeleton_animation_blending.c)

# Usage
This system in built with layer based animation blending(similar to Unity's Layered Animation) in mind. And its general usage consists of:

1. First getting `Skeleton` from model.
```c
Skeleton skeleton = LoadSkeletonFromModel(model);
```

2. Then assign a base pose to begin with. Base pose can be a single pose or a pose which is interpolated between two poses.

   a. Use single pose.
      ```c
      void UpdateSkeletonModelAnimation(Skeleton skeleton, ModelAnimation anim, int frame);
      ```
      or use the following way(above one is preffered).
      ```c
      skeleton.pose = CopyPose(model.bindPose, model.boneCount);
      ```
   b. Use interpolated/blended pose.
      ```c
      void UpdateSkeletonModelAnimationPoseLerp(Skeleton skeleton,
          ModelAnimation animA, int frameA,
          ModelAnimation animB, int frameB, float blendFactor);
      ```

3. Then add any number of layers as needed.
  ```c
  void UpdateSkeletonModelAnimationPoseOverrideLayer(Skeleton skeleton,
        ModelAnimation anim, int frame,
        float factor,     /* weight of layer */
        int flags,        /* not implemented */
        float *boneMask); /* affect per bone */
  ```
  > Currently only override/interpolation is supported. Additive layer will be comming soon.

**More Docs and Features Comming Soon ...**

**Happy Animating :)**

