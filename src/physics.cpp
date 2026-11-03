#include "physics.hpp"
#include "box2d/box2d.h"
#include <iostream>

void delW(b2WorldId* id){
    if (id){
        if (b2World_IsValid(*id)){
            b2DestroyWorld(*id);
        }
        delete id;
    }
}
namespace physics{
    world::world(b2Vec2 gravity){
        b2WorldDef def = b2DefaultWorldDef();
        def.gravity=gravity;
        b2WorldId* id = new b2WorldId();
        *id=b2CreateWorld(&def);
        if (!b2World_IsValid(*id)){
            delete id;
            std::cerr << "Creation of world failed\n";
            return;
        }
        ptr.reset(id, delW);
    }
    void world::simulate(float fps, int substepcount){
        if (ptr.get()){
            b2World_Step(get(), 1/fps, substepcount);
        }
    }
    b2WorldId world::get(){
        return *(ptr.get());
    }
}