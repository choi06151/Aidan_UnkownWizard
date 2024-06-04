// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStartUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGameStartUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* StartBtn;  
	UPROPERTY(meta = (BindWidget))
	class UButton* QuitBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* TutorialBtn;
	
	UFUNCTION()
	void OnStartBtnClicked();
	UFUNCTION()
	void OnQuitBtnClicked();
	UFUNCTION()
	void OnTutorialBtnClicked();

	class ASpawnWidget* SpawnWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> SelectStageUIClass;

	UPROPERTY()
	class ABoss* Boss;
};
