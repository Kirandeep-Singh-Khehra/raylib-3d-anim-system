#include <r3d.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "../../src/skeleton.h"
#define R3D_GetAnimPose(anim, frame) (anim.frameTransforms[(frame % anim.frameCount)])

// Camera setup
Camera3D camera = {
    .position = { -3, 3, 3 },
    .target = { 0, 1.0, 0 },
    .up = { 0, 1, 0 },
    .fovy = 60.0f,
    .projection = CAMERA_PERSPECTIVE
};

R3D_Model model;

int animCount = 0;
R3D_ModelAnimation* anims = 0;

int main(void)
{
    InitWindow(800, 600, "R3D Example");
    R3D_Init(800, 600, 0);

    // Setup lighting
    R3D_Light light = R3D_CreateLight(R3D_LIGHT_DIR);
    R3D_SetLightDirection(light, (Vector3){ -1, -1, -1 });
    R3D_SetLightActive(light, true);

    R3D_SetSSAO(true);
    R3D_SetBloomIntensity(0.03f);
    R3D_SetBloomMode(R3D_BLOOM_ADDITIVE);
    R3D_SetTonemapMode(R3D_TONEMAP_ACES);

    /* --- Set background and ambient lighting colors --- */

    R3D_SetBackgroundColor(GRAY);
    R3D_SetAmbientColor((Color) { 7, 7, 7, 255 });

    printf("const char *, ...\n");
    model = R3D_LoadModel("resources/models/bot.glb");
    printf("const char *, ...\n");
    anims = R3D_LoadModelAnimations("resources/models/bot.glb", &animCount, 60, true);
    printf("const char *, ...\n");

    SetTargetFPS(60);
    DisableCursor();
    // Main loop
    int frame = 0;
    float blend = 0.0f;
    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_THIRD_PERSON);

        if (IsKeyDown(KEY_F)) blend += 0.02f;
        if (IsKeyDown(KEY_H)) blend -= 0.02f;

        blend = Clamp(blend, 0.0, 1.0);

        frame = (frame + 1) % anims[3].frameCount;
        Pose modelPose = PoseLerp(R3D_GetAnimPose(anims[1], frame), R3D_GetAnimPose(anims[3], frame), model.boneCount, blend);
        Pose globalPose = PoseToGlobalTransformPose(modelPose, model.bones, model.boneCount);
        Matrix* framePose = PoseToMatrices(globalPose, model.boneCount);

        free(anims[0].framePoses[0]);
        UnloadPose(modelPose);
        UnloadPose(globalPose);

        anims[0].framePoses[0] = framePose;
        model.anim = &anims[0];
        model.animFrame = 0;

        BeginDrawing();
            R3D_Begin(camera);

            R3D_DrawModel(&model, (Vector3) { 0, 0, 0 }, 0.01f);

            R3D_End();

            DrawFPS(10, 10);
            DrawText("Use F and H to increase and decrease blend", 10, 30, 20, DARKGRAY);
            DrawText(TextFormat("Blend: %.2f", blend), 10, 50, 20, DARKGRAY);
        EndDrawing();
    }

    R3D_Close();
    CloseWindow();
    return 0;
}
