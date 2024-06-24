// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OlderSister.generated.h"

UCLASS()
class MYPROJECT_API AOlderSister : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOlderSister();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void MoveToPlayer();
	void MoveToPlayerWithDelay(float Delay);

private:
	bool bShouldMoveToPlayer;
	FVector TargetLocation;
	FTimerHandle TimerHandle;

	void MoveWithDelay();
};
