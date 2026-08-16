#include "pbc_combat_log.h"

#include "GameTime.h"
#include "Group.h"
#include "Player.h"

#include <deque>
#include <mutex>
#include <unordered_map>

namespace
{
    struct CombatLogEntry
    {
        uint32_t    timestamp;
        std::string line;
    };

    constexpr size_t MAX_ENTRIES_PER_GROUP = 30;
    constexpr size_t MAX_GROUPS            = 256;

    std::unordered_map<uint32_t, std::deque<CombatLogEntry>> s_groupLog;
    std::mutex                                               s_mutex;
}

void PBC_RecordCombatAction(Player* caster, const std::string& action)
{
    if (!caster)
        return;
    Group* group = caster->GetGroup();
    if (!group)
        return;

    std::lock_guard<std::mutex> lock(s_mutex);

    uint32_t key = group->GetGUID().GetCounter();
    if (s_groupLog.size() >= MAX_GROUPS && !s_groupLog.count(key))
        s_groupLog.erase(s_groupLog.begin());   // crude eviction

    std::deque<CombatLogEntry>& log = s_groupLog[key];
    log.push_back(CombatLogEntry{ uint32_t(GameTime::GetGameTime().count()),
                                  caster->GetName() + " " + action });
    while (log.size() > MAX_ENTRIES_PER_GROUP)
        log.pop_front();
}

std::string PBC_GetCombatLogFor(Player* bot, size_t maxLines)
{
    if (!bot)
        return "";
    Group* group = bot->GetGroup();
    if (!group)
        return "";

    std::lock_guard<std::mutex> lock(s_mutex);
    auto itr = s_groupLog.find(group->GetGUID().GetCounter());
    if (itr == s_groupLog.end() || itr->second.empty())
        return "";

    std::string out;
    size_t shown = 0;
    for (auto entryItr = itr->second.rbegin();
         entryItr != itr->second.rend() && shown < maxLines;
         ++entryItr, ++shown)
    {
        if (!out.empty())
            out = "\n" + out;
        out = entryItr->line + out;
    }
    return out;
}
