# RayLib 3D AnimSystem
`Skeleton` and `Pose` based 3D Animation System for RayLib

# Features
 - Layer based animation system.(Similar to Unity's Layered animation system).
 - Overriden/Interpolated and Additive blending implemented at both `Pose` level and `Skeleton` level.
 - Choice of blending using local pose or global pose.
 - Convert global pose to local pose or vice versa.
 - `Pose` is defined as array of `Transform`. So, `ModelAnimation.framePoses[frame]` and `Model.bindPose` are both considered as `Pose` and hence completely compatible with Pose functions.
 - [`BoneMask`](https://github.com/Kirandeep-Singh-Khehra/raylib-3d-anim-system/blob/main/src/bone_mask.c) implementation to assist in split body animation.
 - Mask bones using bone name and regular expression.

# How to use?
1. Include in your project.
```sh
git submodule add https://github.com/Kirandeep-Singh-Khehra/raylib-3d-anim-system.git ./kanim
```

2. Add in you `CMakeLists.txt`
```cmake
add_subdirectory(kanim) # Assuming kanim is in the same directory as this CMakeLists.txt
# ...
# Link to your executable
  target_link_libraries(ray_bullet_test PRIVATE kanim)
```

# Example
Basic usage example is present in [`examples/skeleton/skeleton_additive_blending.c`](https://github.com/Kirandeep-Singh-Khehra/raylib-3d-anim-system/blob/main/examples/skeleton/skeleton_additive_blending.c)

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

   a. Add Override/Interpolation layer
      ```c
      void UpdateSkeletonModelAnimationPoseOverrideLayer(Skeleton skeleton,
            ModelAnimation anim, int frame,
            float factor,     /* weight of layer */
            int flags,        /* not implemented */
            float *boneMask); /* affect per bone */
      ```
   b. Add Additive layer
      ```c
      void UpdateSkeletonModelAnimationPoseAdditiveLayer(Skeleton skeleton,
            ModelAnimation anim, int frame,
            Pose referencePose/* reference pose  */
            float factor,     /* weight of layer */
            int flags,        /* not implemented */
            float *boneMask); /* affect per bone */
      ```
      > **Don't know what reference pose is and why is it needed?**</br>
      > The transforms which when applied to reference pose makes it target pose. Additive layer calculates those transforms and applies to `Skeleton`'s pose.

4. Apply pose to model.
```c
UpdateModelMeshFromPose(model, skeleton.pose);
```

**Happy Animating :)**

