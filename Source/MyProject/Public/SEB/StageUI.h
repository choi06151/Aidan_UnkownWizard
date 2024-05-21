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
	
	void SetScrollText(const FText& NewText);
	void SetInfoText(const FText& NewInfoText);
	void SetDifficultyImage(int32 n);
private:
	
	
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* DifficultyGridPanel;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* StageName;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* InfoText;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SelectStageBtn;

	UFUNCTION()
	void OnSelectStageClicked();


	UPROPERTY(EditAnywhere)
	TSubclassOf<class USelectStageUI> SelectStageUIClass;

	
	
};
