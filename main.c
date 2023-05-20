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

    while (!WindowShouldClose()) {
        UpdateCamera(&camera, CAMERA_FREE);
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
