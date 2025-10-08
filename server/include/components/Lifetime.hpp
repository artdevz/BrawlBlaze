#pragma once

struct Lifetime {
    float lifespan;
    
    Lifetime(float lifespan = 0.0f) : lifespan(lifespan) {}

    bool ReduceLifespan(float amount) {
        if (lifespan <= 0.0f) return true;
        lifespan -= amount;
        return false;
    }
};