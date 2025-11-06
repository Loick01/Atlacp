#pragma once

#include <iostream>

class Element // Could rename this class MapElement, and use it for Player, NPC and Monster class
{
    protected:
        Element();
        ~Element();

    public:
        virtual void Update() = 0;
};