// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/GameStartUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"

void UGameStartUI::NativeConstruct()
{
	Super::NativeConstruct();
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	StartBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnStartBtnClicked);
	QuitBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnQuitBtnClicked);
	TutorialBtn->OnClicked.AddDynamic(this, &UGameStartUI::OnTutorialBtnClicked);

	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
}

void UGameStartUI::OnStartBtnClicked()
{
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
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}

void UGameStartUI::OnTutorialBtnClicked()
{
	// 원하는 맵 이름을 문자열로 지정
	FString LevelName = TEXT("TutorialTestMap");

	// 월드를 가져와서 맵 변경 함수 호출
	UGameplayStatics::OpenLevel(this, FName(*LevelName));
}
