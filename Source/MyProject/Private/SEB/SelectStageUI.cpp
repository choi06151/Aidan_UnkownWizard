// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SelectStageUI.h"

#include "Components/Button.h"
#include "SEB/GameStartUI.h"
#include "SEB/StageUI.h"


void USelectStageUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	ScrollTextArr = { 
		FText::FromString(TEXT("Stage1")), 
		FText::FromString(TEXT("Stage2")), 
		FText::FromString(TEXT("Stage3")),
		FText::FromString(TEXT("Stage4")),
		FText::FromString(TEXT("BEETHOVEN")),
		FText::FromString(TEXT("Stage6")),
		FText::FromString(TEXT("Stage7")),
		FText::FromString(TEXT("Stage8")),
		FText::FromString(TEXT("Stage9"))
	};
	InfoTextArr = { 
		FText::FromString(TEXT("aaaaaaaaaaaaaaaaaaaaaaaaa")), 
		FText::FromString(TEXT("bbbbbbbbbbbbbbbbbbbbbbbbb")), 
		FText::FromString(TEXT("ccccccccccccccccccccccccc")),
		FText::FromString(TEXT("ddddddddddddddddddddddddd")),
		FText::FromString(TEXT("Piano Sonata No. 17 (Op. 31-2) d minor, Tempest")),
		FText::FromString(TEXT("fffffffffffffffffffffffff")),
		FText::FromString(TEXT("ggggggggggggggggggggggggg")),
		FText::FromString(TEXT("hhhhhhhhhhhhhhhhhhhhhhhhh")),
		FText::FromString(TEXT("iiiiiiiiiiiiiiiiiiiiiiiii"))
	};
	DifficultyArr = {1,2,3,4,5,1,2,3,4};
	
	if (!StageUIClass || !MainScroll) return;

	MainScroll->ClearChildren();
	
	for (int i = 0; i < ScrollTextArr.Num(); i++)
	{
		UStageUI* StageUI = CreateWidget<UStageUI>(this, StageUIClass);
		if (StageUI)
		{
			StageUI->SetScrollText(ScrollTextArr[i]);
			StageUI->SetInfoText(InfoTextArr[i]);
			StageUI->SetDifficultyImage(DifficultyArr[i]);
			StageUI->ParentSelectStageUI = this;  
			MainScroll->AddChild(StageUI);
		}
	}
	
}

void USelectStageUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (LeftArrowBtn)
	{
		LeftArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnLeftArrowClicked);
	}

	if (RightArrowBtn)
	{
		RightArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnRightArrowClicked);
	}
	
	if (BackBtn)
	{
		BackBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnBackClicked);
	}

	if (PlayBtn)
	{
		BackBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnPlayClicked);
	}
}


void USelectStageUI::OnLeftArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset - 400.0f);
	}
}

void USelectStageUI::OnRightArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset + 400.0f);
	}
}

void USelectStageUI::OnBackClicked()
{
	// SpawnUI의 Widget Class를 변경
}

void USelectStageUI::OnPlayClicked()
{
	// 게임 시작
}

void USelectStageUI::ChangeStageName(const FText& NewText, const FText& NewInfoText)
{
	if (StageName)
	{
		StageName->SetText(NewText);
	}
	if(InfoText)
	{
		InfoText->SetText(NewInfoText);
	}
}

