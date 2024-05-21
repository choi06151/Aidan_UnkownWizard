// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SelectStageUI.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "SEB/GameStartUI.h"
#include "SEB/StageUI.h"


void USelectStageUI::NativePreConstruct()
{
	Super::NativePreConstruct();

	//음악 기본 정보 저장 -> 추후 데이터베이스로 관리 예정
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

	//MainScroll에 버튼 추가 -> 배열 안에 들어있는 값으로 저장
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

	//배열의 첫번째 값들로 정보 초기화
	if (StageName)
	{
		StageName->SetText(ScrollTextArr[0]);
	}
	if(InfoText)
	{
		InfoText->SetText(InfoTextArr[0]);
	}
}

void USelectStageUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (LeftArrowBtn)
	{
		LeftArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnUPArrowClicked);
	}

	if (RightArrowBtn)
	{
		RightArrowBtn->OnClicked.AddDynamic(this, &USelectStageUI::OnDownArrowClicked);
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


void USelectStageUI::OnUPArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset - 300.0f);
	}
}

void USelectStageUI::OnDownArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset + 300.0f);
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


//버튼 클릭 시 좌측 정보 업데이트
void USelectStageUI::ChangeStageName(const FText& NewText, const FText& NewInfoText, int32 num)
{
	if (StageName)
	{
		StageName->SetText(NewText);
	}
	if(InfoText)
	{
		InfoText->SetText(NewInfoText);
	}

	//난이도 별 이미지 변경
	if(DifficultyGridPanel)
	{
		for(int i=0; i<DifficultyGridPanel->GetChildrenCount(); i++)
		{
			UWidget* ChildWidget = DifficultyGridPanel->GetChildAt(i);
			UImage* ImageWidget = Cast<UImage>(ChildWidget);
			
			if(i>=num) {
				if(ImageWidget)
				{
					FText* Path = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star.Star'")));
					SetStarFill(ImageWidget, Path);
				}
			}
			else
			{
				if(ImageWidget)
				{
					FText* Path = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star_fill.Star_fill'")));
					SetStarFill(ImageWidget, Path);
				}
			}

			
		}
	}
}

void USelectStageUI::SetStarFill(UImage* ImageWidget, FText* Path)
{
	const TCHAR* MyCharArr = *Path->ToString();
	UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, MyCharArr);
	if (NewTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(NewTexture);
		ImageWidget->SetBrush(NewBrush);
	}
}