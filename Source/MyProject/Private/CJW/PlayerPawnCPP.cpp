// Fill out your copyright notice in the Description page of Project Settings.


#include "CJW/PlayerPawnCPP.h"

// Sets default values
APlayerPawnCPP::APlayerPawnCPP()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HP = 100.0f;
}

// Called when the game starts or when spawned
void APlayerPawnCPP::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerPawnCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawnCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

