// BaseCreature.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseCreature.generated.h"

/*
 * 플레이어(AMarinPlayer)와 적(ABaseEnemy) 공통 부모 클래스
 * 
 * 상속 구조:
 * APawn (언리얼 베이스)
 *   └── ABaseCreature		← 여기
 *         ├── AMarinPlayer
 *         └── ABaseEnemy
 *               ├── ALionfish
 *               └── ASharkBoss
 */

class UStaticMeshComponent;

UCLASS(Abstract)										// Abstract : 레벨에 직접 배치 불가, 자식 클래스를 통해서만 사용 가능
class NEMO_API ABaseCreature : public APawn
{
	GENERATED_BODY()

public:
	ABaseCreature();
	
	// ============== 데미지 / 힐 ==============
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount, AActor* Causer);
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyHeal(float Amount);
	
	
	// ================== 상태 ==================
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;
	
	UFUNCTION(BlueprintPure, Category = "Health")
	virtual bool IsDamageImmune() const;				// AMarinPlayer에서 override
														// 대시 중일 때 true 반환
	
	// ================ 즉사 처리 ================
	UFUNCTION(BlueprintCallable, Category = "Health")
	void KillInstantly();								// Lionfish 경직 중 대시로 처치
	
	
	// ================ Getter ================
	UFUNCTION(BlueprintPure, Category="Health")
	float GetCurrentHP() const;
	
	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHP() const;
	
	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealthPercent() const;						// @서희 : HP Bar에서 게이지 너비 계산에 사용

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="Health") 
	float MaxHP;										// BP 디테일 패널에서 액터마다 다르게 설정
														// BP_MarinPlayer → 100 / BP_Lionfish → 30 / BP_BossShark → 300
	
	void ProcessDeath();								// HP가 0이 됐을 때 내부에서 호출
	
	virtual void OnDeath();								// AMarinPlayer → 게임오버 UI
														// ABaseEnemy → Dead 상태 전환
	virtual void OnDamaged(float Amount, AActor* Causer);
														// AMarinPlayer → 화면 빨간 플래시
														// ABaseEnemy → ???
	// ================ 컴포넌트 ================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

private:
	bool bIsDead;
	float CurrentHP;
};
