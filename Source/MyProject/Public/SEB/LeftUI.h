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
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HPBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayTime;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* Score;

	// Ÿ�̸� �ڵ�
	FTimerHandle TimerHandle;
	// �ð� ����
	float TimeElapsed;
	void UpdateTImer();
	FString GetFormattedTime() const;
};
