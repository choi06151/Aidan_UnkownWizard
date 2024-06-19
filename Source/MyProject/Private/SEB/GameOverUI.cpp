// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/GameOverUI.h"

#include "CJW/PlayerPawnCPP.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/LeftUI.h"
#include "SEB/SpawnWidget.h"
#include "SEB/SelectStageUI.h"
#include "SEB/SpawnLeftWidget.h"

void UGameOverUI::NativeConstruct()
{
	Super::NativeConstruct();
	//Set UI
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	SpawnLeftWidget = Cast<ASpawnLeftWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnLeftWidget::StaticClass()));
	APlayerPawnCPP* Player = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	if(nullptr==SpawnWidget)
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
	SelectStageBtn->OnPressed.AddDynamic(this, &UGameOverUI::OnSelectStageClicked);
	RestartBtn->OnPressed.AddDynamic(this, &UGameOverUI::OnRestartClicked);
	

	/*PlayTime->SetText(FText::FromString(SpawnLeftWidget->FinalPlayTime.ToString()));*/
	PlayTime->SetText(SpawnLeftWidget->FinalPlayTime);	
	//Set Count
	CurrentCount = 0;
	Player->UpdateMaxScore();
	Player->UpdateMaxScoreCpp();
	UE_LOG(LogTemp, Error, TEXT("GameOverUI : %d"), SpawnLeftWidget->FinalScore);
	SetMyScore(SpawnLeftWidget->FinalScore);
	GetWorld()->GetTimerManager().SetTimer(CountTimerHandle, this, &UGameOverUI::UpdateCountText, 0.0001f, true);
}

void UGameOverUI::OnSelectStageClicked()
{
	SpawnLeftWidget->isRestart = true;
	WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(SelectStageUIClass);
}

void UGameOverUI::OnRestartClicked()
{
	// UI 숨김
	SetVisibility(ESlateVisibility::Hidden);
	UE_LOG(LogTemp, Error, TEXT("Hidden!!Hidden!!Hidden!!Hidden!!Hidden!!"));
	SpawnLeftWidget->isRestart = true;
	// 게임 시작 
	APlayerPawnCPP* PlayerInfo = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));
	PlayerInfo->StartGamePlayStageCpp();
	if (Boss)
	{
		//여기서 게임 리셋, , 노래 다시 시작, 보스 공격 다시 시작
		Boss->bIsGameStart = true;
		Boss->MusicStart();
		
	}
}

void UGameOverUI::UpdateCountText()
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

void UGameOverUI::SetMyScore(int32 score)
{
	MyScoreCount = score;
	//최신화 
}


