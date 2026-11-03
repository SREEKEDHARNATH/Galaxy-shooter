#pragma once

#include "box2d/id.h"
#include "box2d/math_types.h"
#include "box2d/types.h"
#include <memory>
namespace physics {
    class world{
        std::shared_ptr<b2WorldId> ptr;
        public:
            world(b2Vec2 gravity={0,10});
            void simulate(float fps=60, int substepcount=4);
            b2WorldId get();
    };
    class body{
        std::shared_ptr<b2BodyId> ptr;
        public:
            body(b2WorldId w,b2Vec2 pos, b2BodyType type);
            void attachBox(float w, float  h);
            b2BodyId get();
    };
}