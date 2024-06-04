// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UTestUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Music1;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Music2;
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Music3;
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Show5SecInfo;

	UPROPERTY()
	class ABoss* Boss;

	UFUNCTION()
	void OnMusic1Clicked();
	UFUNCTION()
	void OnMusic2Clicked();
	UFUNCTION()
	void OnMusic3Clicked();
	UFUNCTION()
	void OnShow5SecInfoClicked();
	UFUNCTION()
	void OnPreAnalyzeAllClicked();// 미리 분석하는


	void PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath);
};
