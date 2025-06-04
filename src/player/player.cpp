#include "../../includes/player.hpp"


Player::Player() : _pairCaptured(0), _stoneColor(CELL_EMPTY)
{
}

Player::Player(Cell color) : _pairCaptured(0), _stoneColor(color)
{
}

Player::~Player()
{
}

int Player::getPairCaptured() const
{
    return _pairCaptured;
}

Cell Player::getStoneColor()
{
    return _stoneColor;
}

void Player::resetPairCaptured()
{
    _pairCaptured = 0;
}

void Player::addCapturedPair()
{
    _pairCaptured += 1;
}