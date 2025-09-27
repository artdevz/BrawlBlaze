#include "core/CameraManager.hpp"

CameraManager::CameraManager() {
    camera.offset = { 1280 / 2.0f, 720 / 2.0f };
    camera.target = { 1280 / 2.0f, 720 / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 2.0f;
}

CameraManager& CameraManager::Get() {
    static CameraManager instance;
    return instance;
}

void CameraManager::Update(Vector2 position) { 
    camera.target = position;

    float scroll = GetMouseWheelMove();
    if (scroll != 0.0f) {
        zoom += scroll*0.1f;
        if (zoom < 2.0f) zoom = 2.0f;
        if (zoom > 3.0f) zoom = 3.0f;
        camera.zoom = zoom;
    }
}

Camera2D CameraManager::GetCamera2D() const { return camera; }

float CameraManager::GetZoom() const { return zoom; }

void CameraManager::SetZoom(float newZoom) {
    zoom = newZoom;
    camera.zoom = zoom;
}