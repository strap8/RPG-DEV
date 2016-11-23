#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "Item.hpp"
#include "Weapon.hpp"
#include "Armour.hpp"
#include "SpellBook.hpp"

#include <list>
#include <utility>
#include <iostream>

class Inventory
{
public:

	// Whilst weapons and armour are also items, they have their own
	// specific properties and so cannot be stored inside the same
	// list as the items. We use a list and not a vector as inventories
	// are highly mutable. This way they can also be efficiently sorted
	// The first element of the pair stores a pointer to the item in
	// the item/weapon/armour atlas, defined in main(), and the second
	// element stores the quantity of the item
	std::list<std::pair<Item*, int>> items;
	std::list<std::pair<Weapon*, int>> weapons;
	std::list<std::pair<Armour*, int>> armour;
	std::list<std::pair<SpellBook*, int>> spells;

	Inventory()
	{
	}

	Inventory(std::list<std::pair<Item*, int>> items,
		std::list<std::pair<Weapon*, int>> weapons,
		std::list<std::pair<Armour*, int>> armour,
		std::list<std::pair<SpellBook*, int>> spells)
	{
		this->items = items;
		this->weapons = weapons;
		this->armour = armour;
		this->spells = spells;
	}

	// Remove all items from the inventory, destroying them in the process
	// (They remain in the atlas though)
	void clear()
	{
		this->items.clear();
		this->weapons.clear();
		this->armour.clear();
		this->spells.clear();
	}

	// Add an item to the inventory, specified by a pointer to it
	// from the item atlas (technically does not need to point there,
	// but it should anyway)
	void add_item(Item* item, int count)
	{
		// Perform the same operation as merging, but for a single item
		for (auto& it : this->items)
		{
			if (it.first == item)
			{
				it.second += count;
				return;
			}
		}
		// If the item doesn't already exist in the inventory, then a
		// pair must be created too
		this->items.push_back(std::make_pair(item, count));
	}

	// Same as for items
	void add_weapon(Weapon* weapon, int count)
	{
		for (auto& it : this->weapons)
		{
			if (it.first == weapon)
			{
				it.second += count;
				return;
			}
		}
		this->weapons.push_back(std::make_pair(weapon, count));
	}

	// Same as for items
	void add_armour(Armour* armour, int count)
	{
		for (auto& it : this->armour)
		{
			if (it.first == armour)
			{
				it.second += count;
				return;
			}
		}
		this->armour.push_back(std::make_pair(armour, count));
	}

	// Same as for items
	void add_spell(SpellBook* spell, int count)
	{
		for (auto& it : this->spells)
		{
			if (it.first == spell)
			{
				it.second += count;
				return;
			}
		}
		this->spells.push_back(std::make_pair(spell, count));
	}

	// Remove the specified number of items from the inventory
	void remove_item(Item* item, int count)
	{
		// Iterate through the items, and if they are found then decrease
		// the quantity by the quantity removed
		for (auto& it : this->items)
		{
			if (it.first == item) it.second -= count;
		}
		// Iterate through the list again, and remove any elements from
		// the list that have zero or less for their quantity
		// We do this in two passes because removing an element from
		// a list during a for loop invalidates the iterators, and the
		// loop stops working
		this->items.remove_if([](std::pair<Item*, int>& element)
		{
			return element.second < 1;
		});
	}

	// Same as for items
	void remove_weapon(Weapon* weapon, int count)
	{
		for (auto& it : this->weapons)
		{
			if (it.first == weapon) it.second -= count;
		}
		this->weapons.remove_if([](std::pair<Weapon*, int>& element)
		{
			return element.second < 1;
		});
	}

	// Same as for items
	void remove_armour(Armour* armour, int count)
	{
		for (auto& it : this->armour)
		{
			if (it.first == armour) it.second -= count;
		}
		this->armour.remove_if([](std::pair<Armour*, int>& element)
		{
			return element.second < 1;
		});
	}

	// Same as for items
	void remove_spell(SpellBook* spell, int count)
	{
		for (auto& it : this->spells)
		{
			if (it.first == spell) it.second -= count;
		}
		this->spells.remove_if([](std::pair<SpellBook*, int>& element)
		{
			return element.second < 1;
		});
	}

	// Merge the specified inventory with the current one, adding
	// item quantities together if they already exist and adding the item
	// into a new slot if they do not
	void merge(Inventory* inventory)
	{
		// You can't merge an inventory with itself!
		if (inventory == this) return;

		// Loop through the items to be added, and add them. Our addition
		// function will take care of everything else for us
		for (auto it : inventory->items)
		{
			this->add_item(it.first, it.second);
		}
		// Do the same for the weapons
		for (auto it : inventory->weapons)
		{
			this->add_weapon(it.first, it.second);
		}
		// Do the same for the armour
		for (auto it : inventory->armour)
		{
			this->add_armour(it.first, it.second);
		}

		return;
	}

	// Output a list of the items onto stdout, formatted nicely and
	// numbered if required
	int print_items(bool label = false)
	{
		unsigned int i = 1;

		for (auto it : this->items)
		{
			// Number the items if asked
			if (label) std::cout << i++ << ": ";
			// Output the item name, quantity and description, e.g.
			// Gold Piece (29) - Glimmering discs of wealth
			std::cout << it.first->name << " (" << it.second << ") - ";
			std::cout << it.first->description << std::endl;
		}

		// Return the number of items outputted, for convenience
		return this->items.size();
	}

	// Same as for items
	int print_weapons(bool label = false)
	{
		unsigned int i = 1;

		for (auto it : this->weapons)
		{
			if (label) std::cout << i++ << ": ";
			std::cout << it.first->name << " (" << it.second << ") ";
			std::cout << "+DMG: (" << it.first->damage[0] << "d" << it.first->damage[1] << ")" << std::endl;
			std::cout << it.first->description << std::endl;
		}

		return this->weapons.size();
	}

	// Same as for items
	int print_armour(bool label = false)
	{
		unsigned int i = 1;

		for (auto it : this->armour)
		{
			if (label) std::cout << i++ << ": ";
			std::cout << it.first->name << " (" << it.second << ") - ";
			std::cout << it.first->description << std::endl;
			std::cout << "+HP: " << it.first->healthBoost << std::endl;
		}

		return this->armour.size();
	}

	// Same as for items
	int print_spells(bool label = false)
	{
		unsigned int i = 1;

		for (auto it : this->spells)
		{
			if (label) std::cout << i++ << ": ";
			std::cout << it.first->name << " (" << it.second << ") ";
			std::cout << "+DMG: (" << it.first->damage[0] << "d" << it.first->damage[1] << ")" << std::endl;
			std::cout << it.first->description << std::endl;
		}

		return this->spells.size();
	}

	// Print the entire inventory; items, then weapons, then armour,
	// but if the inventory is empty then output "Nothing"
	void print(bool label = false)
	{
		if (this->items.size() == 0 &&
			this->weapons.size() == 0 &&
			this->armour.size() == 0 &&
			this->spells.size() == 0)
		{
			std::cout << "Nothing" << std::endl;
		}
		else
		{
			this->print_items(label);
			this->print_weapons(label);
			this->print_armour(label);
			this->print_spells(label);
		}

		return;
	}
};

#endif /* INVENTORY_HPP */
