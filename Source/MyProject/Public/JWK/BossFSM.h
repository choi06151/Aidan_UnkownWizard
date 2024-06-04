// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSM.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	IDLE	UMETA(DisplayName = "IDLE"),
	WALK	UMETA(DisplayName = "WALK"),
	ATTACK	UMETA(DisplayName = "ATTACK"),
	PHASE_2	UMETA(DisplayName = "PHASE_2")
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UBossFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBossFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// boss
	UPROPERTY(EditAnywhere)
	class ABoss* me;

	// Animation
	UPROPERTY(EditAnywhere)
	class UAnim_Boss* bossAnim;

	// Boss의 State
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBossState state;

	UPROPERTY(EditAnywhere)
	class AAIController* ai;
	
	int curTime = 0;

	
	//////////////////////////////////////// Boss HP 관련 ////////////////////////////////////////
	UFUNCTION(BlueprintCallable)
	void TakeDamaged(int damage);

	UPROPERTY(EditAnywhere)
	class UAnimMontage* deadMontage;

	
	void SetState(EBossState next);
	
	void TickIdle();
	void TickWalk();
	void TickAttack();
	void TickPhase_2();
};
