#ifndef NPC_H
#define NPC_H

#include <string>
#include <memory>
#include <map>

enum class NPCType {
    SQUIRREL,
    ELF,
    BANDIT
};

class NPC {
protected:
    std::string name;
    float x, y;
    bool alive;
    
    static std::string typeToString(NPCType type);
    
public:
    NPC(const std::string& name, float x, float y);
    virtual ~NPC() = default;
    
    virtual NPCType getType() const = 0;
    virtual bool canKill(const std::shared_ptr<NPC>& other) const = 0;
    
    float distanceTo(const std::shared_ptr<NPC>& other) const;
    
    std::string getName() const;
    float getX() const;
    float getY() const;
    bool isAlive() const;
    void kill();
    
    virtual std::string getInfo() const;
    virtual std::map<std::string, std::string> toMap() const;
    
    static bool isValidCoordinate(float coord);
};

#endif