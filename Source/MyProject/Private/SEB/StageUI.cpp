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
		SelectStageBtn->OnPressed.AddDynamic(this, &UStageUI::OnSelectStageClicked);
	}
}

void UStageUI::SetArtistText(const FText& NewText)
{
	if (ArtistName)
	{
		ArtistName->SetText(NewText);
	}
}

void UStageUI::SetMusicText(const FText& NewInfoText)
{
	if (MusicName)
	{
		MusicName->SetText(NewInfoText);
	}
}

void UStageUI::SetDifficultyImage(int32 n)
{
	if(DifficultyGridPanel)
	{
		DifficultyNum = n;
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

void UStageUI::SetBestScore(int32 score)
{
}

void UStageUI::OnSelectStageClicked()
{
	if (ParentSelectStageUI)
	{
		ParentSelectStageUI->ChangeStageName(ArtistName->GetText(), MusicName->GetText());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ParentSelectStageUI is null"));
	}
	ChangeButtonColor(FColor(255, 100, 22, 255));
}

void UStageUI::ChangeButtonColor(const FColor& NewColor)
{
	if (SelectStageBtn)
	{
		FButtonStyle ButtonStyle = SelectStageBtn->WidgetStyle;

		FSlateBrush NormalBrush = ButtonStyle.Normal;
		NormalBrush.TintColor = FSlateColor(NewColor);
		ButtonStyle.SetNormal(NormalBrush);

		SelectStageBtn->SetStyle(ButtonStyle);
	}
}

