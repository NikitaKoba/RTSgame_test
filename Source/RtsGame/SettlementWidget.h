// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettlementWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTSGAME_API USettlementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Settlement")
	void UpdateSettlementInfo(int32 Population, int32 Gold, int32 Food, int32 Wood, int32 Stone);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PopulationText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* FoodText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WoodText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StoneText;
};