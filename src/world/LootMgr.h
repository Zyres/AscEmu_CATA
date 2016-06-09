/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org/>
 * Copyright (C) 2008-2012 ArcEmu Team <http://www.ArcEmu.org/>
 * Copyright (C) 2005-2007 Ascent Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LOOTMGR_H
#define _LOOTMGR_H

#include "EventableObject.h"
#include "Singleton.h"

#include <map>
#include <vector>
#include <set>

enum LOOTTYPE
{
    LOOT_NORMAL10,	    // normal dungeon / old raid (10/25/40 men) / normal 10 raid
    LOOT_NORMAL25,	    // heroic dungeon / normal 25 raid
    LOOT_HEROIC10,	    // heroic 10 men raid
    LOOT_HEROIC25,	    // heroic 25 men raid
    NUM_LOOT_TYPES
};

struct ItemPrototype;
class MapMgr;
struct RandomProps;
struct ItemRandomSuffixEntry;
class Player;

class LootRoll : public EventableObject
{
    public:

        LootRoll(uint32 timer, uint32 groupcount, uint64 guid, uint32 slotid, uint32 itemid, uint32 itemunk1, uint32 itemunk2, MapMgr* mgr);
        ~LootRoll();

        void PlayerRolled(Player* player, uint8 choice);
        void Finalize();
        int32 event_GetInstanceID();
    private:

        std::map<uint32, uint32> m_NeedRolls;
        std::map<uint32, uint32> m_GreedRolls;
    std::set<uint32> m_passRolls;
        uint32 _groupcount;
        uint32 _slotid;
        uint32 _itemid;
        uint32 _randomsuffixid;
        uint32 _randompropertyid;
        uint32 _remaining;
        uint64 _guid;
        MapMgr* _mgr;
};

typedef std::vector<std::pair<RandomProps*, float>> RandomPropertyVector;
typedef std::vector<std::pair<ItemRandomSuffixEntry*, float>> RandomSuffixVector;

typedef struct
{
    ItemPrototype* itemproto;
    uint32 displayid;
} _LootItem;

typedef std::set<uint32> LooterSet;

struct __LootItem
{
    _LootItem item;
    uint32 iItemsCount;
    RandomProps* iRandomProperty;
    ItemRandomSuffixEntry* iRandomSuffix;
    LootRoll* roll;
    bool passed;
    LooterSet has_looted;
    uint32 ffa_loot;
    bool looted;

    __LootItem()
    {
        looted = false;
        item.itemproto = nullptr;
        item.displayid = 0;
        iItemsCount = 0;
        iRandomProperty = nullptr;
        iRandomSuffix = nullptr;
        roll = nullptr;
        passed = false;
        ffa_loot = 0;
    }
};

struct LootCurencyStoreStruct
{
    uint32	entry;				//the NPC
    uint32	currency_type;
    uint32	currency_amt;
    uint32	max_level;			//if attacker has higher level then this then he will not receive points
    uint32	difficulty_mask;	// same mob might be present in multiple instance types but has to award different loot type
};


class ItemIsLooted
{
    public:

        bool operator()(const __LootItem &item)
        {
            if (item.looted)
                return true;
            else
                return false;
        }
};


class ItemIsNotLooted
{
    public:

        bool operator()(const __LootItem &item)
        {
            if (!item.looted)
                return true;
            else
                return false;
        }
};

typedef struct
{
    _LootItem item;	    /// the item that drops
    float chance;	    /// normal dungeon / normal 10men raid / old raid (10,25, or 40 men)
    float chance2;	    /// heroic dungeon / normal 25men raid
    float chance3;	    /// heroic 10men raid
    float chance4;	    /// heroic 25men raid
    uint32 mincount;	/// minimum quantity to drop
    uint32 maxcount;	/// maximum quantity to drop
    uint32 ffa_loot;	/// can everyone from the group loot the item?
} StoreLootItem;

typedef struct
{
    uint32 count;
    StoreLootItem* items;
} StoreLootList;

typedef struct
{
    uint32 CurrencyType;
    uint32 CurrencyCount;
    LooterSet has_looted;
}__LootCurrency;


typedef struct
{
    std::vector<__LootItem> items;
    std::vector<__LootCurrency> currencies;
    uint32 gold;
    LooterSet looters;
    bool HasRoll()
    {
        for (std::vector< __LootItem >::iterator itr = items.begin(); itr != items.end(); ++itr)
        {
            if (itr->roll != NULL)
                return true;
        }
        return false;
    }
} Loot;

struct tempy
{
    uint32 itemid;
    float chance;
    float chance_2;
    float chance3;
    float chance4;
    uint32 mincount;
    uint32 maxcount;
};

typedef std::map<uint32, StoreLootList> LootStore;

#define PARTY_LOOT_FFA 0
#define PARTY_LOOT_MASTER 2
#define PARTY_LOOT_RR 1
#define PARTY_LOOT_NBG 4
#define PARTY_LOOT_GROUP 3

typedef std::multimap<uint32, LootCurencyStoreStruct> LootCurrencyStore;
typedef std::pair<LootCurrencyStore::const_iterator, LootCurrencyStore::const_iterator> LootCurrencyIdBounds;

class SERVER_DECL LootMgr : public Singleton <LootMgr>
{
    public:
        LootMgr();
        ~LootMgr();

        void AddLoot(Loot* loot, uint32 itemid, uint32 mincount, uint32 maxcount);


        //////////////////////////////////////////////////////////////////////////////////////////
        /// bool HasLootForCreature(uint32 loot_id)
        /// Tells if there's loot cached for the specified creature.
        ///
        /// \param uint32 loot_id - The identifier of the creature
        ///
        /// \return true if there's loot for this creature, false otherwise.
        ///
        //////////////////////////////////////////////////////////////////////////////////////////
        bool HasLootForCreature(uint32 loot_id);

        void FillCreatureLoot(Loot* loot, uint32 loot_id, uint32 type);
        void FillGOLoot(Loot* loot, uint32 loot_id, uint32 type);
        void FillItemLoot(Loot* loot, uint32 loot_id);
        void FillFishingLoot(Loot* loot, uint32 loot_id);
        void FillSkinningLoot(Loot* loot, uint32 loot_id);
        void FillPickpocketingLoot(Loot* loot, uint32 loot_id);
        void FillCurrencyLoot(Loot* loot, uint32 loot_id);
        bool CanGODrop(uint32 LootId, uint32 itemid);
        bool IsPickpocketable(uint32 creatureId);
        bool IsSkinnable(uint32 creatureId);
        bool IsFishable(uint32 zoneid);

        void LoadLoot();
        void LoadLootProp();
        void LoadCurrencyLoot();

        LootCurrencyIdBounds GetLootCurrencyIdBounds(uint32 entry) const;

        LootStore CreatureLoot;
        LootStore FishingLoot;
        LootStore SkinningLoot;
        LootStore GOLoot;
        LootStore ItemLoot;
        LootStore PickpocketingLoot;

        std::map<uint32, std::set<uint32>> quest_loot_go;

        RandomProps* GetRandomProperties(ItemPrototype* proto);
        ItemRandomSuffixEntry* GetRandomSuffix(ItemPrototype* proto);

        bool is_loading;
    private:

        void LoadLootTables(const char* szTableName, LootStore* LootTable);
        void PushLoot(StoreLootList* list, Loot* loot, uint32 type);
        std::map<uint32, RandomPropertyVector> _randomprops;
        std::map<uint32, RandomSuffixVector> _randomsuffix;

        LootCurrencyStore mLootCurrency;
};

#define lootmgr LootMgr::getSingleton()

#endif // _LOOTMGR_H
