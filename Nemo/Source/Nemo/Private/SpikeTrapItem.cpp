#include "SpikeTrapItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCreature.h"

ASpikeTrapItem::ASpikeTrapItem()
{
    SpikeDamage = 10;
    DamageInterval = 0.5f;
    CurrentActivator = nullptr;
    ItemType = "Spike";
}

// 밟는 순간 CurrentActivator 저장 후 반복 타이머 시작
void ASpikeTrapItem::ActivateItem(AActor* Activator)
{
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("[SpikeTrap] Stepped / Timer Start"));
    
    CurrentActivator = Activator;
    GetWorld()->GetTimerManager().SetTimer(
        SpikeDamageTimerHandle,
        this,
        &ASpikeTrapItem::ApplySpikeDamage,
        DamageInterval,
        true
    );
}

// CurrentActivator에게 SpikeDamage 적용
void ASpikeTrapItem::ApplySpikeDamage()
{
    if (CurrentActivator)
    {
        // UGameplayStatics::ApplyDamage 대신
        ABaseCreature* Creature = Cast<ABaseCreature>(CurrentActivator);
        if (Creature)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("[SpikeTrap] Apply Damage: %d"), SpikeDamage));

            Creature->ApplyDamage(SpikeDamage, this);
        }
    }
}

// 타이머에서 반복 호출되는 데미지 처리 함수
// 밟았던 액터가 벗어나면 타이머 정지 및 CurrentActivator 초기화
void ASpikeTrapItem::OnItemEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (OtherActor == CurrentActivator)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("[SpikeTrap] Exit / Timer Stop"));

        GetWorld()->GetTimerManager().ClearTimer(SpikeDamageTimerHandle);
        CurrentActivator = nullptr;
    }
}