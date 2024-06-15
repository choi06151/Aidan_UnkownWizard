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
		FString MusicTitle = TEXT("Requiem ? Lacrimosa");
		FString MusicFilePath = TEXT("/Game/Music/Lacrimosa.Lacrimosa");
		Boss->LoadMusicDataAndSetPatterns(MusicTitle, MusicFilePath);
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnMusic3Clicked"));
	}
}

void UTestUI::OnShow5SecInfoClicked()
{
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + TEXT("Content/Data/butterfly.json");
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::OnShow5SecInfoClicked: Loading JSON from: %s"), *JsonFilePath);

	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// 5초 시점의 정보를 로그에 출력
			float Tempo = JsonObject->GetNumberField(TEXT("tempo"));
			const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField(TEXT("beats"));
			const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField(TEXT("intensity"));
			const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField(TEXT("low"));
			const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField(TEXT("low_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField(TEXT("high_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField(TEXT("high"));

			float Intensity = IntensityArray[15]->AsNumber();
			float LowFrequency = LowArray[15]->AsNumber();
			float LowMidFrequency = LowMidArray[15]->AsNumber();
			float HighMidFrequency = HighMidArray[15]->AsNumber();
			float HighFrequency = HighArray[15]->AsNumber();

			UE_LOG(LogTemp, Warning, TEXT("Tempo: %f"), Tempo);
			for (const auto& BeatValue : BeatsArray)
			{
				float BeatTime = BeatValue->AsNumber();
				if (BeatTime <= 15.0f)
				{
					UE_LOG(LogTemp, Warning, TEXT("Beat at: %f"), BeatTime);
				}
				else
				{
					break;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Intensity at 5 seconds: %f"), Intensity);
			UE_LOG(LogTemp, Warning, TEXT("Low frequency band at 5 seconds: %f"), LowFrequency);
			UE_LOG(LogTemp, Warning, TEXT("Low-mid frequency band at 5 seconds: %f"), LowMidFrequency);
			UE_LOG(LogTemp, Warning, TEXT("High-mid frequency band at 5 seconds: %f"), HighMidFrequency);
			UE_LOG(LogTemp, Warning, TEXT("High frequency band at 5 seconds: %f"), HighFrequency);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file: %s"), *JsonFilePath);
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

void UTestUI::PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Loading JSON from: %s"), *JsonFilePath);
	if (Boss)
	{
		//Boss->LoadMusicDataAndSetPatterns(JsonFilePath, MusicFilePath); // JSON 파일, Music 경로 전달
	}

	else
	{
		UE_LOG(LogTemp, Error, TEXT("Boss is not valid!"));
	}
}
