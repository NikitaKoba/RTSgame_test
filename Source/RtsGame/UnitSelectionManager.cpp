#include "UnitSelectionManager.h"
#include "Selectable.h"
#include "Settlement.h"
#include "SettlementWidget.h"
#include "Unit.h"
#include "Engine/HitResult.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h" 
#include "Engine/World.h"

AUnitSelectionManager::AUnitSelectionManager()
{
    LastClickTime = 0.0f;
    DoubleClickThreshold = 0.3f;
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/BP_SettlementWidget"));
    SettlementWidgetClass = WidgetClassFinder.Class;
}


void AUnitSelectionManager::SelectUnitAtMousePosition(APlayerController* PlayerController, bool bIsCtrlPressed)
{
    if (PlayerController)
    {
        FVector2D MousePosition;
        if (PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            FVector WorldLocation, WorldDirection;
            PlayerController->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection);

            FVector TraceStart = WorldLocation;
            FVector TraceEnd = WorldLocation + (WorldDirection * 10000.0f);

            FHitResult HitResult;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(this);

            GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

            if (HitResult.bBlockingHit)
            {
                AActor* HitActor = HitResult.GetActor();
                UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

                if (HitActor && HitActor->Implements<USelectable>())
                {
                    ISelectable* SelectableInterface = Cast<ISelectable>(HitActor);

                    if (bIsCtrlPressed)
                    {
                        if (SelectedUnits.Contains(HitActor))
                        {
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnDeselected();
                            }
                            SelectedUnits.Remove(HitActor);
                        }
                        else
                        {
                            if (SelectableInterface)
                            {
                                SelectableInterface->OnSelected();
                            }
                            SelectedUnits.Add(HitActor);
                        }
                    }
                    else
                    {
                        DeselectAllUnits();

                        if (SelectableInterface)
                        {
                            SelectableInterface->OnSelected();
                        }
                        SelectedUnits.Add(HitActor);
                    }
                }
                else if (ASettlement* Settlement = Cast<ASettlement>(HitActor))
                {
                    // Проверяем, является ли это двойным щелчком
                    float CurrentTime = GetWorld()->GetTimeSeconds();
                    if (CurrentTime - LastClickTime <= DoubleClickThreshold)
                    {
                        DisplaySettlementInfo(Settlement);
                    }
                    LastClickTime = CurrentTime;
                }
                else
                {
                    DeselectAllUnits();
                    if (SettlementWidget)
                    {
                        SettlementWidget->RemoveFromViewport();
                        SettlementWidget = nullptr;
                    }
                }
            }
        }
    }
}


void AUnitSelectionManager::DisplaySettlementInfo(ASettlement* Settlement)
{
    if (Settlement)
    {
        UE_LOG(LogTemp, Warning, TEXT("DisplaySettlementInfo called"));

        if (!SettlementWidget && SettlementWidgetClass)
        {
            SettlementWidget = CreateWidget<UUserWidget>(GetWorld(), SettlementWidgetClass);
            if (SettlementWidget)
            {
                SettlementWidget->AddToViewport();
                UE_LOG(LogTemp, Warning, TEXT("SettlementInfoWidget added to viewport"));
            }
        }

        USettlementWidget* SettlementWidgetInstance = Cast<USettlementWidget>(SettlementWidget);
        if (SettlementWidgetInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Updating Settlement Info"));
            SettlementWidgetInstance->UpdateSettlementInfo(Settlement->Population, Settlement->Gold, Settlement->Food, Settlement->Wood, Settlement->Stone);
        }
    }
}

void AUnitSelectionManager::DeselectAllUnits()
{
    for (AActor* Unit : SelectedUnits)
    {
        if (Unit && Unit->Implements<USelectable>())
        {
            ISelectable* SelectableInterface = Cast<ISelectable>(Unit);
            if (SelectableInterface)
            {
                SelectableInterface->OnDeselected();
            }
        }
    }
    SelectedUnits.Empty(); // Очищаем список выделенных юнитов
}
void AUnitSelectionManager::MoveSelectedUnitsToLocation(const FVector& TargetLocation)
{
    for (AActor* Unit : SelectedUnits)
    {
        AUnit* UnitToMove = Cast<AUnit>(Unit);
        if (UnitToMove)
        {
            UnitToMove->MoveToLocation(TargetLocation);
        }
    }
}