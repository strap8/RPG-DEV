#ifndef BATTLE_HPP
#define BATTLE_HPP

#include "Random.hpp"
#include "Clock.hpp"
#include "Dialogue.hpp"
#include "Creature.hpp"
#include "Armour.hpp"
#include "Weapon.hpp"

#include <iostream>

class Battle
{
public:
	// Random number generator
	Random rand;
	// Clock
	Clock<> clock;
	// Dialogue used to ask the player battle choices
	Dialogue dialogue;

	// Creatures in combat. creatures[0] is the player
	Creature* creatures[2];

	Battle()
	{
	}

	Battle(Creature* player, Creature* b)
	{
		// Start a battle with the player and another creature
		this->creatures[0] = player;
		this->creatures[1] = b;

		// Set up the dialogue. Defending offers no tactical advangtages
		// in this battle system
		this->dialogue = Dialogue("What will you do?",
		{
			"Attack",
			"Defend"
		});
	}

	// Creature a attacks creature b, and b takes damage accordingly
	void attack(Creature* a, Creature* b)
	{
		std::cout << a->name << " attacks!\n";

		// Damage that a will inflict on b
		int damage = 0;

		// Cumulative modifier to hitRate
		int attackMod = a->attackMod; 

		// If a has equipped a weapon, then add the weapon damage on
		// to the current damage and add the attack modifier 
		// of the weapon on to the current attack modifier
		if (a->equippedWeapon != nullptr)
		{
		// damage is calculated by rolling the perspective amount of dice
		// and by determining the amound of sides the dice you are rolling.
		// damage[0] = amount of times you roll the dice. 
		// damage[1] = the amount of sides a die has.
			for (int i = 0; i < a->equippedWeapon->damage[0]; i++)
			{
				damage += (rand.DrawNumber(1, a->equippedWeapon->damage[1]) + a->DamageMod + ((a->str - 10)/2));
			}

			attackMod += a->equippedWeapon->attackMod;
		}

		// Increase the damage by half the attacker's strength
		//damage += a->str / 2;
		damage += (rand.DrawNumber(1, 4) + a->DamageMod + (a->str-10)/2); // Default damage is rolling a 1d4 (One four-sided die)

		// Damage that b will block
		int defense = 0;

		// Sum the defense values of the armour that b has equipped, and
		// increase the defense by the summed value
		for (int i = 0; i < Armour::Slot::N; ++i)

		{
			if (b->equippedArmour[i] != nullptr)
				defense += b->equippedArmour[i]->defense;
		}

		// Decrease the damage by the damage blocked, then ensure that
		// damage is always inflicted (we do not want battles to last
		// forever, nor to we want attacks to heal the wounded!)
		damage -= defense;
		if (damage < 1) damage = 1;

		// Add the hit rate to the base hit rate and subract the target's
		// dexterity from it. Instead of halving it to normalise it into
		// a percentage, we just double the range of randomly generated
		// values
		clock.sleep(rand.DrawNumber(1234, 2543)); // Add a time dynamic to the battle
		if ((rand.DrawNumber(1, 20) + attackMod) >= (b->armourClass + (b->dex - 10) / 2))
		{
			// The attack hit, so subtract the damage
			std::cout << b->name << " takes " << damage << " damage!\n";
			b->health -= damage;
		}
		else
		{
			// The attack missed
			std::cout << a->name << " missed!\n";
		}
		return;
	}

	// Allow the player to act
	void playerTurn()
	{
		// Activate the dialogue and allow the player to choose their
		// battle option
		int result = this->dialogue.activate();

		switch (result)
		{
			// Attack the enemy
		case 1:
			attack(creatures[0], creatures[1]);
			break;
			// Defend, skipping to the enemy's turn
		case 2:
			std::cout << creatures[0]->name << " defends!\n";
			break;
		default:
			break;
		}

		return;
	}

	// Allow the enemy to attack
	void enemyTurn()
	{
		// Battle system does not currently allow for any kind of
		// tactics, so make the enemy attack blindly
		attack(creatures[1], creatures[0]);

		return;
	}

	// Return true if the creature is dead. Split into it's own function
	// to allow easy addition of effects which simulate death, such as
	// petrifaction or banishment
	bool isdead(Creature* creature)
	{
		if (creature->health <= 0)
		{
			return true;
		}
		return false;
	}

	// Run a round of the battle
	bool activate()
	{
		// The creature with the highest dexterity attacks first, with
		// preference to the player
		if ((rand.DrawNumber(1,20) + creatures[0]->dex) >= (rand.DrawNumber(1, 20) + creatures[1]->dex))
		{
			// Run each turn and check if the foe is dead at the end of
			// each
			this->playerTurn();
			if (isdead(creatures[1]))
			{
				std::cout << creatures[1]->name << " was vanquished!\n";
				return true;
			}

			this->enemyTurn();
			if (isdead(creatures[0]))
			{
				std::cout << creatures[0]->name << " was vanquished!\n";
				return true;
			}
		}
		else
		{
			this->enemyTurn();
			if (isdead(creatures[0]))
			{
				std::cout << creatures[0]->name << " was vanquished!\n";
				return true;
			}

			this->playerTurn();
			if (isdead(creatures[1]))
			{
				std::cout << creatures[1]->name << " was vanquished!\n";
				return true;
			}
		}

		return false;
	}

	// Begin the battle
	void run()
	{
		std::cout << creatures[1]->name << " appears!" << std::endl;

		// Run the battle until one creature dies
		while (!this->activate());

		// If the enemy is dead, then allocate experience to the player
		if (isdead(creatures[1]))
		{
			// Give experience to the player equal to one eigth of the
			// experience the enemy gained to reach it's next level
			unsigned int expGain = creatures[1]->expToLevel(creatures[1]->level + 1) / 8;
			std::cout << "Gained " << expGain << " exp!\n";
			creatures[0]->exp += expGain;

			// Repeatedly level up the player until they are the highest
			// level they can be for their experience
			while (creatures[0]->levelUp());
		}

		return;
	}
};

#endif /* BATTLE_HPP */
