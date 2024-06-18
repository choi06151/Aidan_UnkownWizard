// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LeftUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ULeftUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayTime;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* Score;

	// 타이머 핸들
	FTimerHandle TimerHandle;
	// 시간 변수
	float TimeCheck = 0.01f;
	float TimeElapsed;
	void UpdateTImer();
	FString GetFormattedTime() const;

public:
	void StartTimer();

	class ABoss* Boss;
	class APlayerPawnCPP* Player;
	bool isCheck = false;

	class ASpawnWidget* SpawnWidget;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> GameClearUIClass;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverUIClass;
};
