// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/TestUI.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "JWK/Boss.h"
#include "Components/Button.h"

void UTestUI::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UTestUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Music1)
	{
		Btn_Music1->OnPressed.AddDynamic(this, &UTestUI::OnMusic1Clicked);
	}
	if (Btn_Music2)
	{
		Btn_Music2->OnPressed.AddDynamic(this, &UTestUI::OnMusic2Clicked);
	}
	if (Btn_Music3)
	{
		Btn_Music3->OnPressed.AddDynamic(this, &UTestUI::OnMusic3Clicked);
	}
	if (Btn_Show5SecInfo)
	{
		Btn_Show5SecInfo->OnPressed.AddDynamic(this, &UTestUI::OnPreAnalyzeAllClicked);
	}

	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
}

void UTestUI::OnMusic1Clicked()
{
	//FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/butterfly.butterfly'");
	//// JsonFilePath를 절대 경로로 변환
	//FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/butterfly.json");
	//PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	//UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic1111Clicked"));
	if (Boss)
	{
		Boss->StopMusic();
		FString MusicTitle = TEXT("Butter-Fly");
		FString MusicFilePath = TEXT("/Game/Music/butterfly.butterfly");
		Boss->LoadMusicDataAndSetPatterns(MusicTitle, MusicFilePath);
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic1Clicked"));
	}
}

void UTestUI::OnMusic2Clicked()
{
	//FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/Elise.Elise'");
	//// JsonFilePath를 절대 경로로 변환
	//FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/Elise.json");
	//PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	//UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic22222Clicked"));
	if (Boss)
	{
		Boss->StopMusic();
		FString MusicTitle = TEXT("Fur Elise");
		FString MusicFilePath = TEXT("/Game/Music/Elise.Elise");
		Boss->LoadMusicDataAndSetPatterns(MusicTitle, MusicFilePath);
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic2Clicked"));
	}
}

void UTestUI::OnMusic3Clicked()
{
	//FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/Lacrimosa.Lacrimosa'");
	//// JsonFilePath를 절대 경로로 변환
	//FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/Lacrimosa.json");
	//PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	//UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic3333Clicked"));
	if (Boss)
	{
		Boss->StopMusic();
		FString MusicTitle = TEXT("Requiem ? Lacrimosa");
		FString MusicFilePath = TEXT("/Game/Music/Lacrimosa.Lacrimosa");
		Boss->LoadMusicDataAndSetPatterns(MusicTitle, MusicFilePath);
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic3Clicked"));
	}
}


void UTestUI::OnPreAnalyzeAllClicked()
{
	if (Boss)
	{
		Boss->PreAnalyzeAllMusicData();
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnPreAnalyzeAllClicked: All music data pre-analyzed."));
	}
}


