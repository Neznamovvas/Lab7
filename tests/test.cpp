#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <map>

#include "../include/NPC/NPC.h"
#include "../include/NPC/squirrel.h"
#include "../include/NPC/elf.h"
#include "../include/NPC/bandit.h"
#include "../include/NPC/NPCFactory.h"
#include "../include/Visitor/BattleVisitor.h"
#include "../include/Observer/ConsoleObserver.h"
#include "../include/Game/dice.h"
#include "../include/utils/constants.h"


TEST(NPCTest, CreateNPCs) {
    auto squirrel = std::make_shared<Squirrel>("TestSquirrel", 50.0f, 50.0f);
    auto elf = std::make_shared<Elf>("TestElf", 100.0f, 100.0f);
    auto bandit = std::make_shared<Bandit>("TestBandit", 150.0f, 150.0f);
    
    EXPECT_EQ(squirrel->getName(), "TestSquirrel");
    EXPECT_EQ(elf->getName(), "TestElf");
    EXPECT_EQ(bandit->getName(), "TestBandit");
}

TEST(NPCTest, CheckTypes) {
    auto squirrel = std::make_shared<Squirrel>("S", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("E", 20.0f, 20.0f);
    auto bandit = std::make_shared<Bandit>("B", 30.0f, 30.0f);
    
    EXPECT_EQ(squirrel->getType(), NPCType::SQUIRREL);
    EXPECT_EQ(elf->getType(), NPCType::ELF);
    EXPECT_EQ(bandit->getType(), NPCType::BANDIT);
}

TEST(NPCTest, KillRules) {
    auto squirrel = std::make_shared<Squirrel>("Squirrel", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("Elf", 20.0f, 20.0f);
    auto bandit = std::make_shared<Bandit>("Bandit", 30.0f, 30.0f);
    
    EXPECT_TRUE(squirrel->canKill(elf));
    EXPECT_FALSE(squirrel->canKill(bandit));
    
    EXPECT_TRUE(elf->canKill(bandit));
    EXPECT_FALSE(elf->canKill(squirrel));
    
    EXPECT_TRUE(bandit->canKill(squirrel));
    EXPECT_FALSE(bandit->canKill(elf));
}

TEST(NPCTest, DistanceCalculation) {
    
    auto npc1 = std::make_shared<Squirrel>("N1", 10.0f, 10.0f);
    auto npc2 = std::make_shared<Elf>("N2", 13.0f, 14.0f); 
    
    float distance = npc1->distanceTo(npc2);
    EXPECT_FLOAT_EQ(distance, 5.0f);
}

TEST(NPCTest, AliveState) {
    auto npc = std::make_shared<Squirrel>("Test", 50.0f, 50.0f);
    
    EXPECT_TRUE(npc->isAlive());
    npc->kill();
    EXPECT_FALSE(npc->isAlive());
}



TEST(NPCFactoryTest, CreateNPCByType) {
    auto squirrel = NPCFactory::createNPC("squirrel", "Fluffy", 100.0f, 100.0f);
    auto elf = NPCFactory::createNPC("elf", "Legolas", 200.0f, 200.0f);
    auto bandit = NPCFactory::createNPC("bandit", "Borat", 300.0f, 300.0f);
    
    EXPECT_EQ(squirrel->getType(), NPCType::SQUIRREL);
    EXPECT_EQ(elf->getType(), NPCType::ELF);
    EXPECT_EQ(bandit->getType(), NPCType::BANDIT);
}

TEST(NPCFactoryTest, CreateFromMap) {
    std::map<std::string, std::string> data = {
        {"type", "elf"},
        {"name", "TestElf"},
        {"x", "50.5"},
        {"y", "60.7"}
    };
    
    auto npc = NPCFactory::createFromMap(data);
    
    EXPECT_EQ(npc->getType(), NPCType::ELF);
    EXPECT_EQ(npc->getName(), "TestElf");
    EXPECT_FLOAT_EQ(npc->getX(), 50.5f);
    EXPECT_FLOAT_EQ(npc->getY(), 60.7f);
}

TEST(NPCFactoryTest, UnknownTypeThrows) {
    EXPECT_THROW(
        NPCFactory::createNPC("dragon", "Dragon", 100.0f, 100.0f),
        std::invalid_argument
    );
}



TEST(BattleVisitorTest, BasicBattle) {
    
    auto squirrel = std::make_shared<Squirrel>("Squirrel", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("Elf", 11.0f, 11.0f); 
    
    std::vector<std::shared_ptr<NPC>> npcs = {squirrel, elf};
    
    BattleVisitor visitor(10.0f);
    visitor.visit(npcs);
    
    EXPECT_TRUE(squirrel->isAlive());
    EXPECT_FALSE(elf->isAlive());
}

TEST(BattleVisitorTest, NoBattleWhenFar) {
    auto squirrel = std::make_shared<Squirrel>("S1", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("E1", 90.0f, 90.0f); 
    
    std::vector<std::shared_ptr<NPC>> npcs = {squirrel, elf};
    
    BattleVisitor visitor(10.0f);
    visitor.visit(npcs);
    
    EXPECT_TRUE(squirrel->isAlive());
    EXPECT_TRUE(elf->isAlive());
}



TEST(GameTest, DiceRoll) {
    int roll = Dice::roll();
    EXPECT_GE(roll, 1);
    EXPECT_LE(roll, 6);
}

TEST(GameTest, GameConstants) {
    EXPECT_EQ(Constants::MAP_WIDTH, 100);
    EXPECT_EQ(Constants::MAP_HEIGHT, 100);
    EXPECT_EQ(Constants::GAME_DURATION, 30);
    
    EXPECT_EQ(Constants::MoveDistance::SQUIRREL, 5);
    EXPECT_EQ(Constants::MoveDistance::ELF, 10);
    EXPECT_EQ(Constants::MoveDistance::BANDIT, 10);
    
    EXPECT_EQ(Constants::KillDistance::SQUIRREL, 5);
    EXPECT_EQ(Constants::KillDistance::ELF, 50);
    EXPECT_EQ(Constants::KillDistance::BANDIT, 10);
}



TEST(IntegrationTest, VisitorWithObserver) {
    auto squirrel = std::make_shared<Squirrel>("Squirrel", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("Elf", 11.0f, 11.0f);
    
    std::vector<std::shared_ptr<NPC>> npcs = {squirrel, elf};
    
    BattleVisitor visitor(10.0f);
    auto observer = std::make_shared<ConsoleObserver>();
    visitor.addObserver(observer);
    
    EXPECT_NO_THROW(visitor.visit(npcs));
    EXPECT_TRUE(squirrel->isAlive());
    EXPECT_FALSE(elf->isAlive());
}

TEST(IntegrationTest, KillChainRules) {
    auto squirrel = std::make_shared<Squirrel>("Squirrel", 10.0f, 10.0f);
    auto elf = std::make_shared<Elf>("Elf", 20.0f, 20.0f);
    auto bandit = std::make_shared<Bandit>("Bandit", 30.0f, 30.0f);
    
    EXPECT_TRUE(squirrel->canKill(elf));
    EXPECT_TRUE(elf->canKill(bandit));
    EXPECT_TRUE(bandit->canKill(squirrel));
    
    EXPECT_FALSE(elf->canKill(squirrel));
    EXPECT_FALSE(bandit->canKill(elf));
    EXPECT_FALSE(squirrel->canKill(bandit));
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}