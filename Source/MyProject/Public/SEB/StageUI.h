// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "StageUI.generated.h"


/**
 * 
 */
UCLASS()
class UStageUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(BlueprintReadWrite)
	class USelectStageUI* ParentSelectStageUI;
	
	void SetArtistText(const FText& NewText);
	void SetMusicText(const FText& NewInfoText);
	void SetDifficultyImage(int32 n);
	void SetBestScore(int32 score);
private:
	
	
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* DifficultyGridPanel;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* ArtistName;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* MusicName;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectStageBtn;

	int32 DifficultyNum;

	UFUNCTION()
	void OnSelectStageClicked();


	UPROPERTY(EditAnywhere)
	TSubclassOf<class USelectStageUI> SelectStageUIClass;

	
	
};
