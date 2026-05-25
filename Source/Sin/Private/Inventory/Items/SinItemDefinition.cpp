#include "Inventory/Items/SinItemDefinition.h"

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
