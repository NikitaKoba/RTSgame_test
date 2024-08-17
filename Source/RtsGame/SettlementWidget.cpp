#include "SettlementWidget.h"
#include "Components/TextBlock.h"

void USettlementWidget::UpdateSettlementInfo(int32 Population, int32 Gold, int32 Food, int32 Wood, int32 Stone)
{
	//UE_LOG(LogTemp, Warning, TEXT("Updating Widget - Population: %d, Gold: %d"), Population, Gold);
	if (PopulationText)
	{
		PopulationText->SetText(FText::FromString(FString::Printf(TEXT("Население: %d"), Population)));
	}

	if (GoldText)
	{
		GoldText->SetText(FText::FromString(FString::Printf(TEXT("Золото: %d"), Gold)));
	}

	if (FoodText)
	{
		FoodText->SetText(FText::FromString(FString::Printf(TEXT("Еда: %d"), Food)));
	}

	if (WoodText)
	{
		WoodText->SetText(FText::FromString(FString::Printf(TEXT("Дерево: %d"), Wood)));
	}

	if (StoneText)
	{
		StoneText->SetText(FText::FromString(FString::Printf(TEXT("Камень: %d"), Stone)));
	}
}
