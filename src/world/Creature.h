/*
 * AscEmu Framework based on ArcEmu MMORPG Server
 * Copyright (C) 2014-2015 AscEmu Team <http://www.ascemu.org>
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

#ifndef _WOWSERVER_CREATURE_H
#define _WOWSERVER_CREATURE_H

#include "CreatureDefines.hpp"
#include "UnitDefines.hpp"
#include "Map.h"
#include "Unit.h"
#include "DB2/DB2Structure.hpp"

class CreatureTemplate;
class GossipScript;

SERVER_DECL bool Rand(float chance);
SERVER_DECL bool Rand(uint32 chance);
SERVER_DECL bool Rand(int32 chance);

class CreatureAIScript;
class GossipScript;
class AuctionHouse;
struct Trainer;
class GameEvent;
struct QuestRelation;
class Quest;

#define CALL_SCRIPT_EVENT(obj, func) if (obj->IsInWorld() && obj->IsCreature() && static_cast<Creature*>(obj)->GetScript() != NULL) static_cast<Creature*>(obj)->GetScript()->func

uint8 get_byte(uint32 buffer, uint32 index);

///////////////////
/// Creature object

class SERVER_DECL Creature : public Unit
{
    public:

        Creature(uint64 guid);
        virtual ~Creature();

        GameEvent* mEvent = nullptr;
        
        /// For derived subclasses of Creature
    bool IsVehicle();

    void AddVehicleComponent(uint32 creature_entry, uint32 vehicleid);
        void RemoveVehicleComponent();

        bool Load(CreatureSpawn* spawn, uint32 mode, MapInfo* info);
        void Load(CreatureProto* proto_, float x, float y, float z, float o = 0);

        void AddToWorld();
        void AddToWorld(MapMgr* pMapMgr);
        void RemoveFromWorld(bool addrespawnevent, bool free_guid);
        void RemoveFromWorld(bool free_guid);

        void PrepareForRemove();    /// remove auras, guardians, scripts

        /// Creation
        void Create(const char* creature_name, uint32 mapid, float x, float y, float z, float ang);
        void CreateWayPoint(uint32 WayPointID, uint32 mapid, float x, float y, float z, float ang);

        /// Updates
        void Update(unsigned long time_passed);

        /// Creature inventory
        uint32 GetItemIdBySlot(uint32 slot) { return m_SellItems->at(slot).itemid; }
        uint32 GetItemAmountBySlot(uint32 slot) { return m_SellItems->at(slot).amount; }

    bool HasItems();

    CreatureProto* GetProto();

    bool IsPvPFlagged();
        void SetPvPFlag();
        void RemovePvPFlag();

        bool IsFFAPvPFlagged();
        void SetFFAPvPFlag();
        void RemoveFFAPvPFlag();

        bool IsSanctuaryFlagged();
        void SetSanctuaryFlag();
        void RemoveSanctuaryFlag();

        void SetSpeeds(uint8 type, float speed);

    int32 GetSlotByItemId(uint32 itemid);

    uint32 GetItemAmountByItemId(uint32 itemid);

    void GetSellItemBySlot(uint32 slot, CreatureItem& ci);

    void GetSellItemByItemId(uint32 itemid, CreatureItem& ci);

    ItemExtendedCostEntry* GetItemExtendedCostByItemId(uint32 itemid);

    std::vector<CreatureItem>::iterator GetSellItemBegin();

    std::vector<CreatureItem>::iterator GetSellItemEnd();

    size_t GetSellItemCount();

    void RemoveVendorItem(uint32 itemid);
    void AddVendorItem(uint32 itemid, uint32 amount, ItemExtendedCostEntry* ec);
        void ModAvItemAmount(uint32 itemid, uint32 value);
        void UpdateItemAmount(uint32 itemid);
        /// Quests
        void _LoadQuests();
    bool HasQuests();
    bool HasQuest(uint32 id, uint32 type);
    void AddQuest(QuestRelation* Q);
        void DeleteQuest(QuestRelation* Q);
        Quest const* FindQuest(uint32 quest_id, uint8 quest_relation);
        uint16 GetQuestRelation(uint32 quest_id);
        uint32 NumOfQuests();
    std::list<QuestRelation*>::iterator QuestsBegin();
    std::list<QuestRelation*>::iterator QuestsEnd();
    void SetQuestList(std::list<QuestRelation*>* qst_lst);

    uint32 isVendor() const;

    uint32 isTrainer() const;

    uint32 isClass() const;

    uint32 isProf() const;

    uint32 isQuestGiver() const;

    uint32 isGossip() const;

    uint32 isTaxi() const;

    uint32 isCharterGiver() const;

    uint32 isGuildBank() const;

    uint32 isBattleMaster() const;

    uint32 isBanker() const;

    uint32 isInnkeeper() const;

    uint32 isSpiritHealer() const;

    uint32 isTabardDesigner() const;

    uint32 isAuctioner() const;

    uint32 isStableMaster() const;

    uint32 isArmorer() const;

    uint32 GetHealthFromSpell();

    void SetHealthFromSpell(uint32 value);

    int32 m_speedFromHaste;
        int32 FlatResistanceMod[SCHOOL_COUNT];
        int32 BaseResistanceModPct[SCHOOL_COUNT];
        int32 ResistanceModPct[SCHOOL_COUNT];

        int32 FlatStatMod[5];
        int32 StatModPct[5];
        int32 TotalStatModPct[5];

        int32 ModDamageDone[SCHOOL_COUNT];
        float ModDamageDonePct[SCHOOL_COUNT];
        void CalcResistance(uint32 type);
        void CalcStat(uint32 type);

        bool m_canRegenerateHP;
        void RegenerateHealth();
        void RegenerateMana();
        int BaseAttackType;
        
    /// Invisibility & Stealth Detection - Partha
    bool CanSee(Unit* obj);

        /// Looting
        void generateLoot();
        bool HasLootForPlayer(Player* plr);

        bool Skinned;
        uint32 GetRequiredLootSkill();

        // Misc
    void setEmoteState(uint8 emote);
    uint32 GetSQL_id();

    virtual void setDeathState(DeathState s);

    uint32 GetOldEmote();

    void SendChatMessage(uint8 type, uint32 lang, const char* msg, uint32 delay = 0);
        void SendScriptTextChatMessage(uint32 textid);
        void SendTimedScriptTextChatMessage(uint32 textid, uint32 delay = 0);
        void SendChatMessageToPlayer(uint8 type, uint32 lang, const char* msg, Player* plr);

        // Serialization
        void SaveToDB();
        void LoadAIAgents(CreatureTemplate* t);
        void DeleteFromDB();

        void OnJustDied();
        void OnRemoveCorpse();
        void OnRespawn(MapMgr* m);

        void BuildPetSpellList(WorldPacket & data);

    protected:
        virtual void SafeDelete();      /// use DeleteMe() instead of SafeDelete() to avoid crashes like InWorld Creatures deleted.
    public:
        // In Range
        void AddInRangeObject(Object* pObj);
        void OnRemoveInRangeObject(Object* pObj);
        void ClearInRangeSet();

        // Demon
        void EnslaveExpire();

        // Pet
        void UpdatePet();
    uint32 GetEnslaveCount();

    void SetEnslaveCount(uint32 count);

    uint32 GetEnslaveSpell();

    void SetEnslaveSpell(uint32 spellId);
    bool RemoveEnslave();

        Object* GetPlayerOwner();

        Group* GetGroup();

    int32 GetDamageDoneMod(uint32 school);

    float GetDamageDonePctMod(uint32 school);

    bool IsPickPocketed();

    void SetPickPocketed(bool val = true);

    CreatureAIScript* GetScript();
    void LoadScript();

        void CallScriptUpdate();

    CreatureInfo* GetCreatureInfo();

    void SetCreatureInfo(CreatureInfo* ci);

    void SetCreatureProto(CreatureProto* cp);

    Trainer* GetTrainer();
    void RegenerateFocus();

        CreatureFamilyEntry* myFamily;

    bool IsExotic();


    bool isCritter();
    bool isTrainingDummy();

    void FormationLinkUp(uint32 SqlId);
        void ChannelLinkUpGO(uint32 SqlId);
        void ChannelLinkUpCreature(uint32 SqlId);
        bool haslinkupevent;
        WayPoint* CreateWaypointStruct();
        uint32 spawnid;
        uint32 original_emotestate;

        CreatureSpawn* m_spawn;

        void OnPushToWorld();
        virtual void Despawn(uint32 delay, uint32 respawntime);
        void TriggerScriptEvent(int);

        AuctionHouse* auctionHouse;

        void DeleteMe();
        bool CanAddToWorld();

        // scriptdev2
        uint32 GetNpcTextId();

        WayPointMap* m_custom_waypoint_map;
        void LoadWaypointGroup(uint32 pWaypointGroup);
        void LoadCustomWaypoint(float pX, float pY, float pZ, float pO, uint32 pWaitTime, uint32 pFlags, bool pForwardEmoteOneshot, uint32 pForwardEmoteId, bool pBackwardEmoteOneshot, uint32 pBackwardEmoteId, uint32 pForwardSkinId, uint32 pBackwardSkinId);
        void SwitchToCustomWaypoints();
        Player* m_escorter;
        void DestroyCustomWaypointMap();
    bool IsInLimboState();

    void SetLimboState(bool set);
    uint32 GetLineByFamily(CreatureFamilyEntry* family);
        void RemoveLimboState(Unit* healer);
        void SetGuardWaypoints();
        bool m_corpseEvent;
        MapCell* m_respawnCell;
        bool m_noRespawn;
        uint32 m_respawnTimeOverride;

        float GetBaseParry();
        bool isattackable(CreatureSpawn* spawn);

        void DealDamage(Unit* pVictim, uint32 damage, uint32 targetEvent, uint32 unitEvent, uint32 spellId, bool no_remove_auras = false);
        void TakeDamage(Unit* pAttacker, uint32 damage, uint32 spellid, bool no_remove_auras = false);
        void Die(Unit* pAttacker, uint32 damage, uint32 spellid);

        void HandleMonsterSayEvent(MONSTER_SAY_EVENTS Event);

    uint32 GetType();

    void SetType(uint32 t);

protected:
        CreatureAIScript* _myScriptClass;
        bool m_limbostate;
        Trainer* mTrainer;

        void _LoadGoods();
        void _LoadGoods(std::list<CreatureItem*>* lst);
        void _LoadMovement();

        /// Vendor data
        std::vector<CreatureItem>* m_SellItems;

        /// Taxi data
        uint32 mTaxiNode;

        /// Quest data
        std::list<QuestRelation*>* m_quests;

        uint32 m_enslaveCount;
        uint32 m_enslaveSpell;

        bool m_PickPocketed;
        uint32 _fields[UNIT_END];
        uint32 m_healthfromspell;

        CreatureInfo* creature_info;
        CreatureProto* proto;
    private:
        uint32 m_Creature_type;
};

#endif // _WOWSERVER_CREATURE_H
