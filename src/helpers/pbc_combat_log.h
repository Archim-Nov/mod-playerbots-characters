#ifndef MOD_PBC_COMBAT_LOG_H
#define MOD_PBC_COMBAT_LOG_H

#include <cstddef>
#include <string>

class Player;

// Records one combat action into the caster's group ring buffer.
// Safe to call from any thread (map worker threads included).
void PBC_RecordCombatAction(Player* caster, const std::string& action);

// Returns the group's recent combat actions, oldest first, one per line.
// Empty string when nothing was recorded.  Thread-safe.
std::string PBC_GetCombatLogFor(Player* bot, size_t maxLines);

#endif // MOD_PBC_COMBAT_LOG_H
