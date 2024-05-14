// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Anim_Boss.generated.h"

UCLASS()
class MYPROJECT_API UAnim_Boss : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speed = 0;
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UBossFSM* bossFSM;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAttack = false;

	// UFUNCTION()
	// void AnimNotify_BossAttack();
	//
	// UFUNCTION()
	// void AnimNotify_BossAttackCollision();
	//
	// UFUNCTION()
	// void AnimNotify_AttackEnd();
	//
	// UFUNCTION()
	// void AnimNotify_BossDamageEnd();
	//
	//
	// UFUNCTION()
	// void AnimNotify_BossHit();

	// // Sound
	// UPROPERTY(EditAnywhere)
	// class USoundBase* ;
};
