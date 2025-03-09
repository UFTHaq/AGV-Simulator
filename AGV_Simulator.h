#pragma once

void loadImageAGV();

void resetAGVPosition();

void AGVGoRight(float Speed);

void AGVGoLeft(float Speed);

void AGVGoStraight(float Speed);

void AGV_PID(float Speed, float correction);
