![GitHub last commit](https://img.shields.io/github/last-commit/UFTHaq/AGV-Simulator?style=for-the-badge)
![GitHub top language](https://img.shields.io/github/languages/top/UFTHaq/AGV-Simulator?logo=cpp&style=for-the-badge)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/UFTHaq/AGV-Simulator?style=for-the-badge)
![GitHub Repo stars](https://img.shields.io/github/stars/UFTHaq/AGV-Simulator?color=red&style=for-the-badge)
![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/UFTHaq/AGV-Simulator/total?style=for-the-badge)

![Visual Studio](https://img.shields.io/badge/Visual%20Studio-5C2D91.svg?style=for-the-badge&logo=visual-studio&logoColor=white)
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)

[![License: MIT](https://img.shields.io/badge/License-MIT-black.svg?style=for-the-badge)](https://opensource.org/licenses/MIT)

# AGV-Simulator
Simple agv line follower simulation with control of (PID, Speed, Sensor off Timing).

<div align="center">
<!--   <img src="https://github.com/user-attachments/assets/1e35a476-31ae-4c60-b80d-7cd21cff0aa4" width=180/> -->
</div>

### Features:
- Drag&Drop Track, as long the track is black, it can be detected, atleast in range of {RGBA : {0,0,0,255} to {25,25,25,255}}.
- Base 2 map choices (A & B).
- Using 8 points of sensor.
- 2 Mode (Normal: all sensors is in line, Sport: the sensors is curves, make it better in high speed).
- PID control (Kp, Ki, Kd).
- Speed control.
- Timing off sensor read control.
- RUN and STOP Button (for simulation).
- Press {R} Key to reset all state.
- Press Key Arrow {UP : forward}, {LEFT: Move Left Wheel}, {RIGHT: Move Right Wheel}.
- Screenshot:
<div align="center">
  <img alt="UI-v1.0.0" src="https://github.com/user-attachments/assets/81be34ad-e0d1-4dce-8ba1-0734ab5bf79e" width="600"/>
</div>


### Tips How to Use:
- At start, you need to drag the car to the track. if the car is out off line, it will stop, you can put it to track again by mouse or by arrow keys.

### Build with:
- C++20 in MSVC Visual Studio C++ 2022

### Libraries:
- Raylib : [https://www.raylib.com](https://www.raylib.com/)

### Download:
- Windows-x64 : [AGV-Simulator-Beta-v0.1.0-win64.zip](this will be link download)


