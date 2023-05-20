#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define CHUNK_SIZE 256
#define CHUNK_RENDER_DISTANCE 768
#define HEIGHT_SCALE 50

#define NOISE_HEIGHT_SCALE 2.0f
#define NOISE_SPEED 0.6f

#define CAMERA_MOVEMENT_SPEED 30.0f
#define CAMERA_ROTATION_SPEED 1.0f
#define CAMERA_ZOOM 1.0f
#define CAMERA_SPEEDUP_FACTOR 2.0f

typedef struct Chunk {
    Vector3 position;
    Model model;
    bool loaded;
    Image perlinNoise;
    Texture2D texture;
} Chunk;

Chunk *chunks = NULL;
int chunksCount = 0;

Vector3 WorldToChunkPosition(Vector3 worldPosition) {
    return (Vector3){
        floorf(worldPosition.x / CHUNK_SIZE) * CHUNK_SIZE,
        0,
        floorf(worldPosition.z / CHUNK_SIZE) * CHUNK_SIZE
    };
}

void GenerateChunk(int index) {
    Chunk *chunk = &chunks[index];
    
    // Use chunk position as offset for Perlin noise
    int offsetX = chunk->position.x;
    int offsetY = chunk->position.z;


    chunk->perlinNoise = GenImagePerlinNoise(CHUNK_SIZE + 1, CHUNK_SIZE + 1, offsetX, offsetY, NOISE_HEIGHT_SCALE);
    ImageColorTint(&chunk->perlinNoise, (Color){ 127, 127, 255, 255 });

    Mesh mesh = GenMeshHeightmap(chunk->perlinNoise, (Vector3){ CHUNK_SIZE, HEIGHT_SCALE, CHUNK_SIZE });
    chunk->model = LoadModelFromMesh(mesh);
    
    chunk->texture = LoadTextureFromImage(chunk->perlinNoise);
    SetMaterialTexture(&chunk->model.materials[0], MATERIAL_MAP_DIFFUSE, chunk->texture);
    
    chunk->loaded = true;
}

void UnloadChunk(int index) {
    Chunk *chunk = &chunks[index];
    UnloadModel(chunk->model);
    UnloadTexture(chunk->texture);
    UnloadImage(chunk->perlinNoise);
    
    for (int i = index; i < chunksCount - 1; i++)
        chunks[i] = chunks[i + 1];
    
    chunks = realloc(chunks, --chunksCount * sizeof(Chunk));
}

void UpdateChunks(Vector3 playerPosition) {
    Vector3 chunkPosition = WorldToChunkPosition(playerPosition);
    
    for (int i = 0; i < chunksCount; i++) {
        if (Vector3Distance(chunks[i].position, chunkPosition) > CHUNK_RENDER_DISTANCE)
            UnloadChunk(i--);
        else if (!chunks[i].loaded)
            GenerateChunk(i);
    }
    
    for (int z = -CHUNK_RENDER_DISTANCE; z <= CHUNK_RENDER_DISTANCE; z += CHUNK_SIZE) {
        for (int x = -CHUNK_RENDER_DISTANCE; x <= CHUNK_RENDER_DISTANCE; x += CHUNK_SIZE) {
            Vector3 position = Vector3Add(chunkPosition, (Vector3){ x, 0, z });
            bool chunkExists = false;
            
            for (int i = 0; i < chunksCount; i++) {
                if (Vector3Distance(chunks[i].position, position) < CHUNK_SIZE) {
                    chunkExists = true;
                    break;
                }
            }
            
            if (!chunkExists) {
                chunks = realloc(chunks, ++chunksCount * sizeof(Chunk));
                chunks[chunksCount - 1] = (Chunk){
                    .position = position,
                    .model = (Model){ 0 },
                    .loaded = false,
                    .perlinNoise = (Image){ 0 },
                    .texture = (Texture2D){ 0 }
                };
            }
        }
    }
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib terrain example");
    SetTargetFPS(60);
    DisableCursor(); // hide and lock the cursor

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
    
        UpdateChunks(camera.position);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        for (int i = 0; i < chunksCount; i++)
            if (chunks[i].loaded)
                DrawModel(chunks[i].model, chunks[i].position, 1.0f, WHITE);
        
        EndMode3D();
        EndDrawing();
    }

    for (int i = 0; i < chunksCount; i++)
        if (chunks[i].loaded)
            UnloadChunk(i);
    
    free(chunks);
    CloseWindow();
    
    return 0;
}
