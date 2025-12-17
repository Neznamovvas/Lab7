#ifndef DICE_H
#define DICE_H

#include <random>
#include "../Utils/Constants.h"

class Dice {
public:
    static int roll() {
        return Constants::randomInt(1, 6);
    }

    static int attackRoll() {
        return roll();
    }

    static int defenseRoll() {
        return roll();
    }
};

#endif