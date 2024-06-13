// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSM.generated.h"

UENUM(BlueprintType)
enum class EBossState : uint8
{
	IDLE			UMETA(DisplayName = "IDLE"),
	WALK			UMETA(DisplayName = "WALK"),
	COMMANDWAIT		UMETA(DisplayName = "COMMANDWAIT"),
	COMMAND			UMETA(DisplayName = "COMMAND"),
	PHASE_2			UMETA(DisplayName = "PHASE_2")
	
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
	UPROPERTY(EditAnywhere, Category = "Boss")
	class ABoss* me;

	// Animation
	UPROPERTY(EditAnywhere, Category = "Boss")
	class UAnimMontage* commandWait;

	// Boss의 State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	EBossState state;

	UPROPERTY(EditAnywhere, Category = "Boss")
	class AAIController* ai;

	UPROPERTY(EditAnywhere, Category = "Boss")
	int curTime = 0;

	UPROPERTY(EditAnywhere, Category = "Boss")
	float commandTime  = 0;
	
	////////////////////////////////////// Boss HP 관련 ////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int bossMaxHP = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int bossHP = bossMaxHP;
	UFUNCTION(BlueprintCallable)
	void TakeDamaged(int damage);

	UPROPERTY(EditAnywhere)
	class UAnimMontage* deadMontage;

	
	void SetState(EBossState next);
	
	void TickIdle();
	void TickWalk();
	void TickCommandWait();
	void TickCommand();
	void TickPhase_2();
};
