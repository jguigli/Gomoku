#pragma once
#include "config.hpp"

class Player
{
    public:
        Player();
        explicit Player(Cell color);
        ~Player();
        int getPairCaptured() const;
        Cell getStoneColor();
        void resetPairCaptured();
        void addCapturedPair();

    private:
        int _pairCaptured;
        Cell _stoneColor;
};
