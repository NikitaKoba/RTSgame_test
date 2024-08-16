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