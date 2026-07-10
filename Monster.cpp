#include "Character.h"
#include "Monster.h"
#include "Player.h"
#include "Villager.h"
#include "Board.h"
#include "Hero.h"
#include "Dice.h"
#include "Graphics.h"
#include <iostream>
#include <algorithm>

Monster::Monster(const std::string &name, const std::string &location, const std::string &power, const std::string &powerDescription, MonsterType mtype)
    : Character(name, location, Typechara::MONSTER),
      power(power),
      powerDescription(powerDescription),
      mtype(mtype),
      graphics(nullptr) {}

std::string Monster::getPower() const { return power; }
std::string Monster::getPowerDescription() const { return powerDescription; }
MonsterType Monster::getmonstertype() const { return mtype; }
void Monster::setGraphics(Graphics *g) { graphics = g; }

void Monster::strike(int movementUnits, int diceCount, const std::vector<Player> &players, std::vector<Villager> &villagers, Board *board)
{
    if (graphics)
    {
        graphics->AddLogMessage(getName() + " is striking!");
    }
    else
    {
        std::cout << getName() << " is striking!" << std::endl;
    }

    bool monsterWithPerson = false;
    std::string monsterloc = this->getLoc();

    for (const auto &player : players)
    {
        if (player.getHero() && player.getHero()->getLoc() == monsterloc)
        {
            monsterWithPerson = true;
            break;
        }
    }

    if (!monsterWithPerson)
    {
        for (auto &villager : villagers)
        {
            if (villager.getLoc() == monsterloc)
            {
                monsterWithPerson = true;
                break;
            }
        }
    }

    if (!monsterWithPerson)
    {
        moveTowardPerson(movementUnits, players, villagers, board);
        monsterloc = getLoc();
    }
    else
    {
        if (graphics)
        {
            graphics->AddLogMessage(getName() + " is already with a person at " + monsterloc);
        }
        else
        {
            std::cout << getName() << " is already with a person at " << monsterloc << std::endl;
        }
    }

    if (graphics)
    {
        graphics->ShowDiceRoll(getName(), diceCount);
    }
    else
    {
        std::cout << "Rolling " << diceCount << " dice for " << getName() << std::endl;
    }

    std::vector<int> diceResults = Dice::rollMultiple(diceCount);

    if (graphics)
    {
        graphics->ShowDiceResults(diceResults);
    }
    else
    {
        std::cout << "Dice results: ";
        for (int result : diceResults)
        {
            if (result == 0)
            {
                std::cout << "Blank ";
            }
            else if (result == 1)
            {
                std::cout << "Power(!) ";
            }
            else if (result == 2)
            {
                std::cout << "Attack(*) ";
            }
        }
        std::cout << std::endl;
    }

    std::vector<Hero *> heroinLoc;
    std::vector<Villager *> villinLoc;

    for (const auto &player : players)
    {
        if (player.getHero() && player.getHero()->getLoc() == monsterloc)
        {
            heroinLoc.push_back(player.getHero());
        }
    }

    for (auto &vill : villagers)
    {
        if (vill.getLoc() == monsterloc)
        {
            villinLoc.push_back(&vill);
        }
    }

    if ((!heroinLoc.empty() || !villinLoc.empty()) && Dice::hasAttack(diceResults))
    {
        if (!heroinLoc.empty())
        {
            if (graphics)
            {
                graphics->ShowMonsterAttack(getName(), heroinLoc[0]->getName());
            }
            this->attackHero(heroinLoc[0], board);
        }
        else
        {
            if (graphics)
            {
                graphics->ShowMonsterAttack(getName(), villinLoc[0]->getName());
            }
            this->attackVillager(villinLoc[0], board, villagers);
        }
    }
    else if (Dice::hasPower(diceResults))
    {
        if (!heroinLoc.empty())
        {
            if (graphics)
            {
                graphics->ShowMonsterPower(getName(), getPowerDescription());
            }
            this->usePower(heroinLoc[0], board);
        }
    }
}

void Monster::moveTowardPerson(int units, const std::vector<Player> &Players, const std::vector<Villager> &villagers, Board *board)
{
    std::string monsterloc = this->getLoc();
    std::string targetloc;
    int minDistance = INT_MAX;

    for (const auto &player : Players)
    {
        if (player.getHero())
        {
            std::string heroloc = player.getHero()->getLoc();
            if (heroloc != monsterloc)
            {
                int distance = board->calculateDistance(monsterloc, heroloc);
                if (distance != INT_MAX && distance < minDistance)
                {
                    minDistance = distance;
                    targetloc = heroloc;
                }
            }
        }
    }

    for (const auto &vill : villagers)
    {
        std::string villloc = vill.getLoc();
        if (villloc != monsterloc)
        {
            int distance = board->calculateDistance(monsterloc, villloc);
            if (distance != INT_MAX && distance < minDistance)
            {
                minDistance = distance;
                targetloc = villloc;
            }
        }
    }

    if (targetloc.empty())
    {
        if (graphics)
        {
            graphics->AddLogMessage(this->getName() + " has no target to move toward.");
        }
        else
        {
            std::cout << this->getName() << " has no target to move toward." << std::endl;
        }
    }
    else
    {
        if (graphics)
        {
            graphics->AddLogMessage(this->getName() + " moving towards " + targetloc);
        }
        else
        {
            std::cout << this->getName() << " moving towards " << targetloc << std::endl;
        }

        for (int i = 0; i < units; i++)
        {
            std::string nextloc = board->findShortestPath(monsterloc, targetloc);

            if (nextloc == monsterloc)
            {
                if (graphics)
                {
                    graphics->AddLogMessage(this->getName() + " cannot move further towards target.");
                }
                else
                {
                    std::cout << this->getName() << " cannot move further towards target." << std::endl;
                }
                break;
            }

            std::string fromLoc = monsterloc;
            setLoc(nextloc);
            monsterloc = nextloc;

            if (graphics)
            {
                graphics->DrawMonsterMovement(getName(), fromLoc, monsterloc);
                graphics->AddLogMessage(this->getName() + " moved to " + monsterloc);
            }
            else
            {
                std::cout << this->getName() << " moved to " << monsterloc << std::endl;
            }

            bool reachedP = false;
            for (const auto &player : Players)
            {
                if (player.getHero() && player.getHero()->getLoc() == monsterloc)
                {
                    reachedP = true;
                    break;
                }
            }

            if (!reachedP)
            {
                for (const auto &villager : villagers)
                {
                    if (villager.getLoc() == monsterloc)
                    {
                        reachedP = true;
                        break;
                    }
                }
            }

            if (reachedP || monsterloc == targetloc)
            {
                if (graphics)
                {
                    graphics->AddLogMessage(this->getName() + " reached " + (reachedP ? "a person " : "target location ") + "and stopped moving.");
                }
                else
                {
                    std::cout << this->getName() << " reached " << (reachedP ? "a person " : "target location ") << "and stopped moving." << std::endl;
                }
                break;
            }
        }
    }
}

void Monster::attackHero(Hero *hero, Board *board)
{
    if (graphics)
    {
        graphics->AddLogMessage(getName() + " attacks " + hero->getName() + "!");
    }
    else
    {
        std::cout << getName() << " attacks " << hero->getName() << "!" << std::endl;
    }

    const auto &inventory = hero->getInventory();
    if (!inventory.empty())
    {
        if (graphics)
        {
            graphics->AddLogMessage(hero->getName() + " has items to defend. Use an item? (y/n)");
        }
        else
        {
            std::cout << hero->getName() << " has items to defend. Use an item? (y/n)" << std::endl;
        }

        char a;
        std::cin >> a;

        if (a == 'y' || a == 'Y')
        {
            if (graphics)
            {
                graphics->AddLogMessage("Choose an item to use for defense:");
            }
            else
            {
                std::cout << "Choose an item to use for defense:" << std::endl;
            }

            for (size_t i = 0; i < inventory.size(); i++)
            {
                if (graphics)
                {
                    graphics->AddLogMessage(std::to_string(i + 1) + ". " + inventory[i].getName() + " (" +
                                            inventory[i].getTypeString() + ", Power: " +
                                            std::to_string(inventory[i].getPower()) + ")");
                }
                else
                {
                    std::cout << (i + 1) << ". " << inventory[i].getName() << " (" << inventory[i].getTypeString()
                              << ", Power: " << inventory[i].getPower() << ")" << std::endl;
                }
            }

            int itemchoice;
            std::cin >> itemchoice;

            if (itemchoice > 0 && itemchoice <= static_cast<int>(inventory.size()))
            {
                std::string itemName = inventory[itemchoice - 1].getName();
                hero->removeItem(itemName);

                if (graphics)
                {
                    graphics->AddLogMessage(hero->getName() + " used " + itemName + " to defend against the attack!");
                    graphics->ShowAttackResult(hero->getName(), true, true);
                }
                else
                {
                    std::cout << hero->getName() << " used " << itemName << " to defend against the attack!" << std::endl;
                }

                return;
            }
            else
            {
                if (graphics)
                {
                    graphics->AddLogMessage("Invalid item choice. No item used.");
                }
                else
                {
                    std::cout << "Invalid item choice. No item used." << std::endl;
                }
            }
        }
    }

    if (graphics)
    {
        graphics->AddLogMessage(hero->getName() + " takes damage!");
        graphics->ShowAttackResult(hero->getName(), true, false);
    }
    else
    {
        std::cout << hero->getName() << " takes damage!" << std::endl;
    }

    board->increaseTerrorLevel();
    hero->setLoc("Hospital");

    if (graphics)
    {
        graphics->AddLogMessage(hero->getName() + " will start next turn at the hospital");
    }
    else
    {
        std::cout << hero->getName() << " will start next turn at the hospital" << std::endl;
    }
}

void Monster::attackVillager(Villager *villager, Board *board, std::vector<Villager> &villagers)
{
    if (graphics)
    {
        graphics->AddLogMessage(getName() + " attacks " + villager->getName() + "!");
        graphics->AddLogMessage(villager->getName() + " is killed!");
        graphics->ShowAttackResult(villager->getName(), false, false);
    }
    else
    {
        std::cout << getName() << " attacks " << villager->getName() << "!" << std::endl;
        std::cout << villager->getName() << " is killed!" << std::endl;
    }

    board->increaseTerrorLevel();

    for (auto it = villagers.begin(); it != villagers.end(); ++it)
    {
        if (it->getName() == villager->getName())
        {
            villagers.erase(it);
            break;
        }
    }
}