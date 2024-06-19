// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MusicInfoDT.h"
#include "GameFramework/Actor.h"
#include "SpawnWidget.generated.h"

UCLASS()
class MYPROJECT_API ASpawnWidget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnWidget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClass;

	class USelectStageUI* SelectedMusicInfo;
	FText ArtistName;

	FMusicInfoDT* SpecificRow;
	void MusicPlay();

	void MusicPlayOnly();
	TArray<AActor*> Curtains;
	void CurtainOpenAnim();
	void CurtainCloseAnim();
	void CurtainShakeAnim();

	int32 CurrentStage = 1;

	bool isFirst = true;
private:
	UPROPERTY()
	class UWidgetComponent* WidgetComponent;

	UAnimSequence* OpenCurtainAnim;
	UAnimSequence* ShakeCurtainAnim;

	UPROPERTY()
	class ABoss* Boss;

	
};
