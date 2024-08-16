#include "Unit.h"
#include "Components/BillboardComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"


AUnit::AUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    // Создание компонента для отображения индикатора выделения
    PrimaryActorTick.bCanEverTick = true;

    // Настройка индикатора выделения
    SelectionIndicator = CreateDefaultSubobject<UBillboardComponent>(TEXT("SelectionIndicator"));
    SelectionIndicator->SetupAttachment(RootComponent);
    SelectionIndicator->SetVisibility(false);

    // Настройка AI контроллера
    AIControllerClass = AAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // Настройка декаля выделения
    SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
    SelectionDecal->SetupAttachment(RootComponent);
    SelectionDecal->SetVisibility(false);
    SelectionDecal->SetRelativeRotation(FRotator(-90, 0, 0)); // Декаль на земле
    SelectionDecal->DecalSize = FVector(200, 200, 200); // Размер декали

    
    // Настройка поворота персонажа в направлении движения
    GetCharacterMovement()->bOrientRotationToMovement = true; // Автоматический поворот персонажа
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f); // Скорость поворота

    // Отключаем поворот через контроллер
    bUseControllerRotationYaw = false;
}


void AUnit::BeginPlay()
{
    Super::BeginPlay();

    // Проверка, есть ли у юнита контроллер
    if (Controller == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Контроллер не найден для юнита %s. Спавним контроллер..."), *GetName());
        SpawnDefaultController();  // Спавн AIController, если он не установлен

        if (Controller == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Не удалось создать AIController для юнита %s"), *GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AIController успешно создан для юнита %s"), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Контроллер уже существует для юнита %s"), *GetName());
    }

    // Проверка наличия навигационной системы
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSys)
    {
        UE_LOG(LogTemp, Warning, TEXT("Навигационная система найдена для юнита %s"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Навигационная система не найдена для юнита %s"), *GetName());
    }

    // Создание динамического материала (предыдущий код)
    USkeletalMeshComponent* MeshComponent = GetMesh();
    if (MeshComponent)
    {
        DynamicMaterialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);
    }
}

void AUnit::OnSelected()
{
    if (SelectionDecal)
    {
        SelectionDecal->SetVisibility(true);
    }
    UE_LOG(LogTemp, Warning, TEXT("Unit selected"));
}

void AUnit::OnDeselected()
{
    if (SelectionDecal)
    {
        SelectionDecal->SetVisibility(false);
    }
    UE_LOG(LogTemp, Warning, TEXT("Unit deselected"));
}

void AUnit::MoveToLocation(const FVector& TargetLocation)
{
    AAIController* AIController = Cast<AAIController>(GetController());
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("AIController не найден для юнита %s"), *GetName());
        return;
    }

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (!NavSys)
    {
        UE_LOG(LogTemp, Error, TEXT("NavMesh не найден для юнита %s"), *GetName());
        return;
    }
    FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
    FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
    NewRotation.Pitch = 0.0f;  // Убираем наклон
    NewRotation.Roll = 0.0f;
    SetActorRotation(NewRotation);

    // Проверяем, находится ли юнит на NavMesh
    FNavLocation UnitNavLocation;
    if (!NavSys->ProjectPointToNavigation(GetActorLocation(), UnitNavLocation, FVector(100.0f, 100.0f, 500.0f)))
    {
        UE_LOG(LogTemp, Error, TEXT("Юнит %s не находится на NavMesh. Текущее положение: %s"), *GetName(), *GetActorLocation().ToString());
        return;
    }

    // Проверяем, находится ли целевая точка на NavMesh
    FNavLocation TargetNavLocation;
    if (!NavSys->ProjectPointToNavigation(TargetLocation, TargetNavLocation, FVector(100.0f, 100.0f, 500.0f)))
    {
        UE_LOG(LogTemp, Error, TEXT("Целевая точка %s не находится на NavMesh для юнита %s"), *TargetLocation.ToString(), *GetName());
        return;
    }

    // Пытаемся найти путь
    UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), TargetNavLocation.Location);
    if (!NavPath || !NavPath->IsValid() || NavPath->IsPartial())
    {
        UE_LOG(LogTemp, Error, TEXT("Не удалось найти полный путь для юнита %s от %s до %s"), 
               *GetName(), *GetActorLocation().ToString(), *TargetNavLocation.Location.ToString());
        
        // Попробуем найти ближайшую доступную точку
        FNavLocation NearestNavLocation;
        if (NavSys->GetRandomPointInNavigableRadius(TargetNavLocation.Location, 1000.0f, NearestNavLocation))
        {
            UE_LOG(LogTemp, Warning, TEXT("Найдена ближайшая доступная точка: %s"), *NearestNavLocation.Location.ToString());
            TargetNavLocation = NearestNavLocation;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Не удалось найти ближайшую доступную точку"));
            return;
        }
    }

    // Попытка движения
    EPathFollowingRequestResult::Type MoveResult = AIController->MoveToLocation(TargetNavLocation.Location);
    switch (MoveResult)
    {
        case EPathFollowingRequestResult::Failed:
            UE_LOG(LogTemp, Error, TEXT("MoveToLocation: Failed для юнита %s. Возможно, путь недоступен или проблемы с NavMesh."), *GetName());
            break;
        case EPathFollowingRequestResult::AlreadyAtGoal:
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation: AlreadyAtGoal для юнита %s. Юнит уже в целевой точке."), *GetName());
            break;
        case EPathFollowingRequestResult::RequestSuccessful:
            UE_LOG(LogTemp, Warning, TEXT("MoveToLocation: RequestSuccessful для юнита %s. Перемещение началось к %s"), *GetName(), *TargetNavLocation.Location.ToString());
            break;
    }

    // Отладочная визуализация пути
    if (NavPath && NavPath->IsValid())
    {
        for (int32 i = 0; i < NavPath->PathPoints.Num() - 1; ++i)
        {
            DrawDebugLine(GetWorld(), NavPath->PathPoints[i], NavPath->PathPoints[i + 1], FColor::Green, false, 5.0f, 0, 2.0f);
        }
    }
    DrawDebugSphere(GetWorld(), GetActorLocation(), 50.0f, 12, FColor::Blue, false, 5.0f);
    DrawDebugSphere(GetWorld(), TargetLocation, 50.0f, 12, FColor::Red, false, 5.0f);

    // Отображение линии предполагаемого пути
    DrawDebugLine(GetWorld(), GetActorLocation(), TargetLocation, FColor::Green, false, 5.0f, 0, 2.0f);

    // Проверка и вывод информации о NavMesh
    UNavigationSystemV1* NavSyss = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSyss)
    {
        FNavLocation NavLocation;
        if (NavSyss->ProjectPointToNavigation(GetActorLocation(), NavLocation))
        {
            UE_LOG(LogTemp, Warning, TEXT("Юнит на NavMesh. Проецированная позиция: %s"), *NavLocation.Location.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Юнит НЕ на NavMesh! Текущая позиция: %s"), *GetActorLocation().ToString());
        }
    }
    
}
