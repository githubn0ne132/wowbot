// game_actions.h
#pragma once

#include "globals.h"
#include <string>
#include <cstdint>

// Functions for performing actions in the game

// Calls the internal CastSpell function
std::string CastSpell(int spellId, uint64_t targetGuid);

// Structs required for ClickToMove
struct WOWPOS {
    float x;
    float y;
    float z;
};

struct WGUID {
    uint64_t guid;
};

// Function to move the player to a specific coordinate.
std::string MoveTo(float x, float y, float z);