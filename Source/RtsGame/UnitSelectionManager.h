#pragma once

#include "CoreMinimal.h"
#include "Settlement.h"
#include "GameFramework/Actor.h"
#include "UnitSelectionManager.generated.h"

UCLASS()
class RTSGAME_API AUnitSelectionManager : public AActor
{
	GENERATED_BODY()

public:
	AUnitSelectionManager();
	void MoveSelectedUnitsToLocation(const FVector& TargetLocation);
	void SelectUnitAtMousePosition(APlayerController* PlayerController, bool bIsCtrlPressed); // Добавлен параметр для состояния Ctrl
	void DisplaySettlementInfo(ASettlement* Settlement);
	void DisplaySettlementInfo(APlayerController* PlayerController);
	TArray<AActor*> GetSelectedUnits() const { return SelectedUnits; } // Добавляем метод
	
	UPROPERTY()
	UUserWidget* SettlementWidget;
	float LastClickTime;
	float DoubleClickThreshold;
	// Объявляем переменную для хранения класса виджета
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SettlementWidgetClass;
private:
	TArray<AActor*> SelectedUnits;
	void DeselectAllUnits();
};
