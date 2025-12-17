#include "../../include/NPC/NPCFactory.h"
#include "../../include/NPC/squirrel.h"
#include "../../include/NPC/elf.h"
#include "../../include/NPC/bandit.h"
#include <stdexcept>

std::shared_ptr<NPC> NPCFactory::createNPC(const std::string& typeStr, 
                                         const std::string& name, 
                                         float x, float y) {
    if (typeStr == "squirrel") {
        return std::make_shared<Squirrel>(name, x, y);
    } else if (typeStr == "elf") {
        return std::make_shared<Elf>(name, x, y);
    } else if (typeStr == "bandit") {
        return std::make_shared<Bandit>(name, x, y);
    } else {
        throw std::invalid_argument("Unknown NPC type: " + typeStr);
    }
}

std::shared_ptr<NPC> NPCFactory::createFromMap(const std::map<std::string, std::string>& data) {
    std::string type = data.at("type");
    std::string name = data.at("name");
    float x = std::stof(data.at("x"));
    float y = std::stof(data.at("y"));
    
    return createNPC(type, name, x, y);
}