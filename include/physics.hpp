#pragma once

#include "box2d/id.h"
#include "box2d/math_types.h"
#include <memory>
namespace physics {
    class world{
        std::shared_ptr<b2WorldId> ptr;
        public:
            world(b2Vec2 gravity={0,10});
            void simulate(float fps=60, int substepcount=4);
            b2WorldId get();
    };
}