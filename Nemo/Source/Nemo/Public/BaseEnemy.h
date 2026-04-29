// BaseEnemy.h

#pragma once

#include "CoreMinimal.h"
#include "BaseCreature.h"
#include "BaseEnemy.generated.h"

class UDecalComponent;							// 전투 가독성을 올리는 투영 컴포넌트
class UMaterialInterface;
class UMaterialInstanceDynamic;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,										// 대기 중
	Telegraph,									// 텔레그래프 중 - 공격 예고
	Charging,									// 돌진 중 — 플레이어에게 데미지
	Stunned,									// 경직 중 — 대시로 들이받으면 처치
	Dead										// 사망
};

UCLASS(Abstract)
class NEMO_API ABaseEnemy : public ABaseCreature
{
	GENERATED_BODY()

public:
	ABaseEnemy();
	
	virtual void BeginPlay() override;
	
	// ============== AI 인터페이스 ==============
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void EnterTelegraphState();
	
	UFUNCTION(BlueprintCallable, Category="Enemy")
	virtual void EnterChargingState();									// ALionfish-직선 돌진, ASharkBoss-광역 돌진

	UFUNCTION(BlueprintCallable, Category="Enemy")
	void EnterStunnedState(float InStunDuration);
	
	
	// ============== 상태 조회 ==============
	UFUNCTION(BlueprintPure, Category="Enemy")
	EEnemyState GetEnemyState() const;
	
	UFUNCTION(BlueprintPure, Category="Enemy")
	float GetTelegraphRatio() const;									// 텔레그래프 진행도 (0.0 → 1.0)
																		// 머티리얼에서 데칼 애니메이션에 활용 가능
																		// 원 커짐 + 밝기 증가 + 색 변화 등

	UFUNCTION(BlueprintCallable, Category="Enemy")
	void ResetEnemyState(EEnemyState NewState);							// 사망·외부 이슈로 인해 진행 중인 행동 강제 중단
																		// 타이머 정리, 데칼 정리, 상태 초기화 등을 해주지 않으면 버그 발생
	
protected:
    virtual void Tick(float DeltaTime) override;

    virtual void OnDeath()   override;
    virtual void OnDamaged(float Amount, AActor* Causer) override;		// 경직 중 대시 충돌은 플레이어가 감지하기 때문에 빈 함수임
																		// 자식에서 override해서 추가 처리 가능
	void SetState(EEnemyState NewState);

	void CheckChargeOverlap();											// 돌진 중 플레이어 충돌 감지 + 데미지 적용
																		// ExecuteCharge에서 호출
	

	// ========= Telegraph 데칼 컴포넌트 =========
	UPROPERTY(VisibleAnywhere, Category="Enemy|Telegraph")
	TObjectPtr<UDecalComponent> TelegraphDecal;							// 항상 존재하는 컴포넌트가 아니므로 생성자에 작성하지 않음
	
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Telegraph")
	TObjectPtr<UMaterialInterface> TelegraphMaterial;					// 디테일 패널에서 머티리얼 할당
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> TelegraphMID;					// 런타임에 파라미터 변경 가능, 인스턴스 단위로 값이 분리됨
	
	
    // ============ Telegraph 파라미터 ============
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Telegraph")
	float TelegraphDecalRadius;
	
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Telegraph")
    float TelegraphDuration;											// ALionfish  : TelegraphDuration=1.5s
																		// ASharkBoss : TelegraphDuration=3.0s

    // =========== Charging 파라미터 ===========
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Charging")
	float ChargeDuration;
	
    UPROPERTY(EditDefaultsOnly, Category="Enemy|Combat")
    float ChargeDamage;													// 돌진+충돌로 플레이어에게 입히는 데미지
    
	// =========== Charging 파라미터 ===========
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Stunned")
    float StunDuration;
	
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Stun")
	float StunMultiplier;												// 확장성을 고려한 변수 (해당 프로젝트에서는 의미 X)
	
	
	
private:
    EEnemyState CurrentState;
	
	void UpdateTelegraphDecal();
	
	// ============== AI 인터페이스 ==============
    void ExitTelegraphState();
    void ExitChargingState();
	void ExitStunnedState();

	// ============= Telegraph 타이머 =============
	FTimerHandle TelegraphTimerHandle;
	FTimerHandle ChargeTimerHandle;
	FTimerHandle ChargeOverlapTimerHandle;								// 돌진 중 충돌 감지 반복 타이머
	float TelegraphStartTime;
	
    // ============== Stunned 타이머 ==============
    FTimerHandle StunTimerHandle;
	
	bool bIsStunned;
	
};