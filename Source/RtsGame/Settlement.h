// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EconomicSettlement.h"
#include "Settlement.generated.h"

/**
 * 
 */
UCLASS()
class RTSGAME_API ASettlement : public AEconomicSettlement
{
	GENERATED_BODY()

public:
	ASettlement();  // Конструктор

	protected:
	virtual void BeginPlay() override;  // Переопределение метода BeginPlay

	public:
	virtual void Tick(float DeltaTime) override;  // Переопределение метода Tick

	// Здесь можно добавить дополнительные компоненты или свойства, специфичные для поселения
};