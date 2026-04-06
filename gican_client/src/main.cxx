
#include <iostream>

import raylib;

int main() {
    raylib::Window window(800, 450, "test window", raylib::ConfigFlags::FLAG_WINDOW_RESIZABLE);
    std::cout << "Initializing gican client.\n";

    raylib::Camera3D camera;
    camera.position = {10, 10, 10};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 90;
    camera.projection = raylib::CameraProjection::CAMERA_PERSPECTIVE;

    window.SetTargetFPS(64);

    while (!window.ShouldClose()) {
        window.BeginDrawing();
        window.ClearBackground(raylib::Colors::BLACK);

        camera.BeginMode();
        camera.EndMode();

        raylib::DrawText("Hello world", 6, 6, 16, raylib::Colors::WHITE);
        
        window.EndDrawing();
    }

    return 0;
}