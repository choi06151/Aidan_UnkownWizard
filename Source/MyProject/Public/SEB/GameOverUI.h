// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGameOverUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Thumbnail;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ArtistName;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MusicName;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectStageBtn;  
	UPROPERTY(meta = (BindWidget))
	class UButton* RestartBtn;
	
	UFUNCTION()
	void OnSelectStageClicked();
	UFUNCTION()
	void OnRestartClicked();

	class ASpawnWidget* SpawnWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> SelectStageUIClass;

	UPROPERTY()
	class UWidgetComponent* WidgetComponent;

	FText* ThumbnailPath;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* PlayDataTable;
};
