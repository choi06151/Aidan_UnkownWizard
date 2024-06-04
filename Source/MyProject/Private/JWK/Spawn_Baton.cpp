// Fill out your copyright notice in the Description page of Project Settings.


#include "JWK/Spawn_Baton.h"

// Sets default values for this component's properties
USpawn_Baton::USpawn_Baton()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpawn_Baton::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USpawn_Baton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

