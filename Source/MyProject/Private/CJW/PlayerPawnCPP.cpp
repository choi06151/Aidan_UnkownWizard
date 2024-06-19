// Fill out your copyright notice in the Description page of Project Settings.


#include "CJW/PlayerPawnCPP.h"

#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"

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
	Boss =  Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	
}

// Called every frame
void APlayerPawnCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(SCORE >= 300000)
	{
		if(Boss)
			Boss->bIsPhase = true;
	}
}

// Called to bind functionality to input
void APlayerPawnCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerPawnCPP::UpdateMaxScore()
{
	if (SCORE > MAXSCORE)
		SCORE = MAXSCORE;
}