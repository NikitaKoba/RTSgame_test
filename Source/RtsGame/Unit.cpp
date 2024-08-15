#include "Unit.h"
#include "Components/BillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "Runtime/AIModule/Classes/AITypes.h"
#include "NavigationSystem.h"
#include "GameFramework/FloatingPawnMovement.h"

AUnit::AUnit()
{
	PrimaryActorTick.bCanEverTick = true;

	SelectionIndicator = CreateDefaultSubobject<UBillboardComponent>(TEXT("SelectionIndicator"));
	SelectionIndicator->SetupAttachment(RootComponent);
	SelectionIndicator->SetVisibility(false);
	
	USceneComponent* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = DefaultRoot;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	
	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AUnit::BeginPlay()
{
	Super::BeginPlay();

	UStaticMeshComponent* MeshComponent = FindComponentByClass<UStaticMeshComponent>();
	if (MeshComponent)
	{
		DynamicMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	}
}

void AUnit::OnSelected()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetVectorParameterValue("Color", FLinearColor::Green);
	}

	if (SelectionIndicator)
	{
		SelectionIndicator->SetVisibility(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("Unit selected"));
}

void AUnit::OnDeselected()
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetVectorParameterValue("Color", FLinearColor::White);
	}

	if (SelectionIndicator)
	{
		SelectionIndicator->SetVisibility(false);
	}

	UE_LOG(LogTemp, Warning, TEXT("Unit deselected"));
}
void AUnit::MoveToLocation(const FVector& Location)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), Location, FColor::Red, false, 5.0f, 0, 3.0f);
        
		EPathFollowingRequestResult::Type MoveResults = AIController->MoveToLocation(Location);
		UE_LOG(LogTemp, Warning, TEXT("Unit starting to move to location: %s"), *Location.ToString());
        
		// Проверяем, доступна ли точка назначения
		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		if (NavSys)
		{
			FNavLocation ProjectedLocation;
			if (NavSys->ProjectPointToNavigation(Location, ProjectedLocation))
			{
				UE_LOG(LogTemp, Warning, TEXT("Projected location: %s"), *ProjectedLocation.Location.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to project location to NavMesh"));
				return;
			}
		}
        
		AIController->ReceiveMoveCompleted.RemoveDynamic(this, &AUnit::OnMoveCompleted);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &AUnit::OnMoveCompleted);
        
		EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(Location);
		UE_LOG(LogTemp, Warning, TEXT("Move request result: %d"), (int32)MoveResult);
	}
}

void AUnit::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	FString ResultString;
	switch(Result)
	{
	case EPathFollowingResult::Success:
		ResultString = TEXT("Success");
		break;
	case EPathFollowingResult::Blocked:
		ResultString = TEXT("Blocked");
		break;
	case EPathFollowingResult::OffPath:
		ResultString = TEXT("OffPath");
		break;
	case EPathFollowingResult::Aborted:
		ResultString = TEXT("Aborted");
		break;
	case EPathFollowingResult::Invalid:
		ResultString = TEXT("InvalidPath");
		break;
	default:
		ResultString = FString::Printf(TEXT("Unknown (%d)"), (int32)Result);
		break;
	}
	UE_LOG(LogTemp, Warning, TEXT("Unit move completed. Result: %s"), *ResultString);
}