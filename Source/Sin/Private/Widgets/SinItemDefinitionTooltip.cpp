#include "Widgets/SinItemDefinitionTooltip.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Inventory/Items/SinItemDefinition.h"
#include "Inventory/Items/SinItemFragment_Display.h"
#include "Inventory/Items/SinItemFragment.h"

void USinItemDefinitionTooltip::ShowItemDefinitionTooltip_Implementation(
	UUserWidget* Master,
	USinItemDefinition* ItemDefinition)
{
	if (!ItemDefinition)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const USinItemFragment_Display* Display =
		ItemDefinition->FindFragmentByClass<USinItemFragment_Display>();

	if (!Display)
	{
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (ItemNameText)
	{
		ItemNameText->SetText(Display->DisplayName);
	}

	if (ShortDescriptionText)
	{
		ShortDescriptionText->SetText(Display->ShortDescription);
	}

	if (LongDescriptionText)
	{
		LongDescriptionText->SetText(Display->LongDescription);
	}

	if (IconImage && !Display->Icon.IsNull())
	{
		if (UTexture2D* IconTexture = Display->Icon.LoadSynchronous())
		{
			IconImage->SetBrushFromTexture(IconTexture);
		}
	}
	AnchorWidget = Master;
	Definition = ItemDefinition;
}
