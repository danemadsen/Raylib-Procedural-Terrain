#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define OFFSET_X 560
#define OFFSET_Y -3475
#define IMAGE_SIZE 512 // Image size to see more than one chunk
#define SPEED 4

int offsetX = OFFSET_X;
int offsetY = OFFSET_Y;
float NOISE_HEIGHT_SCALE = 4.0f; // Changed this to be a mutable variable

int main() {
    InitWindow(IMAGE_SIZE, IMAGE_SIZE, "Perlin Noise Debug");

    Image perlinNoise = GenImagePerlinNoise(IMAGE_SIZE, IMAGE_SIZE, offsetX, offsetY, NOISE_HEIGHT_SCALE);
    ImageColorTint(&perlinNoise, (Color){ 127, 127, 255, 255 });
    Texture2D texture = LoadTextureFromImage(perlinNoise);

    int screenshotCounter = 0; // This will be used to generate unique screenshot file names
    bool teleportModeX = false;
    bool teleportModeY = false;
    char teleportBuffer[16] = {0};
    
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // Update
        if(!teleportModeX && !teleportModeY) {
            if(IsKeyDown(KEY_W)) offsetY -= SPEED;
            if(IsKeyDown(KEY_S)) offsetY += SPEED;
            if(IsKeyDown(KEY_A)) offsetX -= SPEED;
            if(IsKeyDown(KEY_D)) offsetX += SPEED;
        }

        if(IsKeyPressed(KEY_KP_ADD)) NOISE_HEIGHT_SCALE /= 1.1f;
        if(IsKeyPressed(KEY_KP_SUBTRACT)) NOISE_HEIGHT_SCALE *= 1.1f;

        // Check for teleport mode
        if(IsKeyPressed(KEY_X)) {
            teleportModeX = !teleportModeX;
            teleportModeY = false;
            memset(teleportBuffer, 0, sizeof(teleportBuffer));
        }
        if(IsKeyPressed(KEY_Y)) {
            teleportModeY = !teleportModeY;
            teleportModeX = false;
            memset(teleportBuffer, 0, sizeof(teleportBuffer));
        }
        if(teleportModeX || teleportModeY) {
            int key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 48 && key <= 57) || key == 45)  // Check if it's a number or '-'
                {
                    int length = strlen(teleportBuffer);
                    if(length < 15) {
                        teleportBuffer[length] = (char)key;
                        teleportBuffer[length+1] = '\0';
                    }
                }
                key = GetCharPressed();
            }
            if(IsKeyPressed(KEY_ENTER)) {
                if(teleportModeX) {
                    errno = 0;
                    long temp = strtol(teleportBuffer, NULL, 10);
                    if (errno == 0) offsetX = (int)temp;
                    teleportModeX = false;
                }
                if(teleportModeY) {
                    errno = 0;
                    long temp = strtol(teleportBuffer, NULL, 10);
                    if (errno == 0) offsetY = (int)temp;
                    teleportModeY = false;
                }
                memset(teleportBuffer, 0, sizeof(teleportBuffer));
            }
        }
        
        // Regenerate the noise if we moved
        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_KP_SUBTRACT)) {
            UnloadTexture(texture);
            UnloadImage(perlinNoise);
            
            perlinNoise = GenImagePerlinNoise(IMAGE_SIZE, IMAGE_SIZE, offsetX, offsetY, NOISE_HEIGHT_SCALE);
            ImageColorTint(&perlinNoise, (Color){ 127, 127, 255, 255 });
            texture = LoadTextureFromImage(perlinNoise);
        }

        if(IsKeyPressed(KEY_P)) {
            char filename[64];
            sprintf(filename, "screenshot_%03d.png", screenshotCounter++);
            TakeScreenshot(filename);
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, WHITE);

        // Draw the coordinates
        char coordText[64];
        sprintf(coordText, "X: %d, Y: %d, Scale: %.2f", offsetX, offsetY, NOISE_HEIGHT_SCALE);
        DrawText(coordText, GetScreenWidth() - MeasureText(coordText, 20) - 10, 10, 20, GREEN);

        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadImage(perlinNoise);
    
    CloseWindow();
    
    return 0;
}