//UClan ID - G21285889
// HoverCar_G21285889.cpp: A program using the TL-Engine

#include <TL-Engine.h>    // TL-Engine include file and namespace
using namespace tle;

struct vector3D
{
    float x;
    float y;
    float z;
};

struct Vector2D
{
    float x;
    float z;
};

Vector2D scalar(float s, Vector2D v)
{
    return{ s * v.x, s * v.z };
}

Vector2D addThree(Vector2D v1, Vector2D v2, Vector2D v3)
{
    return{ v1.x + v2.x + v3.x,v1.z + v2.z + v3.z };
}
bool sphereToBoxCollision(Vector2D Vmin, Vector2D Vmax, float radius, Vector2D currPos)
{
    return(currPos.x + radius > Vmin.x && currPos.x - radius < Vmax.x && currPos.z + radius> Vmin.z && currPos.z - radius < Vmax.z);
}

float distanceSquared(Vector2D v1, Vector2D v2)
{
    return ((v1.x - v2.x) * (v1.x - v2.x) + (v1.z - v2.z) * (v1.z - v2.z));
}

bool sphereToSphereCollision(Vector2D sphereCentre1, Vector2D sphereCentre2, float sphereRadius1, float sphereRadius2)
{
    return(distanceSquared(sphereCentre1, sphereCentre2) <= (sphereRadius1 + sphereRadius2) * (sphereRadius1 + sphereRadius2));
}

void main()
{
    // Create a 3D engine (using TLX engine here) and open a window for it
    I3DEngine* myEngine = New3DEngine(kTLX);
    myEngine->StartWindowed();

    // Add default folder for meshes and other media
    myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
    myEngine->AddMediaFolder(".\\media");

    const vector3D skyboxCoord = { 0.0f,-960.0f,0.0f };
    const vector3D carCoord = { 0.0f,0.0f,-10.0f };
    const vector3D myCamCoord = { 0.0f,10.0f,-30.0f };

    /**** Set up your scene here ****/
    IMesh* skyboxMesh = myEngine->LoadMesh("Skybox 07.x");
    IModel* skyBoxModel = skyboxMesh->CreateModel(skyboxCoord.x, skyboxCoord.y, skyboxCoord.z);

    IMesh* checkPointMesh = myEngine->LoadMesh("Checkpoint.x");
    const int checkPointSize = 3;
    IModel* checkPoints[checkPointSize];

    //IMesh* gridMesh = myEngine->LoadMesh("Grid.x");
    //IModel* grid = gridMesh->CreateModel(0, 0, 0);

   // ICamera* camera = myEngine->CreateCamera(kManual);
   // camera->SetPosition(0, 50,0);
    //camera->RotateZ(90);
   

    IMesh* isleMesh = myEngine->LoadMesh("IsleStraight.x");
    const int islesSize = 6;
    IModel* isles[islesSize];


    IMesh* wallMesh = myEngine->LoadMesh("Wall.x");
    const int wallSize = 4;
    IModel* walls[wallSize];

    IMesh* carMesh = myEngine->LoadMesh("Race2.x");
    IModel* carModel = carMesh->CreateModel(carCoord.x, carCoord.y, carCoord.z);
    const float carScale = 0.5f;
    const float carRadius = 5.0f * carScale;
    carModel->Scale(carScale);


    IMesh* groundMesh = myEngine->LoadMesh("ground.x");
    IModel* groundModel = groundMesh->CreateModel();


    ICamera* camera = myEngine->CreateCamera(kManual);
    camera->SetPosition(myCamCoord.x, myCamCoord.y, myCamCoord.z);
    camera->AttachToParent(carModel);

    //array for checkpoint positions
    vector3D checkPointsPos[checkPointSize] = { {0.0f,0.0f,0.0f},
                                                {10.0f, 0.0f, 120.0f},
                                                {25.0f, 0.0f, 56.0f}
    };
    //loop to initialize checkpoints
    for (int i = 0; i < checkPointSize; i++)
    {
        checkPoints[i] = checkPointMesh->CreateModel(checkPointsPos[i].x, checkPointsPos[i].y, checkPointsPos[i].z);
    }
    checkPoints[1]->RotateLocalY(135.0f);

    const float checkPointsHalfLine = 7.29081f;
    const float checkPointsStructRadius = 1.28539;

    Vector2D checkPointStrutCentres[checkPointSize * 2] = {
   {checkPointsPos[0].x - checkPointsHalfLine - checkPointsStructRadius, checkPointsPos[0].z},
   {checkPointsPos[0].x + checkPointsHalfLine + checkPointsStructRadius, checkPointsPos[0].z},
   {checkPointsPos[1].x - checkPointsHalfLine - checkPointsStructRadius,checkPointsPos[1].z},
   {checkPointsPos[1].x - checkPointsHalfLine - checkPointsStructRadius,checkPointsPos[1].z},
   {checkPointsPos[2].x - checkPointsHalfLine - checkPointsStructRadius, checkPointsPos[2].z},
   {checkPointsPos[2].x + checkPointsHalfLine + checkPointsStructRadius, checkPointsPos[2].z}
    };

    //array for isles positions
    vector3D islesPos[islesSize] = { {-10.0f,0.0f,40.0f},
                                                {10.0f, 0.0f,40.0f},
                                                {-10.0f, 0.0f, 56.0f},
                                                {10.0f, 0.0f, 56.0f},
                                                {-10.0f, 0.0f, 72.0f},
                                                {10.0f, 0.0f, 72.0f }
    };
    //loop to initialize isles
    for (int i = 0; i < islesSize; i++)
    {
        isles[i] = isleMesh->CreateModel(islesPos[i].x, islesPos[i].y, islesPos[i].z);
    }

    //array for walls positions
    vector3D wallsPos[wallSize] = { {-10.0f,0.0f,48.0f},
                                               {10.0f, 0.0f,48.0f},
                                               {-10.0f, 0.0f, 64.0f},
                                               {10.0f, 0.0f, 64.0f}

    };

    const float isleL = 2.67529f;
    const float isleW = 3.41748f;

    struct Box
    {
        Vector2D Vmin;
        Vector2D Vmax;
    };
    const int numBoxes = 2;
    Box boxes[numBoxes];

    boxes[0] = { {isles[0]->GetX() - isleL,isles[0]->GetZ() - isleW},{isles[4]->GetX() + isleL,isles[4]->GetZ() + isleW} };
    boxes[1] = { {isles[1]->GetX() - isleL,isles[1]->GetZ() - isleW},{isles[5]->GetX() + isleL,isles[5]->GetZ() + isleW} };

    //loop to initialize walls
    for (int i = 0; i < wallSize; i++)
    {
        walls[i] = wallMesh->CreateModel(wallsPos[i].x, wallsPos[i].y, wallsPos[i].z);
    }


    float frameTime = 0.0f;
    const float kGameSpeed = 10.0f;
    float carSpeed = frameTime * kGameSpeed;
    float KRotation = 40.0f;


    float Duration = 0.0f;
    float Timer = 3.0f;

    const float cameraMoveSpeed = 20.0f;
    const float cameraRotateSpeed = 0.1f;
    const vector3D initialPos = myCamCoord;

    ;

    float mouseX = myEngine->GetMouseMovementX();
    float mouseY = myEngine->GetMouseMovementY();

    //camera->RotateY(mouseX * cameraRotateSpeed);
    //camera->RotateX(-mouseY * cameraRotateSpeed);

    enum GameStates
    {
        Intro, countdown, Race, GameOver
    };

    enum RaceStates
    {
        start, CP1, CP2, CP3
    };

    GameStates gameState = Intro;
    RaceStates raceState = start;

    IFont* FontCountGo = myEngine->LoadFont("Times New Roman", 80);
    IFont* FontTimer = myEngine->LoadFont("Arial", 30);
    IFont* FontRaceStart = myEngine->LoadFont("Arial", 30);
    ISprite* mySprite = myEngine->CreateSprite("ui_backdrop.jpg", 0.0f, 660.0f);
    //ISprite* mySprite2 = myEngine->CreateSprite("ui_backdrop.jpg", -350.0f, 20.0f);

    Vector2D momentum = { 0.0f,0.0f };
    Vector2D thrust = { 0.0f,0.0f };
    Vector2D drag = { 0.0f,0.0f };
    Vector2D facingVector = { 0.0f,1.0f };
    Vector2D oldPos = { 0.0f, 0.0f };

    float matrix[4][4];
    const float forwardThrustFactor = 0.05f * kGameSpeed;
    const float backwardThrustFactor = forwardThrustFactor / 2.0f;
    const float coeffDrag = -0.1f * kGameSpeed;
    const EKeyCode fwdThrustKey = Key_W;
    const EKeyCode revThrustKey = Key_S;

    // The main game loop, repeat until engine is stopped
    while (myEngine->IsRunning())
    {
        // Draw the scene
        myEngine->DrawScene();

        frameTime = myEngine->Timer();

        /**** Update your scene each frame here ****/
        if (gameState == Intro)
        {
            FontCountGo->Draw("Press SPACE to start", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            if (myEngine->KeyHit(Key_Space))
            {
                gameState = countdown;
            }
        }

        if (gameState == countdown)
        {
            Timer -= frameTime;
            if (Timer > 2)
            {
                FontCountGo->Draw("3", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > 1)
            {
                FontCountGo->Draw("2", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > 0)
            {
                FontCountGo->Draw("1", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > -0.5f)
            {
                FontCountGo->Draw("GO!", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else
            {
                gameState = Race;
                raceState = start;
            }
        }

        switch (gameState)
        {
        case Intro:
            FontCountGo->Draw("Press SPACE to start", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            if (myEngine->KeyHit(Key_Space))
            {
                gameState = countdown;
            }
            break;

        case countdown:
            Timer -= frameTime;
            if (Timer > 2)
            {
                FontCountGo->Draw("3", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > 1)
            {
                FontCountGo->Draw("2", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > 0)
            {
                FontCountGo->Draw("1", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else if (Timer > -0.5f)
            {
                FontCountGo->Draw("GO!", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
            }
            else
            {
                gameState = Race;
                raceState = start;
            }
            break;

        case Race:
            Duration += frameTime;

            FontTimer->Draw("Time: " + std::to_string(int(Duration * 100) / 100), 0.0f, 660.0f,kRed);

            carModel->GetMatrix(&matrix[0][0]);
            Vector2D curPos = { matrix[3][0],matrix[3][2] };

            if (myEngine->KeyHeld(Key_A))
            {
                carModel->RotateY(-KRotation * frameTime);
            }
            if (myEngine->KeyHeld(Key_D))
            {
                carModel->RotateY(KRotation * frameTime);
            }

            if (myEngine->KeyHeld(Key_W))
            {
                thrust = scalar(-frameTime * forwardThrustFactor, facingVector); // Move forward
            }
            else if (myEngine->KeyHeld(Key_S))
            {
                thrust = scalar(frameTime * backwardThrustFactor, facingVector); // Move backward
            }
            else
            {
                thrust = { 0.0f, 0.0f };
            }

            facingVector = { -matrix[2][0], -matrix[2][2] };

            switch (raceState)
            {
            case start:
            case CP3:
            {
                Vector2D Vmin = { checkPointsPos[0].x - checkPointsHalfLine,checkPointsPos[0].z - checkPointsStructRadius };
                Vector2D Vmax = { checkPointsPos[0].x + checkPointsHalfLine,checkPointsPos[0].z + checkPointsStructRadius };
                if (sphereToBoxCollision(Vmin, Vmax, carRadius, curPos))
                {
                    raceState = CP1;
                }
            }
            break;
            case CP1:
            {
                Vector2D Vmin = { checkPointsPos[1].x - checkPointsHalfLine,checkPointsPos[1].z - checkPointsHalfLine };
                Vector2D Vmax = { checkPointsPos[1].x + checkPointsHalfLine,checkPointsPos[1].z + checkPointsHalfLine };
                if (sphereToBoxCollision(Vmin, Vmax, carRadius, curPos))
                {
                    raceState = CP2;
                }
            }
            break;
            case CP2:
            {
                Vector2D Vmin = { checkPointsPos[2].x - checkPointsHalfLine ,checkPointsPos[2].z - checkPointsStructRadius };
                Vector2D Vmax = { checkPointsPos[2].x + checkPointsHalfLine,checkPointsPos[2].z + checkPointsStructRadius };
                if (sphereToBoxCollision(Vmin, Vmax, carRadius, curPos))
                {
                    raceState = CP3;
                }
            }
            break;
            }


            if (raceState == CP3)
            {
                gameState = GameOver;
            }

            for (int i = 0; i < checkPointSize * 2; i++)
            {
                if (sphereToSphereCollision(checkPointStrutCentres[i], curPos, checkPointsStructRadius, carRadius))
                {
                    carModel->SetX(oldPos.x);
                    carModel->SetZ(oldPos.z);
                    momentum = scalar(-1, momentum);
                }
            }

            for (int i = 0; i < numBoxes; i++)
            {
                if (sphereToBoxCollision(boxes[i].Vmin, boxes[i].Vmax, carRadius, curPos))
                {
                    carModel->SetX(oldPos.x);
                    carModel->SetZ(oldPos.z);
                    momentum = scalar(-1, momentum);
                }
            }

            drag = scalar(coeffDrag * frameTime, momentum); // calculating drag

            momentum = addThree(momentum, thrust, drag); // calculate momentum

            carModel->Move(momentum.x, 0, momentum.z);

            oldPos = curPos;

            switch (raceState)
            {
            case start:
                FontRaceStart->Draw("Game has been started", 200.0f, 660.0f, kBlack);
                break;
            case CP1:
                FontRaceStart->Draw("Stage 1 completed", 200.0f, 660.0f, kBlack);
                break;
            case CP2:
                FontRaceStart->Draw("Stage 2 completed", 200.0f, 660.0f, kBlack);
                break;
            case CP3:
                FontRaceStart->Draw("Stage 3 completed", 200.0f, 660.0f, kBlack);
                break;
            }

        }

        if (gameState == GameOver)
        {
            FontCountGo->Draw("Game Over !!! Press ESC to exit", myEngine->GetWidth() / 2, myEngine->GetHeight() / 2, kRed, kCentre);
        }

        if (myEngine->KeyHit(Key_Escape))
        {
            myEngine->Stop();
        }


        //camera movement
        if (myEngine->KeyHeld(Key_Up))
        {
            camera->MoveLocalZ(cameraMoveSpeed * frameTime);
        }
        if (myEngine->KeyHeld(Key_Down))
        {
            camera->MoveLocalZ(-cameraMoveSpeed * frameTime);
        }
        if (myEngine->KeyHeld(Key_Right))
        {
            camera->MoveLocalX(cameraMoveSpeed * frameTime);
        }
        if (myEngine->KeyHeld(Key_Left))
        {
            camera->MoveLocalX(-cameraMoveSpeed * frameTime);
        }
        if (myEngine->KeyHeld(Key_1))
        {
            camera->DetachFromParent();
            camera->SetPosition(initialPos.x, initialPos.y, initialPos.z);
            camera->AttachToParent(carModel);
        }


    }

    // Delete the 3D engine now we are finished with it
    myEngine->Delete();
}


