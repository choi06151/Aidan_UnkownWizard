// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSM.generated.h"

UENUM()
enum class EBoss_state : uint8
{
	IDLE	UMETA(DisplaName = "IDLE"),
	WALK	UMETA(DisplaName = "WALK"),
	ATTACK	UMETA(DisplaName = "ATTACK"),
	Phase	UMETA(DisplaName = "Phase"),
	DEAD	UMETA(DisplaName = "DEAD")
	
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
	EBoss_state state;

	UPROPERTY(EditAnywhere)
	float curTime;

	
private:
	void TickIdle();
	void TickWalk();
	void TickAttack();
	void TickPhase();
	void TickDead();

	void SetState(EBoss_state nextState);
};
