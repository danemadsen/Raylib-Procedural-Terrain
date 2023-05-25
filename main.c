#include "preferences.h"
#include "terrain.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raylib Infinite Terrain");
    SetTargetFPS(60);
    DisableCursor(); // hide and lock the cursor

    ProceduralTerrain terrain = { 0 };

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 130.0f, 130.0f, 130.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector2 oldMousePos = GetMousePosition(); // Initialize with the current mouse position to avoid initial jump

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        Vector2 mouseDelta = { mousePos.x - oldMousePos.x, mousePos.y - oldMousePos.y };
        oldMousePos = mousePos;

        float movementSpeed = CAMERA_MOVEMENT_SPEED;

        if (IsKeyDown(KEY_LEFT_SHIFT)) movementSpeed *= CAMERA_SPEEDUP_FACTOR;

        // Calculate the movement vector
        Vector3 movement = { 0.0f, 0.0f, 0.0f };
        if (IsKeyDown(KEY_W)) movement.x += movementSpeed * GetFrameTime(); // Updated axis to match UpdateCameraPro()
        if (IsKeyDown(KEY_S)) movement.x -= movementSpeed * GetFrameTime(); // Updated axis
        if (IsKeyDown(KEY_A)) movement.y -= movementSpeed * GetFrameTime(); // Updated axis
        if (IsKeyDown(KEY_D)) movement.y += movementSpeed * GetFrameTime(); // Updated axis
        if (IsKeyDown(KEY_SPACE)) movement.z += movementSpeed * GetFrameTime();
        if (IsKeyDown(KEY_LEFT_CONTROL)) movement.z -= movementSpeed * GetFrameTime();

        // Calculate the rotation vector
        Vector3 rotation = { mouseDelta.x * CAMERA_ROTATION_SPEED, mouseDelta.y * CAMERA_ROTATION_SPEED, 0.0f };

        // Update the camera with the calculated vectors
        UpdateCameraPro(&camera, movement, rotation, 0.0f);

        // Reset the rotation vector if the mouse is not moving
        if (mouseDelta.x == 0 && mouseDelta.y == 0) rotation = (Vector3){ 0.0f, 0.0f, 0.0f };
    
        UpdateChunks(camera.position, &terrain);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        for (int i = 0; i < terrain.chunksCount; i++)
            if (terrain.chunks[i].loaded)
                DrawModel(terrain.chunks[i].model, terrain.chunks[i].position, 1.0f, WHITE);
        
        EndMode3D();

         // Draw player coordinates
        char positionText[64];
        sprintf(positionText, "X: %.2f, Y: %.2f, Z: %.2f", camera.position.x, camera.position.y, camera.position.z);
        DrawText(positionText, 10, 10, 20, BLACK);

        EndDrawing();
    }

    for (int i = 0; i < terrain.chunksCount; i++)
        if (terrain.chunks[i].loaded)
            UnloadChunk(i, &terrain);
    
    free(terrain.chunks);
    CloseWindow();
    
    return 0;
}
