// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/GameStartUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"
#include "Components/WidgetComponent.h"

void UGameStartUI::NativeConstruct()
{
	Super::NativeConstruct();
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	StartBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnStartBtnClicked);
	QuitBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnQuitBtnClicked);
	TutorialBtn->OnPressed.AddDynamic(this, &UGameStartUI::OnTutorialBtnClicked);
}

void UGameStartUI::OnStartBtnClicked()
{
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
}
