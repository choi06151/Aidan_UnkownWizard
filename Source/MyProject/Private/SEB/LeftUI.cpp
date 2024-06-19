// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LeftUI.h"

#include "CJW/PlayerPawnCPP.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/GameOverUI.h"
#include "SEB/SpawnLeftWidget.h"
#include "SEB/SpawnWidget.h"

class UWidgetComponent;
class ABoss;

void ULeftUI::NativeConstruct()
{
	Super::NativeConstruct();

	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	SpawnLeftWidget = Cast<ASpawnLeftWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnLeftWidget::StaticClass()));

	// �ð� ���� �ʱ�ȭ
	TimeElapsed = 0.0f;
	PlayTime->SetText(SpawnLeftWidget->FinalPlayTime);
	Score->SetText(FText::AsNumber(SpawnLeftWidget->FinalScore));
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	Player = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));
}

void ULeftUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (SpawnLeftWidget->isRestart)
	{
		PlayTime->SetText(FText::FromString(TEXT("00:00:00")));
		Score->SetText(FText::AsNumber(0));
		SpawnLeftWidget->isRestart = false;
	}

	if (Boss->bIsAttackStart && Boss)
	{
		TimeElapsed += InDeltaTime;

		if (TimeElapsed >= TimeCheck)
		{
			UpdateTImer();
		}
		
	}

	////////////////////////////////////////////////// Game Clear //////////////////////////////////////////////////
	if (Player->HP > 0 && Player->HP <= 100 && !Boss->bGameOver && Boss->bIsMusicFinished)
	{
		Boss->bIsAttackStart = false;
		Boss->bClearGame = true;
		Boss->bGameOver = false;

		SpawnLeftWidget->FinalPlayTime = PlayTime->GetText();

		
		SpawnLeftWidget->FinalScore = Player->SCORE;
		
		UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
		WidgetComponent->SetWidgetClass(GameClearUIClass);
		Boss->bIsMusicFinished = false; // ��������� �� �Ǿ��°�??
		Boss->OnMusicFinished();
		SpawnWidget->isFirst = true;
	}

	////////////////////////////////////////////////// Game Over //////////////////////////////////////////////////
	if (Player->HP <= 0 && Boss->bIsAttackStart && !Boss->bGameOver)
	{
		Boss->bGameOver = true; // GameOver = true
		Boss->bClearGame = false;

		Player->HP = 180;

		//// ������ ��� �⺻ ���ð� ���� ////
		Boss->bIsAttackStart = false;
		Boss->bIsGameStart = false;
		Boss->bIsWalk = false;
		Boss->bIsArrive = false;
		Boss->bIsCommandWait = false;
		Boss->bIsMusicFinished = false;


		// BulletSpawner->DeactivateAllBullets();	// ���ӿ��������� �߻�� �Ѿ� ��Ȱ��ȭ -->��� �����ؾ� ��


		// Boss->OnMusicFinished(); // �� �ʿ� X


		UE_LOG(LogTemp, Warning, TEXT("ABoss::OnMusicFinished: Music has finished."));

		// �뷡�� ������ ź�� �߻� ���� 
		Boss->StopMusicAndFiringGameOver();


		if (SpawnWidget)
		{
			SpawnLeftWidget->FinalPlayTime = PlayTime->GetText();
			
			SpawnLeftWidget->FinalScore = Player->SCORE;
			SpawnWidget->isFirst = true;
			UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
			WidgetComponent->SetWidgetClass(GameOverUIClass);
			UUserWidget* UserWidget = WidgetComponent->GetUserWidgetObject();
			
			if (UserWidget)
			{
				UserWidget->SetVisibility(ESlateVisibility::Visible);
				UE_LOG(LogTemp, Error, TEXT("Visible!!Visible!!Visible!!Visible!!Visible!!"));
			}
			
			UE_LOG(LogTemp, Error, TEXT("Restart Widget Open"));
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("Player HP : %f"), Player->HP);
}

void ULeftUI::UpdateTImer()
{
	// �ð� ����
	TimeCheck += 0.01f;

	// Ÿ�̸� ������Ʈ ����
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
