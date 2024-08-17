#include "Settlement.h"


ASettlement::ASettlement()
{
	PrimaryActorTick.bCanEverTick = true;

	// Создаем и прикрепляем StaticMeshComponent для визуального отображения
	UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	// Можно задать начальные параметры для MeshComponent, такие как масштаб, mesh и т.д.
}
void ASettlement::BeginPlay()
{
	Super::BeginPlay();

	// Дополнительная логика, если требуется
	// Например, настройка визуальных компонентов или начальных условий
}

void ASettlement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Если требуется, добавьте логику для обновления состояния поселения каждый кадр
}