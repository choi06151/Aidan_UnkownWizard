// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/OlderSister.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AOlderSister::AOlderSister()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bShouldMoveToPlayer = false;

}

// Called when the game starts or when spawned
void AOlderSister::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOlderSister::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (bShouldMoveToPlayer)
    {
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction, 1.0f);

        if (FVector::Dist(GetActorLocation(), TargetLocation) < 100.0f)
        {
            bShouldMoveToPlayer = false;
        }
    }

}

// Called to bind functionality to input
void AOlderSister::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AOlderSister::MoveToPlayer()
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn)
    {
        TargetLocation = PlayerPawn->GetActorLocation();
        bShouldMoveToPlayer = true;
    }
}

void AOlderSister::MoveToPlayerWithDelay(float Delay)
{
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AOlderSister::MoveWithDelay, Delay, false);
}

void AOlderSister::MoveWithDelay()
{
    MoveToPlayer();
}

