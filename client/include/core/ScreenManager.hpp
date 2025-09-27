#pragma once

#include <memory>
#include <stack>

#include "screens/Screen.hpp"

class ScreenManager {

public:
    static ScreenManager& Get() {
        static ScreenManager instance;
        return instance;
    }

    void Push(ScreenType type);
    void Pop();

    void Run();
    bool IsEmpty() const;

private:
    std::stack<std::unique_ptr<Screen>> screens;
    std::unique_ptr<Screen> Create(ScreenType type);

};