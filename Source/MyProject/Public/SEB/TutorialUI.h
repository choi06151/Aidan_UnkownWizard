// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialUI.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UTutorialUI : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
public:
	UFUNCTION()
	void StartTextAnimation(FString FullText, float Interval);
	UFUNCTION(BlueprintCallable)
	void StartTextDeletion(float Interval);
private:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* TutorialTextCanvas;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UTextBlock* AnimatedTextBlock;
	UPROPERTY(meta = (BindWidget))
	class UButton* NextBtn;
	
	
	FString TargetText;
	FString CurrentText;
	int32 CurrentIndex;

	FTimerHandle TextAnimationTimer;
	FTimerHandle TextDeletionTimer;
	
	void UpdateText();
	void DeleteText();

	void Welcome();
	void StoryIntroduction();
	void BasicShootingLearning();
	
	int32 cnt=0;

	UFUNCTION()
	void OnNextClicked();
};
