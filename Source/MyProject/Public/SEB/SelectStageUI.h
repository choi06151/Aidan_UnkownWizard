// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
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
	class UTextBlock* StageName;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* InfoText;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* LeftArrowBtn;  
	UPROPERTY(meta = (BindWidget))
	class UButton* RightArrowBtn;
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

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* DifficultyGridPanel;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UStageUI> StageUIClass;

	void SetStarFill(class UImage* ImageWidget, FText* Path);
public:
	UFUNCTION(BlueprintCallable)
	void ChangeStageName(const FText& NewText,  const FText& NewInfoText, int32 num);

};
