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
		Btn_Music1->OnClicked.AddDynamic(this, &UTestUI::OnMusic1Clicked);
	}
	if (Btn_Music2)
	{
		Btn_Music2->OnClicked.AddDynamic(this, &UTestUI::OnMusic2Clicked);
	}
	if (Btn_Music3)
	{
		Btn_Music3->OnClicked.AddDynamic(this, &UTestUI::OnMusic3Clicked);
	}

	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
}

void UTestUI::OnMusic1Clicked()
{
	FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/butterfly.butterfly'");
	// JsonFilePath를 절대 경로로 변환
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/butterfly.json");
	PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic1111Clicked"));
}

void UTestUI::OnMusic2Clicked()
{
	FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/Elise.Elise'");
	// JsonFilePath를 절대 경로로 변환
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/Elise.json");
	PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic22222Clicked"));
}

void UTestUI::OnMusic3Clicked()
{
	FString MusicFilePath = TEXT("/Script/Engine.SoundWave'/Game/Music/Lacrimosa.Lacrimosa'");
	// JsonFilePath를 절대 경로로 변환
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/Lacrimosa.json");
	PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic3333Clicked"));
}

void UTestUI::PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Loading JSON from: %s"), *JsonFilePath);
	if (Boss)
	{
		//Boss->LoadMusicDataAndSetPatterns(JsonFilePath); // JSON 파일 경로 전달
	}

	// 음악 재생
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Playing music from: %s"), *MusicFilePath);
	USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath));
	if (Music)
	{
		UGameplayStatics::PlaySound2D(this, Music);
		//GetWorld()->GetTimerManager().SetTimer(Boss->PatternUpdateTimerHandle, Boss, &ABoss::UpdatePatternConditions, 1.0f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Failed to load music: %s"), *MusicFilePath);
	}
}