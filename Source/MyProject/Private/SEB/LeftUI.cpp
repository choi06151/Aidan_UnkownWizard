// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LeftUI.h"

#include "CJW/PlayerPawnCPP.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnLeftWidget.h"
#include "SEB/SpawnWidget.h"

class UWidgetComponent;
class ABoss;

void ULeftUI::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	SpawnLeftWidget = Cast<ASpawnLeftWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnLeftWidget::StaticClass()));
	
	// 시간 변수 초기화
	TimeElapsed = 0.0f;
	PlayTime->SetText(SpawnLeftWidget->FinalPlayTime);
	Score->SetText(FText::AsNumber(SpawnLeftWidget->FinalScore));
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	Player = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));

	
}

void ULeftUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(SpawnLeftWidget->isRestart)
	{
		PlayTime->SetText(FText::FromString(TEXT("00:00:00")));
		Score->SetText(FText::AsNumber(0));
		SpawnLeftWidget->isRestart = false;
		
	}
	
	if(Boss->bIsAttackStart && Boss)
	{
		TimeElapsed += InDeltaTime;
		
		if(TimeElapsed >= TimeCheck)
		{
			UpdateTImer();
			
		}
	}
	if(Player->HP > 0 && Boss->bIsMusicFinished) // Game Clear
	{
		Boss->bIsAttackStart = false;
		//Boss->bClearGame = true;
		SpawnLeftWidget->FinalPlayTime = PlayTime->GetText();
		FString ScoreStr = Score->GetText().ToString();
		SpawnLeftWidget->FinalScore = FCString::Atoi(*ScoreStr);
		UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
		WidgetComponent->SetWidgetClass(GameClearUIClass);
		Boss->bIsMusicFinished = false;
		Boss->OnMusicFinished();
	}
	/*if(Player->HP <= 0 && Boss->bIsAttackStart) //Game Over
	{
		Boss->bIsAttackStart = false;
		//Boss->bGameOver = true;
		Boss->OnMusicFinished();
		if(SpawnWidget)
		{
			SpawnLeftWidget->FinalPlayTime = PlayTime->GetText();
			FString ScoreStr = Score->GetText().ToString();
			SpawnLeftWidget->FinalScore = FCString::Atoi(*ScoreStr);
			UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
			WidgetComponent->SetWidgetClass(GameOverUIClass);
			
		}
	}*/
}

void ULeftUI::UpdateTImer()
{
	// 시간 증가
	TimeCheck += 0.01f;

	// 타이머 업데이트 로직
	if (PlayTime)
	{
		PlayTime->SetText(FText::FromString(GetFormattedTime()));
	}
}

FString ULeftUI::GetFormattedTime() const
{
	int32 TotalMilliseconds = static_cast<int32>(TimeElapsed * 1000.0f);
	int32 Minutes = TotalMilliseconds / 60000;
	TotalMilliseconds %= 60000;
	int32 Seconds = TotalMilliseconds / 1000;
	int32 Centiseconds = (TotalMilliseconds % 1000) / 10;  

	return FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Centiseconds);
}



