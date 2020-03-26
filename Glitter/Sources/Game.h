#ifndef GAME_H
#define GAME_H

#include <vector>

#include "Camera.h"

enum GameState { PLAY, STOP, PAUSE };

struct GameController
{
    GameState state = STOP;
    std::vector<Camera*> cameras; // Camera currently in use
    Camera* activeCamera;
};

#endif // GAME_H
