#include "terrain.h"
#include "preferences.h"
#include "FastNoiselite.h"

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