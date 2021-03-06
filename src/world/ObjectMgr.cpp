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
 *
 */

#include "StdAfx.h"
#include "QuestLogEntry.hpp"
#include "DB2/DB2Stores.hpp"

initialiseSingleton(ObjectMgr);

const char* NormalTalkMessage = "DMSG";

ObjectMgr::ObjectMgr() :
m_hiItemGuid(0),
m_hiGroupId(0),
m_mailid(0),
m_reportID(0),
m_ticketid(0),
m_setGUID(0),
m_hiCorpseGuid(0),
m_hiGuildId(0),
m_hiPetGuid(0),
m_hiArenaTeamId(0),
TransportersCount(0),
m_hiPlayerGuid(1)
{
    memset(m_InstanceBossInfoMap, 0, sizeof(InstanceBossInfoMap*) * NUM_MAPS);
}


ObjectMgr::~ObjectMgr()
{
    Log.Notice("ObjectMgr", "Deleting Corpses...");
    CorpseCollectorUnload();

    Log.Notice("ObjectMgr", "Deleting Itemsets...");
    for (ItemSetContentMap::iterator i = mItemSets.begin(); i != mItemSets.end(); ++i)
    {
        delete i->second;
    }
    mItemSets.clear();

    Log.Notice("ObjectMgr", "Deleting PlayerCreateInfo...");
    for (PlayerCreateInfoMap::iterator i = mPlayerCreateInfo.begin(); i != mPlayerCreateInfo.end(); ++i)
    {
        delete i->second;
    }
    mPlayerCreateInfo.clear();

    Log.Notice("ObjectMgr", "Deleting Guilds...");
    for (GuildMap::iterator i = mGuild.begin(); i != mGuild.end(); ++i)
    {
        delete i->second;
    }

    Log.Notice("ObjectMgr", "Deleting Vendors...");
    for (VendorMap::iterator i = mVendors.begin(); i != mVendors.end(); ++i)
    {
        delete i->second;
    }

    Log.Notice("ObjectMgr", "Deleting Spell Override...");
    for (OverrideIdMap::iterator i = mOverrideIdMap.begin(); i != mOverrideIdMap.end(); ++i)
    {
        delete i->second;
    }

    Log.Notice("ObjectMgr", "Deleting Trainers...");
    for (TrainerMap::iterator i = mTrainers.begin(); i != mTrainers.end(); ++i)
    {
        Trainer* t = i->second;
        if (t->UIMessage && t->UIMessage != (char*)NormalTalkMessage)
            delete[] t->UIMessage;
        delete t;
    }

    Log.Notice("ObjectMgr", "Deleting Level Information...");
    for (LevelInfoMap::iterator i = mLevelInfo.begin(); i != mLevelInfo.end(); ++i)
    {
        LevelMap* l = i->second;
        for (LevelMap::iterator i2 = l->begin(); i2 != l->end(); ++i2)
        {
            delete i2->second;
        }
        l->clear();
        delete l;
    }

    Log.Notice("ObjectMgr", "Deleting Waypoint Cache...");
    for (std::unordered_map<uint32, WayPointMap*>::iterator i = m_waypoints.begin(); i != m_waypoints.end(); ++i)
    {
        for (WayPointMap::iterator i2 = i->second->begin(); i2 != i->second->end(); ++i2)
            if ((*i2))
                delete(*i2);

        delete i->second;
    }

    Log.Notice("ObjectMgr", "Deleting timed emote Cache...");
    for (std::unordered_map<uint32, TimedEmoteList*>::iterator i = m_timedemotes.begin(); i != m_timedemotes.end(); ++i)
    {
        for (TimedEmoteList::iterator i2 = i->second->begin(); i2 != i->second->end(); ++i2)
            if ((*i2))
            {
                delete[](*i2)->msg;
                delete(*i2);
            }

        delete i->second;
    }

    Log.Notice("ObjectMgr", "Deleting NPC Say Texts...");
    for (uint32 i = 0; i < NUM_MONSTER_SAY_EVENTS; ++i)
    {
        NpcMonsterSay* p;
        for (MonsterSayMap::iterator itr = mMonsterSays[i].begin(); itr != mMonsterSays[i].end(); ++itr)
        {
            p = itr->second;
            for (uint32 j = 0; j < p->TextCount; ++j)
                free((char*)p->Texts[j]);
            delete[] p->Texts;
            free((char*)p->MonsterName);
            delete p;
        }

        mMonsterSays[i].clear();
    }

    Log.Notice("ObjectMgr", "Deleting Charters...");
    for (int i = 0; i < NUM_CHARTER_TYPES; ++i)
    {
        for (std::unordered_map<uint32, Charter*>::iterator itr = m_charters[i].begin(); itr != m_charters[i].end(); ++itr)
        {
            delete itr->second;
        }
    }

    Log.Notice("ObjectMgr", "Deleting Reputation Tables...");
    for (ReputationModMap::iterator itr = this->m_reputation_creature.begin(); itr != m_reputation_creature.end(); ++itr)
    {
        ReputationModifier* mod = itr->second;
        mod->mods.clear();
        delete mod;
    }
    for (ReputationModMap::iterator itr = this->m_reputation_faction.begin(); itr != m_reputation_faction.end(); ++itr)
    {
        ReputationModifier* mod = itr->second;
        mod->mods.clear();
        delete mod;
    }

    for (std::unordered_map<uint32, InstanceReputationModifier*>::iterator itr = this->m_reputation_instance.begin(); itr != this->m_reputation_instance.end(); ++itr)
    {
        InstanceReputationModifier* mod = itr->second;
        mod->mods.clear();
        delete mod;
    }

    Log.Notice("ObjectMgr", "Deleting Groups...");
    for (GroupMap::iterator itr = m_groups.begin(); itr != m_groups.end();)
    {
        Group* pGroup = itr->second;
        ++itr;

        if (pGroup != NULL)
        {
            for (uint32 i = 0; i < pGroup->GetSubGroupCount(); ++i)
            {
                SubGroup* pSubGroup = pGroup->GetSubGroup(i);
                if (pSubGroup != NULL)
                {
                    pSubGroup->Disband();
                }
            }
            delete pGroup;
        }
    }

    Log.Notice("ObjectMgr", "Deleting Player Information...");
    for (std::unordered_map<uint32, PlayerInfo*>::iterator itr = m_playersinfo.begin(); itr != m_playersinfo.end(); ++itr)
    {
        itr->second->m_Group = NULL;
        free(itr->second->name);
        delete itr->second;
    }

    Log.Notice("ObjectMgr", "Deleting GM Tickets...");
    for (GmTicketList::iterator itr = GM_TicketList.begin(); itr != GM_TicketList.end(); ++itr)
        delete(*itr);

    Log.Notice("ObjectMgr", "Deleting Boss Information...");
    for (uint32 i = 0; i < NUM_MAPS; i++)
    {
        if (this->m_InstanceBossInfoMap[i] != NULL)
        {
            for (InstanceBossInfoMap::iterator itr = this->m_InstanceBossInfoMap[i]->begin(); itr != m_InstanceBossInfoMap[i]->end(); ++itr)
                delete(*itr).second;
            delete this->m_InstanceBossInfoMap[i];
            this->m_InstanceBossInfoMap[i] = NULL;
        }
    }

    Log.Notice("ObjectMgr", "Deleting Arena Teams...");
    for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeams.begin(); itr != m_arenaTeams.end(); ++itr)
    {
        delete(*itr).second;
    }

    Log.Notice("ObjectMgr", "Deleting Profession Discoveries...");
    std::set<ProfessionDiscovery*>::iterator itr = ProfessionDiscoveryTable.begin();
    for (; itr != ProfessionDiscoveryTable.end(); itr++)
        delete(*itr);

    Log.Notice("ObjectMgr", "Cleaning up BroadCastStorages...");
    m_BCEntryStorage.clear();

    Log.Notice("ObjectMgr", "Cleaning up spell target constraints...");
    for (SpellTargetConstraintMap::iterator itr = m_spelltargetconstraints.begin(); itr != m_spelltargetconstraints.end(); ++itr)
        delete itr->second;

    m_spelltargetconstraints.clear();

    Log.Notice("ObjectMgr", "Cleaning up vehicle accessories...");
    for (std::map< uint32, std::vector< VehicleAccessoryEntry* >* >::iterator itr = vehicle_accessories.begin(); itr != vehicle_accessories.end(); ++itr)
    {
        std::vector< VehicleAccessoryEntry* > *v = itr->second;

        for (std::vector< VehicleAccessoryEntry* >::iterator itr2 = v->begin(); itr2 != v->end(); ++itr2)
            delete *itr2;
        v->clear();

        delete v;
    }

    vehicle_accessories.clear();


    Log.Notice("ObjectMgr", "Cleaning up worldstate templates");
    for (std::map< uint32, std::multimap< uint32, WorldState >* >::iterator itr = worldstate_templates.begin(); itr != worldstate_templates.end(); ++itr)
    {
        itr->second->clear();
        delete itr->second;
    }

    worldstate_templates.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Groups
Group* ObjectMgr::GetGroupByLeader(Player* pPlayer)
{
    GroupMap::iterator itr;
    Group* ret = NULL;
    m_groupLock.AcquireReadLock();
    for (itr = m_groups.begin(); itr != m_groups.end(); ++itr)
    {
        if (itr->second->GetLeader() == pPlayer->getPlayerInfo())
        {
            ret = itr->second;
            break;
        }
    }

    m_groupLock.ReleaseReadLock();
    return ret;
}

Group* ObjectMgr::GetGroupById(uint32 id)
{
    GroupMap::iterator itr;
    Group* ret = NULL;
    m_groupLock.AcquireReadLock();
    itr = m_groups.find(id);
    if (itr != m_groups.end())
        ret = itr->second;

    m_groupLock.ReleaseReadLock();
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// Player names
void ObjectMgr::DeletePlayerInfo(uint32 guid)
{
    PlayerInfo* pl;
    std::unordered_map<uint32, PlayerInfo*>::iterator i;
    PlayerNameStringIndexMap::iterator i2;
    playernamelock.AcquireWriteLock();
    i = m_playersinfo.find(guid);
    if (i == m_playersinfo.end())
    {
        playernamelock.ReleaseWriteLock();
        return;
    }

    pl = i->second;
    if (pl->m_Group)
    {
        pl->m_Group->RemovePlayer(pl);
    }

    if (pl->guild)
    {
        if (pl->guild->GetGuildLeader() == pl->guid)
            pl->guild->Disband();
        else
            pl->guild->RemoveGuildMember(pl, NULL);
    }

    std::string pnam = std::string(pl->name);
    arcemu_TOLOWER(pnam);
    i2 = m_playersInfoByName.find(pnam);
    if (i2 != m_playersInfoByName.end() && i2->second == pl)
        m_playersInfoByName.erase(i2);

    free(pl->name);
    delete i->second;
    m_playersinfo.erase(i);

    playernamelock.ReleaseWriteLock();
}

PlayerInfo* ObjectMgr::GetPlayerInfo(uint32 guid)
{
    std::unordered_map<uint32, PlayerInfo*>::iterator i;
    PlayerInfo* rv;
    playernamelock.AcquireReadLock();
    i = m_playersinfo.find(guid);
    if (i != m_playersinfo.end())
        rv = i->second;
    else
        rv = NULL;
    playernamelock.ReleaseReadLock();
    return rv;
}

void ObjectMgr::AddPlayerInfo(PlayerInfo* pn)
{
    playernamelock.AcquireWriteLock();
    m_playersinfo[pn->guid] = pn;
    std::string pnam = std::string(pn->name);
    arcemu_TOLOWER(pnam);
    m_playersInfoByName[pnam] = pn;
    playernamelock.ReleaseWriteLock();
}

void ObjectMgr::RenamePlayerInfo(PlayerInfo* pn, const char* oldname, const char* newname)
{
    playernamelock.AcquireWriteLock();
    std::string oldn = std::string(oldname);
    arcemu_TOLOWER(oldn);

    PlayerNameStringIndexMap::iterator itr = m_playersInfoByName.find(oldn);
    if (itr != m_playersInfoByName.end() && itr->second == pn)
    {
        std::string newn = std::string(newname);
        arcemu_TOLOWER(newn);
        m_playersInfoByName.erase(itr);
        m_playersInfoByName[newn] = pn;
    }

    playernamelock.ReleaseWriteLock();
}

void ObjectMgr::LoadSpellSkills()
{
    uint32 i;
    //    int total = sSkillStore.GetNumRows();

    for (i = 0; i < dbcSkillLineSpell.GetNumRows(); i++)
    {
        skilllinespell* sp = dbcSkillLineSpell.LookupRowForced(i);
        if (sp)
        {
            mSpellSkills[sp->spell] = sp;
        }
    }
    Log.Success("ObjectMgr", "%u spell skills loaded.", mSpellSkills.size());
}

skilllinespell* ObjectMgr::GetSpellSkill(uint32 id)
{
    return mSpellSkills[id];
}

SpellEntry* ObjectMgr::GetNextSpellRank(SpellEntry* sp, uint32 level)
{
    // Looks for next spell rank
    if (sp == NULL)
        return NULL;

    skilllinespell* skill = GetSpellSkill(sp->Id);
    if (skill != NULL && skill->next > 0)
    {
        SpellEntry* sp1 = dbcSpell.LookupEntry(skill->next);
        if (sp1->SpellLevel.baseLevel <= level)   // check level
            return GetNextSpellRank(sp1, level);   // recursive for higher ranks
    }
    return sp;
}

void ObjectMgr::LoadPlayersInfo()
{
    PlayerInfo* pn;
    QueryResult* result = CharacterDatabase.Query("SELECT guid, name, race, class, level, gender, zoneid, timestamp, acct FROM characters");
    uint32 period, c;
    if (result)
    {
        period = (result->GetRowCount() / 20) + 1;
        c = 0;
        do
        {
            Field* fields = result->Fetch();
            pn = new PlayerInfo;
            pn->guid = fields[0].GetUInt32();
            pn->name = strdup(fields[1].GetString());
            pn->race = fields[2].GetUInt8();
            pn->cl = fields[3].GetUInt8();
            pn->lastLevel = fields[4].GetUInt32();
            pn->gender = fields[5].GetUInt8();
            pn->lastZone = fields[6].GetUInt32();
            pn->lastOnline = fields[7].GetUInt32();
            pn->acct = fields[8].GetUInt32();
            pn->m_Group = 0;
            pn->subGroup = 0;
            pn->m_loggedInPlayer = NULL;
            pn->guild = NULL;
            pn->guildRank = NULL;
            pn->guildMember = NULL;

            // Raid & heroic Instance IDs
            // Must be done before entering world...
            QueryResult* result2 = CharacterDatabase.Query("SELECT instanceid, mode, mapid FROM instanceids WHERE playerguid = %u", pn->guid);
            if (result2)
            {
                PlayerInstanceMap::iterator itr;
                do
                {
                    uint32 instanceId = result2->Fetch()[0].GetUInt32();
                    uint32 mode = result2->Fetch()[1].GetUInt32();
                    uint32 mapId = result2->Fetch()[2].GetUInt32();
                    if (mode >= NUM_INSTANCE_MODES || mapId >= NUM_MAPS)
                        continue;

                    pn->savedInstanceIdsLock.Acquire();
                    itr = pn->savedInstanceIds[mode].find(mapId);
                    if (itr == pn->savedInstanceIds[mode].end())
                        pn->savedInstanceIds[mode].insert(PlayerInstanceMap::value_type(mapId, instanceId));
                    else
                        (*itr).second = instanceId;

                    ///\todo Instances not loaded yet ~.~
                    //if (!sInstanceMgr.InstanceExists(mapId, pn->m_savedInstanceIds[mapId][mode]))
                    //{
                    //    pn->m_savedInstanceIds[mapId][mode] = 0;
                    //    CharacterDatabase.Execute("DELETE FROM instanceids WHERE mapId = %u AND instanceId = %u AND mode = %u", mapId, instanceId, mode);
                    //}

                    pn->savedInstanceIdsLock.Release();
                }
                while (result2->NextRow());
                delete result2;
            }

            if (pn->race == RACE_HUMAN || pn->race == RACE_DWARF || pn->race == RACE_GNOME || pn->race == RACE_NIGHTELF || pn->race == RACE_DRAENEI || pn->race == RACE_WORGEN)
                pn->team = 0;
            else
                pn->team = 1;

            if (GetPlayerInfoByName(pn->name) != NULL)
            {
                // gotta rename him
                char temp[300];
                snprintf(temp, 300, "%s__%X__", pn->name, pn->guid);
                Log.Notice("ObjectMgr", "Renaming duplicate player %s to %s. (%u)", pn->name, temp, pn->guid);
                CharacterDatabase.WaitExecute("UPDATE characters SET name = '%s', login_flags = %u WHERE guid = %u",
                                              CharacterDatabase.EscapeString(std::string(temp)).c_str(), (uint32)LOGIN_FORCED_RENAME, pn->guid);


                free(pn->name);
                pn->name = strdup(temp);
            }

            std::string lpn = std::string(pn->name);
            arcemu_TOLOWER(lpn);
            m_playersInfoByName[lpn] = pn;

            //this is startup -> no need in lock -> don't use addplayerinfo
            m_playersinfo[(uint32)pn->guid] = pn;

            if (!((++c) % period))
                Log.Notice("PlayerInfo", "Done %u/%u, %u%% complete.", c, result->GetRowCount(), c * 100 / result->GetRowCount());
        }
        while (result->NextRow());

        delete result;
    }
    Log.Success("ObjectMgr", "%u players loaded.", m_playersinfo.size());
    LoadGuilds();
}

PlayerInfo* ObjectMgr::GetPlayerInfoByName(const char* name)
{
    std::string lpn = std::string(name);
    arcemu_TOLOWER(lpn);
    PlayerNameStringIndexMap::iterator i;
    PlayerInfo* rv = NULL;
    playernamelock.AcquireReadLock();

    i = m_playersInfoByName.find(lpn);
    if (i != m_playersInfoByName.end())
        rv = i->second;

    playernamelock.ReleaseReadLock();
    return rv;
}

#ifdef ENABLE_ACHIEVEMENTS
void ObjectMgr::LoadCompletedAchievements()
{
    QueryResult* result = CharacterDatabase.Query("SELECT achievement FROM character_achievement GROUP BY achievement");

    if (!result)
    {
        Log.Error("MySQL", "Query failed: SELECT achievement FROM character_achievement");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        allCompletedAchievements.insert(fields[0].GetUInt32());
    }
    while (result->NextRow());
    delete result;
}
#endif

void ObjectMgr::LoadPlayerCreateInfo()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM playercreateinfo");

    if (!result)
    {
        Log.Error("MySQL", "Query failed: SELECT * FROM playercreateinfo");
        return;
    }
    if (result->GetFieldCount() != 25)
    {
        Log.Error("MySQL", "Wrong field count in table playercreateinfo (got %lu, need 25)", result->GetFieldCount());
        delete result;
        return;
    }

    PlayerCreateInfo* pPlayerCreateInfo;

    do
    {
        Field* fields = result->Fetch();
        pPlayerCreateInfo = new PlayerCreateInfo;

        pPlayerCreateInfo->index = fields[0].GetUInt8();
        pPlayerCreateInfo->race = fields[1].GetUInt8();
        pPlayerCreateInfo->factiontemplate = fields[2].GetUInt32();
        pPlayerCreateInfo->class_ = fields[3].GetUInt8();
        pPlayerCreateInfo->mapId = fields[4].GetUInt32();
        pPlayerCreateInfo->zoneId = fields[5].GetUInt32();
        pPlayerCreateInfo->positionX = fields[6].GetFloat();
        pPlayerCreateInfo->positionY = fields[7].GetFloat();
        pPlayerCreateInfo->positionZ = fields[8].GetFloat();
        pPlayerCreateInfo->displayId = fields[9].GetUInt16();
        pPlayerCreateInfo->strength = fields[10].GetUInt8();
        pPlayerCreateInfo->ability = fields[11].GetUInt8();
        pPlayerCreateInfo->stamina = fields[12].GetUInt8();
        pPlayerCreateInfo->intellect = fields[13].GetUInt8();
        pPlayerCreateInfo->spirit = fields[14].GetUInt8();
        pPlayerCreateInfo->health = fields[15].GetUInt32();
        pPlayerCreateInfo->mana = fields[16].GetUInt32();
        pPlayerCreateInfo->rage = fields[17].GetUInt32();
        pPlayerCreateInfo->focus = fields[18].GetUInt32();
        pPlayerCreateInfo->energy = fields[19].GetUInt32();
        pPlayerCreateInfo->attackpower = fields[20].GetUInt32();
        pPlayerCreateInfo->mindmg = fields[21].GetFloat();
        pPlayerCreateInfo->maxdmg = fields[22].GetFloat();
        pPlayerCreateInfo->introid = fields[23].GetUInt32();

        std::string taxiMaskStr = fields[24].GetString();
        std::vector<std::string> tokens = StrSplit(taxiMaskStr, " ");

        memset(pPlayerCreateInfo->taximask, 0, sizeof(pPlayerCreateInfo->taximask));
        int index;
        std::vector<std::string>::iterator iter;
        for (iter = tokens.begin(), index = 0; (index < 12) && (iter != tokens.end()); ++iter, ++index)
        {
            pPlayerCreateInfo->taximask[index] = atol((*iter).c_str());
        }

        QueryResult* sk_sql = WorldDatabase.Query("SELECT * FROM playercreateinfo_skills WHERE indexid=%u", pPlayerCreateInfo->index);

        if (sk_sql)
        {
            do
            {
                Field* fields2 = sk_sql->Fetch();
                CreateInfo_SkillStruct tsk;
                tsk.skillid = fields2[1].GetUInt32();
                tsk.currentval = fields2[2].GetUInt32();
                tsk.maxval = fields2[3].GetUInt32();
                pPlayerCreateInfo->skills.push_back(tsk);
            }
            while (sk_sql->NextRow());
            delete sk_sql;
        }
        QueryResult* sp_sql = WorldDatabase.Query("SELECT * FROM playercreateinfo_spells WHERE indexid=%u", pPlayerCreateInfo->index);

        if (sp_sql)
        {
            do
            {
                pPlayerCreateInfo->spell_list.insert(sp_sql->Fetch()[1].GetUInt32());
            }
            while (sp_sql->NextRow());
            delete sp_sql;
        }

        QueryResult* items_sql = WorldDatabase.Query("SELECT * FROM playercreateinfo_items WHERE indexid=%u", pPlayerCreateInfo->index);

        if (items_sql)
        {
            do
            {
                Field* fields2 = items_sql->Fetch();
                CreateInfo_ItemStruct itm;
                itm.protoid = fields2[1].GetUInt32();
                itm.slot = fields2[2].GetUInt8();
                itm.amount = fields2[3].GetUInt32();
                pPlayerCreateInfo->items.push_back(itm);
            }
            while (items_sql->NextRow());
            delete items_sql;
        }

        QueryResult* bars_sql = WorldDatabase.Query("SELECT * FROM playercreateinfo_bars WHERE class=%u", pPlayerCreateInfo->class_);

        if (bars_sql)
        {
            do
            {
                Field* fields2 = bars_sql->Fetch();
                CreateInfo_ActionBarStruct bar;
                bar.button = fields2[2].GetUInt32();
                bar.action = fields2[3].GetUInt32();
                bar.type = fields2[4].GetUInt32();
                bar.misc = fields2[5].GetUInt32();
                pPlayerCreateInfo->actionbars.push_back(bar);
            }
            while (bars_sql->NextRow());
            delete bars_sql;
        }

        mPlayerCreateInfo[pPlayerCreateInfo->index] = pPlayerCreateInfo;
    }
    while (result->NextRow());

    delete result;

    Log.Success("ObjectMgr", "%u player create infos loaded.", mPlayerCreateInfo.size());
    LoadXpToLevelTable();
    GenerateLevelUpInfo();
}

//////////////////////////////////////////////////////////////////////////////////////////
// DK:LoadGuilds()
void ObjectMgr::LoadGuilds()
{
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM guilds");
    if (result)
    {
        uint32 period = (result->GetRowCount() / 20) + 1;
        uint32 c = 0;
        do
        {
            Guild* pGuild = Guild::Create();
            if (!pGuild->LoadFromDB(result->Fetch()))
            {
                delete pGuild;
            }
            else
                mGuild.insert(std::make_pair(pGuild->GetGuildId(), pGuild));

            if (!((++c) % period))
                Log.Notice("Guilds", "Done %u/%u, %u%% complete.", c, result->GetRowCount(), c * 100 / result->GetRowCount());

        }
        while (result->NextRow());
        delete result;
    }
    Log.Success("ObjectMgr", "%u guilds loaded.", mGuild.size());
}

Corpse* ObjectMgr::LoadCorpse(uint32 guid)
{
    Corpse* pCorpse;
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM corpses WHERE guid =%u ", guid);

    if (!result)
        return NULL;

    do
    {
        Field* fields = result->Fetch();
        pCorpse = new Corpse(HIGHGUID_TYPE_CORPSE, fields[0].GetUInt32());
        pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
        pCorpse->SetZoneId(fields[5].GetUInt32());
        pCorpse->SetMapId(fields[6].GetUInt32());
        pCorpse->LoadValues(fields[7].GetString());
        if (pCorpse->GetDisplayId() == 0)
        {
            delete pCorpse;
            pCorpse = NULL;
            continue;
        }

        pCorpse->SetLoadedFromDB(true);
        pCorpse->SetInstanceID(fields[8].GetUInt32());
        pCorpse->AddToWorld();
    }
    while (result->NextRow());

    delete result;

    return pCorpse;
}


//////////////////////////////////////////////////////////////////////////////////////////
/// Live corpse retreival.
/// comments: I use the same tricky method to start from the last corpse instead of the first
//////////////////////////////////////////////////////////////////////////////////////////
Corpse* ObjectMgr::GetCorpseByOwner(uint32 ownerguid)
{
    CorpseMap::const_iterator itr;
    Corpse* rv = NULL;
    _corpseslock.Acquire();
    for (itr = m_corpses.begin(); itr != m_corpses.end(); ++itr)
    {
        if (GET_LOWGUID_PART(itr->second->GetOwner()) == ownerguid)
        {
            rv = itr->second;
            break;
        }
    }
    _corpseslock.Release();


    return rv;
}

void ObjectMgr::DelinkPlayerCorpses(Player* pOwner)
{
    //dupe protection agaisnt crashs
    Corpse* c;
    c = this->GetCorpseByOwner(pOwner->GetLowGUID());
    if (!c)return;
    sEventMgr.AddEvent(c, &Corpse::Delink, EVENT_CORPSE_SPAWN_BONES, 1, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
    CorpseAddEventDespawn(c);
}

void ObjectMgr::LoadGMTickets()
{
    QueryResult* result = CharacterDatabase.Query("SELECT ticketid, playerGuid, name, level, map, posX, posY, posZ, message, timestamp, deleted, assignedto, comment FROM gm_tickets WHERE deleted = false");

    GM_Ticket* ticket;
    if (result == 0)
        return;

    uint32 deleted = 0;

    do
    {
        Field* fields = result->Fetch();

        ticket = new GM_Ticket;
        ticket->guid = fields[0].GetUInt64();
        ticket->playerGuid = fields[1].GetUInt64();
        ticket->name = fields[2].GetString();
        ticket->level = fields[3].GetUInt32();
        ticket->map = fields[4].GetUInt32();
        ticket->posX = fields[5].GetFloat();
        ticket->posY = fields[6].GetFloat();
        ticket->posZ = fields[7].GetFloat();
        ticket->message = fields[8].GetString();
        ticket->timestamp = fields[9].GetUInt32();

        deleted = fields[10].GetUInt32();

        if (deleted == 1)
            ticket->deleted = true;
        else
            ticket->deleted = false;

        ticket->assignedToPlayer = fields[11].GetUInt64();
        ticket->comment = fields[12].GetString();

        AddGMTicket(ticket, true);

    }
    while (result->NextRow());

    Log.Success("ObjectMgr", "%u active GM Tickets loaded.", result->GetRowCount());
    delete result;
}

void ObjectMgr::LoadInstanceBossInfos()
{
    char* p, *q, *trash;
    MapInfo* mapInfo;
    QueryResult* result = WorldDatabase.Query("SELECT mapid, creatureid, trash, trash_respawn_override FROM instance_bosses");

    if (result == NULL)
        return;

    uint32 cnt = 0;
    do
    {
        InstanceBossInfo* bossInfo = new InstanceBossInfo();
        bossInfo->mapid = (uint32)result->Fetch()[0].GetUInt32();

        mapInfo = WorldMapInfoStorage.LookupEntry(bossInfo->mapid);
        if (mapInfo == NULL || mapInfo->type == INSTANCE_NULL)
        {
            LOG_DETAIL("Not loading boss information for map %u! (continent or unknown map)", bossInfo->mapid);
            delete bossInfo;
            continue;
        }
        if (bossInfo->mapid >= NUM_MAPS)
        {
            LOG_DETAIL("Not loading boss information for map %u! (map id out of range)", bossInfo->mapid);
            delete bossInfo;
            continue;
        }

        bossInfo->creatureid = (uint32)result->Fetch()[1].GetUInt32();
        trash = strdup(result->Fetch()[2].GetString());
        q = trash;
        p = strchr(q, ' ');
        while (p)
        {
            *p = 0;
            uint32 val = atoi(q);
            if (val)
                bossInfo->trash.insert(val);
            q = p + 1;
            p = strchr(q, ' ');
        }
        free(trash);
        bossInfo->trashRespawnOverride = (uint32)result->Fetch()[3].GetUInt32();


        if (this->m_InstanceBossInfoMap[bossInfo->mapid] == NULL)
            this->m_InstanceBossInfoMap[bossInfo->mapid] = new InstanceBossInfoMap;
        this->m_InstanceBossInfoMap[bossInfo->mapid]->insert(InstanceBossInfoMap::value_type(bossInfo->creatureid, bossInfo));
        cnt++;
    }
    while (result->NextRow());

    delete result;
    Log.Success("ObjectMgr", "%u boss information loaded.", cnt);
}

void ObjectMgr::SaveGMTicket(GM_Ticket* ticket, QueryBuffer* buf)
{
    std::stringstream ss;

    ss << "DELETE FROM gm_tickets WHERE ticketid = ";
    ss << ticket->guid;
    ss << ";";

    if (buf == NULL)
        CharacterDatabase.ExecuteNA(ss.str().c_str());
    else
        buf->AddQueryStr(ss.str());

    ss.rdbuf()->str("");

    ss << "INSERT INTO gm_tickets (ticketid, playerguid, name, level, map, posX, posY, posZ, message, timestamp, deleted, assignedto, comment) VALUES(";
    ss << ticket->guid << ", ";
    ss << ticket->playerGuid << ", '";
    ss << CharacterDatabase.EscapeString(ticket->name) << "', ";
    ss << ticket->level << ", ";
    ss << ticket->map << ", ";
    ss << ticket->posX << ", ";
    ss << ticket->posY << ", ";
    ss << ticket->posZ << ", '";
    ss << CharacterDatabase.EscapeString(ticket->message) << "', ";
    ss << ticket->timestamp << ", ";

    if (ticket->deleted)
        ss << uint32(1);
    else
        ss << uint32(0);
    ss << ",";

    ss << ticket->assignedToPlayer << ", '";
    ss << CharacterDatabase.EscapeString(ticket->comment) << "');";

    if (buf == NULL)
        CharacterDatabase.ExecuteNA(ss.str().c_str());
    else
        buf->AddQueryStr(ss.str());
}

void ObjectMgr::LoadAchievementRewards()
{
    AchievementRewards.clear();                           // need for reload case

    QueryResult* result = WorldDatabase.Query("SELECT entry, gender, title_A, title_H, item, sender, subject, text FROM achievement_reward");

    if (!result)
    {
        LOG_DETAIL("Loaded 0 achievement rewards. DB table `achievement_reward` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();

        if (!dbcAchievementStore.LookupEntryForced(entry))
        {
            sLog.Error("ObjectMgr", "Achievement reward entry %u has wrong achievement, ignore", entry);
            continue;
        }

        AchievementReward reward;
        reward.gender = fields[1].GetUInt32();
        reward.titel_A = fields[2].GetUInt32();
        reward.titel_H = fields[3].GetUInt32();
        reward.itemId = fields[4].GetUInt32();
        reward.sender = fields[5].GetUInt32();
        reward.subject = fields[6].GetString() ? fields[6].GetString() : "";
        reward.text = fields[7].GetString() ? fields[7].GetString() : "";

        if (reward.gender > 2)
            sLog.Error("ObjectMgr", "achievement reward %u has wrong gender %u.", entry, reward.gender);

        bool dup = false;
        AchievementRewardsMapBounds bounds = AchievementRewards.equal_range(entry);
        for (AchievementRewardsMap::const_iterator iter = bounds.first; iter != bounds.second; ++iter)
        {
            if (iter->second.gender == 2 || reward.gender == 2)
            {
                dup = true;
                sLog.Error("ObjectMgr", "Achievement reward %u must have single GENDER_NONE (%u), ignore duplicate case", 2, entry);
                break;
            }
        }

        if (dup)
            continue;

        // must be title or mail at least
        if (!reward.titel_A && !reward.titel_H && !reward.sender)
        {
            sLog.Error("ObjectMgr", "achievement_reward %u not have title or item reward data, ignore.", entry);
            continue;
        }

        if (reward.titel_A)
        {
            CharTitlesEntry const* titleEntry = dbcCharTitlesEntry.LookupEntryForced(reward.titel_A);
            if (!titleEntry)
            {
                sLog.Error("ObjectMgr", "achievement_reward %u has invalid title id (%u) in `title_A`, set to 0", entry, reward.titel_A);
                reward.titel_A = 0;
            }
        }

        if (reward.titel_H)
        {
            CharTitlesEntry const* titleEntry = dbcCharTitlesEntry.LookupEntryForced(reward.titel_H);
            if (!titleEntry)
            {
                sLog.Error("ObjectMgr", "achievement_reward %u has invalid title id (%u) in `title_A`, set to 0", entry, reward.titel_H);
                reward.titel_H = 0;
            }
        }

        //check mail data before item for report including wrong item case
        if (reward.sender)
        {
            if (!CreatureNameStorage.LookupEntry(reward.sender))
            {
                sLog.Error("ObjectMgr", "achievement_reward %u has invalid creature entry %u as sender, mail reward skipped.", entry, reward.sender);
                reward.sender = 0;
            }
        }
        else
        {
            if (reward.itemId)
                sLog.Error("ObjectMgr", "achievement_reward %u not have sender data but have item reward, item will not rewarded", entry);

            if (!reward.subject.empty())
                sLog.Error("ObjectMgr", "achievement_reward %u not have sender data but have mail subject.", entry);

            if (!reward.text.empty())
                sLog.Error("ObjectMgr", "achievement_reward %u not have sender data but have mail text.", entry);
        }

        if (reward.itemId)
        {
            if (reward.itemId == 0)
            {
                sLog.Error("ObjectMgr", "achievement_reward %u has invalid item id %u, reward mail will be without item.", entry, reward.itemId);
            }
        }

        AchievementRewards.insert(AchievementRewardsMap::value_type(entry, reward));
        ++count;

    }
    while (result->NextRow());

    delete result;

    sLog.Success("ObjectMgr", "Loaded %u achievement rewards", count);
}

void ObjectMgr::SetHighestGuids()
{
    QueryResult* result;

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM characters");
    if (result)
    {
        m_hiPlayerGuid.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM playeritems");
    if (result)
    {
        m_hiItemGuid.SetVal((uint32)result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM corpses");
    if (result)
    {
        m_hiCorpseGuid.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = WorldDatabase.Query("SELECT MAX(id) FROM creature_spawns");
    if (result)
    {
        m_hiCreatureSpawnId.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = WorldDatabase.Query("SELECT MAX(id) FROM gameobject_spawns");
    if (result)
    {
        m_hiGameObjectSpawnId.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(group_id) FROM groups");
    if (result)
    {
        m_hiGroupId.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(charterid) FROM charters");
    if (result)
    {
        m_hiCharterId.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(guildid) FROM guilds");
    if (result)
    {
        m_hiGuildId.SetVal(result->Fetch()[0].GetUInt32());
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(UID) FROM playerbugreports");
    if (result != NULL)
    {
        m_reportID.SetVal(uint32(result->Fetch()[0].GetUInt64() + 1));
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(ticketid) FROM gm_tickets");
    if (result)
    {
        m_ticketid.SetVal(uint32(result->Fetch()[0].GetUInt64() + 1));
        delete result;
    }


    result = CharacterDatabase.Query("SELECT MAX(message_id) FROM mailbox");
    if (result)
    {
        m_mailid.SetVal(uint32(result->Fetch()[0].GetUInt64() + 1));
        delete result;
    }

    result = CharacterDatabase.Query("SELECT MAX(setGUID) FROM equipmentsets");
    if (result != NULL)
    {
        m_setGUID.SetVal(uint32(result->Fetch()[0].GetUInt32() + 1));
        delete result;
    }

    Log.Notice("ObjectMgr", "HighGuid(CORPSE) = %u", m_hiCorpseGuid.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(PLAYER) = %u", m_hiPlayerGuid.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(GAMEOBJ) = %u", m_hiGameObjectSpawnId.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(UNIT) = %u", m_hiCreatureSpawnId.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(ITEM) = %u", m_hiItemGuid.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(CONTAINER) = %u", m_hiItemGuid.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(GROUP) = %u", m_hiGroupId.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(CHARTER) = %u", m_hiCharterId.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(GUILD) = %u", m_hiGuildId.GetVal());
    Log.Notice("ObjectMgr", "HighGuid(BUGREPORT) = %u", uint32(m_reportID.GetVal() - 1));
    Log.Notice("ObjectMgr", "HighGuid(TICKET) = %u", uint32(m_ticketid.GetVal() - 1));
    Log.Notice("ObjectMgr", "HighGuid(MAIL) = %u", uint32(m_mailid.GetVal()));
    Log.Notice("ObjectMgr", "HighGuid(EQUIPMENTSET) = %u", uint32(m_setGUID.GetVal() - 1));
}

uint32 ObjectMgr::GenerateReportID()
{
    return ++m_reportID;
}

uint32 ObjectMgr::GenerateTicketID()
{
    return ++m_ticketid;
}


uint32 ObjectMgr::GenerateEquipmentSetID()
{
    return ++m_setGUID;
}

uint32 ObjectMgr::GenerateMailID()
{
    return ++m_mailid;
}

uint32 ObjectMgr::GenerateLowGuid(uint32 guidhigh)
{
    ARCEMU_ASSERT(guidhigh == HIGHGUID_TYPE_ITEM || guidhigh == HIGHGUID_TYPE_CONTAINER || guidhigh == HIGHGUID_TYPE_PLAYER);

    uint32 ret;
    if (guidhigh == HIGHGUID_TYPE_ITEM)
    {

        ret = ++m_hiItemGuid;

    }
    else if (guidhigh == HIGHGUID_TYPE_PLAYER)
    {
        ret = ++m_hiPlayerGuid;
    }
    else
    {

        ret = ++m_hiItemGuid;

    }
    return ret;
}

void ObjectMgr::ProcessGameobjectQuests()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM gameobject_quest_item_binding");
    QueryResult* result2 = WorldDatabase.Query("SELECT * FROM gameobject_quest_pickup_binding");

    GameObjectInfo* gameobject_info;
    Quest const* qst;

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            gameobject_info = GameObjectNameStorage.LookupEntry(fields[0].GetUInt32());
            qst = objmgr.GetQuestTemplate(fields[1].GetUInt32());
            if (gameobject_info && qst)
                gameobject_info->itemMap[qst].insert(std::make_pair(fields[2].GetUInt32(), fields[3].GetUInt32()));

        }
        while (result->NextRow());
        delete result;
    }

    if (result2)
    {
        do
        {
            Field* fields = result2->Fetch();
            gameobject_info = GameObjectNameStorage.LookupEntry(fields[0].GetUInt32());
            qst = objmgr.GetQuestTemplate(fields[1].GetUInt32());
            if (gameobject_info && qst)
                gameobject_info->goMap.insert(std::make_pair(qst, fields[2].GetUInt32()));

        }
        while (result2->NextRow());
        delete result2;
    }

    result = WorldDatabase.Query("SELECT * FROM npc_gossip_textid");
    if (result)
    {
        uint32 entry, text;
        do
        {
            entry = result->Fetch()[0].GetUInt32();
            text = result->Fetch()[1].GetUInt32();

            mNpcToGossipText[entry] = text;

        }
        while (result->NextRow());
        delete result;
    }
    Log.Success("ObjectMgr", "%u NPC Gossip TextIds loaded.", mNpcToGossipText.size());
}

Player* ObjectMgr::GetPlayer(const char* name, bool caseSensitive)
{
    Player* rv = NULL;
    PlayerStorageMap::const_iterator itr;
    _playerslock.AcquireReadLock();

    if (!caseSensitive)
    {
        std::string strName = name;
        arcemu_TOLOWER(strName);
        for (itr = _players.begin(); itr != _players.end(); ++itr)
        {
            if (!stricmp(itr->second->GetNameString()->c_str(), strName.c_str()))
            {
                rv = itr->second;
                break;
            }
        }
    }
    else
    {
        for (itr = _players.begin(); itr != _players.end(); ++itr)
        {
            if (!strcmp(itr->second->GetName(), name))
            {
                rv = itr->second;
                break;
            }
        }
    }

    _playerslock.ReleaseReadLock();

    return rv;
}

Player* ObjectMgr::GetPlayer(uint32 guid)
{
    Player* rv;

    _playerslock.AcquireReadLock();
    PlayerStorageMap::const_iterator itr = _players.find(guid);
    rv = (itr != _players.end()) ? itr->second : NULL;
    _playerslock.ReleaseReadLock();

    return rv;
}

PlayerCreateInfo* ObjectMgr::GetPlayerCreateInfo(uint8 race, uint8 class_) const
{
    PlayerCreateInfoMap::const_iterator itr;
    for (itr = mPlayerCreateInfo.begin(); itr != mPlayerCreateInfo.end(); ++itr)
    {
        if ((itr->second->race == race) && (itr->second->class_ == class_))
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::AddGuild(Guild* pGuild)
{
    ARCEMU_ASSERT(pGuild != NULL);
    mGuild[pGuild->GetGuildId()] = pGuild;
}

uint32 ObjectMgr::GetTotalGuildCount()
{
    return (uint32)mGuild.size();
}

bool ObjectMgr::RemoveGuild(uint32 guildId)
{
    GuildMap::iterator i = mGuild.find(guildId);
    if (i == mGuild.end())
    {
        return false;
    }

    mGuild.erase(i);
    return true;
}

Guild* ObjectMgr::GetGuild(uint32 guildId)
{
    GuildMap::const_iterator itr = mGuild.find(guildId);
    if (itr == mGuild.end())
        return NULL;
    return itr->second;
}

Guild* ObjectMgr::GetGuildByLeaderGuid(uint64 leaderGuid)
{
    GuildMap::const_iterator itr;
    for (itr = mGuild.begin(); itr != mGuild.end(); ++itr)
    {
        if (itr->second->GetGuildLeader() == leaderGuid)
            return itr->second;
    }
    return NULL;
}

Guild* ObjectMgr::GetGuildByGuildName(std::string guildName)
{
    GuildMap::const_iterator itr;
    for (itr = mGuild.begin(); itr != mGuild.end(); ++itr)
    {
        if (itr->second->GetGuildName() == guildName)
            return itr->second;
    }
    return NULL;
}


void ObjectMgr::AddGMTicket(GM_Ticket* ticket, bool startup)
{
    ARCEMU_ASSERT(ticket != NULL);
    GM_TicketList.push_back(ticket);

    if (!startup)
        SaveGMTicket(ticket, NULL);
}

void ObjectMgr::UpdateGMTicket(GM_Ticket* ticket)
{
    SaveGMTicket(ticket, NULL);
}

void ObjectMgr::DeleteGMTicketPermanently(uint64 ticketGuid)
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->guid == ticketGuid)
        {
            i = GM_TicketList.erase(i);
        }
        else
        {
            ++i;
        }
    }

    CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE guid=%u", ticketGuid);      // kill from db
}

void ObjectMgr::DeleteAllRemovedGMTickets()
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->deleted)
        {
            i = GM_TicketList.erase(i);
        }
        else
        {
            ++i;
        }
    }

    CharacterDatabase.Execute("DELETE FROM gm_tickets WHERE deleted=1");
}

void ObjectMgr::RemoveGMTicketByPlayer(uint64 playerGuid)
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->playerGuid == playerGuid && !(*i)->deleted)
        {
            (*i)->deleted = true;
            SaveGMTicket((*i), NULL);
        }
        ++i;
    }
}

void ObjectMgr::RemoveGMTicket(uint64 ticketGuid)
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->guid == ticketGuid && !(*i)->deleted)
        {
            (*i)->deleted = true;
            SaveGMTicket((*i), NULL);
        }
        ++i;
    }
}

GM_Ticket* ObjectMgr::GetGMTicketByPlayer(uint64 playerGuid)
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->playerGuid == playerGuid && !(*i)->deleted)
        {
            return (*i);
        }
        ++i;
    }
    return NULL;
}

GM_Ticket* ObjectMgr::GetGMTicket(uint64 ticketGuid)
{
    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
    {
        if ((*i)->guid == ticketGuid)
        {
            return (*i);
        }
        ++i;
    }
    return NULL;
}

//std::list<GM_Ticket*>* ObjectMgr::GetGMTicketsByPlayer(uint64 playerGuid)
//{
//    std::list<GM_Ticket*>* list = new std::list<GM_Ticket*>();
//    for (GmTicketList::iterator i = GM_TicketList.begin(); i != GM_TicketList.end();)
//    {
//        if ((*i)->playerGuid == playerGuid)
//        {
//            list->push_back((*i));
//        }
//        ++i;
//    }
//    return list;
//}

void ObjectMgr::LoadVendors()
{
    std::unordered_map<uint32, std::vector<CreatureItem>*>::const_iterator itr;
    std::vector<CreatureItem> *items;
    CreatureItem itm;

    QueryResult* result = WorldDatabase.Query("SELECT * FROM vendors");
    if (result != NULL)
    {
        if (result->GetFieldCount() < 6)
        {
            Log.Notice("ObjectMgr", "Invalid format in vendors (%u/6) columns, not enough data to proceed.", result->GetFieldCount());
            delete result;
            return;
        }
        else if (result->GetFieldCount() > 6)
        {
            Log.Notice("ObjectMgr", "Invalid format in vendors (%u/6) columns, loading anyway because we have enough data", result->GetFieldCount());
        }

        ItemExtendedCostEntry* ec = NULL;
        do
        {
            Field* fields = result->Fetch();

            itr = mVendors.find(fields[0].GetUInt32());

            if (itr == mVendors.end())
            {
                items = new std::vector < CreatureItem > ;
                mVendors[fields[0].GetUInt32()] = items;
            }
            else
            {
                items = itr->second;
            }

            itm.itemid = fields[1].GetUInt32();
            itm.amount = fields[2].GetUInt32();
            itm.available_amount = fields[3].GetUInt32();
            itm.max_amount = fields[3].GetUInt32();
            itm.incrtime = fields[4].GetUInt32();
            if (fields[5].GetUInt32() > 0)
            {
                ec = dbcItemExtendedCost.LookupEntry(fields[5].GetUInt32());
                if (ec == NULL)
                    Log.Error("LoadVendors", "Extendedcost for item %u references nonexistent EC %u", fields[1].GetUInt32(), fields[5].GetUInt32());
            }
            else
                ec = NULL;
            itm.extended_cost = ec;
            items->push_back(itm);
        }
        while (result->NextRow());

        delete result;
    }
    Log.Success("ObjectMgr", "%u vendors loaded.", mVendors.size());
}

void ObjectMgr::ReloadVendors()
{
    mVendors.clear();
    LoadVendors();
}

std::vector<CreatureItem>* ObjectMgr::GetVendorList(uint32 entry)
{
    return mVendors[entry];
}

void ObjectMgr::LoadAIThreatToSpellId()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM ai_threattospellid");

    if (!result)
    {
        return;
    }

    SpellEntry* sp;

    do
    {
        Field* fields = result->Fetch();
        sp = dbcSpell.LookupEntryForced(fields[0].GetUInt32());
        if (sp != NULL)
        {
            sp->ThreatForSpell = fields[1].GetUInt32();
            sp->ThreatForSpellCoef = fields[2].GetFloat();
        }
        else
            Log.Error("AIThreatSpell", "Cannot apply to spell %u; spell is nonexistent.", fields[0].GetUInt32());

    }
    while (result->NextRow());

    delete result;
}

void ObjectMgr::LoadSpellProcs()
{
    SpellEntry* sp;
    QueryResult* result = WorldDatabase.Query("SELECT * FROM spell_proc");
    if (result)
    {
        do
        {
            Field* f = result->Fetch();
            uint32 spe_spellId = f[0].GetUInt32();
            uint32 spe_NameHash = f[1].GetUInt32();

            if (spe_spellId)
            {
                sp = dbcSpell.LookupEntryForced(spe_spellId);
                if (sp != NULL)
                {
                    int x;
                    for (x = 0; x < 3; ++x)
                        if (sp->ProcOnNameHash[x] == 0)
                            break;

                    if (x != 3)
                    {
                        sp->ProcOnNameHash[x] = spe_NameHash;
                    }
                    else
                        LOG_ERROR("Wrong ProcOnNameHash for Spell: %u!", spe_spellId);

                    sp->procFlags = f[2].GetUInt32();

                    if (f[3].GetUInt32() == 1)
                        sp->procFlags |= PROC_TARGET_SELF;
                    if (f[4].GetInt32() >= 0)
                        sp->procChance = f[4].GetUInt32();
                    if (f[5].GetInt32() >= 0)
                        sp->procCharges = f[5].GetInt32();

                    sp->proc_interval = f[6].GetUInt32();

                    if (f[7].GetInt32() >= 0)
                        sp->eff[0].EffectTriggerSpell = f[7].GetUInt32();
                    if (f[8].GetInt32() >= 0)
                        sp->eff[1].EffectTriggerSpell = f[8].GetUInt32();
                    if (f[9].GetInt32() >= 0)
                        sp->eff[2].EffectTriggerSpell = f[9].GetUInt32();
                }
            }
        }
        while (result->NextRow());
        delete result;
    }
}

void ObjectMgr::LoadSpellEffectsOverride()
{
    SpellEntry* sp;
    QueryResult* result = WorldDatabase.Query("SELECT * FROM spell_effects_override");
    if (result)
    {
        do
        {
            Field* f = result->Fetch();
            uint32 seo_SpellId = f[0].GetUInt32();
            uint32 seo_EffectId = f[1].GetUInt32();
            uint32 seo_Disable = f[2].GetUInt32();
            uint32 seo_Effect = f[3].GetUInt32();
            uint32 seo_BasePoints = f[4].GetUInt32();
            uint32 seo_ApplyAuraName = f[5].GetUInt32();
            //uint32 seo_SpellGroupRelation = f[6].GetUInt32();
            uint32 seo_MiscValue = f[7].GetUInt32();
            uint32 seo_TriggerSpell = f[8].GetUInt32();
            uint32 seo_ImplicitTargetA = f[9].GetUInt32();
            uint32 seo_ImplicitTargetB = f[10].GetUInt32();
            uint32 seo_EffectCustomFlag = f[11].GetUInt32();

            if (seo_SpellId)
            {
                sp = dbcSpell.LookupEntryForced(seo_SpellId);
                if (sp != NULL)
                {
                    if (seo_Disable)
                        sp->eff[seo_EffectId].Effect = SPELL_EFFECT_NULL;

                    if (seo_Effect)
                        sp->eff[seo_EffectId].Effect = seo_Effect;

                    if (seo_BasePoints)
                        sp->eff[seo_EffectId].EffectBasePoints = seo_BasePoints;

                    if (seo_ApplyAuraName)
                        sp->eff[seo_EffectId].EffectApplyAuraName = seo_ApplyAuraName;

                    //                    if (seo_SpellGroupRelation)
                    //                        sp->EffectSpellGroupRelation[seo_EffectId] = seo_SpellGroupRelation;

                    if (seo_MiscValue)
                        sp->eff[seo_EffectId].EffectMiscValue = seo_MiscValue;

                    if (seo_TriggerSpell)
                        sp->eff[seo_EffectId].EffectTriggerSpell = seo_TriggerSpell;

                    if (seo_ImplicitTargetA)
                        sp->eff[seo_EffectId].EffectImplicitTargetA = seo_ImplicitTargetA;

                    if (seo_ImplicitTargetB)
                        sp->eff[seo_EffectId].EffectImplicitTargetB = seo_ImplicitTargetB;

                    if (seo_EffectCustomFlag != 0)
                        sp->EffectCustomFlag[seo_EffectId] = seo_EffectCustomFlag;
                }
                else
                {
                    Log.Error("ObjectMgr", "Tried to load a spell effect override for a nonexistant spell: %u", seo_SpellId);
                }
            }

        }
        while (result->NextRow());
        delete result;
    }
}

Item* ObjectMgr::CreateItem(uint32 entry, Player* owner)
{
    ItemPrototype* proto = ItemPrototypeStorage.LookupEntry(entry);
    if (proto == NULL) return 0;

    if (proto->InventoryType == INVTYPE_BAG)
    {
        Container* pContainer = new Container(HIGHGUID_TYPE_CONTAINER, GenerateLowGuid(HIGHGUID_TYPE_CONTAINER));
        pContainer->Create(entry, owner);
        pContainer->SetStackCount(1);
        return pContainer;
    }
    else
    {
        Item* pItem = new Item;
        pItem->Init(HIGHGUID_TYPE_ITEM, GenerateLowGuid(HIGHGUID_TYPE_ITEM));
        pItem->Create(entry, owner);
        pItem->SetStackCount(1);

        if (owner != NULL)
        {
            uint32* played = owner->GetPlayedtime();
            pItem->SetCreationTime(played[1]);
        }

        return pItem;
    }
}

Item* ObjectMgr::LoadItem(uint32 lowguid)
{
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM playeritems WHERE guid = %u", lowguid);
    Item* pReturn = 0;

    if (result)
    {
        ItemPrototype* pProto = ItemPrototypeStorage.LookupEntry(result->Fetch()[2].GetUInt32());
        if (!pProto)
            return NULL;

        if (pProto->InventoryType == INVTYPE_BAG)
        {
            Container* pContainer = new Container(HIGHGUID_TYPE_CONTAINER, lowguid);
            pContainer->LoadFromDB(result->Fetch());
            pReturn = pContainer;
        }
        else
        {
            Item* pItem = new Item;
            pItem->Init(HIGHGUID_TYPE_ITEM, lowguid);
            pItem->LoadFromDB(result->Fetch(), NULL, false);
            pReturn = pItem;
        }
        delete result;
    }

    return pReturn;
}

void ObjectMgr::LoadCorpses(MapMgr* mgr)
{
    Corpse* pCorpse = NULL;

    QueryResult* result = CharacterDatabase.Query("SELECT * FROM corpses WHERE instanceid = %u", mgr->GetInstanceID());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            pCorpse = new Corpse(HIGHGUID_TYPE_CORPSE, fields[0].GetUInt32());
            pCorpse->SetPosition(fields[1].GetFloat(), fields[2].GetFloat(), fields[3].GetFloat(), fields[4].GetFloat());
            pCorpse->SetZoneId(fields[5].GetUInt32());
            pCorpse->SetMapId(fields[6].GetUInt32());
            pCorpse->SetInstanceID(fields[7].GetUInt32());
            pCorpse->LoadValues(fields[8].GetString());
            if (pCorpse->GetDisplayId() == 0)
            {
                delete pCorpse;
                continue;
            }

            pCorpse->PushToWorld(mgr);
        }
        while (result->NextRow());

        delete result;
    }
}

#ifdef ENABLE_ACHIEVEMENTS
AchievementCriteriaEntryList const & ObjectMgr::GetAchievementCriteriaByType(AchievementCriteriaTypes type)
{
    return m_AchievementCriteriasByType[type];
}

void ObjectMgr::LoadAchievementCriteriaList()
{
    for (uint32 rowId = 0; rowId < dbcAchievementCriteriaStore.GetNumRows(); ++rowId)
    {
        AchievementCriteriaEntry const* criteria = dbcAchievementCriteriaStore.LookupRowForced(rowId);
        if (!criteria)
            continue;

        m_AchievementCriteriasByType[criteria->requiredType].push_back(criteria);
    }
}
#endif

std::list<ItemPrototype*>* ObjectMgr::GetListForItemSet(uint32 setid)
{
    return mItemSets[setid];
}

void ObjectMgr::CorpseAddEventDespawn(Corpse* pCorpse)
{
    if (!pCorpse->IsInWorld())
        delete pCorpse;
    else
        sEventMgr.AddEvent(pCorpse->GetMapMgr(), &MapMgr::EventCorpseDespawn, pCorpse->GetGUID(), EVENT_CORPSE_DESPAWN, 600000, 1, EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
}

void ObjectMgr::CorpseCollectorUnload()
{
    CorpseMap::const_iterator itr;
    _corpseslock.Acquire();
    for (itr = m_corpses.begin(); itr != m_corpses.end();)
    {
        Corpse* c = itr->second;
        ++itr;
        if (c->IsInWorld())
            c->RemoveFromWorld(false);
        delete c;
    }
    m_corpses.clear();
    _corpseslock.Release();
}

GossipMenu::GossipMenu(uint64 Creature_Guid, uint32 Text_Id) : TextId(Text_Id), CreatureGuid(Creature_Guid)
{}

void GossipMenu::AddItem(uint8 Icon, const char* Text, int32 Id /* = -1 */, int8 Extra /* = 0 */)
{
    GossipMenuItem Item;

    Item.Icon = Icon;
    Item.Extra = Extra;
    Item.Text = Text;
    Item.m_gBoxMessage = "";
    Item.m_gBoxMoney = 0;
    Item.Id = uint32(Menu.size());

    if (Id > 0)
        Item.IntId = Id;
    else
        Item.IntId = Item.Id;

    Menu.push_back(Item);
}

void GossipMenu::AddMenuItem(uint8 Icon, const char* Message, uint32 dtSender, uint32 dtAction, const char* BoxMessage, uint32 BoxMoney, bool Coded)
{
    GossipMenuItem Item;

    Item.Icon = Icon;
    Item.Extra = Coded;
    Item.Text = Message;
    Item.m_gBoxMessage = BoxMessage;
    Item.m_gBoxMoney = BoxMoney;
    Item.Id = uint32(Menu.size());
    Item.IntId = dtAction;

    Menu.push_back(Item);
}

void GossipMenu::AddItem(GossipMenuItem* GossipItem)
{
    Menu.push_back(*GossipItem);
}

void GossipMenu::BuildPacket(WorldPacket& Packet)
{
    Packet << CreatureGuid;
    Packet << uint32(0);            // some new menu type in 2.4?
    Packet << TextId;
    Packet << uint32(Menu.size());

    for (std::vector<GossipMenuItem>::iterator iter = Menu.begin();
         iter != Menu.end(); ++iter)
    {
        Packet << iter->Id;
        Packet << iter->Icon;
        Packet << iter->Extra;
        Packet << uint32(iter->m_gBoxMoney);    // money required to open menu, 2.0.3
        Packet << iter->Text;
        Packet << iter->m_gBoxMessage;          // accept text (related to money) pop up box, 2.0.3
    }
}

void GossipMenu::SendTo(Player* Plr)
{
    WorldPacket data(SMSG_GOSSIP_MESSAGE, Menu.size() * 50 + 24);
    BuildPacket(data);
    data << uint32(0);  // 0 quests obviously
    Plr->GetSession()->SendPacket(&data);
}

void ObjectMgr::CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr)
{
    if (TextID == 0)
    {
        //TextID = 0 will not show the gossip to the player. Using "2" since it's the default value in GossipScript::GossipHello()
        LOG_ERROR("Object with GUID " I64FMT " is trying to create a GossipMenu with TextID == 0", Guid);
        TextID = 2;
    }

    GossipMenu* Menu = new GossipMenu(Guid, TextID);
    ARCEMU_ASSERT(Menu != NULL);

    if (Plr->CurrentGossipMenu != NULL)
        delete Plr->CurrentGossipMenu;

    Plr->CurrentGossipMenu = Menu;
    *Location = Menu;
}

GossipMenuItem GossipMenu::GetItem(uint32 Id)
{
    if (Id >= Menu.size())
    {
        GossipMenuItem k;
        k.IntId = 1;
        k.Extra = 0;

        k.Id = 0;
        k.Icon = 0;
        k.m_gSender = 0;
        k.m_gAction = 0;
        k.m_gBoxMoney = 0;

        return k;
    }
    else
    {
        return Menu[Id];
    }
}

uint32 ObjectMgr::GetGossipTextForNpc(uint32 ID)
{
    return mNpcToGossipText[ID];
}

void ObjectMgr::LoadTrainers()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM trainer_defs");
    QueryResult* result2;
    Field* fields2;
    const char* temp;
    size_t len;

    LoadDisabledSpells();

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        Trainer* tr = new Trainer;
        tr->RequiredSkill = fields[1].GetUInt32();
        tr->RequiredSkillLine = fields[2].GetUInt32();
        tr->RequiredClass = fields[3].GetUInt32();
        tr->RequiredRace = fields[4].GetUInt32();
        tr->RequiredRepFaction = fields[5].GetUInt32();
        tr->RequiredRepValue = fields[6].GetUInt32();
        tr->TrainerType = fields[7].GetUInt32();
        tr->Can_Train_Gossip_TextId = fields[9].GetUInt32();
        tr->Cannot_Train_GossipTextId = fields[10].GetUInt32();

        if (!tr->Can_Train_Gossip_TextId)
            tr->Can_Train_Gossip_TextId = 1;
        if (!tr->Cannot_Train_GossipTextId)
            tr->Cannot_Train_GossipTextId = 1;

        temp = fields[8].GetString();
        len = strlen(temp);
        if (len)
        {
            tr->UIMessage = new char[len + 1];
            strcpy(tr->UIMessage, temp);
            tr->UIMessage[len] = 0;
        }
        else
        {
            tr->UIMessage = new char[strlen(NormalTalkMessage) + 1];
            strcpy(tr->UIMessage, NormalTalkMessage);
            tr->UIMessage[strlen(NormalTalkMessage)] = 0;
        }

        //now load the spells
        result2 = WorldDatabase.Query("SELECT * FROM trainer_spells where entry='%u'", entry);
        if (!result2)
        {
            Log.Error("LoadTrainers", "Trainer with no spells, entry %u.", entry);
            if (tr->UIMessage != NormalTalkMessage)
                delete[] tr->UIMessage;

            delete tr;
            continue;
        }
        if (result2->GetFieldCount() != 9)
        {
            Log.LargeErrorMessage("Trainers table format is invalid. Please update your database.", NULL);
            delete tr;
            delete result;
            delete result2;
            return;
        }
        else
        {
            do
            {
                fields2 = result2->Fetch();
                TrainerSpell ts;
                bool abrt = false;
                uint32 CastSpellID = fields2[1].GetUInt32();
                uint32 LearnSpellID = fields2[2].GetUInt32();

                ts.pCastSpell = NULL;
                ts.pLearnSpell = NULL;
                ts.pCastRealSpell = NULL;

                if (CastSpellID != 0)
                {
                    ts.pCastSpell = dbcSpell.LookupEntryForced(CastSpellID);
                    if (ts.pCastSpell)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            if (ts.pCastSpell->eff[k].Effect == SPELL_EFFECT_LEARN_SPELL)
                            {
                                ts.pCastRealSpell = dbcSpell.LookupEntryForced(ts.pCastSpell->eff[k].EffectTriggerSpell);
                                if (ts.pCastRealSpell == NULL)
                                {
                                    Log.Error("Trainers", "Trainer %u contains cast spell %u that is non-teaching", entry, CastSpellID);
                                    abrt = true;
                                }
                                break;
                            }
                        }
                    }

                    if (abrt)
                        continue;
                }

                if (LearnSpellID != 0)
                {
                    ts.pLearnSpell = dbcSpell.LookupEntryForced(LearnSpellID);
                }

                if (ts.pCastSpell == NULL && ts.pLearnSpell == NULL)
                {
                    Log.Error("LoadTrainers", "Trainer %u without valid spells (%u/%u).", entry, CastSpellID, LearnSpellID);
                    continue; //omg a bad spell !
                }

                if (ts.pCastSpell && !ts.pCastRealSpell)
                    continue;

                ts.Cost = fields2[3].GetUInt32();
                ts.RequiredSpell = fields2[4].GetUInt32();
                ts.RequiredSkillLine = fields2[5].GetUInt32();
                ts.RequiredSkillLineValue = fields2[6].GetUInt32();
                ts.RequiredLevel = fields2[7].GetUInt32();
                ts.DeleteSpell = fields2[8].GetUInt32();
                //IsProfession is true if the TrainerSpell will teach a primary profession
                if (ts.RequiredSkillLine == 0 && ts.pCastRealSpell != NULL && ts.pCastRealSpell->eff[1].Effect == SPELL_EFFECT_SKILL)
                {
                    uint32 skill = ts.pCastRealSpell->eff[1].EffectMiscValue;
                    skilllineentry* sk = dbcSkillLine.LookupEntryForced(skill);
                    ARCEMU_ASSERT(sk != NULL);
                    if (sk->type == SKILL_TYPE_PROFESSION)
                        ts.IsProfession = true;
                    else
                        ts.IsProfession = false;
                }
                else
                    ts.IsProfession = false;

                tr->Spells.push_back(ts);
            }
            while (result2->NextRow());
            delete result2;

            tr->SpellCount = (uint32)tr->Spells.size();

            //and now we insert it to our lookup table
            if (!tr->SpellCount)
            {
                if (tr->UIMessage != NormalTalkMessage)
                    delete[] tr->UIMessage;
                delete tr;
                continue;
            }

            mTrainers.insert(TrainerMap::value_type(entry, tr));
        }

    }
    while (result->NextRow());
    delete result;
    Log.Success("ObjectMgr", "%u trainers loaded.", mTrainers.size());
}

Trainer* ObjectMgr::GetTrainer(uint32 Entry)
{
    TrainerMap::iterator iter = mTrainers.find(Entry);
    if (iter == mTrainers.end())
        return NULL;

    return iter->second;
}

void ObjectMgr::GenerateLevelUpInfo()
{
    // Generate levelup information for each class.
    PlayerCreateInfo* PCI;
    for (uint32 Class = WARRIOR; Class <= DRUID; ++Class)
    {
        // These are empty.
        if (Class == 10)
            continue;

        // Search for a playercreateinfo.
        for (uint32 Race = RACE_HUMAN; Race <= RACE_WORGEN; ++Race)
        {
            PCI = GetPlayerCreateInfo(static_cast<uint8>(Race), static_cast<uint8>(Class));

            if (PCI == 0)
                continue;   // Class not valid for this race.

            // Generate each level's information
            uint32 MaxLevel = sWorld.m_genLevelCap + 1;
            LevelInfo* lvl = 0, lastlvl;
            lastlvl.HP = PCI->health;
            lastlvl.Mana = PCI->mana;
            lastlvl.Stat[0] = PCI->strength;
            lastlvl.Stat[1] = PCI->ability;
            lastlvl.Stat[2] = PCI->stamina;
            lastlvl.Stat[3] = PCI->intellect;
            lastlvl.Stat[4] = PCI->spirit;
            LevelMap* lMap = new LevelMap;

            // Create first level.
            lvl = new LevelInfo;
            *lvl = lastlvl;

            // Insert into map
            lMap->insert(LevelMap::value_type(1, lvl));

            uint32 val;
            for (uint32 Level = 2; Level < MaxLevel; ++Level)
            {
                lvl = new LevelInfo;

                // Calculate Stats
                for (uint32 s = 0; s < 5; ++s)
                {
                    val = GainStat(static_cast<uint16>(Level), static_cast<uint8>(Class), static_cast<uint8>(s));
                    lvl->Stat[s] = lastlvl.Stat[s] + val;
                }

                // Calculate HP/Mana
                uint32 TotalHealthGain = 0;
                uint32 TotalManaGain = 0;

                switch (Class)
                {
                    case WARRIOR:
                        if (Level < 13)
                            TotalHealthGain += 19;
                        else if (Level < 36)
                            TotalHealthGain += Level + 6;
                        //                    else if (Level >60) TotalHealthGain+=Level+100;
                        else if (Level > 60)
                            TotalHealthGain += Level + 206;
                        else
                            TotalHealthGain += 2 * Level - 30;
                        break;
                    case HUNTER:
                        if (Level < 13)
                            TotalHealthGain += 17;
                        //                    else if (Level >60) TotalHealthGain+=Level+45;
                        else if (Level > 60)
                            TotalHealthGain += Level + 161;
                        else
                            TotalHealthGain += Level + 4;

                        if (Level < 11)
                            TotalManaGain += 29;
                        else if (Level < 27)
                            TotalManaGain += Level + 18;
                        //                    else if (Level>60)TotalManaGain+=Level+20;
                        else if (Level > 60)
                            TotalManaGain += Level + 150;
                        else
                            TotalManaGain += 45;
                        break;
                    case ROGUE:
                        if (Level < 15)
                            TotalHealthGain += 17;
                        //                    else if (Level >60) TotalHealthGain+=Level+110;
                        else if (Level > 60)
                            TotalHealthGain += Level + 191;
                        else
                            TotalHealthGain += Level + 2;
                        break;
                    case DRUID:
                        if (Level < 17)
                            TotalHealthGain += 17;
                        //                    else if (Level >60) TotalHealthGain+=Level+55;
                        else if (Level > 60)
                            TotalHealthGain += Level + 176;
                        else
                            TotalHealthGain += Level;

                        if (Level < 26)
                            TotalManaGain += Level + 20;
                        //                    else if (Level>60)TotalManaGain+=Level+25;
                        else if (Level > 60)
                            TotalManaGain += Level + 150;
                        else
                            TotalManaGain += 45;
                        break;
                    case MAGE:
                        if (Level < 23)
                            TotalHealthGain += 15;
                        //                    else if (Level >60) TotalHealthGain+=Level+40;
                        else if (Level > 60)
                            TotalHealthGain += Level + 190;
                        else
                            TotalHealthGain += Level - 8;

                        if (Level < 28)
                            TotalManaGain += Level + 23;
                        //                    else if (Level>60)TotalManaGain+=Level+26;
                        else if (Level > 60)
                            TotalManaGain += Level + 115;
                        else
                            TotalManaGain += 51;
                        break;
                    case SHAMAN:
                        if (Level < 16)
                            TotalHealthGain += 17;
                        //                    else if (Level >60) TotalHealthGain+=Level+75;
                        else if (Level > 60)
                            TotalHealthGain += Level + 157;
                        else
                            TotalHealthGain += Level + 1;

                        if (Level < 22)
                            TotalManaGain += Level + 19;
                        //                    else if (Level>60)TotalManaGain+=Level+70;
                        else if (Level > 60)
                            TotalManaGain += Level + 175;
                        else
                            TotalManaGain += 49;
                        break;
                    case WARLOCK:
                        if (Level < 17)
                            TotalHealthGain += 17;
                        //                    else if (Level >60) TotalHealthGain+=Level+50;
                        else if (Level > 60)
                            TotalHealthGain += Level + 192;
                        else
                            TotalHealthGain += Level - 2;

                        if (Level < 30)
                            TotalManaGain += Level + 21;
                        //                    else if (Level>60)TotalManaGain+=Level+25;
                        else if (Level > 60)
                            TotalManaGain += Level + 121;
                        else
                            TotalManaGain += 51;
                        break;
                    case PALADIN:
                        if (Level < 14)
                            TotalHealthGain += 18;
                        //                    else if (Level >60) TotalHealthGain+=Level+55;
                        else if (Level > 60)
                            TotalHealthGain += Level + 167;
                        else
                            TotalHealthGain += Level + 4;

                        if (Level < 30)
                            TotalManaGain += Level + 17;
                        //                    else if (Level>60)TotalManaGain+=Level+100;
                        else if (Level > 60)
                            TotalManaGain += Level + 131;
                        else
                            TotalManaGain += 42;
                        break;
                    case PRIEST:
                        if (Level < 21)
                            TotalHealthGain += 15;
                        //                    else if (Level >60) TotalHealthGain+=Level+40;
                        else if (Level > 60)
                            TotalHealthGain += Level + 157;
                        else
                            TotalHealthGain += Level - 6;

                        if (Level < 22)
                            TotalManaGain += Level + 22;
                        else if (Level < 32)
                            TotalManaGain += Level + 37;
                        //                    else if (Level>60)TotalManaGain+=Level+35;
                        else if (Level > 60)
                            TotalManaGain += Level + 207;
                        else
                            TotalManaGain += 54;
                        break;
                    case DEATHKNIGHT: // Based on 55-56 more testing will be done.
                        if (Level < 60)
                            TotalHealthGain += 92;
                        /*else if (Level <60) TotalHealthGain+=??;
                        else if (Level <70) TotalHealthGain+=??;*/
                        else
                            TotalHealthGain += 92;
                        break;
                }

                // Apply HP/Mana
                lvl->HP = lastlvl.HP + TotalHealthGain;
                lvl->Mana = lastlvl.Mana + TotalManaGain;

                lastlvl = *lvl;

                // Apply to map.
                lMap->insert(LevelMap::value_type(Level, lvl));
            }

            // Now that our level map is full, let's create the class/race pair
            std::pair<uint32, uint32> p;
            p.first = Race;
            p.second = Class;

            // Insert back into the main map.
            mLevelInfo.insert(LevelInfoMap::value_type(p, lMap));
        }
    }
    Log.Notice("ObjectMgr", "%u level up information generated.", mLevelInfo.size());
}

void ObjectMgr::LoadXpToLevelTable()
{
    _playerXPperLevel.clear(); //For reloading
    _playerXPperLevel.resize(sWorld.m_levelCap);

    for (uint8 level = 0; level < sWorld.m_levelCap; ++level)
        _playerXPperLevel[level] = 0;

    QueryResult* result = WorldDatabase.Query("SELECT player_lvl, next_lvl_req_xp FROM player_xp_for_level");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 current_level = fields[0].GetUInt8();
            uint32 current_xp = fields[1].GetUInt32();

            if (current_level >= sWorld.m_levelCap)
                continue;

            _playerXPperLevel[current_level] = current_xp;
        }
        while (result->NextRow());
    }
}

uint32 ObjectMgr::GetXPToLevel(uint32 level)
{
    if (level < _playerXPperLevel.size())
        return _playerXPperLevel[level];
    return 0;
}

LevelInfo* ObjectMgr::GetLevelInfo(uint32 Race, uint32 Class, uint32 Level)
{
    // Iterate levelinfo map until we find the right class+race.
    LevelInfoMap::iterator itr = mLevelInfo.begin();
    for (; itr != mLevelInfo.end(); ++itr)
    {
        if (itr->first.first == Race && itr->first.second == Class)
        {
            // We got a match.
            // Let's check that our level is valid first.
            if (Level > sWorld.m_genLevelCap)   // too far.
                Level = sWorld.m_genLevelCap;

            // Pull the level information from the second map.
            LevelMap::iterator it2 = itr->second->find(Level);
            ARCEMU_ASSERT(it2 != itr->second->end());

            return it2->second;
        }
    }
    return NULL;
}

void ObjectMgr::LoadDefaultPetSpells()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM petdefaultspells");
    if (result)
    {
        do
        {
            Field* f = result->Fetch();
            uint32 Entry = f[0].GetUInt32();
            uint32 spell = f[1].GetUInt32();
            SpellEntry* sp = dbcSpell.LookupEntryForced(spell);

            if (spell && Entry && sp)
            {
                PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
                if (itr != mDefaultPetSpells.end())
                    itr->second.insert(sp);
                else
                {
                    std::set<SpellEntry*> s;
                    s.insert(sp);
                    mDefaultPetSpells[Entry] = s;
                }
            }
        }
        while (result->NextRow());
        delete result;
    }
}

std::set<SpellEntry*>* ObjectMgr::GetDefaultPetSpells(uint32 Entry)
{
    PetDefaultSpellMap::iterator itr = mDefaultPetSpells.find(Entry);
    if (itr == mDefaultPetSpells.end())
        return 0;

    return &(itr->second);
}

void ObjectMgr::LoadPetSpellCooldowns()
{
    for (DBCStorage< CreatureSpellDataEntry >::iterator itr = dbcCreatureSpellData.begin(); itr != dbcCreatureSpellData.end(); ++itr)
    {
        CreatureSpellDataEntry* csde = *itr;

        for (uint32 j = 0; j < 3; ++j)
        {
            uint32 SpellId = csde->Spells[j];
            uint32 Cooldown = csde->Cooldowns[j] * 10;

            if (SpellId != 0)
            {
                PetSpellCooldownMap::iterator itr = mPetSpellCooldowns.find(SpellId);
                if (itr == mPetSpellCooldowns.end())
                {
                    if (Cooldown)
                        mPetSpellCooldowns.insert(std::make_pair(SpellId, Cooldown));
                }
                else
                {
                    uint32 SP2 = mPetSpellCooldowns[SpellId];
                    ARCEMU_ASSERT(Cooldown == SP2);
                }
            }
        }
    }
}

uint32 ObjectMgr::GetPetSpellCooldown(uint32 SpellId)
{
    PetSpellCooldownMap::iterator itr = mPetSpellCooldowns.find(SpellId);
    if (itr != mPetSpellCooldowns.end())
        return itr->second;

    SpellEntry* sp = dbcSpell.LookupEntry(SpellId);
    if (sp->RecoveryTime > sp->CategoryRecoveryTime)
        return sp->RecoveryTime;
    else
        return sp->CategoryRecoveryTime;
}

void ObjectMgr::LoadSpellOverride()
{
    //    int i = 0;
    std::stringstream query;
    QueryResult* result = WorldDatabase.Query("SELECT DISTINCT overrideId FROM spelloverride");

    if (!result)
    {
        return;
    }

    //    int num = 0;
    //    uint32 total =(uint32) result->GetRowCount();
    SpellEntry* sp;
    uint32 spellid;

    do
    {
        Field* fields = result->Fetch();
        query.rdbuf()->str("");
        query << "SELECT spellId FROM spelloverride WHERE overrideId = ";
        query << fields[0].GetUInt32();
        QueryResult* resultIn = WorldDatabase.Query(query.str().c_str());
        std::list<SpellEntry*>* list = new std::list < SpellEntry* > ;
        if (resultIn)
        {
            do
            {
                Field* fieldsIn = resultIn->Fetch();
                spellid = fieldsIn[0].GetUInt32();
                sp = dbcSpell.LookupEntryForced(spellid);
                if (!spellid || !sp)
                    continue;
                list->push_back(sp);
            }
            while (resultIn->NextRow());
            delete resultIn;
        }
        if (list->size() == 0)
            delete list;
        else
            mOverrideIdMap.insert(OverrideIdMap::value_type(fields[0].GetUInt32(), list));
    }
    while (result->NextRow());
    delete result;
    Log.Success("ObjectMgr", "%u spell overrides loaded.", mOverrideIdMap.size());
}

void ObjectMgr::SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_)
{
    mVendors[Entry] = list_;
}

void ObjectMgr::LoadCreatureTimedEmotes()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM creature_timed_emotes order by rowid asc");
    if (!result)return;

    do
    {
        Field* fields = result->Fetch();
        spawn_timed_emotes* te = new spawn_timed_emotes;
        te->type = fields[2].GetUInt8();
        te->value = fields[3].GetUInt32();
        char* str = (char*)fields[4].GetString();
        if (str)
        {
            uint32 len = (int)strlen(str);
            te->msg = new char[len + 1];
            memcpy(te->msg, str, len + 1);
        }
        else te->msg = NULL;
        te->msg_type = static_cast<uint8>(fields[5].GetUInt32());
        te->msg_lang = static_cast<uint8>(fields[6].GetUInt32());
        te->expire_after = fields[7].GetUInt32();

        std::unordered_map<uint32, TimedEmoteList*>::const_iterator i;
        uint32 spawnid = fields[0].GetUInt32();
        i = m_timedemotes.find(spawnid);
        if (i == m_timedemotes.end())
        {
            TimedEmoteList* m = new TimedEmoteList;
            m->push_back(te);
            m_timedemotes[spawnid] = m;
        }
        else
        {
            i->second->push_back(te);
        }
    }
    while (result->NextRow());

    Log.Notice("ObjectMgr", "%u timed emotes cached.", result->GetRowCount());
    delete result;
}

TimedEmoteList* ObjectMgr::GetTimedEmoteList(uint32 spawnid)
{
    std::unordered_map<uint32, TimedEmoteList*>::const_iterator i;
    i = m_timedemotes.find(spawnid);
    if (i != m_timedemotes.end())
    {
        TimedEmoteList* m = i->second;
        return m;
    }
    else return NULL;
}

void ObjectMgr::LoadCreatureWaypoints()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM creature_waypoints");
    if (!result)return;

    do
    {
        Field* fields = result->Fetch();
        WayPoint* wp = new WayPoint;
        wp->id = fields[1].GetUInt32();
        wp->x = fields[2].GetFloat();
        wp->y = fields[3].GetFloat();
        wp->z = fields[4].GetFloat();
        wp->waittime = fields[5].GetUInt32();
        wp->flags = fields[6].GetUInt32();
        wp->forwardemoteoneshot = fields[7].GetBool();
        wp->forwardemoteid = fields[8].GetUInt32();
        wp->backwardemoteoneshot = fields[9].GetBool();
        wp->backwardemoteid = fields[10].GetUInt32();
        wp->forwardskinid = fields[11].GetUInt32();
        wp->backwardskinid = fields[12].GetUInt32();

        std::unordered_map<uint32, WayPointMap*>::const_iterator i;
        uint32 spawnid = fields[0].GetUInt32();
        i = m_waypoints.find(spawnid);
        if (i == m_waypoints.end())
        {
            WayPointMap* m = new WayPointMap;
            if (m->size() <= wp->id)
                m->resize(wp->id + 1);
            (*m)[wp->id] = wp;
            m_waypoints[spawnid] = m;
        }
        else
        {
            if (i->second->size() <= wp->id)
                i->second->resize(wp->id + 1);

            (*(i->second))[wp->id] = wp;
        }
    }
    while (result->NextRow());

    Log.Notice("ObjectMgr", "%u waypoints cached.", result->GetRowCount());
    delete result;
}

WayPointMap* ObjectMgr::GetWayPointMap(uint32 spawnid)
{
    std::unordered_map<uint32, WayPointMap*>::const_iterator i;
    i = m_waypoints.find(spawnid);
    if (i != m_waypoints.end())
    {
        WayPointMap* m = i->second;
        // we don't wanna erase from the map, because some are used more
        // than once (for instances)

        //m_waypoints.erase(i);
        return m;
    }
    else return NULL;
}

Pet* ObjectMgr::CreatePet(uint32 entry)
{
    uint32 guid;
    guid = ++m_hiPetGuid;
    return new Pet(Arcemu::Util::MAKE_PET_GUID(entry, guid));
}

// This is a cataclysm feature
/*void ObjectMgr::CreateCharCreationPet(uint32 entry, uint32 ownerGUID)
{
	string name;
	if (entry == 3122)
		name = "Raptor";
	else
		name = "Wolf";

	std::stringstream ss;

	ss.rdbuf()->str("");

	ss << "INSERT INTO playerpets VALUES('"
		<< ownerGUID << "','"
		<< 1 << "','"
		<< name << "','"
		<< entry << "','"
		<< 0 << "','"
		<< 11 << "','"
		<< 1 << "','"
		<< "117440514 0,117440513 0,117440512 0,0 49408,0 49408,0 49408,0 49408,100663298 0,100663297 0,100663296 0," << "','"
		<< 500 << "','"
		<< 0 << "','"
		<< 0 << "','"
		<< 883 << "','"
		<< 1 << "','"
		<< 1 << "','"
		<< 1 << "','"
		<< 100 << "','"
		<< 310 << "','"
		<< 8308733 << "','"
		<< 1 << "','"
		<< 1 << "')";

	CharacterDatabase.Execute(ss.str().c_str());
}*/

Player* ObjectMgr::CreatePlayer(uint8 _class)
{
    uint32 guid;
    guid = ++m_hiPlayerGuid;

    Player* result = NULL;
    switch (_class)
    {
        case WARRIOR:
            result = new Warrior(guid);
            break;
        case PALADIN:
            result = new Paladin(guid);
            break;
        case HUNTER:
            result = new Hunter(guid);
            break;
        case ROGUE:
            result = new Rogue(guid);
            break;
        case PRIEST:
            result = new Priest(guid);
            break;
        case DEATHKNIGHT:
            result = new DeathKnight(guid);
            break;
        case SHAMAN:
            result = new Shaman(guid);
            break;
        case MAGE:
            result = new Mage(guid);
            break;
        case WARLOCK:
            result = new Warlock(guid);
            break;
        case DRUID:
            result = new Druid(guid);
            break;
    }
    return result;
}

void ObjectMgr::AddPlayer(Player* p) //add it to global storage
{
    _playerslock.AcquireWriteLock();
    _players[p->GetLowGUID()] = p;
    _playerslock.ReleaseWriteLock();
}

void ObjectMgr::RemovePlayer(Player* p)
{
    _playerslock.AcquireWriteLock();
    _players.erase(p->GetLowGUID());
    _playerslock.ReleaseWriteLock();

}

Corpse* ObjectMgr::CreateCorpse()
{
    uint32 guid;
    guid = ++m_hiCorpseGuid;

    return new Corpse(HIGHGUID_TYPE_CORPSE, guid);
}

void ObjectMgr::AddCorpse(Corpse* p) //add it to global storage
{
    _corpseslock.Acquire();
    m_corpses[p->GetLowGUID()] = p;
    _corpseslock.Release();
}

void ObjectMgr::RemoveCorpse(Corpse* p)
{
    _corpseslock.Acquire();
    m_corpses.erase(p->GetLowGUID());
    _corpseslock.Release();
}

Corpse* ObjectMgr::GetCorpse(uint32 corpseguid)
{
    Corpse* rv;
    _corpseslock.Acquire();
    CorpseMap::const_iterator itr = m_corpses.find(corpseguid);
    rv = (itr != m_corpses.end()) ? itr->second : 0;
    _corpseslock.Release();
    return rv;
}

Transporter* ObjectMgr::GetTransporter(uint32 guid)
{
    Transporter* rv;
    _TransportLock.Acquire();
    std::unordered_map<uint32, Transporter*>::const_iterator itr = mTransports.find(guid);
    rv = (itr != mTransports.end()) ? itr->second : 0;
    _TransportLock.Release();
    return rv;
}

void ObjectMgr::AddTransport(Transporter* pTransporter)
{
    _TransportLock.Acquire();
    mTransports[pTransporter->GetUIdFromGUID()] = pTransporter;
    _TransportLock.Release();
}

Transporter* ObjectMgr::GetTransporterByEntry(uint32 entry)
{
    Transporter* ret = nullptr;
    _TransportLock.Acquire();
    auto transporter = mTransports.find(entry);
    if (transporter != mTransports.end())
        ret = transporter->second;
    _TransportLock.Release();
    return ret;
}

void ObjectMgr::LoadGuildCharters()
{
    m_hiCharterId.SetVal(0);
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM charters");
    if (!result)
        return;
    do
    {
        Charter* c = new Charter(result->Fetch());
        m_charters[c->CharterType].insert(std::make_pair(c->GetID(), c));
        if (c->GetID() > int64(m_hiCharterId.GetVal()))
            m_hiCharterId.SetVal(c->GetID());
    }
    while (result->NextRow());
    delete result;
    Log.Success("ObjectMgr", "%u charters loaded.", m_charters[0].size());
}

Charter* ObjectMgr::GetCharter(uint32 CharterId, CharterTypes Type)
{
    Charter* rv;
    std::unordered_map<uint32, Charter*>::iterator itr;
    m_charterLock.AcquireReadLock();
    itr = m_charters[Type].find(CharterId);
    rv = (itr == m_charters[Type].end()) ? 0 : itr->second;
    m_charterLock.ReleaseReadLock();
    return rv;
}

Charter* ObjectMgr::CreateCharter(uint32 LeaderGuid, CharterTypes Type)
{
    uint32 charterid = 0;
    charterid = ++m_hiCharterId;

    Charter* c = new Charter(charterid, LeaderGuid, Type);
    m_charters[c->CharterType].insert(std::make_pair(c->GetID(), c));

    return c;
}

Charter::Charter(Field* fields)
{
    uint32 f = 0;
    CharterId = fields[f++].GetUInt32();
    CharterType = fields[f++].GetUInt32();
    LeaderGuid = fields[f++].GetUInt32();
    GuildName = fields[f++].GetString();
    ItemGuid = fields[f++].GetUInt64();
    SignatureCount = 0;
    Slots = GetNumberOfSlotsByType();
    Signatures = new uint32[Slots];

    for (uint32 i = 0; i < Slots; ++i)
    {
        Signatures[i] = fields[f++].GetUInt32();
        if (Signatures[i])
            ++SignatureCount;
    }

    // Unknown... really?
    Unk1 = 0;
    Unk2 = 0;
    Unk3 = 0;
    PetitionSignerCount = 0;
}

void Charter::AddSignature(uint32 PlayerGuid)
{
    if (SignatureCount >= Slots)
        return;

    SignatureCount++;
    uint32 i;
    for (i = 0; i < Slots; ++i)
    {
        if (Signatures[i] == 0)
        {
            Signatures[i] = PlayerGuid;
            break;
        }
    }

    ARCEMU_ASSERT(i != Slots);
}

void Charter::RemoveSignature(uint32 PlayerGuid)
{
    for (uint32 i = 0; i < Slots; ++i)
    {
        if (Signatures[i] == PlayerGuid)
        {
            Signatures[i] = 0;
            SignatureCount--;
            SaveToDB();
            break;
        }
    }
}

void Charter::Destroy()
{
    if (Slots == 0)            // ugly hack because of f*cked memory
        return;

    //meh remove from objmgr
    objmgr.RemoveCharter(this);
    // Kill the players with this (in db/offline)
    CharacterDatabase.Execute("DELETE FROM charters WHERE charterId = %u", CharterId);
    Player* p;
#ifdef WIN32
    __try
    {
#endif
        for (uint32 i = 0; i < Slots; ++i)
        {
            if (!Signatures[i])
                continue;
            p = objmgr.GetPlayer(Signatures[i]);
            if (p != NULL)
                p->m_charters[CharterType] = 0;
        }
#ifdef WIN32
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return;
    }
#endif

    // click, click, boom!
    delete this;
}

void Charter::SaveToDB()
{
    std::stringstream ss;
    uint32 i;

    ss << "DELETE FROM charters WHERE charterId = ";
    ss << CharterId;
    ss << ";";

    CharacterDatabase.Execute(ss.str().c_str());

    ss.rdbuf()->str("");

    ss << "INSERT INTO charters VALUES(" << CharterId << "," << CharterType << "," << LeaderGuid << ",'" << GuildName << "'," << ItemGuid;

    for (i = 0; i < Slots; ++i)
        ss << "," << Signatures[i];

    for (; i < 9; ++i)
        ss << ",0";

    ss << ")";
    CharacterDatabase.Execute(ss.str().c_str());
}

Charter* ObjectMgr::GetCharterByItemGuid(uint64 guid)
{
    m_charterLock.AcquireReadLock();
    for (int i = 0; i < NUM_CHARTER_TYPES; ++i)
    {
        std::unordered_map<uint32, Charter*>::iterator itr = m_charters[i].begin();
        for (; itr != m_charters[i].end(); ++itr)
        {
            if (itr->second->ItemGuid == guid)
            {
                m_charterLock.ReleaseReadLock();
                return itr->second;
            }
        }
    }
    m_charterLock.ReleaseReadLock();
    return NULL;
}

Charter* ObjectMgr::GetCharterByGuid(uint64 playerguid, CharterTypes type)
{
    m_charterLock.AcquireReadLock();
    std::unordered_map<uint32, Charter*>::iterator itr = m_charters[type].begin();
    for (; itr != m_charters[type].end(); ++itr)
    {
        if (playerguid == itr->second->LeaderGuid)
        {
            m_charterLock.ReleaseReadLock();
            return itr->second;
        }

        for (uint32 j = 0; j < itr->second->SignatureCount; ++j)
        {
            if (itr->second->Signatures[j] == playerguid)
            {
                m_charterLock.ReleaseReadLock();
                return itr->second;
            }
        }
    }
    m_charterLock.ReleaseReadLock();
    return NULL;
}

Charter* ObjectMgr::GetCharterByName(std::string & charter_name, CharterTypes Type)
{
    Charter* rv = 0;
    m_charterLock.AcquireReadLock();
    std::unordered_map<uint32, Charter*>::iterator itr = m_charters[Type].begin();
    for (; itr != m_charters[Type].end(); ++itr)
    {
        if (itr->second->GuildName == charter_name)
        {
            rv = itr->second;
            break;
        }
    }

    m_charterLock.ReleaseReadLock();
    return rv;
}

void ObjectMgr::RemoveCharter(Charter* c)
{
    if (c == NULL)
        return;
    if (c->CharterType >= NUM_CHARTER_TYPES)
    {
        Log.Notice("ObjectMgr", "Charter %u cannot be destroyed as type %u is not a sane type value.", c->CharterId, c->CharterType);
        return;
    }
    m_charterLock.AcquireWriteLock();
    m_charters[c->CharterType].erase(c->CharterId);
    m_charterLock.ReleaseWriteLock();
}

void ObjectMgr::LoadReputationModifierTable(const char* tablename, ReputationModMap* dmap)
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM %s", tablename);
    ReputationModMap::iterator itr;
    ReputationModifier* modifier;
    ReputationMod mod;

    if (result)
    {
        do
        {
            mod.faction[0] = result->Fetch()[1].GetUInt32();
            mod.faction[1] = result->Fetch()[2].GetUInt32();
            mod.value = result->Fetch()[3].GetInt32();
            mod.replimit = result->Fetch()[4].GetUInt32();

            itr = dmap->find(result->Fetch()[0].GetUInt32());
            if (itr == dmap->end())
            {
                modifier = new ReputationModifier;
                modifier->entry = result->Fetch()[0].GetUInt32();
                modifier->mods.push_back(mod);
                dmap->insert(ReputationModMap::value_type(result->Fetch()[0].GetUInt32(), modifier));
            }
            else
            {
                itr->second->mods.push_back(mod);
            }
        }
        while (result->NextRow());
        delete result;
    }
    Log.Notice("ObjectMgr", "%u reputation modifiers on %s.", dmap->size(), tablename);
}

void ObjectMgr::LoadReputationModifiers()
{
    LoadReputationModifierTable("reputation_creature_onkill", &m_reputation_creature);
    LoadReputationModifierTable("reputation_faction_onkill", &m_reputation_faction);
    LoadInstanceReputationModifiers();
}

ReputationModifier* ObjectMgr::GetReputationModifier(uint32 entry_id, uint32 faction_id)
{
    // first, try fetching from the creature table (by faction is a fallback)
    ReputationModMap::iterator itr = m_reputation_creature.find(entry_id);
    if (itr != m_reputation_creature.end())
        return itr->second;

    // fetch from the faction table
    itr = m_reputation_faction.find(faction_id);
    if (itr != m_reputation_faction.end())
        return itr->second;

    // no data. fallback to default -5 value.
    return 0;
}

void ObjectMgr::LoadMonsterSay()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM npc_monstersay");
    if (!result) return;

    uint32 Entry, Event;
    Field* fields = result->Fetch();
    do
    {
        Entry = fields[0].GetUInt32();
        Event = fields[1].GetUInt32();

        if (Event >= NUM_MONSTER_SAY_EVENTS)
            continue;

        if (mMonsterSays[Event].find(Entry) != mMonsterSays[Event].end())
        {
            LOG_ERROR("Duplicate monstersay event %u for entry %u, skipping", Event, Entry);
            continue;
        }

        NpcMonsterSay* ms = new NpcMonsterSay;
        ms->Chance = fields[2].GetFloat();
        ms->Language = fields[3].GetUInt32();
        ms->Type = fields[4].GetUInt32();
        ms->MonsterName = fields[5].GetString() ? strdup(fields[5].GetString()) : strdup("None");

        char* texts[5];
        char* text;
        uint32 textcount = 0;

        for (uint32 i = 0; i < 5; ++i)
        {
            text = (char*)fields[6 + i].GetString();
            if (!text) continue;
            if (strlen(fields[6 + i].GetString()) < 5)
                continue;

            texts[textcount] = strdup(fields[6 + i].GetString());

            // check for ;
            if (texts[textcount][strlen(texts[textcount]) - 1] == ';')
                texts[textcount][strlen(texts[textcount]) - 1] = 0;

            ++textcount;
        }

        if (!textcount)
        {
            free(((char*)ms->MonsterName));
            delete ms;
            continue;
        }

        ms->Texts = new const char*[textcount];
        memcpy(ms->Texts, texts, sizeof(char*) * textcount);
        ms->TextCount = textcount;

        mMonsterSays[Event].insert(std::make_pair(Entry, ms));

    }
    while (result->NextRow());
    Log.Success("ObjectMgr", "%u monster say events loaded.", result->GetRowCount());
    delete result;
}

NpcMonsterSay* ObjectMgr::HasMonsterSay(uint32 Entry, MONSTER_SAY_EVENTS Event)
{
    MonsterSayMap::iterator itr = mMonsterSays[Event].find(Entry);
    if (itr == mMonsterSays[Event].end())
        return NULL;

    return itr->second;
}

void ObjectMgr::LoadInstanceReputationModifiers()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM reputation_instance_onkill");
    if (!result)
        return;
    do
    {
        Field* fields = result->Fetch();
        InstanceReputationMod mod;
        mod.mapid = fields[0].GetUInt32();
        mod.mob_rep_reward = fields[1].GetInt32();
        mod.mob_rep_limit = fields[2].GetUInt32();
        mod.boss_rep_reward = fields[3].GetInt32();
        mod.boss_rep_limit = fields[4].GetUInt32();
        mod.faction[0] = fields[5].GetUInt32();
        mod.faction[1] = fields[6].GetUInt32();

        std::unordered_map<uint32, InstanceReputationModifier*>::iterator itr = m_reputation_instance.find(mod.mapid);
        if (itr == m_reputation_instance.end())
        {
            InstanceReputationModifier* m = new InstanceReputationModifier;
            m->mapid = mod.mapid;
            m->mods.push_back(mod);
            m_reputation_instance.insert(std::make_pair(m->mapid, m));
        }
        else
            itr->second->mods.push_back(mod);

    }
    while (result->NextRow());
    delete result;
    Log.Success("ObjectMgr", "%u instance reputation modifiers loaded.", m_reputation_instance.size());
}

bool ObjectMgr::HandleInstanceReputationModifiers(Player* pPlayer, Unit* pVictim)
{
    uint32 team = pPlayer->GetTeam();
    bool is_boss;
    if (!pVictim->IsCreature())
        return false;

    std::unordered_map<uint32, InstanceReputationModifier*>::iterator itr = m_reputation_instance.find(pVictim->GetMapId());
    if (itr == m_reputation_instance.end())
        return false;

    is_boss = false;//TO< Creature* >(pVictim)->GetCreatureInfo() ? ((Creature*)pVictim)->GetCreatureInfo()->Rank : 0;
    if (static_cast< Creature* >(pVictim)->GetProto()->boss)
        is_boss = true;

    // Apply the bonuses as normal.
    int32 replimit;
    int32 value;

    for (std::vector<InstanceReputationMod>::iterator i = itr->second->mods.begin(); i != itr->second->mods.end(); ++i)
    {
        if (!(*i).faction[team])
            continue;

        if (is_boss)
        {
            value = i->boss_rep_reward;
            replimit = i->boss_rep_limit;
        }
        else
        {
            value = i->mob_rep_reward;
            replimit = i->mob_rep_limit;
        }

        if (!value || (replimit && pPlayer->GetStanding(i->faction[team]) >= replimit))
            continue;

        //value *= sWorld.getRate(RATE_KILLREPUTATION);
        value = float2int32(value * sWorld.getRate(RATE_KILLREPUTATION));
        pPlayer->ModStanding(i->faction[team], value);
    }

    return true;
}

void ObjectMgr::LoadDisabledSpells()
{
    QueryResult* result = WorldDatabase.Query("SELECT * FROM spell_disable");
    if (result)
    {
        do
        {
            m_disabled_spells.insert(result->Fetch()[0].GetUInt32());
        }
        while (result->NextRow());
        delete result;
    }

    Log.Notice("ObjectMgr", "%u disabled spells.", m_disabled_spells.size());
}

void ObjectMgr::ReloadDisabledSpells()
{
    m_disabled_spells.clear();
    LoadDisabledSpells();
}

void ObjectMgr::LoadGroups()
{
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM groups");
    if (result)
    {
        if (result->GetFieldCount() != 52)
        {
            Log.LargeErrorMessage("groups table format is invalid. Please update your database.", NULL);
            return;
        }
        do
        {
            Group* g = new Group(false);
            g->LoadFromDB(result->Fetch());
        }
        while (result->NextRow());
        delete result;
    }

    Log.Success("ObjectMgr", "%u groups loaded.", this->m_groups.size());
}

void ObjectMgr::LoadArenaTeams()
{
    QueryResult* result = CharacterDatabase.Query("SELECT * FROM arenateams");
    if (result != NULL)
    {
        if (result->GetFieldCount() != 22)
        {
            Log.LargeErrorMessage("arenateams table format is invalid. Please update your database.", NULL);
            return;
        }
        do
        {
            ArenaTeam* team = new ArenaTeam(result->Fetch());
            AddArenaTeam(team);
            if (team->m_id > uint32(m_hiArenaTeamId.GetVal()))
                m_hiArenaTeamId.SetVal(uint32(team->m_id));

        }
        while (result->NextRow());
        delete result;
    }

    /* update the ranking */
    UpdateArenaTeamRankings();
}

ArenaTeam* ObjectMgr::GetArenaTeamByGuid(uint32 guid, uint32 Type)
{
    m_arenaTeamLock.Acquire();
    for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeamMap[Type].begin(); itr != m_arenaTeamMap[Type].end(); ++itr)
    {
        if (itr->second->HasMember(guid))
        {
            m_arenaTeamLock.Release();
            return itr->second;
        }
    }
    m_arenaTeamLock.Release();
    return NULL;
}

ArenaTeam* ObjectMgr::GetArenaTeamById(uint32 id)
{
    std::unordered_map<uint32, ArenaTeam*>::iterator itr;
    m_arenaTeamLock.Acquire();
    itr = m_arenaTeams.find(id);
    m_arenaTeamLock.Release();
    return (itr == m_arenaTeams.end()) ? NULL : itr->second;
}

ArenaTeam* ObjectMgr::GetArenaTeamByName(std::string & name, uint32 Type)
{
    m_arenaTeamLock.Acquire();
    for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeams.begin(); itr != m_arenaTeams.end(); ++itr)
    {
        if (!strnicmp(itr->second->m_name.c_str(), name.c_str(), name.size()))
        {
            m_arenaTeamLock.Release();
            return itr->second;
        }
    }
    m_arenaTeamLock.Release();
    return NULL;
}

void ObjectMgr::RemoveArenaTeam(ArenaTeam* team)
{
    m_arenaTeamLock.Acquire();
    m_arenaTeams.erase(team->m_id);
    m_arenaTeamMap[team->m_type].erase(team->m_id);
    m_arenaTeamLock.Release();
}

void ObjectMgr::AddArenaTeam(ArenaTeam* team)
{
    m_arenaTeamLock.Acquire();
    m_arenaTeams[team->m_id] = team;
    m_arenaTeamMap[team->m_type].insert(std::make_pair(team->m_id, team));
    m_arenaTeamLock.Release();
}

class ArenaSorter
{
    public:

        bool operator()(ArenaTeam* const & a, ArenaTeam* const & b)
        {
            return (a->m_stat_rating > b->m_stat_rating);
        }

        bool operator()(ArenaTeam*& a, ArenaTeam*& b)
        {
            return (a->m_stat_rating > b->m_stat_rating);
        }
};

void ObjectMgr::UpdateArenaTeamRankings()
{
    m_arenaTeamLock.Acquire();
    for (uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
    {
        std::vector<ArenaTeam*> ranking;

        for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
            ranking.push_back(itr->second);

        std::sort(ranking.begin(), ranking.end(), ArenaSorter());
        uint32 rank = 1;
        for (std::vector<ArenaTeam*>::iterator itr = ranking.begin(); itr != ranking.end(); ++itr)
        {
            if ((*itr)->m_stat_ranking != rank)
            {
                (*itr)->m_stat_ranking = rank;
                (*itr)->SaveToDB();
            }
            ++rank;
        }
    }
    m_arenaTeamLock.Release();
}

void ObjectMgr::ResetArenaTeamRatings()
{
    m_arenaTeamLock.Acquire();
    for (uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
    {
        for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
        {
            ArenaTeam* team = itr->second;
            if (team)
            {
                team->m_stat_gamesplayedseason = 0;
                team->m_stat_gamesplayedweek = 0;
                team->m_stat_gameswonseason = 0;
                team->m_stat_gameswonweek = 0;
                team->m_stat_rating = 1500;
                for (uint32 j = 0; j < team->m_memberCount; ++j)
                {
                    team->m_members[j].Played_ThisSeason = 0;
                    team->m_members[j].Played_ThisWeek = 0;
                    team->m_members[j].Won_ThisSeason = 0;
                    team->m_members[j].Won_ThisWeek = 0;
                    team->m_members[j].PersonalRating = 1500;
                }
                team->SaveToDB();
            }
        }
    }
    m_arenaTeamLock.Release();

    UpdateArenaTeamRankings();
}

void ObjectMgr::UpdateArenaTeamWeekly()
{
    // reset weekly matches count for all teams and all members
    m_arenaTeamLock.Acquire();
    for (uint32 i = 0; i < NUM_ARENA_TEAM_TYPES; ++i)
    {
        for (std::unordered_map<uint32, ArenaTeam*>::iterator itr = m_arenaTeamMap[i].begin(); itr != m_arenaTeamMap[i].end(); ++itr)
        {
            ArenaTeam* team = itr->second;
            if (team)
            {
                team->m_stat_gamesplayedweek = 0;
                team->m_stat_gameswonweek = 0;
                for (uint32 j = 0; j < team->m_memberCount; ++j)
                {
                    team->m_members[j].Played_ThisWeek = 0;
                    team->m_members[j].Won_ThisWeek = 0;
                }
                team->SaveToDB();
            }
        }
    }
    m_arenaTeamLock.Release();
}

void ObjectMgr::ResetDailies()
{
    _playerslock.AcquireReadLock();
    PlayerStorageMap::iterator itr = _players.begin();
    for (; itr != _players.end(); itr++)
    {
        Player* pPlayer = itr->second;
        pPlayer->DailyMutex.Acquire();
        pPlayer->m_finishedDailies.clear();
        pPlayer->DailyMutex.Release();
    }
    _playerslock.ReleaseReadLock();
}

void ObjectMgr::LoadSpellTargetConstraints()
{
    enum { CREATURE_TYPE, GAMEOBJECT_TYPE };

    Log.Notice("ObjectMgr", "Loading spell target constraints...");

    // Let's try to be idiot proof :/
    QueryResult* result = WorldDatabase.Query("SELECT * FROM spelltargetconstraints WHERE SpellID > 0 ORDER BY SpellID");

    if (result != NULL)
    {
        uint32 oldspellid = 0;
        SpellTargetConstraint* stc = NULL;
        Field* fields = NULL;

        do
        {
            fields = result->Fetch();

            if (fields != NULL)
            {
                uint32 spellid = fields[0].GetUInt32();

                if (oldspellid != spellid)
                {
                    stc = new SpellTargetConstraint;

                    m_spelltargetconstraints.insert(std::pair< uint32, SpellTargetConstraint* >(spellid, stc));
                }

                uint32 type = fields[1].GetUInt32();
                uint32 value = fields[2].GetUInt32();

                if (type == CREATURE_TYPE)
                    stc->AddCreature(value);
                else
                    stc->AddGameobject(value);

                oldspellid = spellid;
            }
        }
        while (result->NextRow());
    }

    delete result;

    Log.Notice("ObjectMgr", "Loaded constraints for %u spells...", m_spelltargetconstraints.size());
}

SpellTargetConstraint* ObjectMgr::GetSpellTargetConstraintForSpell(uint32 spellid)
{
    SpellTargetConstraintMap::const_iterator itr = m_spelltargetconstraints.find(spellid);

    if (itr != m_spelltargetconstraints.end())
        return itr->second;
    else
        return NULL;
}

uint32 ObjectMgr::GenerateArenaTeamId()
{
    uint32 ret;
    ret = ++m_hiArenaTeamId;

    return ret;
}

uint32 ObjectMgr::GenerateGroupId()
{
    uint32 r;
    r = ++m_hiGroupId;

    return r;
}

uint32 ObjectMgr::GenerateGuildId()
{
    uint32 r;
    r = ++m_hiGuildId;

    return r;
}

uint32 ObjectMgr::GenerateCreatureSpawnID()
{
    uint32 r;
    r = ++m_hiCreatureSpawnId;

    return r;
}

uint32 ObjectMgr::GenerateGameObjectSpawnID()
{
    uint32 r;
    r = ++m_hiGameObjectSpawnId;

    return r;
}

void ObjectMgr::AddPlayerCache(uint32 guid, PlayerCache* cache)
{
    m_playerCacheLock.Acquire();
    cache->AddRef();
    PlayerCacheMap::iterator itr = m_playerCache.find(guid);
    if (itr != m_playerCache.end())
    {
        itr->second->DecRef();
        itr->second = cache;
    }
    else
        m_playerCache.insert(std::make_pair(guid, cache));
    m_playerCacheLock.Release();
}

void ObjectMgr::RemovePlayerCache(uint32 guid)
{
    m_playerCacheLock.Acquire();
    PlayerCacheMap::iterator itr = m_playerCache.find(guid);
    if (itr != m_playerCache.end())
    {
        itr->second->DecRef();
        m_playerCache.erase(itr);
    }
    m_playerCacheLock.Release();
}

PlayerCache* ObjectMgr::GetPlayerCache(uint32 guid)
{
    m_playerCacheLock.Acquire();
    PlayerCacheMap::iterator itr = m_playerCache.find(guid);
    if (itr != m_playerCache.end())
    {
        PlayerCache* ret = itr->second;
        ret->AddRef();
        m_playerCacheLock.Release();
        return ret;
    }
    m_playerCacheLock.Release();
    return NULL;
}

PlayerCache* ObjectMgr::GetPlayerCache(const char* name, bool caseSensitive /*= true*/)
{
    PlayerCache* ret = NULL;
    m_playerCacheLock.Acquire();
    PlayerCacheMap::iterator itr;

    if (!caseSensitive)
    {
        std::string strName = name;
        arcemu_TOLOWER(strName);
        for (itr = m_playerCache.begin(); itr != m_playerCache.end(); ++itr)
        {
            std::string cachename;
            itr->second->GetStringValue(CACHE_PLAYER_NAME, cachename);
            if (!stricmp(cachename.c_str(), strName.c_str()))
            {
                ret = itr->second;
                ret->AddRef();
                break;
            }
        }
    }
    else
    {
        for (itr = m_playerCache.begin(); itr != m_playerCache.end(); ++itr)
        {
            std::string cachename;
            itr->second->GetStringValue(CACHE_PLAYER_NAME, cachename);
            if (!strcmp(cachename.c_str(), name))
            {
                ret = itr->second;
                itr->second->AddRef();
                break;
            }
        }
    }

    m_playerCacheLock.Release();

    return ret;
}

void ObjectMgr::LoadVehicleAccessories()
{
    QueryResult *result = WorldDatabase.Query("SELECT creature_entry, accessory_entry, seat FROM vehicle_accessories;");

    if (result != NULL)
    {
        do
        {
            Field *row = result->Fetch();
            VehicleAccessoryEntry *entry = new VehicleAccessoryEntry();
            uint32 creature_entry = 0;

            creature_entry = row[0].GetUInt32();
            entry->accessory_entry = row[1].GetUInt32();
            entry->seat = row[2].GetUInt32();

            std::map< uint32, std::vector< VehicleAccessoryEntry* >* >::iterator itr = vehicle_accessories.find(creature_entry);

            if (itr != vehicle_accessories.end())
            {
                itr->second->push_back(entry);
            }
            else
            {
                std::vector< VehicleAccessoryEntry* >* v = new std::vector< VehicleAccessoryEntry* >();
                v->push_back(entry);
                vehicle_accessories.insert(std::make_pair(creature_entry, v));
            }

        }
        while (result->NextRow());
        delete result;
    }
}

std::vector< VehicleAccessoryEntry* >* ObjectMgr::GetVehicleAccessories(uint32 creature_entry)
{
    std::map< uint32, std::vector< VehicleAccessoryEntry* >* >::iterator itr = vehicle_accessories.find(creature_entry);

    if (itr == vehicle_accessories.end())
        return NULL;
    else
        return itr->second;
}

void ObjectMgr::LoadWorldStateTemplates()
{
    QueryResult *result = WorldDatabase.QueryNA("SELECT DISTINCT map FROM worldstate_templates ORDER BY map;");

    if (result == NULL)
        return;

    do
    {
        Field *row = result->Fetch();
        uint32 mapid = row[0].GetUInt32();

        worldstate_templates.insert(std::make_pair(mapid, new std::multimap< uint32, WorldState >()));

    }
    while (result->NextRow());

    delete result;

    result = WorldDatabase.QueryNA("SELECT map, zone, field, value FROM worldstate_templates;");

    if (result == NULL)
        return;

    do
    {
        Field *row = result->Fetch();
        WorldState ws;

        uint32 mapid = row[0].GetUInt32();
        uint32 zone = row[1].GetUInt32();
        ws.field = row[2].GetUInt32();
        ws.value = row[3].GetUInt32();

        std::map< uint32, std::multimap< uint32, WorldState >* >::iterator itr
            = worldstate_templates.find(mapid);

        if (itr == worldstate_templates.end())
            continue;

        itr->second->insert(std::make_pair(zone, ws));

    }
    while (result->NextRow());
    delete result;
}

std::multimap< uint32, WorldState >* ObjectMgr::GetWorldStatesForMap(uint32 map) const{
    std::map< uint32, std::multimap< uint32, WorldState >* >::const_iterator itr = worldstate_templates.find(map);

    if (itr == worldstate_templates.end())
        return NULL;
    else
        return itr->second;
}

AreaTrigger const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    for (AreaTriggerContainer::const_iterator itr = _areaTriggerStore.begin(); itr != _areaTriggerStore.end(); ++itr)
    {
        if (itr->second.Mapid == Map)
        {
            AreaTriggerEntry const* atEntry = dbcAreaTrigger.LookupEntry(itr->first);
            if (atEntry)
                return &itr->second;
        }
    }
    return NULL;
}

void ObjectMgr::LoadAreaTrigger()
{
    uint32 oldMSTime = getMSTime();

    _areaTriggerStore.clear();                                  // need for reload case
    //													0		1	2		3		4	5			6			7			8				9					10
    QueryResult* result = WorldDatabase.Query("SELECT entry, type, map, screen, name, position_x, position_y, position_z, orientation, required_honor_rank, required_level FROM areatriggers");
    if (!result)
    {
        Log.Notice("AreaTrigger", "Loaded 0 area trigger teleport definitions. DB table `areatriggers` is empty.");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        ++count;

        uint32 Trigger_ID = fields[0].GetUInt32();
        uint8 trigger_type = fields[1].GetUInt8();

        AreaTrigger at;

        at.Mapid = fields[2].GetUInt16();
        at.x = fields[5].GetFloat();
        at.y = fields[6].GetFloat();
        at.z = fields[7].GetFloat();
        at.o = fields[8].GetFloat();

        AreaTriggerEntry const* atEntry = dbcAreaTrigger.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            Log.Notice("AreaTrigger", "Area trigger (ID:%u) does not exist in `AreaTrigger.dbc`.", Trigger_ID);
            continue;
        }

        MapEntry const* mapEntry = dbcMap.LookupEntry(at.Mapid);
        if (!mapEntry)
        {
            Log.Notice("AreaTrigger", "Area trigger (ID:%u) target map (ID: %u) does not exist in `Map.dbc`.", Trigger_ID, at.Mapid);
            continue;
        }

        if (at.x == 0 && at.y == 0 && at.z == 0 && (trigger_type == 1 || trigger_type == 4))    // check target coordinates only for teleport triggers
        {
            Log.Notice("AreaTrigger", "Area trigger (ID:%u) target coordinates not provided.", Trigger_ID);
            continue;
        }

        _areaTriggerStore[Trigger_ID] = at;

    }
    while (result->NextRow());

    Log.Success("AreaTrigger", "Loaded %u area trigger teleport definitions", count);
}

void ObjectMgr::LoadEventScripts()
{
    Log.Notice("ObjectMgr", "Loading Event Scripts...");

    bool success = false;
    const char* eventScriptsQuery = "SELECT event_id, function, script_type, data_1, data_2, data_3, data_4, data_5, x, y, z, o, delay, next_event FROM event_scripts WHERE event_id > 0 ORDER BY event_id";
    auto result = WorldDatabase.Query(&success, eventScriptsQuery);

    if (!success)
    {
        Log.Error("ObjectMgr", "Failed on Loading Queries from event_scripts.");
        return;
    }
    else
    {
        if (!result)
        {
            Log.Notice("ObjectMgr", "Loaded 0 event_scripts. DB table `event_scripts` is empty.");
            return;
        }
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 event_id = fields[0].GetUInt32();
        SimpleEventScript eventscript;

        eventscript.eventId     = event_id;
        eventscript.function    = static_cast<uint8>(ScriptCommands(fields[1].GetUInt8()));
        eventscript.scripttype  = static_cast<uint8>(EasyScriptTypes(fields[2].GetUInt8()));
        eventscript.data_1      = fields[3].GetUInt32();
        eventscript.data_2      = fields[4].GetUInt32();
        eventscript.data_3      = fields[5].GetUInt32();
        eventscript.data_4      = fields[6].GetUInt32();
        eventscript.data_5      = fields[7].GetUInt32();
        eventscript.x           = fields[8].GetUInt32();
        eventscript.y           = fields[9].GetUInt32();
        eventscript.z           = fields[10].GetUInt32();
        eventscript.o           = fields[11].GetUInt32();
        eventscript.delay       = fields[12].GetUInt32();
        eventscript.nextevent   = fields[13].GetUInt32();

        SimpleEventScript* SimpleEventScript = &mEventScriptMaps.insert(EventScriptMaps::value_type(event_id, eventscript))->second;

        // for search by spellid ( data_1 is spell id )
        if (eventscript.data_1 && eventscript.scripttype == static_cast<uint8>(EasyScriptTypes::SCRIPT_TYPE_SPELL_EFFECT))
            mSpellEffectMaps.insert(SpellEffectMaps::value_type(eventscript.data_1, SimpleEventScript));


        ++count;
    } while (result->NextRow());

    Log.Success("ObjectMgr", "Loaded event_scripts for %u events...", count);
}

EventScriptBounds ObjectMgr::GetEventScripts(uint32 event_id) const
{
    return EventScriptBounds(mEventScriptMaps.lower_bound(event_id), mEventScriptMaps.upper_bound(event_id));
}

SpellEffectMapBounds ObjectMgr::GetSpellEffectBounds(uint32 data_1) const
{
    return SpellEffectMapBounds(mSpellEffectMaps.lower_bound(data_1), mSpellEffectMaps.upper_bound(data_1));
}

bool ObjectMgr::CheckforScripts(Player* plr, uint32 event_id)
{
    EventScriptBounds EventScript = objmgr.GetEventScripts(event_id);
    if (EventScript.first == EventScript.second)
        return false;

    for (EventScriptMaps::const_iterator itr = EventScript.first; itr != EventScript.second; ++itr)
    {
        sEventMgr.AddEvent(this, &ObjectMgr::EventScriptsUpdate, plr, itr->second.eventId, EVENT_EVENT_SCRIPTS, itr->second.delay, 1, 0);
    }

    return true;
}

bool ObjectMgr::CheckforDummySpellScripts(Player* plr, uint32 data_1)
{
    SpellEffectMapBounds EventScript = objmgr.GetSpellEffectBounds(data_1);
    if (EventScript.first == EventScript.second)
        return false;

    for (SpellEffectMaps::const_iterator itr = EventScript.first; itr != EventScript.second; ++itr)
    {
        sEventMgr.AddEvent(this, &ObjectMgr::EventScriptsUpdate, plr, itr->second->eventId, EVENT_EVENT_SCRIPTS, itr->second->delay, 1, 0);
    }

    return true;
}

void ObjectMgr::EventScriptsUpdate(Player* plr, uint32 next_event)
{
    EventScriptBounds EventScript = objmgr.GetEventScripts(next_event);

    for (EventScriptMaps::const_iterator itr = EventScript.first; itr != EventScript.second; ++itr)
    {
        if (itr->second.scripttype == static_cast<uint8>(EasyScriptTypes::SCRIPT_TYPE_SPELL_EFFECT) || itr->second.scripttype == static_cast<uint8>(EasyScriptTypes::SCRIPT_TYPE_DUMMY))
        {
            switch (itr->second.function)
            {
            case static_cast<uint8>(ScriptCommands::SCRIPT_COMMAND_RESPAWN_GAMEOBJECT):
            {
                Object* target = plr->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), itr->second.data_1);
                if (target == NULL)
                    return;

                static_cast<GameObject*>(target)->Despawn(1000, itr->second.data_2);

                break;
            }

            case static_cast<uint8>(ScriptCommands::SCRIPT_COMMAND_KILL_CREDIT):
            {
                QuestLogEntry* pQuest = plr->GetQuestLogForEntry(itr->second.data_2);
                if (pQuest != NULL)
                {
                    if (pQuest->GetMobCount(itr->second.data_5) < pQuest->GetQuest()->ReqCreatureOrGOId[itr->second.data_5])
                    {
                        pQuest->SetMobCount(itr->second.data_5, pQuest->GetMobCount(itr->second.data_5) + 1);
                        pQuest->SendUpdateAddKill(itr->second.data_5);
                        pQuest->UpdatePlayerFields();
                    }
                }
                break;
            }
            }
        }

        if (itr->second.scripttype == static_cast<uint8>(EasyScriptTypes::SCRIPT_TYPE_GAMEOBJECT) || itr->second.scripttype == static_cast<uint8>(EasyScriptTypes::SCRIPT_TYPE_DUMMY))
        {
            switch (itr->second.function)
            {
            case static_cast<uint8>(ScriptCommands::SCRIPT_COMMAND_ACTIVATE_OBJECT):
            {
                if ((itr->second.x || itr->second.y || itr->second.z) == 0)
                {
                    Object* target = plr->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), itr->second.data_1);
                    if (target == NULL)
                        return;

                    if (static_cast<GameObject*>(target)->GetState() != GAMEOBJECT_STATE_OPEN)
                    {
                        static_cast<GameObject*>(target)->SetState(GAMEOBJECT_STATE_OPEN);
                    }
                    else
                    {
                        static_cast<GameObject*>(target)->SetState(GAMEOBJECT_STATE_CLOSED);
                    }
                }
                else
                {
                    Object* target = plr->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(itr->second.x, itr->second.y, itr->second.z, itr->second.data_1);
                    if (target == NULL)
                        return;

                    if (static_cast<GameObject*>(target)->GetState() != GAMEOBJECT_STATE_OPEN)
                    {
                        static_cast<GameObject*>(target)->SetState(GAMEOBJECT_STATE_OPEN);
                    }
                    else
                    {
                        static_cast<GameObject*>(target)->SetState(GAMEOBJECT_STATE_CLOSED);
                    }
                }
            }
            break;
            }
        }

        if (itr->second.nextevent != 0)
        {
            objmgr.CheckforScripts(plr, itr->second.nextevent);
        }
    }
}

void ObjectMgr::LoadQuests()
{
    // For reload case
    for (QuestMap::const_iterator itr = mQuestTemplates.begin(); itr != mQuestTemplates.end(); ++itr)
        delete itr->second;
    mQuestTemplates.clear();

    //mExclusiveQuestGroups.clear();

    //                                                      
    QueryResult* result = WorldDatabase.Query("SELECT "
        //0     1      2        3        4           5       6            7             8              9               10             11                 12
        "Id, Method, Level, MinLevel, MaxLevel, ZoneOrSort, Type, SuggestedPlayers, LimitTime, RequiredClasses, RequiredRaces, RequiredSkillId, RequiredSkillPoints, "
        //         13                 14                    15                   16                      17                  18                         19                  20
        "RequiredFactionId1, RequiredFactionId2, RequiredFactionValue1, RequiredFactionValue2, RequiredMinRepFaction, RequiredMaxRepFaction, RequiredMinRepValue, RequiredMaxRepValue, "
        //     21         22             23                24             25              26                    27                28            29              30              31
        "PrevQuestId, NextQuestId, ExclusiveGroup, NextQuestIdChain, RewardXPId, RewardOrRequiredMoney, RewardMoneyMaxLevel, RewardSpell, RewardSpellCast, RewardHonor, RewardHonorMultiplier, "
        //         32                  33            34             35               36         37         38            39                40                41               42                 43
        "RewardMailTemplateId, RewardMailDelay, SourceItemId, SourceItemCount, SourceSpellId, Flags, SpecialFlags, MinimapTargetMark, RewardTitleId, RequiredPlayerKills, RewardTalents, RewardArenaPoints, "
        //      44            45                    46                    47                  48               49             50              51             52                53                54                55               56
        "RewardSkillId, RewardSkillPoints, RewardReputationMask, QuestGiverPortrait, QuestTurnInPortrait, RewardItemId1, RewardItemId2, RewardItemId3, RewardItemId4, RewardItemCount1, RewardItemCount2, RewardItemCount3, RewardItemCount4, "
        //         57                  58                  59                    60                    61                   62                      63                  64                        65                       66                      67                      68
        "RewardChoiceItemId1, RewardChoiceItemId2, RewardChoiceItemId3, RewardChoiceItemId4, RewardChoiceItemId5, RewardChoiceItemId6, RewardChoiceItemCount1, RewardChoiceItemCount2, RewardChoiceItemCount3, RewardChoiceItemCount4, RewardChoiceItemCount5, RewardChoiceItemCount6, "
        //        69                70                71                72             73                   74                      75                     76                    77                      78
        "RewardFactionId1, RewardFactionId2, RewardFactionId3, RewardFactionId4, RewardFactionId5, RewardFactionValueId1, RewardFactionValueId2, RewardFactionValueId3, RewardFactionValueId4, RewardFactionValueId5, "
        //                79                          80                           81                              82                           83
        "RewardFactionValueIdOverride1, RewardFactionValueIdOverride2, RewardFactionValueIdOverride3, RewardFactionValueIdOverride4, RewardFactionValueIdOverride5, "
        //    84        85      86      87          88       89        90      91          92             93              94
        "PointMapId, PointX, PointY, PointOption, Title, Objectives, Details, EndText, CompletedText, OfferRewardText, RequestItemsText, "
        //        95              96               97               98                  99                     100                      101                  102
        "RequiredNpcOrGo1, RequiredNpcOrGo2, RequiredNpcOrGo3, RequiredNpcOrGo4, RequiredNpcOrGoCount1, RequiredNpcOrGoCount2, RequiredNpcOrGoCount3, RequiredNpcOrGoCount4, "
        //         103                     104                    105                   106                     107                       108                     109                       110
        "RequiredSourceItemId1, RequiredSourceItemId2, RequiredSourceItemId3, RequiredSourceItemId4, RequiredSourceItemCount1, RequiredSourceItemCount2, RequiredSourceItemCount3, RequiredSourceItemCount4, "
        //       111               112             113             114              115             116                 117                   118               119               120                 121                 122
        "RequiredItemId1, RequiredItemId2, RequiredItemId3, RequiredItemId4, RequiredItemId5, RequiredItemId6, RequiredItemCount1, RequiredItemCount2, RequiredItemCount3, RequiredItemCount4, RequiredItemCount5, RequiredItemCount6, "
        //      123             124                 125                126                  127              128              129             130           131
        "RequiredSpell, RequiredSpellCast1, RequiredSpellCast2, RequiredSpellCast3, RequiredSpellCast4, ObjectiveText1, ObjectiveText2, ObjectiveText3, ObjectiveText4, "
        //     132                  133               134               135                  136                  137                     138                   139
        "RewardCurrencyId1, RewardCurrencyId2, RewardCurrencyId3, RewardCurrencyId4, RewardCurrencyCount1, RewardCurrencyCount2, RewardCurrencyCount3, RewardCurrencyCount4, "
        //      140                  141                 142                   143                    144                    145                     146                   147
        "RequiredCurrencyId1, RequiredCurrencyId2, RequiredCurrencyId3, RequiredCurrencyId4, RequiredCurrencyCount1, RequiredCurrencyCount2, RequiredCurrencyCount3, RequiredCurrencyCount4, "
        //      148                  149                 150                   151               152          153
        "QuestGiverTextWindow, QuestGiverTargetName, QuestTurnTextWindow, QuestTurnTargetName, SoundAccept, SoundTurnIn, "
        //      154          155            156            157               158                159                  160                  161                162             163
        "DetailsEmote1, DetailsEmote2, DetailsEmote3, DetailsEmote4, DetailsEmoteDelay1, DetailsEmoteDelay2, DetailsEmoteDelay3, DetailsEmoteDelay4, EmoteOnIncomplete, EmoteOnComplete, "
        //      164                 165               166                167                   168                       169                     170                  171
        "OfferRewardEmote1, OfferRewardEmote2, OfferRewardEmote3, OfferRewardEmote4, OfferRewardEmoteDelay1, OfferRewardEmoteDelay2, OfferRewardEmoteDelay3, OfferRewardEmoteDelay4, "
        //    172           173              174                175
        "ReqEmoteId, ReqExploreTrigger1, ReqExploreTrigger2, ReqExploreTrigger3"
        " FROM quests");
    if (!result)
    {
        Log.Error("ObjectMgr", ">> Loaded 0 quests definitions from quests");
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 quest_id = fields[0].GetUInt32();

        Quest* newQuest = new Quest();

        newQuest->QuestId = quest_id;
        newQuest->QuestMethod = fields[1].GetUInt32();
        newQuest->QuestLevel = fields[2].GetInt32();
        newQuest->MinLevel = fields[3].GetUInt32();
        newQuest->MaxLevel = fields[4].GetUInt32();
        newQuest->ZoneOrSort = fields[5].GetInt32();
        newQuest->Type = fields[6].GetUInt32();
        newQuest->SuggestedPlayers = fields[7].GetUInt32();
        newQuest->LimitTime = fields[8].GetUInt32();
        newQuest->SkillOrClassMask = fields[9].GetUInt32();
        newQuest->RequiredRaces = fields[10].GetUInt32();
        newQuest->RequiredSkillId = fields[11].GetUInt32();
        newQuest->RequiredSkillValue = fields[12].GetUInt32();
        newQuest->RepObjectiveFaction = fields[13].GetUInt32();
        newQuest->RepObjectiveFaction2 = fields[14].GetUInt32();
        newQuest->RepObjectiveValue = fields[15].GetInt32();
        newQuest->RepObjectiveValue2 = fields[16].GetInt32();
        newQuest->RequiredMinRepFaction = fields[17].GetUInt32();
        newQuest->RequiredMaxRepFaction = fields[18].GetUInt32();
        newQuest->RequiredMinRepValue = fields[19].GetInt32();
        newQuest->RequiredMaxRepValue = fields[20].GetInt32();
        newQuest->PrevQuestId = fields[21].GetInt32();
        newQuest->NextQuestId = fields[22].GetInt32();
        newQuest->ExclusiveGroup = fields[23].GetInt32();
        newQuest->NextQuestInChain = fields[24].GetUInt32();
        newQuest->XPId = fields[25].GetUInt32();
        newQuest->RewOrReqMoney = fields[26].GetInt32();
        newQuest->RewMoneyMaxLevel = fields[27].GetUInt32();
        newQuest->RewSpell = fields[28].GetUInt32();
        newQuest->RewSpellCast = fields[29].GetInt32();
        newQuest->RewHonorAddition = fields[30].GetUInt32();
        newQuest->RewHonorMultiplier = fields[31].GetFloat();
        newQuest->RewMailTemplateId = fields[32].GetUInt32();
        newQuest->RewMailDelaySecs = fields[33].GetUInt32();
        newQuest->SrcItemId = fields[34].GetUInt32();
        newQuest->SrcItemCount = fields[35].GetUInt32();
        newQuest->SrcSpell = fields[36].GetUInt32();
        newQuest->QuestFlags = fields[37].GetUInt32();
        newQuest->SpecialFlags = fields[38].GetUInt16();
        newQuest->MinimapTargetMark = fields[39].GetUInt32();
        newQuest->CharTitleId = fields[40].GetUInt32();
        newQuest->PlayersSlain = fields[41].GetUInt32();
        newQuest->BonusTalents = fields[42].GetUInt32();
        newQuest->RewArenaPoints = fields[43].GetInt32();
        newQuest->RewSkillLineId = fields[44].GetUInt32();
        newQuest->RewSkillPoints = fields[45].GetUInt32();
        newQuest->RewRepMask = fields[46].GetUInt32();
        newQuest->QuestGiverPortrait = fields[47].GetUInt32();
        newQuest->QuestTurnInPortrait = fields[48].GetUInt32();


        for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
            newQuest->RewItemId[i] = fields[49 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
            newQuest->RewItemCount[i] = fields[53 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
            newQuest->RewChoiceItemId[i] = fields[57 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
            newQuest->RewChoiceItemCount[i] = fields[63 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            newQuest->RewRepFaction[i] = fields[69 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            newQuest->RewRepValueId[i] = fields[74 + i].GetInt32();

        for (uint8 i = 0; i < QUEST_REPUTATIONS_COUNT; ++i)
            newQuest->RewRepValue[i] = fields[79 + i].GetInt32();

        newQuest->PointMapId = fields[84].GetUInt32();
        newQuest->PointX = fields[85].GetFloat();
        newQuest->PointY = fields[86].GetFloat();
        newQuest->PointOpt = fields[87].GetUInt32();
        newQuest->Title = fields[88].GetString();
        newQuest->Objectives = fields[89].GetString();
        newQuest->Details = fields[90].GetString();
        newQuest->EndText = fields[91].GetString();
        newQuest->OfferRewardText = fields[92].GetString();
        newQuest->RequestItemsText = fields[93].GetString();
        newQuest->CompletedText = fields[94].GetString();


        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
            newQuest->ReqCreatureOrGOId[i] = fields[95 + i].GetInt32();

        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
            newQuest->ReqCreatureOrGOCount[i] = fields[99 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_SOURCE_ITEM_IDS_COUNT; ++i)
            newQuest->ReqSourceId[i] = fields[103 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_SOURCE_ITEM_IDS_COUNT; ++i)
            newQuest->ReqSourceCount[i] = fields[107 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
            newQuest->ReqItemId[i] = fields[111 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
            newQuest->ReqItemCount[i] = fields[117 + i].GetUInt32();

        newQuest->RequiredSpell = fields[123].GetUInt32();

        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i) // To be removed
            newQuest->RequiredSpellCast[i] = fields[124 + i].GetUInt32();

        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
            newQuest->ObjectiveText[i] = fields[128 + i].GetString();

        for (uint8 i = 0; i < QUEST_CURRENCY_COUNT; ++i)
            newQuest->RewCurrencyId[i] = fields[132 + i].GetUInt32();


        for (uint8 i = 0; i < QUEST_CURRENCY_COUNT; ++i)
            newQuest->RewCurrencyCount[i] = fields[136 + i].GetUInt32();


        newQuest->QuestGiverTextWindow = fields[148].GetString();
        newQuest->QuestGiverTargetName = fields[149].GetString();
        newQuest->QuestTurnTextWindow = fields[150].GetString();
        newQuest->QuestTurnTargetName = fields[151].GetString();
        newQuest->SoundAccept = fields[152].GetUInt32();
        newQuest->SoundTurnIn = fields[153].GetUInt32();

        for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
            newQuest->DetailsEmote[i] = fields[154 + i].GetUInt32();

        for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
            newQuest->DetailsEmoteDelay[i] = fields[158 + i].GetUInt32();

        newQuest->IncompleteEmote = fields[162].GetUInt32();
        newQuest->CompleteEmote = fields[163].GetUInt32();

        for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
            newQuest->OfferRewardEmote[i] = fields[164 + i].GetInt32();

        for (int i = 0; i < QUEST_EMOTE_COUNT; ++i)
            newQuest->OfferRewardEmoteDelay[i] = fields[168 + i].GetUInt32();

        newQuest->ReqEmoteId = fields[172].GetUInt32();

        newQuest->m_reqExploreTrigger[0] = 0;
        newQuest->m_reqExploreTrigger[1] = 0;
        newQuest->m_reqExploreTrigger[2] = 0;

        for (int i = 0; i < 3; ++i)
            newQuest->m_reqExploreTrigger[i] = fields[173 + i].GetUInt32();

        newQuest->QuestFlags |= newQuest->SpecialFlags << 20;
        if (newQuest->QuestFlags & QUEST_SPECIAL_FLAG_AUTO_ACCEPT)
            newQuest->QuestFlags |= QUEST_FLAGS_AUTO_ACCEPT;

        newQuest->m_reqitemscount = 0;
        newQuest->m_reqCreatureOrGOcount = 0;
        newQuest->m_rewitemscount = 0;
        newQuest->m_rewchoiceitemscount = 0;
        newQuest->m_rewCurrencyCount = 0;
        newQuest->m_reqCurrencyCount = 0;

        newQuest->m_reqMobType[0] = 0;
        newQuest->m_reqMobType[1] = 0;
        newQuest->m_reqMobType[2] = 0;
        newQuest->m_reqMobType[3] = 0;

        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        {
            if (newQuest->ReqCreatureOrGOId[i] != 0)
            {
                if (newQuest->ReqCreatureOrGOId[i] < 0)
                {
                    auto gameobject_info = GameObjectNameStorage.LookupEntry(newQuest->ReqCreatureOrGOId[i] * -1);
                    if (gameobject_info)
                    {
                        newQuest->m_reqMobType[i] = QUEST_MOB_TYPE_GAMEOBJECT;
                        newQuest->ReqCreatureOrGOId[i] *= -1;
                    }
                    else
                    {
                        // if quest has neither valid gameobject, log it.
                        LOG_DEBUG("Quest %lu has required_mobtype[%d]==%lu, it's not a valid GameObject.", newQuest->GetQuestId(), i, newQuest->ReqCreatureOrGOId[i]);
                    }
                }
                else
                {
                    CreatureInfo* c_info = CreatureNameStorage.LookupEntry(newQuest->ReqCreatureOrGOId[i]);
                    if (c_info)
                        newQuest->m_reqMobType[i] = QUEST_MOB_TYPE_CREATURE;
                    else
                    {
                        // if quest has neither valid creature, log it.
                        LOG_DEBUG("Quest %lu has required_mobtype[%d]==%lu, it's not a valid Creature.", newQuest->GetQuestId(), i, newQuest->ReqCreatureOrGOId[i]);
                    }
                }

                newQuest->m_reqCreatureOrGOcount++;
            }
        }

        for (int i = 0; i < QUEST_ITEM_OBJECTIVES_COUNT; ++i)
            if (newQuest->ReqItemId[i])
                ++newQuest->m_reqitemscount;

        for (int i = 0; i < QUEST_REWARDS_COUNT; ++i)
            if (newQuest->RewItemId[i])
                ++newQuest->m_rewitemscount;

        for (int i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
            if (newQuest->RewChoiceItemId[i])
                ++newQuest->m_rewchoiceitemscount;

        for (int i = 0; i < QUEST_CURRENCY_COUNT; ++i)
            if (newQuest->RewCurrencyId[i])
                ++newQuest->m_rewCurrencyCount;

        for (int i = 0; i < QUEST_CURRENCY_COUNT; ++i)
            if (newQuest->ReqCurrencyCount[i])
                ++newQuest->m_reqCurrencyCount;

        newQuest->pQuestScript = NULL;

        mQuestTemplates[quest_id] = newQuest;
    } while (result->NextRow());

    std::map<uint32, uint32> usedMailTemplates;

    // Post processing
    for (QuestMap::iterator iter = mQuestTemplates.begin(); iter != mQuestTemplates.end(); ++iter)
    {
        Quest * qinfo = iter->second;

        // additional quest integrity checks (GO, creature_template and item_template must be loaded already)

        if (qinfo->GetQuestMethod() >= 3)
            Log.Debug("ObjectMgr", "Quest %u has `Method` = %u, expected values are 0, 1 or 2.", qinfo->GetQuestId(), qinfo->GetQuestMethod());

        if (qinfo->SpecialFlags > QUEST_SPECIAL_FLAG_DB_ALLOWED)
        {
            Log.Debug("ObjectMgr", "Quest %u has `SpecialFlags` = %u, above max flags not allowed for database.", qinfo->GetQuestId(), qinfo->SpecialFlags);
        }

        if (qinfo->HasFlag(QUEST_FLAGS_DAILY) && qinfo->HasFlag(QUEST_FLAGS_WEEKLY))
        {
            Log.Debug("ObjectMgr", "Weekly Quest %u is marked as daily quest in `QuestFlags`, removed daily flag.", qinfo->GetQuestId());
            qinfo->QuestFlags &= ~QUEST_FLAGS_DAILY;
        }

        if (qinfo->HasFlag(QUEST_FLAGS_DAILY))
        {
            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
            {
                Log.Debug("ObjectMgr", "Daily Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE);
            }
        }

        if (qinfo->HasFlag(QUEST_FLAGS_WEEKLY))
        {
            if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE))
            {
                Log.Debug("ObjectMgr", "Weekly Quest %u not marked as repeatable in `SpecialFlags`, added.", qinfo->GetQuestId());
                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAG_REPEATABLE);
            }
        }

        if (qinfo->HasFlag(QUEST_FLAGS_AUTO_REWARDED))
        {
            // at auto-reward can be rewarded only RewChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j)
            {
                if (uint32 id = qinfo->RewChoiceItemId[j])
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewChoiceItemId%d` = %u but item from `RewChoiceItemId%d` can't be rewarded with quest flag QUEST_FLAGS_AUTO_REWARDED.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest ignore this data
                }
            }
        }

        // client quest log visual (sort case)
        if (qinfo->ZoneOrSort < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->ZoneOrSort));
            if (!qSort)
            {
                Log.Debug("ObjectMgr", "Quest %u has `ZoneOrSort` = %i (sort case) but quest sort with this id does not exist.",
                    qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }

            //check SkillOrClass value (class case).
            if (sQuestMgr.ClassByQuestSort(-int32(qinfo->ZoneOrSort)))
            {
                // SkillOrClass should not have class case when class case already set in ZoneOrSort.
                if (qinfo->SkillOrClassMask < 0)
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ZoneOrSort` = %i (class sort case) and `SkillOrClassMask` = %i (class case), redundant.",
                        qinfo->GetQuestId(), qinfo->ZoneOrSort, qinfo->SkillOrClassMask);
                }
            }

            //check for proper SkillOrClass value (skill case)
            if (int32 skill_id = sQuestMgr.SkillByQuestSort(-int32(qinfo->ZoneOrSort)))
            {
                // skill is positive value in SkillOrClass
                if (qinfo->SkillOrClassMask != skill_id)
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ZoneOrSort` = %i (skill sort case) but `SkillOrClassMask` does not have a corresponding value (%i).",
                        qinfo->GetQuestId(), qinfo->ZoneOrSort, skill_id);
                    //override, and force proper value here?
                }
            }
        }

        // SkillOrClassMask (class case)
        if (qinfo->SkillOrClassMask < 0)
        {
            if (!(-int32(qinfo->SkillOrClassMask) & CLASSMASK_ALL_PLAYABLE))
            {
                Log.Debug("ObjectMgr", "Quest %u has `SkillOrClassMask` = %i (class case) but classmask does not have valid class",
                    qinfo->GetQuestId(), qinfo->SkillOrClassMask);
            }
        }
        // SkillOrClassMask (skill case)
        else if (qinfo->SkillOrClassMask > 0)
        {
            if (!dbcSkillLine.LookupEntry(qinfo->SkillOrClassMask))
            {
                Log.Debug("ObjectMgr", "Quest %u has `SkillOrClass` = %u (skill case) but skill (%i) does not exist",
                    qinfo->GetQuestId(), qinfo->SkillOrClassMask, qinfo->SkillOrClassMask);
            }
        }

        if (qinfo->RequiredSkillValue)
        {
            if (qinfo->SkillOrClassMask <= 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RequiredSkillValue` = %u but `SkillOrClass` = %i (class case), value ignored.",
                    qinfo->GetQuestId(), qinfo->RequiredSkillValue, qinfo->SkillOrClassMask);
                // no changes, quest can't be done for this requirement (fail at wrong skill id)
            }
        }
        // else Skill quests can have 0 skill level, this is ok

        if (qinfo->RepObjectiveFaction2 && !sFactionStore.LookupEntry(qinfo->RepObjectiveFaction2))
        {
            Log.Debug("ObjectMgr", "Quest %u has `RepObjectiveFaction2` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RepObjectiveFaction2, qinfo->RepObjectiveFaction2);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RepObjectiveFaction && !sFactionStore.LookupEntry(qinfo->RepObjectiveFaction))
        {
            Log.Debug("ObjectMgr", "Quest %u has `RepObjectiveFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RepObjectiveFaction, qinfo->RepObjectiveFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMinRepFaction))
        {
            Log.Debug("ObjectMgr", "Quest %u has `RequiredMinRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMinRepFaction, qinfo->RequiredMinRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMaxRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMaxRepFaction))
        {
            Log.Debug("ObjectMgr", "Quest %u has `RequiredMaxRepFaction` = %u but faction template %u does not exist, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepFaction, qinfo->RequiredMaxRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMaxRepValue && qinfo->RequiredMaxRepValue <= qinfo->RequiredMinRepValue)
        {
            Log.Debug("ObjectMgr", "Quest %u has `RequiredMaxRepValue` = %d and `RequiredMinRepValue` = %d, quest can't be done.",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepValue, qinfo->RequiredMinRepValue);
            // no changes, quest can't be done for this requirement
        }

        if (!qinfo->RepObjectiveFaction && qinfo->RepObjectiveValue > 0)
        {
            Log.Debug("ObjectMgr", "Quest %u has `RepObjectiveValue` = %d but `RepObjectiveFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RepObjectiveValue);
            // warning
        }

        if (!qinfo->RepObjectiveFaction2 && qinfo->RepObjectiveValue2 > 0)
        {
            Log.Debug("ObjectMgr", "Quest %u has `RepObjectiveValue2` = %d but `RepObjectiveFaction2` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RepObjectiveValue2);
            // warning
        }

        if (!qinfo->RequiredMinRepFaction && qinfo->RequiredMinRepValue > 0)
        {
            Log.Debug("ObjectMgr", "Quest %u has `RequiredMinRepValue` = %d but `RequiredMinRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RequiredMinRepValue);
            // warning
        }

        if (!qinfo->RequiredMaxRepFaction && qinfo->RequiredMaxRepValue > 0)
        {
            Log.Debug("ObjectMgr", "Quest %u has `RequiredMaxRepValue` = %d but `RequiredMaxRepFaction` is 0, value has no effect",
                qinfo->GetQuestId(), qinfo->RequiredMaxRepValue);
            // warning
        }

        if (qinfo->CharTitleId && !dbcCharTitlesEntry.LookupEntry(qinfo->CharTitleId))
        {
            Log.Debug("ObjectMgr", "Quest %u has `CharTitleId` = %u but CharTitle Id %u does not exist, quest can't be rewarded with title.",
                qinfo->GetQuestId(), qinfo->GetCharTitleId(), qinfo->GetCharTitleId());
            qinfo->CharTitleId = 0;
            // quest can't reward this title
        }

        if (qinfo->SrcItemId)
        {
            if (!ItemPrototypeStorage.LookupEntry(qinfo->SrcItemId))
            {
                Log.Debug("ObjectMgr", "Quest %u has `SrcItemId` = %u but item with entry %u does not exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->SrcItemId, qinfo->SrcItemId);
                qinfo->SrcItemId = 0;                       // quest can't be done for this requirement
            }
            else if (qinfo->SrcItemCount == 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `SrcItemId` = %u but `SrcItemCount` = 0, set to 1 but need fix in DB.",
                    qinfo->GetQuestId(), qinfo->SrcItemId);
                qinfo->SrcItemCount = 1;                    // update to 1 for allow quest work for backward compatibility with DB
            }
        }
        else if (qinfo->SrcItemCount > 0)
        {
            Log.Debug("ObjectMgr", "Quest %u has `SrcItemId` = 0 but `SrcItemCount` = %u, useless value.",
                qinfo->GetQuestId(), qinfo->SrcItemCount);
            qinfo->SrcItemCount = 0;                          // no quest work changes in fact
        }

        if (qinfo->SrcSpell)
        {
            SpellEntry const* spellInfo = dbcSpell.LookupEntry(qinfo->SrcSpell);
            if (!spellInfo)
            {
                Log.Debug("ObjectMgr", "Quest %u has `SrcSpell` = %u but spell %u doesn't exist, quest can't be done.",
                    qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
        }

        for (uint8 j = 0; j < QUEST_ITEM_OBJECTIVES_COUNT; ++j)
        {
            uint32 id = qinfo->ReqItemId[j];
            if (id)
            {
                if (qinfo->ReqItemCount[j] == 0)
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqItemId%d` = %u but `ReqItemCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done for this requirement
                }

                if (!ItemPrototypeStorage.LookupEntry(id))
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqItemId%d` = %u but item with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->ReqItemCount[j] = 0;             // prevent incorrect work of quest
                }
            }
            else if (qinfo->ReqItemCount[j] > 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `ReqItemId%d` = 0 but `ReqItemCount%d` = %u, quest can't be done.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqItemCount[j]);
                qinfo->ReqItemCount[j] = 0;                 // prevent incorrect work of quest
            }
        }

        for (uint8 j = 0; j < QUEST_SOURCE_ITEM_IDS_COUNT; ++j)
        {
            uint32 id = qinfo->ReqSourceId[j];
            if (id)
            {
                if (!ItemPrototypeStorage.LookupEntry(id))
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqSourceId%d` = %u but item with entry %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, id);
                    // no changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->ReqSourceCount[j]>0)
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqSourceId%d` = 0 but `ReqSourceCount%d` = %u.",
                        qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqSourceCount[j]);
                    // no changes, quest ignore this data
                }
            }
        }

        for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            uint32 id = qinfo->ReqSpell[j];
            if (id)
            {
                SpellEntry const* spellInfo = dbcSpell.LookupEntry(id);
                if (!spellInfo)
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqSpellCast%d` = %u but spell %u does not exist, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, id);
                    continue;
                }
            }
        }

        for (uint8 j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            int32 id = qinfo->ReqCreatureOrGOId[j];
            if (id < 0 && !GameObjectNameStorage.LookupEntry(-id))
            {
                Log.Debug("ObjectMgr", "Quest %u has `ReqCreatureOrGOId%d` = %i but gameobject %u does not exist, quest can't be done.",
                    qinfo->GetQuestId(), j + 1, id, uint32(-id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id > 0 && !CreatureNameStorage.LookupEntry(id))
            {
                Log.Debug("ObjectMgr", "Quest %u has `ReqCreatureOrGOId%d` = %i but creature with entry %u does not exist, quest can't be done.",
                    qinfo->GetQuestId(), j + 1, id, uint32(id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id)
            {
                if (!qinfo->ReqCreatureOrGOCount[j])
                {
                    Log.Debug("ObjectMgr", "Quest %u has `ReqCreatureOrGOId%d` = %u but `ReqCreatureOrGOCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can be incorrectly done, but we already report this
                }
            }
            else if (qinfo->ReqCreatureOrGOCount[j]>0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `ReqCreatureOrGOId%d` = 0 but `ReqCreatureOrGOCount%d` = %u.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqCreatureOrGOCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REWARD_CHOICES_COUNT; ++j)
        {
            uint32 id = qinfo->RewChoiceItemId[j];
            if (id)
            {
                if (!ItemPrototypeStorage.LookupEntry(id))
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewChoiceItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewChoiceItemId[j] = 0;          // no changes, quest will not reward this
                }

                if (!qinfo->RewChoiceItemCount[j])
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewChoiceItemId%d` = %u but `RewChoiceItemCount%d` = 0, quest can't be done.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done
                }
            }
            else if (qinfo->RewChoiceItemCount[j] > 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RewChoiceItemId%d` = 0 but `RewChoiceItemCount%d` = %u.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewChoiceItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REWARDS_COUNT; ++j)
        {
            uint32 id = qinfo->RewItemId[j];
            if (id)
            {
                if (!ItemPrototypeStorage.LookupEntry(id))
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewItemId%d` = %u but item with entry %u does not exist, quest will not reward this item.",
                        qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewItemId[j] = 0;                // no changes, quest will not reward this item
                }

                if (!qinfo->RewItemCount[j])
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewItemId%d` = %u but `RewItemCount%d` = 0, quest will not reward this item.",
                        qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes
                }
            }
            else if (qinfo->RewItemCount[j] > 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RewItemId%d` = 0 but `RewItemCount%d` = %u.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (uint8 j = 0; j < QUEST_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewRepFaction[j])
            {
                if (abs(qinfo->RewRepValueId[j]) > 9)
                {
                    Log.Debug("ObjectMgr", "Quest %u has RewRepValueId%d = %i. That is outside the range of valid values (-9 to 9).", qinfo->GetQuestId(), j + 1, qinfo->RewRepValueId[j]);
                }
                if (!sFactionStore.LookupEntry(qinfo->RewRepFaction[j]))
                {
                    Log.Debug("ObjectMgr", "Quest %u has `RewRepFaction%d` = %u but raw faction (faction.dbc) %u does not exist, quest will not reward reputation for this faction.", qinfo->GetQuestId(), j + 1, qinfo->RewRepFaction[j], qinfo->RewRepFaction[j]);
                    qinfo->RewRepFaction[j] = 0;            // quest will not reward this
                }
            }

            else if (qinfo->RewRepValue[j] != 0)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RewRepFaction%d` = 0 but `RewRepValue%d` = %i.",
                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewRepValue[j]);
                // no changes, quest ignore this data
            }
        }

        if (qinfo->RewSpell)
        {
            SpellEntry const* spellInfo = dbcSpell.LookupEntry(qinfo->RewSpell);

            if (!spellInfo)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RewSpell` = %u but spell %u does not exist, spell removed as display reward.",
                    qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }
        }

        if (qinfo->RewSpellCast > 0)
        {
            SpellEntry const* spellInfo = dbcSpell.LookupEntry(qinfo->RewSpellCast);

            if (!spellInfo)
            {
                Log.Debug("ObjectMgr", "Quest %u has `RewSpellCast` = %u but spell %u does not exist, quest will not have a spell reward.",
                    qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }
        }

        if (qinfo->NextQuestInChain)
        {
            QuestMap::iterator qNextItr = mQuestTemplates.find(qinfo->NextQuestInChain);
            if (qNextItr == mQuestTemplates.end())
            {
                Log.Debug("ObjectMgr", "Quest %u has `NextQuestInChain` = %u but quest %u does not exist, quest chain will not work.",
                    qinfo->GetQuestId(), qinfo->NextQuestInChain, qinfo->NextQuestInChain);
                qinfo->NextQuestInChain = 0;
            }
            else
                qNextItr->second->prevChainQuests.push_back(qinfo->GetQuestId());
        }

        // fill additional data stores
        if (qinfo->PrevQuestId)
        {
            if (mQuestTemplates.find(abs(qinfo->GetPrevQuestId())) == mQuestTemplates.end())
            {
                Log.Debug("ObjectMgr", "Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            }
            else
            {
                qinfo->prevQuests.push_back(qinfo->PrevQuestId);
            }
        }

        if (qinfo->NextQuestId)
        {
            QuestMap::iterator qNextItr = mQuestTemplates.find(abs(qinfo->GetNextQuestId()));
            if (qNextItr == mQuestTemplates.end())
            {
                Log.Debug("ObjectMgr", "Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            }
            else
            {
                int32 signedQuestId = qinfo->NextQuestId < 0 ? -int32(qinfo->GetQuestId()) : int32(qinfo->GetQuestId());
                qNextItr->second->prevQuests.push_back(signedQuestId);
            }
        }

        if (qinfo->ExclusiveGroup)
            mExclusiveQuestGroups.insert(std::pair<int32, uint32>(qinfo->ExclusiveGroup, qinfo->GetQuestId()));
    }
    Log.Debug("ObjectMgr", ">> Loaded %lu quests definitions", (unsigned long)mQuestTemplates.size());
}

void ObjectMgr::LoadQuestLoot(uint32 GO_Entry, uint32 Item_Entry)
{
    // Find the quest that has that item
    uint32 QuestID = 0;
    uint32 i;

    for (QuestMap::const_iterator itr = mQuestTemplates.begin(); itr != mQuestTemplates.end(); ++itr)
    {
        for (i = 0; i < MAX_REQUIRED_QUEST_ITEM; ++i)
        {
            if (itr->second->ReqItemId[i] == Item_Entry)
            {
                QuestID = itr->second->GetQuestId();
                sQuestMgr.m_ObjectLootQuestList[GO_Entry] = QuestID;
                return;
            }
        }
    }
}