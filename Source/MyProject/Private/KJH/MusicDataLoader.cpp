// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/MusicDataLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"
#include "JsonUtilities.h"

// 주어진 파일 경로에서 JSON 형식의 음악 데이터를 읽어와 FMusicData 구조체에 저장
bool UMusicDataLoader::LoadMusicDataFromFile(const FString& FilePath, FMusicData& OutMusicData)
{
    FString JsonString;
    // 절대 경로 사용
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to load file to string: %s"), *FilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject; // JSON 객체를 담을 포인터 선언
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString); // JSON 문자열을 읽기 위한 리더 생성

    // "tempo" 필드의 값을 읽어와서 OutMusicData의 Tempo에 저장
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // 템포 값을 파싱하여 저장
        OutMusicData.Tempo = JsonObject->GetNumberField("tempo");

        // "beats" 배열을 파싱하여 OutMusicData의 Beats 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField("beats");
        for (const TSharedPtr<FJsonValue>& Value : BeatsArray)
        {
            OutMusicData.Beats.Add(Value->AsNumber()); // 각 비트 값을 배열에 추가
        }

        // "intensity" 배열을 파싱하여 OutMusicData의 Intensity 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField("intensity");
        for (const TSharedPtr<FJsonValue>& Value : IntensityArray)
        {
            OutMusicData.Intensity.Add(Value->AsNumber()); // 각 강도 값을 배열에 추가
        }

        // "low" 배열을 파싱하여 OutMusicData의 LowFrequencyBand 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField("low");
        for (const TSharedPtr<FJsonValue>& Value : LowArray)
        {
            OutMusicData.LowFrequencyBand.Add(Value->AsNumber()); // 각 저주파수 대역 값을 배열에 추가
        }

        // "low_mid" 배열을 파싱하여 OutMusicData의 LowMidFrequencyBand 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField("low_mid");
        for (const TSharedPtr<FJsonValue>& Value : LowMidArray)
        {
            OutMusicData.LowMidFrequencyBand.Add(Value->AsNumber()); // 각 저중주파수 대역 값을 배열에 추가
        }

        // "high_mid" 배열을 파싱하여 OutMusicData의 HighMidFrequencyBand 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField("high_mid");
        for (const TSharedPtr<FJsonValue>& Value : HighMidArray)
        {
            OutMusicData.HighMidFrequencyBand.Add(Value->AsNumber()); // 각 중고주파수 대역 값을 배열에 추가
        }

        // "high" 배열을 파싱하여 OutMusicData의 HighFrequencyBand 배열에 저장
        const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField("high");
        for (const TSharedPtr<FJsonValue>& Value : HighArray)
        {
            OutMusicData.HighFrequencyBand.Add(Value->AsNumber()); // 각 고주파수 대역 값을 배열에 추가
        }

        UE_LOG(LogTemp, Warning, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Successfully deserialized JSON data."));
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to deserialize JSON data: %s"), *FilePath);
        return false;
    }
}


