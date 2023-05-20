#include "preferences.h"
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>

typedef struct Chunk {
    Vector3 position;
    Model model;
    bool loaded;
    Image perlinNoise;
    Texture2D texture;
} Chunk;

extern Chunk *chunks;
extern int chunksCount;

Vector3 WorldToChunkPosition(Vector3 worldPosition);
void GenerateChunk(int index);
void UnloadChunk(int index);
void UpdateChunks(Vector3 playerPosition);