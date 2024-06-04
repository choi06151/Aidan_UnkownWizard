// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/GameClearUI.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/MusicInfoDT.h"
#include "SEB/SpawnWidget.h"
#include "SEB/SelectStageUI.h"

void UGameClearUI::NativeConstruct()
{
	Super::NativeConstruct();
	//Set UI
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	if(!SpawnWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnWidget is Null"));
		return;
	}

	FMusicInfoDT* SpecificRow = SpawnWidget->SpecificRow;
	
	ArtistName->SetText(FText::FromString(SpecificRow->ArtistName));
	MusicName->SetText(FText::FromString(SpecificRow->MusicName));
	ThumbnailPath = new FText(FText::FromString(SpecificRow->Thumbnail));
	UTexture2D* ThumbnailTexture = LoadObject<UTexture2D>(nullptr, *ThumbnailPath->ToString());
	if (ThumbnailTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(ThumbnailTexture);
		Thumbnail->SetBrush(NewBrush);
	}
	BestScore->SetText(FText::AsNumber(SpecificRow->BestScore));
	
	//Button
	SelectStageBtn->OnPressed.AddDynamic(this, &UGameClearUI::OnSelectStageClicked);
	RestartBtn->OnPressed.AddDynamic(this, &UGameClearUI::OnRestartClicked);

	//MusicPlay Test => 나중에 이런식으로 끌어서 쓰면 됨. 
	SpawnWidget->MusicPlay();
	
	//Set Count
	CurrentCount = 0;
	
	SetMyScore(SpecificRow->BestScore);

	GetWorld()->GetTimerManager().SetTimer(CountTimerHandle, this, &UGameClearUI::UpdateCountText, 0.0001f, true);
}

void UGameClearUI::OnSelectStageClicked()
{
	WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(SelectStageUIClass);
}

void UGameClearUI::OnRestartClicked()
{
}

void UGameClearUI::UpdateCountText()
{
	if(CurrentCount >= MyScoreCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountTimerHandle);
		return;
	}

	CurrentCount++;
	if(MyScore)
	{
		MyScore->SetText(FText::AsNumber(CurrentCount));
	}
}

void UGameClearUI::SetMyScore(int32 score)
{
	MyScoreCount = score;
}
