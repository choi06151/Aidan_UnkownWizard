// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "MusicInfoDT.h"
#include "SpawnWidget.h"
#include "SelectStageUI.generated.h"

/**
 * 
 */
UCLASS()
class USelectStageUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
private:
	UPROPERTY(EditAnywhere)
	TArray<FText> ScrollTextArr;
	
	UPROPERTY(EditAnywhere)
	TArray<FText> InfoTextArr;

	UPROPERTY(EditAnywhere)
	TArray<int32> DifficultyArr;

	
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UScrollBox* MainScroll;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* ArtistName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* MusicName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* BestScore;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* Thumbnail;
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* DifficultyGridPanel;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* UpArrowBtn;  
	UPROPERTY(meta = (BindWidget))
	class UButton* DownArrowBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* PlayBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackBtn;
	
	
	UFUNCTION()
	void OnUPArrowClicked();
	UFUNCTION()
	void OnDownArrowClicked();
	UFUNCTION()
	void OnBackClicked();
	UFUNCTION()
	void OnPlayClicked();

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStageUI> StageUIClass;

	void SetStarFill(class UImage* ImageWidget, FText* Path);

	class ASpawnWidget* SpawnWidget;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> GameStartUIClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> GameOverUIClass;
	
	
	FText* ThumbnailPath;
	void PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath);
public:
	UFUNCTION(BlueprintCallable)
	void ChangeStageName(const FText& NewText,  const FText& NewInfoText);

	FMusicInfoDT* FindRowByColumnValue(const FString& ColumnName1, const FString& ColumnValue1, const FString& ColumnName2, const FString& ColumnValue2);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UDataTable* MusicDataTable;

	FText MusicInfo;
	

	
};
