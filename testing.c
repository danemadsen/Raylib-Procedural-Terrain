#include "raylib.h"

#define CHUNK_SIZE 256
#define NOISE_HEIGHT_SCALE 4
#define OFFSET_X -900
#define OFFSET_Y -3500
#define IMAGE_SIZE 2048 // Image size to see more than one chunk

int main() {
    InitWindow(800, 600, "Perlin Noise Debug");

    Image perlinNoise = GenImagePerlinNoise(IMAGE_SIZE, IMAGE_SIZE, OFFSET_X, OFFSET_Y, NOISE_HEIGHT_SCALE);
    ImageColorTint(&perlinNoise, (Color){ 127, 127, 255, 255 });

    ExportImage(perlinNoise, "perlinNoise.png");

    UnloadImage(perlinNoise);
    
    CloseWindow();
    
    return 0;
}
