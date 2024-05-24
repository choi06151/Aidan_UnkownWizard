// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/TestUI.h"

#include "Kismet/GameplayStatics.h"
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
	PlayMusicAndLoadData(TEXT("Content/Music/butterfly.mp3"), TEXT("Content/Data/butterfly.json"));
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic1111Clicked"));
}

void UTestUI::OnMusic2Clicked()
{
	PlayMusicAndLoadData(TEXT("Content/Music/Lacrimosa.mp3"), TEXT("Content/Data/Lacrimosa.json"));
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic22222Clicked"));
}

void UTestUI::OnMusic3Clicked()
{
	PlayMusicAndLoadData(TEXT("Content/Music/Symphony_25.mp3"), TEXT("Content/Data/Symphony25.json"));
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic3333Clicked"));
}

void UTestUI::PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT(" UTestUI::PlayMusicAndLoadData: JSON 로드 중: %s"), *JsonFilePath);
	if (Boss)
	{
		Boss->LoadMusicDataAndSetPatterns(JsonFilePath); // JSON 파일 경로 전달
	}

	// 음악 재생
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: 음악 재생 중: %s"), *MusicFilePath);
	USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath));
	if (Music)
	{
		UGameplayStatics::PlaySound2D(this, Music);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: 음악 로드 실패: %s"), *MusicFilePath);
	}
}