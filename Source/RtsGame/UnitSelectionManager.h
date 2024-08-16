#pragma once

#include "CoreMinimal.h"
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
	TArray<AActor*> GetSelectedUnits() const { return SelectedUnits; } // Добавляем метод

private:
	TArray<AActor*> SelectedUnits;
	void DeselectAllUnits();
};
