#include "raylib.h"
#include "iostream"

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isOnGround(AnimData data,int windowHeight){
    return data.pos.y>=windowHeight-data.rec.height;
}

AnimData updateAnimData(AnimData data,float deltaTime,int maxFrame){
    //Update running time
    data.runningTime+=deltaTime;

    if(data.runningTime>=data.updateTime){
        data.runningTime=0.0;
        //Update animation frame
        data.rec.x=data.frame*data.rec.width;
        data.frame++;
        if(data.frame>maxFrame){
            data.frame=0;
        }
    }

    return data;
}

int main(){

    int windowDiminsions[2];
    windowDiminsions[0]=512;
    windowDiminsions[1]=380;

    
    //Initialize the window
    InitWindow(windowDiminsions[0],windowDiminsions[1],"Dapper Dasher!");

    //Acceleration due to gravity (pixels/sec)/sec
    const int gravity{1'000};

    //Nebula variables
    Texture2D nebula=LoadTexture("./textures/12_nebula_spritesheet.png");

    const int sizeOfNebulae{6}; 
    AnimData nebulae[sizeOfNebulae]{};

    for(int i=0;i<sizeOfNebulae;i++){
        nebulae[i].rec.x=0.0;
        nebulae[i].rec.y=0.0;
        nebulae[i].rec.width=nebula.width/8;
        nebulae[i].rec.height=nebula.height/8;
        nebulae[i].pos.x=windowDiminsions[0]+i*300;
        nebulae[i].pos.y=windowDiminsions[1]-nebula.height/8;
        nebulae[i].frame=0;
        nebulae[i].runningTime=0.0;
        nebulae[i].updateTime=0.0;
    }

    //Finish line
    float finishLine{nebulae[sizeOfNebulae-1].pos.x+300};

    //Nebula X velocity(pixels/sec)
    int nebVel{-200};

    //Scarfy variables
    Texture2D scarfy=LoadTexture("./textures/scarfy.png");

    AnimData scarfyData;

    scarfyData.rec.width=scarfy.width/6;
    scarfyData.rec.height=scarfy.height;
    scarfyData.rec.x=0;
    scarfyData.rec.y=0;
    scarfyData.pos.x=windowDiminsions[0]/2-scarfyData.rec.width/2;
    scarfyData.pos.y=windowDiminsions[1]-scarfyData.rec.height;
    scarfyData.frame=0;
    scarfyData.updateTime=1.0/12.0;
    scarfyData.runningTime=0.0;

   


    //Jump velocity(pixels/sec)
    const int jumpVal{-600};
    bool isInAir{};

    
    int velocity{0};

    //Load background textures
    Texture2D background=LoadTexture("./textures/far-buildings.png");
    float bgX{};
    Texture2D midground=LoadTexture("./textures/back-buildings.png");
    float mgX{};
    Texture2D foreground=LoadTexture("./textures/foreground.png");
    float fgX{};

    //Collision detection variable
    bool collision{false};
    
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        //Delta Time(Time since the last frame)
        const float dT{GetFrameTime()};

        

        //Start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        //Scroll background to left
        bgX-=100*dT;
        if(bgX<=-background.width*2){
            bgX=0.0;
        }

        //Scroll midground to left
        mgX-=40*dT;
        if(mgX<=-midground.width*2){
            mgX=0.0;
        }

        //Scroll foreground to left
        fgX-=80*dT;
        if(fgX<=-foreground.width*2){
            fgX=0.0;
        }

        //Draw background
        Vector2 bg1Pos={bgX,0.0};
        DrawTextureEx(background,bg1Pos,0.0,2.0,WHITE);
        Vector2 bg2Pos{bgX+background.width*2,0.0};
        DrawTextureEx(background,bg2Pos,0.0,2.0,WHITE);

        //Draw midground
        Vector2 mg1Pos{mgX,0.0};
        DrawTextureEx(midground,mg1Pos,0.0,2.0,WHITE);
        Vector2 mg2Pos{mgX+midground.width*2,0.0};
        DrawTextureEx(midground,mg2Pos,0.0,2.0,WHITE);

        //Draw foreground
        Vector2 fg1Pos{fgX,0.0};
        DrawTextureEx(foreground,fg1Pos,0.0,2.0,WHITE);
        Vector2 fg2Pos{fgX+foreground.width*2,0.0};
        DrawTextureEx(foreground,fg2Pos,0.0,2.0,WHITE);

        //Ground check
        if(isOnGround(scarfyData,windowDiminsions[1])){
            velocity=0;
            isInAir=false;
        }else{
            //Rectangle is in the air
            velocity+=gravity*dT;
            isInAir=true;
        }
        
        //Check for jumping
        if(IsKeyPressed(KEY_SPACE) && !isInAir){
            velocity+=jumpVal;
        }

        //Update nebulae positions
        for(int i=0;i<sizeOfNebulae;i++){
            nebulae[i].pos.x+=nebVel*dT;
        }
        
        //move finish line
        finishLine+=nebVel*dT;

        //Update scarfy position
        scarfyData.pos.y+=velocity*dT;


        //Update scary's animation frame
        if(!isInAir){
            scarfyData=updateAnimData(scarfyData,dT,5);
        }
        
        //Update nebulae animations
        for(int i=0;i<sizeOfNebulae;i++){  
            nebulae[i]=updateAnimData(nebulae[i],dT,7);
        }

        //Collision detection
        
        for(AnimData nebula: nebulae){
            int pad{50};
            Rectangle nebRec{
                nebula.pos.x+pad,
                nebula.pos.y+pad,
                nebula.rec.width-pad*2,
                nebula.rec.height-pad*2
            };

            Rectangle scarfyRec{
                scarfyData.pos.x,
                scarfyData.pos.y,
                scarfyData.rec.width,
                scarfyData.rec.height
            };

            if(CheckCollisionRecs(nebRec,scarfyRec)){
                collision=true;
            }
        }
     
        //Draw textures
        if(collision){
            //Lose the game
            DrawText("Game Over!",windowDiminsions[0]/4,windowDiminsions[1]/2,50,BLUE);
        }else if(scarfyData.pos.x>finishLine){
            //Win the game
            DrawText("You Win!",windowDiminsions[0]/4,windowDiminsions[1]/2,50,RED);
        }   
        else{
            //Draw nebulae textures
            for(int i=0;i<sizeOfNebulae;i++){
                DrawTextureRec(nebula,nebulae[i].rec,nebulae[i].pos,WHITE);
            }
            //Draw scarfy
            DrawTextureRec(scarfy,scarfyData.rec,scarfyData.pos,WHITE);
        }
        

        

        

        //Stop drawing
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

    return 0;
}