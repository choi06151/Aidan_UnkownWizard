// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossFSM.h"
#include "Animation/AnimInstance.h"
#include "Anim_Boss.generated.h"

UCLASS()
class MYPROJECT_API UAnim_Boss : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	class UBossFSM* bossFSM;

	UPROPERTY()
	class ABoss* me;
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	EBossState state;

	UPROPERTY(EditAnywhere, Category = "Spawn_Widget")
	class ASpawnWidget* SpawnWidget;
	
	//////////////////////////////////////// Notify ////////////////////////////////////////
	void AnimNotify_Attack_Start();
	void AnimNotify_Boss_Throw();
};
