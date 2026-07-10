#ifndef MONSTER_HPP
#define MONSTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <map>

#include "Character.h"
#include "Board.h"

class Player;
class Villager;
class Board;
class Graphics;
class Hero;

enum class MonsterType
{
    Dracula,
    InvisibleMan
};

class Monster : public Character
{
protected:
    std::string power;
    std::string powerDescription;
    MonsterType mtype;
    Graphics *graphics;

public:
    Monster(const std::string &name, const std::string &location, const std::string &power, const std::string &powerDescription, MonsterType mtype);

    std::string getPower() const;
    std::string getPowerDescription() const;
    MonsterType getmonstertype() const;
    void setGraphics(Graphics *g);

    void strike(int movementUnits, int diceCount, const std::vector<Player> &player, std::vector<Villager> &villagers, Board *board);
    void moveTowardPerson(int units, const std::vector<Player> &Players, const std::vector<Villager> &villagers, Board *board);
    void attackHero(Hero *hero, Board *board);
    void attackVillager(Villager *villager, Board *board, std::vector<Villager> &villagers);

    virtual void usePower(Hero *hero, Board *board) = 0;
    virtual bool isDefeated() const = 0;

    
};

#endif