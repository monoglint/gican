
#include <iostream>
#include <string>

import raylib;
import util;

int main() {
    raylib::Window window(800, 450, "test window", raylib::ConfigFlags::FLAG_WINDOW_RESIZABLE);

    std::cout << util::ansi_format::BLUE << "Note: Running gican_client will likely open a terminal. If you are seeing this, that is the case. The terminal is currently open for easy debugging.\n" << util::ansi_format::RESET;

    raylib::Camera3D camera;
    camera.position = {10, 10, 10};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 90;
    camera.projection = raylib::CameraProjection::CAMERA_PERSPECTIVE;

    window.SetTargetFPS(64);

    std::string date = __DATE__;
    std::string time = __TIME__;

    while (!window.ShouldClose()) {
        window.BeginDrawing();
        window.ClearBackground(raylib::Colors::BLACK);

        camera.BeginMode();
        camera.EndMode();

        raylib::DrawText(std::string("This is gican - Last compiled ") + date + " AT " + time + ".", 6, 6, 16, raylib::Colors::WHITE);
        
        window.EndDrawing();
    }

    return 0;
}