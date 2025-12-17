#include "../../include/NPC/NPC.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <stdexcept>

NPC::NPC(const std::string& name, float x, float y) 
    : name(name), x(x), y(y), alive(true) {
    if (x <= 0 || x > 500 || y <= 0 || y > 500) {
        throw std::invalid_argument("Coordinates must be in range 0 < x,y <= 500");
    }
}

float NPC::distanceTo(const std::shared_ptr<NPC>& other) const {
    float dx = x - other->x;
    float dy = y - other->y;
    return std::sqrt(dx*dx + dy*dy);
}

std::string NPC::getName() const { return name; }
float NPC::getX() const { return x; }
float NPC::getY() const { return y; }
bool NPC::isAlive() const { return alive; }
void NPC::kill() { alive = false; }

std::string NPC::getInfo() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1);
    ss << "Type: " << (getType() == NPCType::SQUIRREL ? "squirrel" : 
                       getType() == NPCType::ELF ? "elf" : "bandit")
       << ", Name: " << name 
       << ", Coordinates: (" << x << ", " << y << ")"
       << ", Status: " << (alive ? "alive" : "dead");
    return ss.str();
}

std::map<std::string, std::string> NPC::toMap() const {
    std::string typeStr;
    switch(getType()) {
        case NPCType::SQUIRREL: typeStr = "squirrel"; break;
        case NPCType::ELF: typeStr = "elf"; break;
        case NPCType::BANDIT: typeStr = "bandit"; break;
    }
    
    return {
        {"type", typeStr},
        {"name", name},
        {"x", std::to_string(x)},
        {"y", std::to_string(y)}
    };
}