#include "physics.hpp"
#include "box2d/box2d.h"
#include "box2d/collision.h"
#include "box2d/id.h"
#include "box2d/types.h"
#include <iostream>

void delW(b2WorldId* id){
    if (id){
        if (b2World_IsValid(*id)){
            b2DestroyWorld(*id);
        }
        delete id;
    }
}
void delB(b2BodyId* id){
    if (id){
        if (b2Body_IsValid(*id)){
            b2DestroyBody(*id);
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

    body::body(b2WorldId w,b2Vec2 pos, b2BodyType type){
        if (!b2World_IsValid(w)){
            std::cerr << "Invalid world passed to body\n";
            return;
        }
        b2BodyDef def = b2DefaultBodyDef();
        def.position=pos;
        def.type=type;
        def.isAwake=true;
        b2BodyId* id = new b2BodyId();
        *id = b2CreateBody(w,&def);
        if (!b2Body_IsValid(*id)){
            delete id;
            std::cerr << "Creation of Body failed\n";
            return;
        }
        ptr.reset(id, delB);
    }
    void body::attachBox(float w, float  h){
        auto poly = b2MakeBox(w/2, h/2);
        b2ShapeDef def = b2DefaultShapeDef();
        def.density=1;
        def.enableContactEvents=true;
        def.material.friction=0;
        def.material.restitution=0;
        b2ShapeId id = b2CreatePolygonShape(get(), &def, &poly);
        if (!b2Shape_IsValid(id)){
            std::cerr << "Creation of world failed\n";
        }
    }
    b2BodyId body::get(){
        return *(ptr.get());
    }
}