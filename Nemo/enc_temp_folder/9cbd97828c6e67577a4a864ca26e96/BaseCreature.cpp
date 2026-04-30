// BaseCreature.cpp

#include "BaseCreature.h"
#include "Components/StaticMeshComponent.h"

ABaseCreature::ABaseCreature()
{
 	PrimaryActorTick.bCanEverTick = false;
	
	// ================ 초기화 ================
	MaxHP = 100.f;
	CurrentHP = 0.f;
	bIsDead = false;
	
	// ============= 컴포넌트 생성 =============
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = RootComp;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);
}


void ABaseCreature::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;											// BP에서 변경한 MaxHP 값을 적용하기 위해 BeginPlay에서 진행
}

void ABaseCreature::ApplyDamage(float Amount, AActor* Causer)
{
	if (bIsDead) return;
	if (Amount < 0.f) return;
	if (IsDamageImmune()) return;
	
	float PrevHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP - Amount, 0.f, MaxHP);
	float ActualDamage = PrevHP - CurrentHP;
	
	OnDamaged(ActualDamage, Causer);
}

void ABaseCreature::ApplyHeal(float Amount)
{
	if (bIsDead) return;
	if (Amount < 0.f) return;
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.f, MaxHP);
	
	// @서희
	// HP Bar 업데이트
}

bool ABaseCreature::IsAlive() const
{
	return !bIsDead;
}

bool ABaseCreature::IsDamageImmune() const
{
	return false;
}

void ABaseCreature::KillInstantly()
{
	if (bIsDead) return;
	CurrentHP = 0.f;
	ProcessDeath();
}

float ABaseCreature::GetCurrentHP() const
{
	return CurrentHP;
}

float ABaseCreature::GetMaxHP() const
{
	return MaxHP;
}

float ABaseCreature::GetHealthPercent() const
{
	return (MaxHP > 0.f) ? FMath::Clamp(CurrentHP / MaxHP * 100, 0.f, 100.f) : 0;
}

void ABaseCreature::ProcessDeath()
{
	bIsDead = true;
	OnDeath();
}

void ABaseCreature::OnDeath()
{
}																// 자식 클래스에서 override

void ABaseCreature::OnDamaged(float Amount, AActor* Causer)
{
}																// 자식 클래스에서 override
