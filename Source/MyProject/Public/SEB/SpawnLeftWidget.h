// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnLeftWidget.generated.h"

UCLASS()
class MYPROJECT_API ASpawnLeftWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnLeftWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FText FinalPlayTime = FText::FromString(TEXT("00:00:00"));
	int32 FinalScore;

	bool isRestart = false;
};
