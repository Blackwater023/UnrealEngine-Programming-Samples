#include "StateTags/ZRB_StateStatusCCTags.h"

namespace State
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "State.Dead", "Dead: blocks most actions")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Invulnerable, "State.Invulnerable", "Cannot take damage")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Exploration, "State.Exploration", "In exploration mode")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Overworld, "State.Overworld", "In overworld mode")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InCombat, "State.InCombat", "In Combat: combat rules/mechanics are active")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Blocking, "State.Blocking", "Actively blocking/guarding")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Interruptible, "State.Interruptible", "Current action can be interrupted")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Uninterruptible, "State.Uninterruptible", "Current action cannot be interrupted")
}
