// AGV_Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// ========================================
// 
// Created : 06/03/2025
// Author  : UFTHaq
//
// ========================================


#include "raylib.h"
#include <cmath>
#include <string>
#include <vector>
#include "AGV_Simulator.h"

const std::string AGV_TEXTURE { "Resources/Design/AGV.png" };
const std::string FONT_LOC{ "Resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Medium.ttf" };

Font fontGeneral{};

struct Sensor
{
    Vector2 coor{};
    Vector2 positionsMode{};
    float weight{};
    float coorGap{};
    int value{};
};

std::vector<Sensor> sensors{};

enum SensorMODE
{
    MODE_NORMAL,
    MODE_SPORT
};

SensorMODE sensorMode = MODE_NORMAL;

struct ImageSize
{
    float w{};
    float h{};
};

ImageSize inputFlexibleSize{};
ImageSize outputFlexibleSize{};

Image trackImage{};
Texture2D trackTexture{};

Rectangle flexible_panel_input{};
Rectangle flexible_panel_output{};

ImageSize CalculateFlexibleImage()
{
    ImageSize imageOldSize = { (float)trackImage.width, (float)trackImage.height };

    int new_height = trackImage.height;
    int new_width = int((float(new_height) / imageOldSize.h) * imageOldSize.w);
    //std::cout << "height: " << new_height << "\nwidth: " << new_width << std::endl;
    ImageSize imageNewSize = { (float)new_width, (float)new_height };

    return imageNewSize;
}

Rectangle FlexibleRectangle(Rectangle& BaseRect, float ObjectWidth, float ObjectHeight)
{
    Rectangle flexibleRect{};
    float baseRatio{ BaseRect.width / BaseRect.height };
    float objectRatio{ ObjectWidth / ObjectHeight };

    if (ObjectWidth == ObjectHeight) {
        if (BaseRect.height < BaseRect.width) {
            float h = BaseRect.height;
            float w = h;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - w) / 2,
                w,
                h
            };
        }
        else {
            float w = BaseRect.width;
            float h = w;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - w) / 2,
                w,
                h
            };
        }

    }
    else if (ObjectWidth > ObjectHeight) {

        float w = BaseRect.width;
        float h = ObjectHeight / ObjectWidth * BaseRect.width;
        flexibleRect = {
            BaseRect.x + (BaseRect.width - w) / 2,
            BaseRect.y + (BaseRect.height - h) / 2,
            w,
            h
        };
        objectRatio = flexibleRect.width / flexibleRect.height;

        if (objectRatio < baseRatio) {
            float h = BaseRect.height;
            float w = ObjectWidth / ObjectHeight * BaseRect.height;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - h) / 2,
                w,
                h
            };
        }


    }
    else {
        float h = BaseRect.height;
        float w = ObjectWidth / ObjectHeight * BaseRect.height;
        flexibleRect = {
            BaseRect.x + (BaseRect.width - w) / 2,
            BaseRect.y + (BaseRect.height - h) / 2,
            w,
            h
        };
        objectRatio = flexibleRect.width / flexibleRect.height;

        if (objectRatio > baseRatio) {
            float w = BaseRect.width;
            float h = ObjectHeight / ObjectWidth * BaseRect.width;
            flexibleRect = {
                BaseRect.x + (BaseRect.width - w) / 2,
                BaseRect.y + (BaseRect.height - h) / 2,
                w,
                h
            };
            objectRatio = flexibleRect.width / flexibleRect.height;
        }
    }

    return flexibleRect;
}



class AGV {
public:
    float x, y, angle;
    float length;
    float velocityLeft, velocityRight;

    Image image{};
    Texture2D texture{};

    AGV(float startX, float startY) {
        x = startX;
        y = startY;
        angle = 0.0f;
        length = 40.0f;
        velocityLeft = velocityRight = 0.0f;
    }

    void Update(float dt) {
        float forwardSpeed = (velocityLeft + velocityRight) / 2.0f;
        float angularVelocity = (velocityRight - velocityLeft) / length;

        x += forwardSpeed * cos(angle * DEG2RAD) * dt;
        y += forwardSpeed * sin(angle * DEG2RAD) * dt;
        angle += angularVelocity * dt * RAD2DEG;
    }

    void loadImage(std::string file)
    {
        image = LoadImage(file.c_str());
        texture = LoadTexture(file.c_str());
    }
};

AGV agv(640, 360);

void HandleInput(float dt) {
    float const Speed = 150.0f;
    float const Brake = 5.0f;

    if (IsKeyDown(KEY_UP)) {
        agv.velocityLeft = Speed;
        agv.velocityRight = Speed;
    }
    //else if (IsKeyDown(KEY_DOWN)) {
    //    agv.velocityLeft = -Speed;
    //    agv.velocityRight = -Speed;
    //}
    else {
        agv.velocityLeft *= 0.80f; // Natural slowdown
        agv.velocityRight *= 0.80f;
    }

    float turnSpeed = 50.0F;

    if (IsKeyDown(KEY_RIGHT)) {
        agv.velocityLeft += turnSpeed;
        agv.velocityRight += -turnSpeed;
    }
    else if (IsKeyDown(KEY_LEFT)) {
        agv.velocityLeft += -turnSpeed;
        agv.velocityRight += turnSpeed;
    }
}

void GrabAndRotate(float agvWidth, float agvHeight)
{
    Rectangle agvRect = { agv.x, agv.y, agvWidth, agvHeight };
    Vector2 agvOrigin = { agvWidth / 2,agvHeight / 2 };

    Rectangle grabRect = {
    agvRect.x - agvWidth / 2,
    agvRect.y - agvHeight / 2,
    agvWidth,
    agvHeight
    };

    Color color = RED;
    if (CheckCollisionPointRec(GetMousePosition(), grabRect))
    {
        color = BLUE;
        DrawRectanglePro(agvRect, agvOrigin, agv.angle, color);
    }


    static bool draggingAGV{};
    if (CheckCollisionPointRec(GetMousePosition(), grabRect))
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            draggingAGV = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        draggingAGV = false;
    }

    if (draggingAGV == true)
    {
        agv.x += GetMouseDelta().x;
        agv.y += GetMouseDelta().y;

        color = GREEN;
        DrawRectanglePro(agvRect, agvOrigin, agv.angle, color);
    }
}


enum TextAlign {
    LEFT,
    CENTER,
    RIGHT
};

void DrawTextCustom(Rectangle& panel, std::string text, int align, float size, float space, const Color color)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(fontGeneral, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    float margin = 0.25F;
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * margin,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * margin),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }

    DrawTextEx(fontGeneral, text.c_str(), text_coor, font_size, font_space, color);
}

void DrawTextCustom(Rectangle& panel, std::string text, int align, float size, float space, const Font& font, const Color color)
{
    float font_size = panel.height * size;
    float font_space = space;
    Vector2 text_measure = MeasureTextEx(fontGeneral, text.c_str(), font_size, font_space);
    Vector2 text_coor{};
    float margin = 0.25F;
    if (align == CENTER) {
        text_coor = {
            panel.x + (panel.width - text_measure.x) / 2,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == LEFT) {
        text_coor = {
            panel.x + panel.height * margin,
            panel.y + (panel.height - text_measure.y) / 2
        };
    }
    else if (align == RIGHT) {
        text_coor = {
            panel.x + panel.width - (text_measure.x + panel.height * margin),
            panel.y + (panel.height - text_measure.y) / 2
        };
    }

    DrawTextEx(font, text.c_str(), text_coor, font_size, font_space, color);
}




int main()
{

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(1600, 900, "AGV Simulator");
    SetTargetFPS(60);

    agv.loadImage(AGV_TEXTURE);

    sensors.emplace_back(Sensor{ {}, { 55, 55}, -3, -35 });
    sensors.emplace_back(Sensor{ {}, { 60, 55}, -2, -25 });
    sensors.emplace_back(Sensor{ {}, { 63, 55}, -1, -15 });
    sensors.emplace_back(Sensor{ {}, { 65, 55}, -0, -5 });
    sensors.emplace_back(Sensor{ {}, { 65, 55}, +0, +5 });
    sensors.emplace_back(Sensor{ {}, { 63, 55}, +1, +15 });
    sensors.emplace_back(Sensor{ {}, { 60, 55}, +2, +25 });
    sensors.emplace_back(Sensor{ {}, { 55, 55}, +3, +35 });

    int agvWidth = 90;
    int agvHeight = 90;

    fontGeneral = LoadFontEx(FONT_LOC.c_str(), 60, 0, 0);

    int takeScreenShot{ 0 };
    Image screenImage{};

    agv.x = 175;
    agv.y = 350;

    const Color BACKGROUND = { 30,30,40,255 };

    std::vector<std::string> sectionsControl{ "MAP", "RUN", "SPEED", "PID" };

    bool RUN_SIMULATION = { false };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        HandleInput(dt);
        agv.Update(dt);
        

        Rectangle line = { 400,200,300,20 };
        DrawRectangleRec(line, BLACK);

        BeginDrawing();
        ClearBackground(BACKGROUND);

        // MAIN
        {
            float pad{ 30 };
            Rectangle PanelBase{
                pad,
                pad,
                GetScreenWidth() - (pad * 2),
                GetScreenHeight() - (pad * 2)
            };

            float spacing{ 10 };
            Rectangle MainLeftSection{
                PanelBase.x,
                PanelBase.y,
                PanelBase.width * 0.20F + spacing,
                PanelBase.height
            };

            Rectangle MainRightSection{
                MainLeftSection.x + MainLeftSection.width + spacing,
                MainLeftSection.y,
                PanelBase.width - MainLeftSection.width - spacing,
                MainLeftSection.height
            };

            // MainLeftSection
            {
                Rectangle PanelInputImage{
                    MainLeftSection.x,
                    MainLeftSection.y,
                    MainLeftSection.width,
                    MainLeftSection.height * 0.275F + spacing
                };
                //DrawRectangleLinesEx(PanelInputImage, 0.5F, WHITE);
                DrawRectangleRounded(PanelInputImage, 0.05F, 10, LIGHTGRAY);

                // PanelInputImage
                {
                    // DRAGDROP INFORMATION
                    if (trackTexture.height == 0)
                    {
                        std::string text{ "DRAG DROP YOUR ARENA" };
                        DrawTextCustom(PanelInputImage, text, CENTER, 0.14F, 1.0F, fontGeneral, BLACK);
                    }

                    // DRAGDROP HANDLER
                    if (IsFileDropped()) {
                        FilePathList dropped_file = LoadDroppedFiles();

                        const char* c_file_path = dropped_file.paths[0];
                        std::string cpp_file_path = std::string(c_file_path);

                        if (IsFileExtension(c_file_path, ".png"))
                        {
                            if (trackImage.height != 0) UnloadImage(trackImage);
                            trackImage = LoadImage(c_file_path);

                            if (trackTexture.height != 0) UnloadTexture(trackTexture);
                            //trackTexture = LoadTextureFromImage(trackImage);
                            trackTexture = LoadTexture(c_file_path);

                            inputFlexibleSize = CalculateFlexibleImage();
                            flexible_panel_input = FlexibleRectangle(PanelInputImage, inputFlexibleSize.w, inputFlexibleSize.h);
                        
                            agv.x = 175;
                            agv.y = 350;
                        }

                        takeScreenShot = 2;

                        // agar ketika run, langsung bisa simulasi.

                        UnloadDroppedFiles(dropped_file);
                    }

                    if (trackTexture.height != 0) 
                    {
                        // Draw input
                        {
                            Rectangle source{
                                0,0,(float)trackTexture.width, (float)trackTexture.height
                            };
                            Rectangle dest{ flexible_panel_input };

                            pad = 5.0F;
                            dest = {
                                dest.x + (pad * 1),
                                dest.y + (pad * 1),
                                dest.width - (pad * 2),
                                dest.height - (pad * 2)
                            };
                            DrawTexturePro(trackTexture, source, dest, { 0,0 }, 0, WHITE);
                            //DrawRectangleLinesEx(dest, 1.5F, GRAY);
                        }
                    }
                }

                // PANEL AGV BASE
                Rectangle AGV_Base{
                    PanelInputImage.x,
                    PanelInputImage.y + PanelInputImage.height + spacing,
                    PanelInputImage.width,
                    MainLeftSection.height * 0.15F + spacing
                };
                DrawRectangleRounded(AGV_Base, 0.1F, 10, LIGHTGRAY);

                // PANEL SETUP
                Rectangle SetupSection{
                    PanelInputImage.x,
                    AGV_Base.y + AGV_Base.height + spacing,
                    PanelInputImage.width,
                    MainLeftSection.height - PanelInputImage.height - AGV_Base.height - spacing - spacing
                };
                DrawRectangleRounded(SetupSection, 0.05F, 10, LIGHTGRAY);

                static bool RUN = { false };

                {
                    //size_t sections = 
                }

            }


            // MainRightSection
            {
                Rectangle PanelOutputImage{
                    (float)(int)MainRightSection.x,
                    (float)(int)MainRightSection.y,
                    (float)(int)PanelBase.width - MainLeftSection.width - spacing,
                    (float)(int)MainRightSection.height * 0.9F + spacing,
                };
                //DrawRectangleLinesEx(PanelOutputImage, 0.5F, WHITE);
                //DrawRectangleRounded(PanelOutputImage, 0.025F, 10, p->ColorPanel);
                DrawRectangleRounded(PanelOutputImage, 0.025F, 10, LIGHTGRAY);

                outputFlexibleSize = CalculateFlexibleImage();
                flexible_panel_output = FlexibleRectangle(PanelOutputImage, outputFlexibleSize.w, outputFlexibleSize.h);

                if (trackTexture.height != 0)
                {
                    // Draw Arena
                    {
                        Rectangle source{
                            0,0,(float)trackTexture.width, (float)trackTexture.height
                        };
                        Rectangle dest{ flexible_panel_output };

                        pad = 5.0F;
                        dest = {
                            dest.x + (pad * 1),
                            dest.y + (pad * 1),
                            dest.width - (pad * 2),
                            dest.height - (pad * 2)
                        };
                        DrawTexturePro(trackTexture, source, dest, { 0,0 }, 0, WHITE);
                        //DrawRectangleLinesEx(dest, 1.5F, GRAY);
                    }
                }

                // Footer
                Rectangle FooterSection{
                    PanelOutputImage.x,
                    PanelOutputImage.y + PanelOutputImage.height + spacing,
                    PanelOutputImage.width,
                    MainRightSection.height - PanelOutputImage.height - spacing
                };
                DrawRectangleRounded(FooterSection, 0.3F, 10, LIGHTGRAY);

                // RUN BUTTON
                Rectangle runButton{
                    FooterSection.x + spacing,
                    FooterSection.y + spacing,
                    FooterSection.width * 0.1F,
                    FooterSection.height - (spacing * 2)
                };

                Color runButtonColor = BACKGROUND;
                std::string runButtonText{ "RUN" };
                if (CheckCollisionPointRec(GetMousePosition(), runButton))
                {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {
                        RUN_SIMULATION = !RUN_SIMULATION;
                    }
                }

                if (RUN_SIMULATION)
                {
                    runButtonColor = RED;
                    runButtonText = "STOP";
                }

                DrawRectangleRounded(runButton, 0.25F, 10, runButtonColor);
                DrawTextCustom(runButton, runButtonText, CENTER, 0.9F, 1, WHITE);


                // 8 BIT SENSOR DISPLAY
                float sensorDisplayW = 0.4F * FooterSection.width;
                Rectangle sensorDisplay{
                    FooterSection.x + ((FooterSection.width - sensorDisplayW) / 2),
                    FooterSection.y,
                    sensorDisplayW,
                    FooterSection.height
                };

                {
                    for (size_t i = 0; i < sensors.size(); i++)
                    {
                        Rectangle sensorDisplayBit{
                            sensorDisplay.x + (i * sensorDisplay.width / sensors.size()),
                            sensorDisplay.y,
                            sensorDisplay.width / sensors.size(),
                            sensorDisplay.height
                        };

                        float pad = spacing;
                        sensorDisplayBit = {
                            sensorDisplayBit.x + pad,
                            sensorDisplayBit.y + pad,
                            sensorDisplayBit.width - (pad * 2),
                            sensorDisplayBit.height - (pad * 2)
                        };

                        DrawRectangleRounded(sensorDisplayBit, 0.25, 10, BACKGROUND);
                        std::string text = { std::to_string(sensors.at(i).value) };
                        DrawTextCustom(sensorDisplayBit, text, CENTER, 1, 1, WHITE);
                    }
                }


                if (RUN_SIMULATION)
                {
                    float Speed = 150.0F;

                    float sumWeight{};
                    float positiveSensors{};

                    for (auto& sensor : sensors)
                    {
                        if (sensor.value == 1)
                        {
                            sumWeight += sensor.weight;
                            positiveSensors++;
                        }
                    }


                    float error{ sumWeight / positiveSensors };

                    // PID
                    float dt = GetFrameTime();

                    float Kp = { 19 };
                    float Ki = { 0.1F };
                    float Kd = { 1.0F };

                    float integral = error * dt;
                    static float prevError = 0;
                    float derivative = (error - prevError) / dt;
                    prevError = error;

                    float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);

                    if (positiveSensors == 0)
                    {
                        RUN_SIMULATION = false;
                        prevError = 0;
                    }
                    if (RUN_SIMULATION)
                    {
                        agv.velocityLeft = Speed - correction;
                        agv.velocityRight = Speed + correction;
                    }
                }

            }
        }

        //// TITLE
        //{
        //    Vector2 textCoor{};
        //    float fontSize = 60.0F;
        //    float fontSpace = 1.0F;
        //    Color color = WHITE;

        //    DrawTextEx(fontGeneral, "AGV SIMULATOR", textCoor, fontSize, fontSpace, color);
        //}



        Rectangle agvRect = { agv.x, agv.y, agvWidth, agvHeight };
        Vector2 agvOrigin = { agvWidth / 2,agvHeight / 2 };

        GrabAndRotate(agvWidth, agvHeight);

        DrawTexturePro(agv.texture, { 0,0,378,378 }, agvRect, agvOrigin, agv.angle, WHITE);

        {
            //Vector2 front{};
            //float value = 80;
            //front.x = (agvRect.x) + (cos(agv.angle * DEG2RAD) * value);
            //front.y = (agvRect.y) + (sin(agv.angle * DEG2RAD) * value);

            //DrawCircleV(front, 5, WHITE);
        }

        //Vector2 sensorArea{};
        //float valueSensorArea = 60;
        //sensorArea.x = (agvRect.x) + (cos(agv.angle * DEG2RAD) * valueSensorArea);
        //sensorArea.y = (agvRect.y) + (sin(agv.angle * DEG2RAD) * valueSensorArea);

        for (Sensor& sensor : sensors)
        {
            Vector2 sensorArea{};
            float valueSensorArea = 60;

            if (sensorMode == MODE_NORMAL)
            {
                valueSensorArea = sensor.positionsMode.y;
            }
            else
            {
                valueSensorArea = sensor.positionsMode.x;
            }


            sensorArea.x = (agvRect.x) + (cos(agv.angle * DEG2RAD) * valueSensorArea);
            sensorArea.y = (agvRect.y) + (sin(agv.angle * DEG2RAD) * valueSensorArea);

            float sensorOffset = sensor.coorGap;
            sensor.coor.x = (sensorArea.x) + (cos((agv.angle + 90) * DEG2RAD) * sensorOffset);
            sensor.coor.y = (sensorArea.y) + (sin((agv.angle + 90) * DEG2RAD) * sensorOffset);


            Color circleColor = BLACK;
            DrawCircleLinesV(sensor.coor, 4, circleColor);

            //if (CheckCollisionCircleRec(sensor.coor, 4, line))
            //{
            //    circleColor = WHITE;
            //    DrawCircleV(sensor.coor, 3, circleColor);
            //}

            Color lineMagnetic = { 18,18,18,255 };
            //Color sensorRead = {};

            if (screenImage.height != 0)
            {
                bool detected = false;

                int numSamples = 8;
                float radius = 3;

                for (int i = 0; i < numSamples; i++)
                {
                    float angle = (i * 360.0f / numSamples) * DEG2RAD;
                    int sampleX = (int)(sensor.coor.x + cos(angle) * radius);
                    int sampleY = (int)(sensor.coor.y + sin(angle) * radius);

                    if (sampleX >= 0 && sampleX < screenImage.width &&
                        sampleY >= 0 && sampleY < screenImage.height)
                    {
                        Color sensorRead = GetImageColor(screenImage, sampleX, sampleY);
                        if (sensorRead.r <= lineMagnetic.r &&
                            sensorRead.g <= lineMagnetic.g &&
                            sensorRead.b <= lineMagnetic.b &&
                            sensorRead.a == lineMagnetic.a)
                        {
                            detected = true;
                            break;  // Stop checking if touch black
                        }
                    }
                }

                if (detected)
                {
                    circleColor = WHITE;
                    DrawCircleV(sensor.coor, 4, circleColor);
                    sensor.value = 1;
                }
                else {
                    sensor.value = 0;
                }
            }

        }


        EndDrawing();


        if (takeScreenShot > 0)
        {
            // harus langsung reload untuk mengambil dari 
            UnloadImage(screenImage);
            screenImage = LoadImageFromScreen();

            takeScreenShot--;
        }

    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
