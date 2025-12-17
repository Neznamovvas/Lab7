#include "../../include/Game/GameNPCFactory.h"
#include "../../include/utils/constants.h"
#include <vector>
#include <stdexcept>

std::shared_ptr<GameNPC> GameNPCFactory::createGameNPC(const std::string& typeStr,
                                                      const std::string& name,
                                                      float x, float y,
                                                      int id) {
    if (typeStr == "squirrel") {
        return std::make_shared<GameSquirrel>(name, x, y, id);
    } else if (typeStr == "elf") {
        return std::make_shared<GameElf>(name, x, y, id);
    } else if (typeStr == "bandit") {
        return std::make_shared<GameBandit>(name, x, y, id);
    } else {
        throw std::invalid_argument("Unknown NPC type: " + typeStr);
    }
}

std::string GameNPCFactory::getRandomSquirrelName() {
    static std::vector<std::string> names = {"Fluffy", "Redtail", "Nutty", "Whiskers", "Chip"};
    return names[Constants::randomInt(0, names.size() - 1)];
}

std::string GameNPCFactory::getRandomElfName() {
    static std::vector<std::string> names = {"Legolas", "Aragorn", "Elrond", "Galadriel", "Thranduil"};
    return names[Constants::randomInt(0, names.size() - 1)];
}

std::string GameNPCFactory::getRandomBanditName() {
    static std::vector<std::string> names = {"Borat", "Goro", "Drago", "Khan", "Ragnar"};
    return names[Constants::randomInt(0, names.size() - 1)];
}

std::shared_ptr<GameNPC> GameNPCFactory::createRandomGameNPC(int id) {
    
    std::string type;
    std::string name;
    
    int typeNum = Constants::randomInt(0, 2);
    switch(typeNum) {
        case 0:
            type = "squirrel";
            name = getRandomSquirrelName() + '[' + std::to_string(id) + ']';
            break;
        case 1:
            type = "elf";
            name = getRandomElfName() + '[' + std::to_string(id) + ']';
            break;
        case 2:
            type = "bandit";
            name = getRandomBanditName() + '[' + std::to_string(id) + ']';
            break;
    }
    
    
    float x = Constants::randomFloat(1.0f, Constants::MAP_WIDTH - 1.0f);
    float y = Constants::randomFloat(1.0f, Constants::MAP_HEIGHT - 1.0f);
    
    return createGameNPC(type, name, x, y, id);
}