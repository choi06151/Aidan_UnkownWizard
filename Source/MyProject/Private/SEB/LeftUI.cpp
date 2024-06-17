// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LeftUI.h"

#include "CJW/PlayerPawnCPP.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"

class UWidgetComponent;
class ABoss;

void ULeftUI::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	
	// 시간 변수 초기화
	TimeElapsed = 0.0f;
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	Player = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));
}

void ULeftUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(Boss->bIsAttackStart && Boss)
	{
		TimeElapsed += InDeltaTime;
		
		if(TimeElapsed >= TimeCheck)
		{
			UpdateTImer();
			
		}
	}
	if(Boss->bIsMusicFinished)
	{
		Boss->bIsAttackStart = false;
		
		UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
		WidgetComponent->SetWidgetClass(GameClearUIClass);
		Boss->bIsMusicFinished = false;
	}
	int32 hp = Player->HP;
	if(hp <= 0)
	{
		Boss->bIsAttackStart = false;
		if(SpawnWidget)
		{
			UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
			WidgetComponent->SetWidgetClass(GameOverUIClass);
		}
	}
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



