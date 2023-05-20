#include "raylib.h"
#include <stdlib.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define MAP_SIZE 256
#define MAP_SCALE 1
#define HEIGHT_SCALE 50

#define NOISE_HEIGHT_SCALE 2.0f
#define NOISE_SPEED 0.6f

Image perlinNoise;
    
float GetHeight(int x, int z) {
    float height = ((Color *)perlinNoise.data)[x + z * perlinNoise.width].r / 255.0f * HEIGHT_SCALE;
    return height;
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib terrain example");

    perlinNoise = GenImagePerlinNoise(MAP_SIZE, MAP_SIZE, 0, 0, NOISE_HEIGHT_SCALE);
    ImageColorTint(&perlinNoise, (Color){ 127, 127, 255, 255 }); // tint for visual effect

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 130.0f, 130.0f, 130.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Generate terrain mesh from perlinNoise Image
    Mesh terrainMesh = GenMeshHeightmap(perlinNoise, (Vector3){ MAP_SIZE, HEIGHT_SCALE, MAP_SIZE });
    Model terrain = LoadModelFromMesh(terrainMesh);

    // Apply perlin noise image as texture
    Texture2D texture = LoadTextureFromImage(perlinNoise);
    SetMaterialTexture(&terrain.materials[0], MATERIAL_MAP_DIFFUSE, texture);

    SetTargetFPS(60);
    DisableCursor(); // hide and lock the cursor

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);

        if (IsKeyDown(KEY_Z)) camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        DrawModel(terrain, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE); // draw the terrain model

        EndMode3D();
        EndDrawing();
    }

    // Unload the mesh and texture
    UnloadTexture(texture);
    UnloadModel(terrain);
    UnloadImage(perlinNoise);
    CloseWindow();
    return 0;
}
