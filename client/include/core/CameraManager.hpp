#pragma once

#include <raylib.h>

class CameraManager {

public:
    CameraManager();
    static CameraManager& Get();

    void Update(Vector2 position);

    Camera2D GetCamera2D() const;
    void SetZoom(float zoom);
    float GetZoom() const;

private:
    Camera2D camera;
    float zoom;

};