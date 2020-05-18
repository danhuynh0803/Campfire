#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <vector>

#include "Camera.h"

enum GameState { PLAY, STOP, PAUSE };

struct GameManager
{
    GameState state = STOP;
    std::vector<Camera*> cameras;
    Camera* activeCamera; // Camera currenty in use in game
};

#endif // GAME_MANAGER_H
