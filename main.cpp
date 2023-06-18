#include "raylib.h"

struct AnimData 
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

// check if the rectangle is on ground
bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

// animation function 
AnimData updateAnimDate(AnimData data, float deltaTime, int maxFrame)
{
    // update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if(data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

// scroll background function 
float scrollBackground(Texture2D layer, float layerX, float deltaTime, int speed)
{
    layerX -= speed * deltaTime;
        if (layerX <= - layer.width * 2)
        {
            layerX = 0.0;
        }
     return layerX;
}

// draw double background
void drawRepeatedTextures(float layerX, Texture2D layer)
{
    Vector2 pic1Pos = {static_cast<float>(layerX), 0.0f };
    DrawTextureEx(layer, pic1Pos, 0.0, 2.0, WHITE);
    Vector2 pic2Pos = {static_cast<float>(layerX) + layer.width*2, 0.0};
    DrawTextureEx(layer, pic2Pos, 0.0, 2.0, WHITE);
}


int main() 
{
    // initialize the window
    int windowDimentions[2];
    windowDimentions[0] = 512; // width
    windowDimentions[1] = 375; // height
    InitWindow(windowDimentions[0], windowDimentions[1], "Dapper Dasher");

    /*
    * TEXTURES LOAD
    */
    // scarfy 
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    // nebula 
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    // backgrounds 
    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};

    // Data for Scarfy 
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width/6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimentions[0]/2 - scarfyData.rec.width/2;
    scarfyData.pos.y = windowDimentions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 9.0; // 9 frames per second
    scarfyData.runningTime = 0.0;

    // Data for nebulae 
    const int sizeOfNebulae = 6;
    AnimData nebulae[sizeOfNebulae]{};

    int nebDistan{500};

    for (int i = 0; i<sizeOfNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].pos.y = windowDimentions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].runningTime = 1.0/16.0;
        nebulae[i].pos.x = windowDimentions[0] + i*nebDistan;
    }

    // nebula X velocity (pixels/sec)
    int velocity = 0;
    int nebulaVel{-200};
    
    // jum velocity (pixels/sec)
    const int jumpVel = -600;

    // acceleration due to gravity (pixels/sec)/sec
    const int gravity = 800;

    // is the rectangle in the air? 
    bool isInAir{};

    bool collision{false};

    float finishLine{ nebulae[sizeOfNebulae-1].pos.x};
    
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        // delta time (time since last frame) 
        const float dT = GetFrameTime();
        
        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // scroll backgrounds 
        bgX = scrollBackground(background, bgX, dT, 20);
        mgX = scrollBackground(midground, mgX, dT, 40);
        fgX = scrollBackground(foreground, fgX, dT, 80);


        // draw a background 
        drawRepeatedTextures(bgX, background);
        drawRepeatedTextures(mgX, midground);
        drawRepeatedTextures(fgX, foreground);

        // perform ground check
        if (isOnGround(scarfyData, windowDimentions[1]))
        {
            // on ground
            velocity = 0;
            isInAir = false;

        }
        else
        {
            // on the air 
            velocity += gravity * dT;
            isInAir = true;
        }

        // jump check
        if (IsKeyPressed(KEY_SPACE) && !isInAir)
        {
            velocity += jumpVel;
        }

        /*
        * POSITIONS 
        */
       // update the position of each nebula
        for (int i=0; i<sizeOfNebulae; i++)
        {
            nebulae[i].pos.x += nebulaVel * dT;
        }

        // update finish line
        finishLine += nebulaVel * dT;

        // update scarfy position
        scarfyData.pos.y += velocity * dT;

        /**
         * ANIMATION
        */
        // update each nebula animation frame
        for (int i=0; i<sizeOfNebulae; i++)
        {
            nebulae[i] = updateAnimDate(nebulae[i], dT, 7);
        }
        // update scarfy animation frame
        if (!isInAir) 
        {
            scarfyData = updateAnimDate(scarfyData, dT, 5);
        }

        /*
        * Game results
        */ 
        for (AnimData nebula: nebulae) // range based for loop 
        {
            float pad{50};
            Rectangle nebRec
            {
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2*pad,
                nebula.rec.height - 2*pad
            };
            Rectangle scarfyRec
            {
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };
            if (CheckCollisionRecs( nebRec, scarfyRec))
            {
                collision = true;
            }
        } 

        // check collition 
        if (collision)
        {
            DrawText("Game Over!", windowDimentions[0]/3, windowDimentions[1]/2,
            30, RED);
        }
        else if (finishLine <= scarfyData.pos.x + scarfyData.rec.width && !isInAir)
        {
            DrawText("You Win!", windowDimentions[0]/3, windowDimentions[1]/2,
                30, RED);
        }
        else 
        {
            //draw nebula
            for (int i=0; i<sizeOfNebulae; i++)
            {
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            // draw scarfy 
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        // stop drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);

    CloseWindow();
}
