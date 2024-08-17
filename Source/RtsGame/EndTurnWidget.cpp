#include "EndTurnWidget.h"
#include "Components/Button.h"
#include "RTSGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void UEndTurnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Привязка функции OnEndTurnClicked к событию нажатия кнопки
	if (EndTurnButton)
	{
		EndTurnButton->OnClicked.AddDynamic(this, &UEndTurnWidget::OnEndTurnClicked);
	}
}

void UEndTurnWidget::OnEndTurnClicked()
{
	// Получение текущего GameMode и вызов метода EndTurn
	ARTSGameModeBase* GameMode = Cast<ARTSGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->EndTurn();
	}
}
