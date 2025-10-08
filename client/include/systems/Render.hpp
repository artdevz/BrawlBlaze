#pragma once

#include <raylib.h>

#include "../../../common/include/EntityManager.hpp"

class Render {

public:
    void RenderTile(EntityManager& entityManager);
    void RenderActor(EntityManager& entityManager, uint32_t localID);
    void RenderLifebar(EntityManager& entityManager, uint32_t localID);

};