#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
// #include "JWK/BossFSM.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	// bossFSM = CreateDefaultSubobject<UBossFSM>(TEXT("bossFSM"));
	
	movementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movementComp"));
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

