#include "preferences.h"
#include "rl_procedural.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Infinite Terrain");
    SetTargetFPS(60);
    DisableCursor();

    ProceduralTerrain terrain = (ProceduralTerrain){
        .chunks = NULL,
        .chunksCount = 0,
        .chunkSize = 64,
        .chunkRenderDistance = 64 * 18,
        .elevationControl = -1.0f,
        .meshScale = 50,
        .noiseScale = 0.5f,
        .seed = 12,
        .octaves = 4,
        .lacunarity = 2.0f,
        .gain = 0.5f
    };

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 1.0f, 30.0f, 1.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector2 oldMousePos = GetMousePosition();
    bool cursorDisabled = true;

    while (!WindowShouldClose()) {
        // Check if F3 key has been pressed
        if (IsKeyPressed(KEY_F3)) {
            cursorDisabled = !cursorDisabled;
    
            // Toggle cursor state
            if (cursorDisabled) {
                DisableCursor();
            } else {
                EnableCursor();
            }
        }

        // Skip mouse updates if cursor is not disabled
        if (cursorDisabled) {
            Vector2 mousePos = GetMousePosition();
            Vector2 mouseDelta = { mousePos.x - oldMousePos.x, mousePos.y - oldMousePos.y };
            oldMousePos = mousePos;

            float movementSpeed = CAMERA_MOVEMENT_SPEED;

            if (IsKeyDown(KEY_LEFT_SHIFT)) movementSpeed *= CAMERA_SPEEDUP_FACTOR;

            Vector3 movement = { 0.0f, 0.0f, 0.0f };
            if (IsKeyDown(KEY_W)) movement.x += movementSpeed * GetFrameTime();
            if (IsKeyDown(KEY_S)) movement.x -= movementSpeed * GetFrameTime();
            if (IsKeyDown(KEY_A)) movement.y -= movementSpeed * GetFrameTime();
            if (IsKeyDown(KEY_D)) movement.y += movementSpeed * GetFrameTime();
            if (IsKeyDown(KEY_SPACE)) movement.z += movementSpeed * GetFrameTime();
            if (IsKeyDown(KEY_LEFT_CONTROL)) movement.z -= movementSpeed * GetFrameTime();

            Vector3 rotation = { mouseDelta.x * CAMERA_ROTATION_SPEED, mouseDelta.y * CAMERA_ROTATION_SPEED, 0.0f };

            UpdateCameraPro(&camera, movement, rotation, 0.0f);

            if (mouseDelta.x == 0 && mouseDelta.y == 0) rotation = (Vector3){ 0.0f, 0.0f, 0.0f };
        }
        
        UpdateChunks(camera.position, &terrain);

        BeginDrawing();
        ClearBackground(SKYBLUE);
        BeginMode3D(camera);

        for (int i = 0; i < terrain.chunksCount; i++) {
            if (terrain.chunks[i].loaded) {          
                DrawModel(terrain.chunks[i].model, terrain.chunks[i].position, 1.0f, WHITE);
            }
        }

        EndMode3D();

        char positionText[64];
        sprintf(positionText, "X: %.2f, Y: %.2f, Z: %.2f", camera.position.x, camera.position.y, camera.position.z);
        DrawText(positionText, 10, 10, 20, BLACK);

        EndDrawing();
    }

    for (int i = terrain.chunksCount - 1; i >= 0; i--) if (terrain.chunks[i].loaded) UnloadChunk(i, &terrain);

    free(terrain.chunks);
    CloseWindow();

    return 0;
}
