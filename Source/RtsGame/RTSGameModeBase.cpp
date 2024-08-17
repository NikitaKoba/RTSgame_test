#include "RTSGameModeBase.h"

#include "EconomicSettlement.h"
#include "Unit.h"
#include "EconomicSettlement.h"  // Включаем заголовок AEconomicSettlement
#include "Unit.h"  // Включаем заголовок AUnit
#include "EngineUtils.h"  // Включаем для TActorIterator
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


ARTSGameModeBase::ARTSGameModeBase()
{
	CurrentTurn = 1;

	// Загружаем класс виджета завершения хода
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/BP_EndTurnWidget"));
	EndTurnWidgetClass = WidgetClassFinder.Class;
}

void ARTSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called: Starting first turn"));
	StartNewTurn();

	if (EndTurnWidgetClass)
	{
		EndTurnWidget = CreateWidget<UUserWidget>(GetWorld(), EndTurnWidgetClass);
		if (EndTurnWidget)
		{
			EndTurnWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("EndTurnWidget added to viewport"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create EndTurnWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EndTurnWidgetClass is null"));
	}
}

void ARTSGameModeBase::EndTurn()
{
	// Обновление экономики для всех поселений
	for (TActorIterator<AEconomicSettlement> It(GetWorld()); It; ++It)
	{
		It->UpdateEconomyForTurn();
	}

	// Перевод всех юнитов в состояние завершения хода
	for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
	{
		It->EndTurn();
	}

	// Запуск нового хода
	StartNewTurn();
}

void ARTSGameModeBase::StartNewTurn()
{
	CurrentTurn++;

	UE_LOG(LogTemp, Warning, TEXT("Starting new turn: %d"), CurrentTurn);

	// Сбросить все действия юнитов для нового хода
	for (TActorIterator<AUnit> It(GetWorld()); It; ++It)
	{
		It->StartNewTurn();
		UE_LOG(LogTemp, Warning, TEXT("New turn started: bCanMove = true"));
	}
}
