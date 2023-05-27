#include "raylib.h"
#include "raymath.h"
#include "stb_perlin.h"
#include <stdlib.h>

typedef struct Chunk {
    Vector3 position;
    Model model;
    Mesh mesh;
    Image perlinNoise;
    bool loaded;
} Chunk;

typedef struct ProceduralTerrain {
    Chunk *chunks;

    int chunksCount;
    int chunkSize;
    int chunkRenderDistance;

    float elevationControl;
    int meshScale;
    float noiseScale;

    int seed;
    int octaves;
    float lacunarity;
    float gain;
} ProceduralTerrain;

void GenChunk(int index, ProceduralTerrain *terrain) {
    Chunk *chunk = &terrain->chunks[index];

    int offsetX = chunk->position.x;
    int offsetY = chunk->position.z;

    // Generate perlin noise image
    int width = terrain->chunkSize + 1;
    int height = terrain->chunkSize + 1;
    chunk->perlinNoise = (Image){ .data = RL_MALLOC(width * height * sizeof(Color)), .width = width, .height = height, .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, .mipmaps = 1 };
    Color *pixels = (Color*)chunk->perlinNoise.data;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = (float)(x + offsetX)*(terrain->noiseScale/(float)width);
            float ny = (float)(y + offsetY)*(terrain->noiseScale/(float)height);
            float p = stb_perlin_fbm_noise3(nx, ny, 1.0f, terrain->lacunarity, terrain->gain, terrain->octaves);

            // Map 'p' to a curve to adjust the terrain elevation distribution
            p = p * (terrain->elevationControl * pow(p, 2) - terrain->elevationControl * p + 1.0f);
            if (p < -1.0f) p = -1.0f;
            if (p > 1.0f) p = 1.0f;

            float np = (p + 1.0f)/2.0f;
    
            int intensity = (int)(np * 255.0f);
            pixels[y * width + x] = (Color){ intensity, intensity, intensity, 255 };
        }
    }

    chunk->mesh = GenMeshHeightmap(chunk->perlinNoise, (Vector3){ terrain->chunkSize, terrain->meshScale, terrain->chunkSize });
    chunk->model = LoadModelFromMesh(chunk->mesh);

    Image image = ImageCopy(chunk->perlinNoise);
    ImageColorTint(&image, (Color){ 0, 128, 0, 255 });
    Texture2D texture = LoadTextureFromImage(image);

    SetMaterialTexture(&chunk->model.materials[0], MATERIAL_MAP_DIFFUSE, texture);
    chunk->loaded = true;
}


void UnloadChunk(int index, ProceduralTerrain *terrain) {
    Chunk *chunk = &terrain->chunks[index];
    if (chunk->loaded) {
        UnloadModel(chunk->model);
        UnloadImage(chunk->perlinNoise);
        chunk->loaded = false;
    }
    
    for (int i = index; i < terrain->chunksCount - 1; i++)
        terrain->chunks[i] = terrain->chunks[i + 1];
    
    terrain->chunks = realloc(terrain->chunks, --terrain->chunksCount * sizeof(Chunk));
}


void UpdateChunks(Vector3 playerPosition, ProceduralTerrain *terrain) {
    Vector3 chunkPosition = {
        (floorf(playerPosition.x / terrain->chunkSize) * terrain->chunkSize),
        0,
        (floorf(playerPosition.z / terrain->chunkSize) * terrain->chunkSize)
    };
    
    for (int i = 0; i < terrain->chunksCount; i++) {
        if (Vector3Distance(terrain->chunks[i].position, chunkPosition) > terrain->chunkRenderDistance)
            UnloadChunk(i--, terrain);
        else if (!terrain->chunks[i].loaded)
            GenChunk(i, terrain);
    }
    
    for (int z = -terrain->chunkRenderDistance; z <= terrain->chunkRenderDistance; z += terrain->chunkSize) {
        for (int x = -terrain->chunkRenderDistance; x <= terrain->chunkRenderDistance; x += terrain->chunkSize) {
            Vector3 position = Vector3Add(chunkPosition, (Vector3){ x, 0, z });
            bool chunkExists = false;
            
            for (int i = 0; i < terrain->chunksCount; i++) {
                if (Vector3Distance(terrain->chunks[i].position, position) < terrain->chunkSize) {
                    chunkExists = true;
                    break;
                }
            }
            
            if (!chunkExists) {
                terrain->chunks = realloc(terrain->chunks, ++terrain->chunksCount * sizeof(Chunk));
                terrain->chunks[terrain->chunksCount - 1] = (Chunk){
                    .position = position,
                    .model = (Model){ { 0 } },
                    .loaded = false,
                };
            }
        }
    }
}