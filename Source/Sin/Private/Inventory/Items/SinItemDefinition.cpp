#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_GrantedEffects.h"
#include "Inventory/Items/SinItemFragment_Stats.h"
#include "Inventory/Items/SinItemFragment_GrantedPerks.h"

USinItemFragment* USinItemDefinition::FindFragmentByClass(TSubclassOf<USinItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		return nullptr;
	}

	for (USinItemFragment* Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}
	return nullptr;
}

USinItemFragment_Stats* USinItemDefinition::GetStatsFragment() const
{
	return Cast<USinItemFragment_Stats>(
	FindFragmentByClass(USinItemFragment_Stats::StaticClass())
);
}

USinItemFragment_GrantedEffects* USinItemDefinition::GetGrantedEffectsFragment() const
{
	return Cast<USinItemFragment_GrantedEffects>(
		FindFragmentByClass(USinItemFragment_GrantedEffects::StaticClass())
	);
}

USinItemFragment_GrantedPerks* USinItemDefinition::GetPerksFragment() const
{
	return Cast<USinItemFragment_GrantedPerks>(
		FindFragmentByClass(USinItemFragment_GrantedPerks::StaticClass())
	);
}

USinItemFragment_Equipment* USinItemDefinition::GetEquipmentFragment() const
{
	return Cast<USinItemFragment_Equipment>(
		FindFragmentByClass(USinItemFragment_Equipment::StaticClass())
	);
}
