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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float speed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class ABoss* me;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	EBoss_state state;
};
