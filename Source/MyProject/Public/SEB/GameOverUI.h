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
	TSubclassOf<class USelectStageUI> SelectStageUIClass;

	
	
	UPROPERTY()
	class UWidgetComponent* WidgetComponent;

	
	
	FText* ThumbnailPath;

public:

	// LeftUI 인스턴스를 저장할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class ULeftUI> LeftUIClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class ULeftUI* LeftUIInstance;
	
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
	class ABoss* Boss;

};
