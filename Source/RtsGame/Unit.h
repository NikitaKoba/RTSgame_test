#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Selectable.h"
#include "GameFramework/Character.h"
#include "Components/SplineComponent.h"  
#include "Components/SphereComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "Runtime/AIModule/Classes/AITypes.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/NavMovementComponent.h"
#include "Unit.generated.h"

UCLASS()
class RTSGAME_API AUnit : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	AUnit();

	virtual void OnSelected() override;
	virtual void OnDeselected() override;
	void MoveToLocation(const FVector& TargetLocation);
	bool IsWithinMovementRadius(const FVector& TargetLocation, const FVector& CurrentLocation) const;
	bool HasEnoughSteps(float Distance) const;
	bool AttemptMove(const FVector& TargetLocation);
	void DrawMovementBoundary();
	void LogMoveWarning(const FString& Message) const;
	void LogMoveError(const FString& Message) const;
	void UpdateMovementRadius();
	void ShowMovementRadius();
	void HideMovementRadius();

	float GetMovementRadius() const;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection", meta = (AllowPrivateAccess = "true"))
	UBillboardComponent* SelectionIndicator;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	UDecalComponent* SelectionDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDecalComponent* MovementRadiusDecal;
	float StepsTakenThisTurn;  // Сколько юнит прошел за этот ход
	bool bCanMove;             // Может ли юнит двигаться в этом ход
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	USplineComponent* MovementPath; // SplineComponent для отображения пути

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementPoints; // Очки хода юнита
	
	UFUNCTION()
	void ShowMovementPath(const FVector& TargetLocation);
	
	// Методы для обработки начала и конца хода
	void StartNewTurn();       // Начало нового хода
	void EndTurn();    
	float MovementRadius;  // Определяем это в классе AUnit
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UMaterialInstanceDynamic* DynamicMaterialInstance;
	UMaterialInstanceDynamic* MovementRadiusMaterial;
	FVector InitialPosition;
};