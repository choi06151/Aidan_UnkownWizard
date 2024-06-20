// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/GameStartUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "CJW/PlayerPawnCPP.h"
void UGameStartUI::NativeConstruct()
{
	Super::NativeConstruct();
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	StartBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnStartBtnClicked);
	QuitBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnQuitBtnClicked);
	TutorialBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnTutorialBtnClicked);

	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	PlayerInfo = Cast<APlayerPawnCPP>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerPawnCPP::StaticClass()));
}

void UGameStartUI::OnStartBtnClicked()
{
	UGameplayStatics::PlaySound2D(this, SpawnWidget->SFX_Button);
	if (Boss)
	{
		Boss->PreAnalyzeAllMusicData();
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnPreAnalyzeAllClicked: All music data pre-analyzed."));
	}
	// SpawnWidget의 WidgetClass를 변경
	UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(SelectStageUIClass);
}

void UGameStartUI::OnQuitBtnClicked()
{
	UGameplayStatics::PlaySound2D(this, SpawnWidget->SFX_Button);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}

void UGameStartUI::OnTutorialBtnClicked()
{
	UGameplayStatics::PlaySound2D(this, SpawnWidget->SFX_Button);
	PlayerInfo->StartTutorialStageCpp();
}
