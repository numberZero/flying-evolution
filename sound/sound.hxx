#pragma once
#include <SDL.h>
#include "types.hxx"

void initAudio();
void closeAudio();
void updateAudio(Float left, Float right, Float time);
