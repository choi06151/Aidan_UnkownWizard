// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/TutorialUI.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"

void UTutorialUI::NativeConstruct()
{
	Super::NativeConstruct();
	NextBtn->OnClicked.AddDynamic(this, &UTutorialUI::OnNextClicked);
	Welcome();
	
}

void UTutorialUI::StartTextAnimation(FString FullText, float Interval)
{
	TargetText = FullText;
	CurrentText = "";
	CurrentIndex = 0;

	if (AnimatedTextBlock)
	{
		GetWorld()->GetTimerManager().SetTimer(TextAnimationTimer, this, &UTutorialUI::UpdateText, Interval, true);
	}
}

void UTutorialUI::StartTextDeletion(float Interval)
{
	if (AnimatedTextBlock && !CurrentText.IsEmpty())
	{
		GetWorld()->GetTimerManager().SetTimer(TextDeletionTimer, this, &UTutorialUI::DeleteText, Interval, true);
	}
}

void UTutorialUI::UpdateText()
{
	if (CurrentIndex < TargetText.Len())
	{
		CurrentText.AppendChar(TargetText[CurrentIndex]);
		AnimatedTextBlock->SetText(FText::FromString(CurrentText));
		CurrentIndex++;
	}
	else
	{
		StartTextDeletion(0.05f);
		GetWorld()->GetTimerManager().ClearTimer(TextAnimationTimer);
	}
}

void UTutorialUI::DeleteText()
{
	if (CurrentText.Len() > 0)
	{
		CurrentIndex--;
		CurrentText = TargetText.Left(CurrentIndex);
		AnimatedTextBlock->SetText(FText::FromString(CurrentText));
	}
	else
	{
		
		GetWorld()->GetTimerManager().ClearTimer(TextDeletionTimer);
		//OnNextClicked();
	}
}

void UTutorialUI::Welcome()
{
	StartTextAnimation(TEXT("Welcome to the Tutorial!"), 0.1f);
	cnt++;
	
}

void UTutorialUI::StoryIntroduction()
{
	StartTextAnimation(TEXT("천재를 시기한 평범한 궁정음악가의 질투에서 시작된 광기에 찬 파멸의 서곡!"), 0.1f);
	cnt++;
}

void UTutorialUI::BasicShootingLearning()
{
	StartTextAnimation(TEXT("기본 슈팅 : 검지와 엄지 튕김"), 0.1f);
	cnt++;
	// 여기서 UI 껏다켰다를 하고 싶은데 글자가 다 나오기 전에 꺼져버리네
	TutorialTextCanvas->SetVisibility(ESlateVisibility::Hidden);
}

void UTutorialUI::OnNextClicked()
{
	switch (cnt)
	{
	case 1:
		StoryIntroduction();
		break;
	case 2:
		BasicShootingLearning();
		break;
	}
}
