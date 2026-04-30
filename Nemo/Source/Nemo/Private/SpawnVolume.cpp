// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);


}

AActor* ASpawnVolume::SpawnRandomItem(FName ActorType)
{
	if (FActorSpawnRow* SelectedRow = GetRandomItem(ActorType))
	{
		if (UClass* ActualClass = SelectedRow->ActorClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}
	return nullptr;
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



FVector ASpawnVolume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	//박스 컴포넌트의 절반 길이만큼을 반환한다.
	//200, 100, 50 Scale (2,1,1) -> 400,100,50
	//중심부터 끝까지의 거리
	FVector BoxOrigin = SpawningBox->GetComponentLocation();
	//
	return FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X)
		, FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y)
		, FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}



AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}

//ActorType은 Item(아이템), Target(플레이어 수집 목표), Creature(괴물)로 분류
//자세한 사항은 SpawnRate 데이터 레이블 확인!
FActorSpawnRow* ASpawnVolume::GetRandomItem(FName ActorType)
{
	if (!ActorDataTable) return nullptr;

	TArray<FActorSpawnRow*> AllRows;
	ActorDataTable->GetAllRows<FActorSpawnRow>(TEXT(""), AllRows);

	// 1. 현재 구역 타입에 맞는 아이템만 필터링
	TArray<FActorSpawnRow*> FilteredRows;
	float TotalWeight = 0.0f;

	for (auto* Row : AllRows)
	{
		if (Row->ActorType == ActorType)
		{
			FilteredRows.Add(Row);
			TotalWeight += Row->SpawnWeight;
		}
	}

	if (FilteredRows.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%d"), FilteredRows.Num());
		return nullptr;
	}
	// 2. 필터링된 목록에서 가중치 랜덤 선택
	float RandomPivot = FMath::FRandRange(0.0f, TotalWeight);
	float CurrentSum = 0.0f;

	for (auto* Row : FilteredRows)
	{
		CurrentSum += Row->SpawnWeight;
		if (RandomPivot <= CurrentSum) return Row;
	}

	return nullptr;
}