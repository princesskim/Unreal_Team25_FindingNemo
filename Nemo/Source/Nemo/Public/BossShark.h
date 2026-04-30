// BossShark.h

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BossShark.generated.h"

UCLASS()
class NEMO_API ABossShark : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ABossShark();
	
protected:
	virtual void CheckChargeOverlap() override;	
	
private:
	UPROPERTY(EditDefaultsOnly, Category="SharkBoss")
	float ChargeRadius;   // 광역 돌진 범위
};
