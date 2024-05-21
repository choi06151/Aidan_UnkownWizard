// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/StageUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "SEB/SelectStageUI.h"

void UStageUI::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UStageUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (SelectStageBtn)
	{
		SelectStageBtn->OnClicked.AddDynamic(this, &UStageUI::OnSelectStageClicked);
	}
}

void UStageUI::SetScrollText(const FText& NewText)
{
	if (StageName)
	{
		StageName->SetText(NewText);
	}
}

void UStageUI::SetInfoText(const FText& NewInfoText)
{
	if (InfoText)
	{
		InfoText->SetText(NewInfoText);
	}
}

void UStageUI::SetDifficultyImage(int32 n)
{
	if(DifficultyGridPanel)
	{
		for(int i=0; i<DifficultyGridPanel->GetChildrenCount(); i++)
		{
			if(i>=n) break; // n개 이상의 이미지는 변경하지 않음
			UWidget* ChildWidget = DifficultyGridPanel->GetChildAt(i);
            UImage* ImageWidget = Cast<UImage>(ChildWidget);

			if(ImageWidget)
			{
				UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star_fill.Star_fill'"));
				if (NewTexture)
				{
					FSlateBrush NewBrush;
					NewBrush.SetResourceObject(NewTexture);
					ImageWidget->SetBrush(NewBrush);
				}
			}
		}
	}
}

void UStageUI::OnSelectStageClicked()
{
	if (ParentSelectStageUI)
	{
		ParentSelectStageUI->ChangeStageName(StageName->GetText(), InfoText->GetText());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ParentSelectStageUI is null"));
	}
}

