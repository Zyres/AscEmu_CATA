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

#ifndef _QUESTMGR_H
#define _QUESTMGR_H

#include "QuestDefines.hpp"
#include "CommonTypes.hpp"
#include "QuestLogEntry.hpp"
#include "Gossip.h"

#include <vector>
#include <unordered_map>
#include <list>

class Quest;

struct QuestRelation
{
    Quest const* qst;
    uint8 type;
};

struct QuestAssociation
{
    Quest const* qst;
    uint8 item_count;
};

struct QuestPOIPoint
{
    int32 x;
    int32 y;

    QuestPOIPoint() : x(0), y(0) {}

    QuestPOIPoint(int32 px, int32 py) :
        x(px),
        y(py) {}
};

struct QuestPOI
{
    uint32 PoiId;
    int32 ObjectiveIndex;
    uint32 MapId;
    uint32 MapAreaId;
    uint32 FloorId;
    uint32 Unk3;
    uint32 Unk4;

    std::vector<QuestPOIPoint> points;

    QuestPOI() : PoiId(0), ObjectiveIndex(0), MapId(0), MapAreaId(0), FloorId(0), Unk3(0), Unk4(0) {}

    QuestPOI(uint32 poiId, int32 objIndex, uint32 mapId, uint32 mapAreaId, uint32 floorId, uint32 unk3, uint32 unk4) :
        PoiId(poiId),
        ObjectiveIndex(objIndex),
        MapId(mapId),
        MapAreaId(mapAreaId),
        FloorId(floorId),
        Unk3(unk3),
        Unk4(unk4) {}
};

typedef std::vector<QuestPOI> QuestPOIVector;
typedef std::unordered_map<uint32, QuestPOIVector> QuestPOIMap;

class Item;
typedef std::list<QuestRelation*> QuestRelationList;
typedef std::list<QuestAssociation*> QuestAssociationList;


class SERVER_DECL QuestMgr : public Singleton <QuestMgr>
{
    public:

        ~QuestMgr();

        uint32 PlayerMeetsReqs(Player* plr, Quest const* qst, bool skiplevelcheck);

        uint32 CalcStatus(Object* quest_giver, Player* plr);
        uint32 CalcQuestStatus(Object* quest_giver, Player* plr, QuestRelation* qst);
        uint32 CalcQuestStatus(Object* quest_giver, Player* plr, Quest const* qst, uint8 type, bool skiplevelcheck);
        uint32 CalcQuestStatus(Player* plr, uint32 qst);
        uint32 ActiveQuestsCount(Object* quest_giver, Player* plr);

        //Packet Forging...
        void BuildOfferReward(WorldPacket* data, Quest const* qst, Object* qst_giver, uint32 menutype, uint32 language, Player* plr);
        void BuildQuestDetails(WorldPacket* data, Quest const* qst, Object* qst_giver, uint32 menutype, uint32 language, Player* plr);
        void BuildRequestItems(WorldPacket* data, Quest const* qst, Object* qst_giver, uint32 status, uint32 language);
        void BuildQuestComplete(Player*, Quest const* qst);
        void BuildQuestList(WorldPacket* data, Object* qst_giver, Player* plr, uint32 language);
        bool OnActivateQuestGiver(Object* qst_giver, Player* plr);
        bool isRepeatableQuestFinished(Player* plr, Quest* qst);

        void SendQuestUpdateAddKill(Player* plr, uint32 questid, uint32 entry, uint32 count, uint32 tcount, uint64 guid);
        void BuildQuestUpdateAddItem(WorldPacket* data, uint32 itemid, uint32 count);
        void BuildQuestUpdateComplete(WorldPacket* data, Quest* qst);
        void BuildQuestFailed(WorldPacket* data, uint32 questid);
        void BuildQuestPOIResponse(WorldPacket & data, uint32 questid);
        void SendPushToPartyResponse(Player* plr, Player* pTarget, uint8 response);

        bool OnGameObjectActivate(Player* plr, GameObject* go);
        void OnPlayerKill(Player* plr, Creature* victim, bool IsGroupKill);
        void _OnPlayerKill(Player* plr, uint32 entry, bool IsGroupKill);
        void OnPlayerCast(Player* plr, uint32 spellid, uint64 & victimguid);
        void OnPlayerEmote(Player* plr, uint32 emoteid, uint64 & victimguid);
        void OnPlayerItemPickup(Player* plr, Item* item);
        void OnPlayerExploreArea(Player* plr, uint32 AreaID);
        void AreaExplored(Player* plr, uint32 QuestID);// scriptdev2

        void OnQuestAccepted(Player* plr, Quest const* qst, Object* qst_giver);
        void OnQuestFinished(Player* plr, Quest const* qst, Object* qst_giver, uint32 reward_slot);

        void GiveQuestRewardReputation(Player* plr, Quest const* qst, Object* qst_giver);

        uint32 GenerateQuestXP(Player* plr, Quest const* qst);
        uint32 GenerateRewardMoney(Player* plr, Quest const* qst);

        void SendQuestInvalid(QuestFailedReasons reason, Player* plyr);
        void SendQuestFailed(FAILED_REASON failed, Quest const* qst, Player* plyr);
        void SendQuestUpdateFailed(Quest const* pQuest, Player* plyr);
        void SendQuestUpdateFailedTimer(Quest const* pQuest, Player* plyr);
        void SendQuestLogFull(Player* plyr);

        void LoadNPCQuests(Creature* qst_giver);
        void LoadGOQuests(GameObject* go);

        QuestRelationList* GetCreatureQuestList(uint32 entryid);
        QuestRelationList* GetGOQuestList(uint32 entryid);
        QuestAssociationList* GetQuestAssociationListForItemId(uint32 itemId);
        uint32 GetGameObjectLootQuest(uint32 GO_Entry);
        void SetGameObjectLootQuest(uint32 GO_Entry, uint32 Item_Entry);

        bool CanStoreReward(Player* plyr, Quest const* qst, uint32 reward_slot);

        inline int32 QuestHasMob(Quest* qst, uint32 mob)
        {
            for (uint32 i = 0; i < 4; ++i)
                if (qst->ReqCreatureOrGOId[i] == (int32)mob)
                    return qst->ReqCreatureOrGOCount[i];
            return -1;
        }

        inline int32 GetOffsetForMob(Quest* qst, uint32 mob)
        {
            for (uint32 i = 0; i < 4; ++i)
                if (qst->ReqCreatureOrGOId[i] == (int32)mob)
                    return i;

            return -1;
        }

        inline int32 GetOffsetForItem(Quest* qst, uint32 itm)
        {
            for (uint32 i = 0; i < MAX_REQUIRED_QUEST_ITEM; ++i)
                if (qst->ReqItemId[i] == itm)
                    return i;

            return -1;
        }

        inline uint8 ClassByQuestSort(int32 QuestSort)
        {
            switch (QuestSort)
            {
            case QUEST_SORT_WARLOCK:        return WARLOCK;
            case QUEST_SORT_WARRIOR:        return WARRIOR;
            case QUEST_SORT_SHAMAN:         return SHAMAN;
            case QUEST_SORT_PALADIN:        return PALADIN;
            case QUEST_SORT_MAGE:           return MAGE;
            case QUEST_SORT_ROGUE:          return ROGUE;
            case QUEST_SORT_HUNTER:         return HUNTER;
            case QUEST_SORT_PRIEST:         return PRIEST;
            case QUEST_SORT_DRUID:          return DRUID;
            case QUEST_SORT_DEATH_KNIGHT:   return DEATHKNIGHT;
            }
            return 0;
        }

        inline uint32 SkillByQuestSort(int32 QuestSort)
        {
            switch (QuestSort)
            {
            case QUEST_SORT_HERBALISM:      return SKILL_HERBALISM;
            case QUEST_SORT_FISHING:        return SKILL_FISHING;
            case QUEST_SORT_BLACKSMITHING:  return SKILL_BLACKSMITHING;
            case QUEST_SORT_ALCHEMY:        return SKILL_ALCHEMY;
            case QUEST_SORT_LEATHERWORKING: return SKILL_LEATHERWORKING;
            case QUEST_SORT_ENGINERING:     return SKILL_ENGINERING;
            case QUEST_SORT_TAILORING:      return SKILL_TAILORING;
            case QUEST_SORT_COOKING:        return SKILL_COOKING;
            case QUEST_SORT_FIRST_AID:      return SKILL_FIRST_AID;
            case QUEST_SORT_JEWELCRAFTING:  return SKILL_JEWELCRAFTING;
            case QUEST_SORT_INSCRIPTION:    return SKILL_INSCRIPTION;
            case QUEST_SORT_ARCHAEOLOGY:    return SKILL_ARCHAEOLOGY;
            }
            return 0;
        }

        void LoadExtraQuestStuff();

        //////////////////////////////////////////////////////////////////////////////////////////
        /// Fills the packet with the quests that the quest giver which the player qualifies for.
        ///
        /// \param Creature* quest giver
        /// \param Player*  player for whom quests are qualified
        /// \param Arcemu::Gossip::Menu& - menu to fill with quests.
        ///
        /// \returns void
        ///
        //////////////////////////////////////////////////////////////////////////////////////////
        void FillQuestMenu(Creature*, Player*, Arcemu::Gossip::Menu &);

         std::unordered_map<uint32, uint32> m_ObjectLootQuestList;

    private:

        std::unordered_map<uint32, std::list<QuestRelation*>* > m_npc_quests;
        std::unordered_map<uint32, std::list<QuestRelation*>* > m_obj_quests;
        std::unordered_map<uint32, std::list<QuestRelation*>* > m_itm_quests;
        QuestPOIMap m_QuestPOIMap;

        std::unordered_map<uint32, std::list<QuestAssociation*>* > m_quest_associations;
        inline std::unordered_map<uint32, std::list<QuestAssociation*>* >& GetQuestAssociationList()
        {return m_quest_associations;}


        template <class T> void _AddQuest(uint32 entryid, Quest const* qst, uint8 type);


        template <class T> std::unordered_map<uint32, std::list<QuestRelation*>* >& _GetList();

        void AddItemQuestAssociation(uint32 itemId, Quest const* qst, uint8 item_count);

        // Quest Loading
        void _RemoveChar(char* c, std::string* str);
        void _CleanLine(std::string* str);
};

template<> inline std::unordered_map<uint32, std::list<QuestRelation*>* >& QuestMgr::_GetList<Creature>()
{ return m_npc_quests; }
template<> inline std::unordered_map<uint32, std::list<QuestRelation*>* >& QuestMgr::_GetList<GameObject>()
{ return m_obj_quests; }
template<> inline std::unordered_map<uint32, std::list<QuestRelation*>* >& QuestMgr::_GetList<Item>()
{ return m_itm_quests; }


#define sQuestMgr QuestMgr::getSingleton()

#endif // _QUESTMGR_H
