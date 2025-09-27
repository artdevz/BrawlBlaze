#pragma once

enum class ScreenType { None, Menu, Host, Join, Game, Exit };

class Screen {

public:
    virtual ~Screen() = default;
    
    virtual void Init() {}
    virtual void Update() = 0;
    virtual void Draw() = 0;

    virtual bool ShouldClose() const { return false; }
    virtual ScreenType NextScreen() { return ScreenType::None; }

};