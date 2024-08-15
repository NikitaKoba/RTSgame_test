#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Selectable.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "Runtime/AIModule/Classes/AITypes.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/NavMovementComponent.h"
#include "Unit.generated.h"

UCLASS()
class RTSGAME_API AUnit : public APawn, public ISelectable
{
	GENERATED_BODY()

public:
	AUnit();

	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	void MoveToLocation(const FVector& Location);

protected:
	virtual void BeginPlay() override;

private:
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Selection", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* SelectionIndicator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* MovementComponent;
	
	UFUNCTION() // Обязательно помечаем как UFUNCTION
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
};
