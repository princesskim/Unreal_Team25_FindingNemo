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
		, FMath::FRandRange(0, BoxExtent.Z)
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