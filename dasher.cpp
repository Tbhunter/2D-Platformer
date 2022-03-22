#include "raylib.h"

struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

AnimData updateAnimData(AnimData data, float deltaTime, int maxFrame)
{
    //update running time
    data.runningTime += deltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        //update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;

        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    //window dimension
    int windowDimensions[2] = {512, 380};
    InitWindow(windowDimensions[0], windowDimensions[1], "Dapper Dasher");

    //acceleration due to gravity (pixels/sec^2)
    const int gravity{2'000};

    //nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int sizeofNebulae{6};
    AnimData nebulae[sizeofNebulae]{};

    for (int i = 0; i < sizeofNebulae; i++)
    {
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = nebula.width / 8;
        nebulae[i].rec.height = nebula.height / 8;
        nebulae[i].pos.y = windowDimensions[1] - nebula.height / 8;
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0.0;
        nebulae[i].updateTime = 0.0;
        nebulae[i].pos.x = windowDimensions[0] + i * 300;
    }

    float finishLine{nebulae[sizeofNebulae - 1].pos.x};

    //neb x velocity (pix/sec)
    int nebVel{-300};

    //scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData;
    scarfyData.rec.width = scarfy.width / 6;
    scarfyData.rec.height = scarfy.height;
    scarfyData.rec.x = 0;
    scarfyData.rec.y = 0;
    scarfyData.pos.x = windowDimensions[0] / 2 - scarfyData.rec.width / 2;
    scarfyData.pos.y = windowDimensions[1] - scarfyData.rec.height;
    scarfyData.frame = 0;
    scarfyData.updateTime = 1.0 / 12.0;
    scarfyData.runningTime = 0.0;

    int velocity{0};

    //has scarfy jumped
    bool hasJumped{};

    //jump velocity(pix/sec)
    const int jumpVel{-800};

    Texture2D backGround = LoadTexture("textures/far-buildings.png");
    Texture2D midGround = LoadTexture("textures/back-buildings.png");
    Texture2D foreGround = LoadTexture("textures/foreground.png");
    float bgX{};
    float mgX{};
    float fgX{};

    bool collision{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        //delta time - time since last frame
        const float dT{GetFrameTime()};

        //start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        bgX -= 20 * dT;
        mgX -= 40 * dT;
        fgX -= 80 * dT;

        if (bgX <= -backGround.width * 2)
        {
            bgX = 0.0;
        }

        if (mgX <= -midGround.width * 2)
        {
            mgX = 0.0;
        }

        if (fgX <= -foreGround.width * 2)
        {
            fgX = 0.0;
        }

        Vector2 bg1Pos{bgX, 0.0};
        Vector2 bg2Pos{bgX + backGround.width * 2};
        DrawTextureEx(backGround, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(backGround, bg2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        Vector2 mg2Pos{mgX + midGround.width * 2};
        DrawTextureEx(midGround, mg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midGround, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        Vector2 fg2Pos{fgX + foreGround.width * 2};
        DrawTextureEx(foreGround, fg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreGround, fg2Pos, 0.0, 2.0, WHITE);

        if (isOnGround(scarfyData, windowDimensions[1]))
        {
            velocity = 0;
            hasJumped = false;
        }
        else
        {
            velocity += gravity * dT;
        }

        //jump check
        if (IsKeyPressed(KEY_SPACE) && !hasJumped)
        {
            hasJumped = true;
            velocity += jumpVel;
        }

        for (int i = 0; i < sizeofNebulae; i++)
        {
            //update nebula position
            nebulae[i].pos.x += nebVel * dT;
        }

        finishLine += nebVel * dT;

        for (int i = 0; i < sizeofNebulae; i++)
        {
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }
        //update scarfy animation and position
        scarfyData.pos.y += velocity * dT;
        if (!hasJumped)
        {
            //update animation frame
            scarfyData = updateAnimData(scarfyData, dT, 5);
        }

        for (AnimData nebula : nebulae)
        {
            float pad{50};
            Rectangle nebRec{
                nebula.pos.x + pad,
                nebula.pos.y + pad,
                nebula.rec.width - 2 * pad,
                nebula.rec.height - 2 * pad};
            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height};
            if (CheckCollisionRecs(nebRec, scarfyRec))
            {
                collision = true;
            }
        }
        if (collision)
        {
            DrawText("Game Over...", windowDimensions[0] / 4, windowDimensions[1] / 2, 55, RED);
        }
        else if (scarfyData.pos.x >= finishLine)
        {
            DrawText("You Win!", windowDimensions[0] / 2, windowDimensions[1] / 2, 55, YELLOW);
        }

        else
        {

            for (int i = 0; i < sizeofNebulae; i++)
            {
                //draw nebula
                DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
            }
            //draw scarfy
            DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
        }

        //stop drawing
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(backGround);
    UnloadTexture(midGround);
    UnloadTexture(foreGround);
    CloseWindow();
}