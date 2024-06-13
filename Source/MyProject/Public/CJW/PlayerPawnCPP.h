// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawnCPP.generated.h"

UCLASS()
class MYPROJECT_API APlayerPawnCPP : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawnCPP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;




public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SCORE;
};
