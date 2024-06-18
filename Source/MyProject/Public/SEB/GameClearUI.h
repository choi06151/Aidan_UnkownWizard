// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameClearUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGameClearUI : public UUserWidget
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
	class UTextBlock* BestScore;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MyScore;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayTime;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectStageBtn;  
	UPROPERTY(meta = (BindWidget))
	class UButton* RestartBtn;
	
	UFUNCTION()
	void OnSelectStageClicked();
	UFUNCTION()
	void OnRestartClicked();

	class ASpawnWidget* SpawnWidget;
	class ASpawnLeftWidget* SpawnLeftWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> SelectStageUIClass;

	UPROPERTY()
	class UWidgetComponent* WidgetComponent;

	FText* ThumbnailPath;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* PlayDataTable;
	UFUNCTION(BlueprintCallable)
	void UpdateCountText();
	// 최종 Score 설정
	void SetMyScore(int32 score);
private:
	int32 CurrentCount;
	int32 MyScoreCount;
	FTimerHandle CountTimerHandle;
};
