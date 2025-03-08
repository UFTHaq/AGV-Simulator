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
#include <format>
#include "AGV_Simulator.h"

const std::string AGV_TEXTURE { "Resources/Design/AGV.png" };
const std::string FONT_LOC{ "Resources/Fonts/Sofia_Sans_Condensed/static/SofiaSansCondensed-Medium.ttf" };
const std::string MAP_A_LOC{ "Resources/Design/Track1.png" };
const std::string MAP_B_LOC{ "Resources/Design/Track2.png" };

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
    float speed{180.0F};

    float Kp{ 20.0F };
    float Ki{ 0.1F };
    float Kd{ 1.0F };

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
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    }

    float getSpeed() const
    {
        return speed;
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

struct ButtonMap {
    std::string text{};
    bool isChoosen{};
    bool isHover{};
    std::string mapLoc{};
    const Color colorPressed{ 79, 100, 166, 255 };
    const Color colorNormal{ 180, 180, 180, 255 };

    ButtonMap
    (
        const std::string text, bool isChoosen, const std::string mapLoc
    ) :
        text(text), isChoosen(isChoosen), mapLoc(mapLoc) 
    {

    };

    void chooseThisButton()
    {
        isChoosen = true;
    }

    void resetChoosen()
    {
        isChoosen = false;
    }

    std::string getDisplay() const
    {
        return text;
    }

    Color getColorButton() const {
        if (isChoosen || isHover) return colorPressed;
        else return colorNormal;
    }

    Color getColorText() const {
        return isChoosen ? WHITE : BLACK;
    }
};

std::string simulationMAP{};

struct ButtonMode {
    std::string text{};
    bool isChoosen{};
    SensorMODE sensorMode{};
    bool isHover{};
    const Color colorPressed{ 79, 100, 166, 255 };
    const Color colorNormal{ 180, 180, 180, 255 };

    ButtonMode
    (
        const std::string text, bool isChoosen, SensorMODE sensorMode
    ) :
        text(text), isChoosen(isChoosen), sensorMode(sensorMode) {};

    void chooseThisButton()
    {
        isChoosen = true;
    }

    void resetChoosen()
    {
        isChoosen = false;
    }

    std::string getDisplay() const
    {
        return text;
    }

    SensorMODE getMode() const
    {
        return sensorMode;
    }

    Color getColorButton() const {
        if (isChoosen || isHover) return colorPressed;
        else return colorNormal;
    }

    Color getColorText() const {
        return isChoosen ? WHITE : BLACK;
    }
};

struct SliderInput {
    Rectangle area{};
    float value{};
    float minValue{};
    float maxValue{};
    bool dragging{};

    Rectangle minValRect{};
    Rectangle sliderBarRect{};
    Rectangle maxValRect{};

    SliderInput(Rectangle area, float initialValue, float minValue, float maxValue, bool dragging) :
        area(area), value(initialValue), minValue(minValue), maxValue(maxValue), dragging(dragging)
    {
        float smallRectWidthCoef = 0.235F;
        float spaceRect = 0.04F;

        minValRect = {
            area.x,
            area.y,
            area.width * smallRectWidthCoef,
            area.height
        };

        sliderBarRect = {
            minValRect.x + minValRect.width + (area.width * spaceRect),
            area.y,
            area.width * (1.0F - (2 * (smallRectWidthCoef + spaceRect))),
            area.height
        };

        maxValRect = {
            sliderBarRect.x + sliderBarRect.width + (area.width * spaceRect),
            area.y,
            area.width * smallRectWidthCoef,
            area.height
        };

        float pad = 2;
        float topPadding = 2.5;

        minValRect = {
            minValRect.x + (pad * 2),
            minValRect.y + (pad * topPadding),
            minValRect.width - (pad * 2 * 2),
            minValRect.height - (pad * topPadding * 2),
        };

        sliderBarRect = {
            sliderBarRect.x + (pad * 2),
            sliderBarRect.y + (pad * topPadding),
            sliderBarRect.width - (pad * 2 * 2),
            sliderBarRect.height - (pad * topPadding * 2),
        };

        maxValRect = {
            maxValRect.x + (pad * 2),
            maxValRect.y + (pad * topPadding),
            maxValRect.width - (pad * 2 * 2),
            maxValRect.height - (pad * topPadding * 2),
        };
    };



    void Draw() {
        float fontSize = 0.8F;
        std::string valueStr{};

        //DrawRectangleRoundedLines(minValRect, 0.2F, 10, 0.5F, WHITE);
        //DrawRectangleRounded(minValRect, 0.2F, 10, BLACK);
        valueStr = std::format("{:.2f}", minValue);
        DrawTextCustom(minValRect, valueStr, 1, fontSize, 0, BLACK);

        //DrawRectangleRoundedLines(maxValRect, 0.2F, 10, 0.5F, WHITE);
        //DrawRectangleRounded(maxValRect, 0.2F, 10, BLACK);
        valueStr = std::format("{:.0f}", maxValue);
        DrawTextCustom(maxValRect, valueStr, 1, fontSize, 0, BLACK);

        float handleX = sliderBarRect.x + ((value - minValue) / (maxValue - minValue)) * sliderBarRect.width;
        float handleW = 12;
        Rectangle handleRect{
            handleX - (handleW * 0.5F),
            sliderBarRect.y,
            handleW,
            sliderBarRect.height
        };
        const Color color{ 79, 100, 166, 255 };

        Rectangle SliderBarRectWithPad{
            sliderBarRect.x - (handleW * 0.5F),
            sliderBarRect.y,
            sliderBarRect.width + (handleW * 1),
            sliderBarRect.height
        };
        //DrawRectangleRoundedLines(SliderBarRectWithPad, 0.0F, 10, 0.5F, WHITE);
        DrawRectangleRounded(SliderBarRectWithPad, 0.2F, 10, { 25,32,45,255 });
        DrawRectangleRounded(handleRect, 0.2F, 10, color);

        valueStr = std::format("{:.2f}", value);
        DrawTextCustom(SliderBarRectWithPad, valueStr, 1, fontSize, 0, WHITE);
    }


    void Update() {
        Vector2 mousePos = GetMousePosition();

        if (CheckCollisionPointRec(mousePos, sliderBarRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragging = true;
        }

        if (dragging) {
            if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                dragging = false;
            }
            else {
                float normalizedValue = (mousePos.x - sliderBarRect.x) / sliderBarRect.width;
                value = minValue + normalizedValue * (maxValue - minValue);

            }
        }

        if (CheckCollisionPointRec(mousePos, sliderBarRect)) {
            float wheelDelta = GetMouseWheelMove();

            int totalSteps = 20;
            float factor = (maxValue - minValue) / totalSteps;

            value += wheelDelta * factor;
        }

        if (value < minValue) value = minValue;
        if (value > maxValue) value = maxValue;
    }

    void Run() {
        Update();
        Draw();
    }

    float GetValue() const {
        return value;
    }

};

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

    fontGeneral = LoadFontEx(FONT_LOC.c_str(), 80, 0, 0);
    SetTextureFilter(fontGeneral.texture, TEXTURE_FILTER_BILINEAR);

    int takeScreenShot{ 0 };
    Image screenImage{};

    agv.x = 175;
    agv.y = 350;

    const Color BACKGROUND = { 30,30,40,255 };

    std::vector<std::string> parameterControls{ "MAP", "MODE" };
    std::vector<ButtonMap> argumentMap{ {"A", 0, MAP_A_LOC}, {"B", 0, MAP_B_LOC} };
    std::vector<ButtonMode> argumentMode{ {"NORMAL", 1, MODE_NORMAL}, {"SPORT", 0, MODE_SPORT} };

    std::vector<std::string> parameterPID{ "SPEED", "Kp", "Ki", "Kd" };

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
                            trackTexture = LoadTexture(c_file_path);

                            SetTextureFilter(trackTexture, TEXTURE_FILTER_BILINEAR);

                            inputFlexibleSize = CalculateFlexibleImage();
                            flexible_panel_input = FlexibleRectangle(PanelInputImage, inputFlexibleSize.w, inputFlexibleSize.h);
                        
                            resetAGVPosition();

                            for (auto& map : argumentMap)
                            {
                                map.resetChoosen();
                            }
                        }

                        takeScreenShot = 2;

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
                    MainLeftSection.height * 0.155F + spacing
                };
                DrawRectangleRounded(SetupSection, 0.05F, 10, LIGHTGRAY);

                // PANEL PID & Speed
                Rectangle PIDSection{
                    PanelInputImage.x,
                    SetupSection.y + SetupSection.height + spacing,
                    PanelInputImage.width,
                    MainLeftSection.height - PanelInputImage.height - AGV_Base.height - SetupSection.height - (spacing * 3)
                };
                DrawRectangleRounded(PIDSection, 0.05F, 10, LIGHTGRAY);


                {
                    // PANEL CONTROLS

                    float pad = 5;
                    SetupSection = {
                        SetupSection.x,
                        SetupSection.y + (pad * 1),
                        SetupSection.width,
                        SetupSection.height - (pad * 2)
                    };

                    Rectangle PanelParameter{
                        SetupSection.x,
                        SetupSection.y,
                        SetupSection.width * 0.30F + spacing,
                        SetupSection.height
                    };

                    Rectangle PanelArgument{
                        PanelParameter.x + PanelParameter.width + spacing,
                        PanelParameter.y,
                        (SetupSection.width - PanelParameter.width - spacing) * 0.975F,
                        PanelParameter.height
                    };

                    size_t parameterCounts = parameterControls.size();

                    for (size_t i = 0; i < parameterCounts; i++)
                    {
                        {
                            Rectangle parameter{
                                PanelParameter.x,
                                PanelParameter.y + (i * PanelParameter.height / parameterCounts),
                                PanelParameter.width,
                                PanelParameter.height / parameterCounts
                            };

                            std::string text = parameterControls.at(i);
                            DrawTextCustom(parameter, text, LEFT, 0.6F, 1.0F, fontGeneral, BLACK);
                        }

                        {
                            Rectangle argument{
                                PanelArgument.x,
                                PanelArgument.y + (i * PanelArgument.height / parameterCounts),
                                PanelArgument.width,
                                PanelArgument.height / parameterCounts
                            };

                            float buttonPad = 5;
                            float buttonPadFactor = 1.25F;

                            if (parameterControls.at(i) == "MAP")
                            {
                                size_t buttonCounts = argumentMap.size();
                                for (size_t j = 0; j < buttonCounts; j++)
                                {
                                    auto& theButton = argumentMap.at(j);

                                    Rectangle buttonBase{
                                        argument.x + (j * argument.width / buttonCounts),
                                        argument.y,
                                        argument.width / buttonCounts,
                                        argument.height
                                    };

                                    Rectangle ButtonRect{
                                        buttonBase.x + (buttonPad * 1),
                                        buttonBase.y + (buttonPad * buttonPadFactor),
                                        buttonBase.width - (buttonPad * 2),
                                        buttonBase.height - (buttonPad * 2 * buttonPadFactor)
                                    };

                                    if (CheckCollisionPointRec(GetMousePosition(), ButtonRect)) {
                                        theButton.isHover = true;

                                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                            for (auto& button : argumentMap) {
                                                button.resetChoosen();
                                            }
                                            theButton.chooseThisButton();

                                            // RELOAD THE MAP
                                            if (trackImage.height != 0) UnloadImage(trackImage);
                                            trackImage = LoadImage(theButton.mapLoc.c_str());

                                            if (trackTexture.height != 0) UnloadTexture(trackTexture);
                                            trackTexture = LoadTexture(theButton.mapLoc.c_str());

                                            SetTextureFilter(trackTexture, TEXTURE_FILTER_BILINEAR);

                                            inputFlexibleSize = CalculateFlexibleImage();
                                            flexible_panel_input = FlexibleRectangle(PanelInputImage, inputFlexibleSize.w, inputFlexibleSize.h);

                                            resetAGVPosition();

                                            takeScreenShot = 2;
                                        }
                                    }
                                    else {
                                        theButton.isHover = false;
                                    }

                                    if (theButton.isChoosen)
                                    {
                                        simulationMAP = theButton.getDisplay();
                                    }

                                    DrawRectangleRounded(ButtonRect, 0.2F, 10, theButton.getColorButton());
                                    std::string text = theButton.getDisplay();
                                    DrawTextCustom(ButtonRect, text, CENTER, 0.75F, 1.0F, fontGeneral, theButton.getColorText());

                                }
                            }

                            if (parameterControls.at(i) == "MODE")
                            {
                                size_t buttonCounts = argumentMode.size();
                                for (size_t j = 0; j < buttonCounts; j++)
                                {
                                    auto& theButton = argumentMode.at(j);

                                    Rectangle buttonBase{
                                        argument.x + (j * argument.width / buttonCounts),
                                        argument.y,
                                        argument.width / buttonCounts,
                                        argument.height
                                    };

                                    Rectangle ButtonRect{
                                        buttonBase.x + (buttonPad * 1),
                                        buttonBase.y + (buttonPad * buttonPadFactor),
                                        buttonBase.width - (buttonPad * 2),
                                        buttonBase.height - (buttonPad * 2 * buttonPadFactor)
                                    };

                                    if (CheckCollisionPointRec(GetMousePosition(), ButtonRect)) {
                                        theButton.isHover = true;

                                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                            for (auto& button : argumentMode) {
                                                button.resetChoosen();
                                            }
                                            theButton.chooseThisButton();
                                        }
                                    }
                                    else {
                                        theButton.isHover = false;
                                    }

                                    if (theButton.isChoosen)
                                    {
                                        sensorMode = theButton.sensorMode;
                                    }

                                    DrawRectangleRounded(ButtonRect, 0.2F, 10, theButton.getColorButton());
                                    std::string text = theButton.getDisplay();
                                    DrawTextCustom(ButtonRect, text, CENTER, 0.55F, 1.0F, fontGeneral, theButton.getColorText());

                                }
                            }
                        }
                    }
                }

                {
                    // PANEL SPEED AND PID

                    float pad = 5;
                    PIDSection = {
                        PIDSection.x,
                        PIDSection.y + (pad * 1),
                        PIDSection.width,
                        PIDSection.height - (pad * 2)
                    };

                    Rectangle PanelParameter{
                        PIDSection.x,
                        PIDSection.y,
                        PIDSection.width * 0.30F + spacing,
                        PIDSection.height
                    };

                    Rectangle PanelArgument{
                        PanelParameter.x + PanelParameter.width + spacing,
                        PanelParameter.y,
                        (PIDSection.width - PanelParameter.width - spacing) * 0.975F,
                        PanelParameter.height
                    };

                    size_t parameterCounts = parameterPID.size();

                    for (size_t i = 0; i < parameterCounts; i++)
                    {
                        {
                            Rectangle parameter{
                                PanelParameter.x,
                                PanelParameter.y + (i * PanelParameter.height / parameterCounts),
                                PanelParameter.width,
                                PanelParameter.height / parameterCounts
                            };

                            std::string text = parameterPID.at(i);
                            DrawTextCustom(parameter, text, LEFT, 0.6F, 1.0F, fontGeneral, BLACK);
                        }

                        {
                            Rectangle argument{
                                PanelArgument.x,
                                PanelArgument.y + (i * PanelArgument.height / parameterCounts),
                                PanelArgument.width,
                                PanelArgument.height / parameterCounts
                            };

                            float pad = 10.0F;

                            argument = {
                                argument.x,
                                argument.y + (pad * 1),
                                argument.width - pad,
                                argument.height - (pad * 2)
                            };

                            if (parameterPID.at(i) == "SPEED") {

                                float minValue = 50.0F;
                                float maxValue = 1000.0F;

                                static SliderInput SliderSpeed{ argument, agv.getSpeed(), minValue, maxValue, false };
                                SliderSpeed.Run();

                                if (CheckCollisionPointRec(GetMousePosition(), argument))
                                {
                                    float newVal = (float)SliderSpeed.GetValue();
                                    static float oldVal = newVal;

                                    if (newVal != oldVal)
                                    {
                                        agv.speed = newVal;
                                        oldVal = newVal;
                                    }

                                }
                            }
                            else if (parameterPID.at(i) == "Kp")
                            {
                                float minValue = 2.0F;
                                float maxValue = 150.0F;

                                static SliderInput SliderKp{ argument, agv.Kp, minValue, maxValue, false };
                                SliderKp.Run();

                                if (CheckCollisionPointRec(GetMousePosition(), argument))
                                {
                                    float newVal = (float)SliderKp.GetValue();
                                    static float oldVal = newVal;

                                    if (newVal != oldVal)
                                    {
                                        agv.Kp = newVal;
                                        oldVal = newVal;
                                    }
                                }
                            }
                            else if (parameterPID.at(i) == "Ki")
                            {
                                float minValue = 0.01F;
                                float maxValue = 10.0F;

                                static SliderInput SliderKi{ argument, agv.Ki, minValue, maxValue, false };
                                SliderKi.Run();

                                if (CheckCollisionPointRec(GetMousePosition(), argument))
                                {
                                    float newVal = (float)SliderKi.GetValue();
                                    static float oldVal = newVal;

                                    if (newVal != oldVal)
                                    {
                                        agv.Ki = newVal;
                                        oldVal = newVal;
                                    }
                                }
                            }
                            else if (parameterPID.at(i) == "Kd")
                            {
                                float minValue = 0.01F;
                                float maxValue = 10.0F;

                                static SliderInput SliderKd{ argument, agv.Kd, minValue, maxValue, false };
                                SliderKd.Run();

                                if (CheckCollisionPointRec(GetMousePosition(), argument))
                                {
                                    float newVal = (float)SliderKd.GetValue();
                                    static float oldVal = newVal;

                                    if (newVal != oldVal)
                                    {
                                        agv.Kd = newVal;
                                        oldVal = newVal;
                                    }
                                }
                            }
                        }

                    }

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
                    float Speed = agv.getSpeed();

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

                    float Kp = agv.Kp;
                    float Ki = agv.Ki;
                    float Kd = agv.Kd;

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

            Color lineMagnetic = { 25,25,25,255 };

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

void resetAGVPosition()
{
    agv.x = 175;
    agv.y = 350;
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
