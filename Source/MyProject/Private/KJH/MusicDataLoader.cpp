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
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to load file to string파일을 문자열로 로드하는 데 실패했습니다: %s"), *FilePath);
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

        // "frequency_bands" 객체를 파싱하여 OutMusicData의 FrequencyBands 맵에 저장
        const TSharedPtr<FJsonObject> FrequencyBandsObject = JsonObject->GetObjectField("frequency_bands");
        for (const auto& Elem : FrequencyBandsObject->Values)
        {
            TArray<TSharedPtr<FJsonValue>> BandArray = Elem.Value->AsArray(); // 주파수 대역 배열을 가져옴
            FFrequencyBand BandValues;
            for (const TSharedPtr<FJsonValue>& BandValue : BandArray)
            {
                BandValues.Values.Add(BandValue->AsNumber()); // 각 주파수 대역 값을 배열에 추가
            }
            OutMusicData.FrequencyBands.Add(Elem.Key, BandValues); // 주파수 대역 데이터를 맵에 추가
        }

        NormalizeIntensity(OutMusicData);
        NormalizeFrequencyBands(OutMusicData);
        UE_LOG(LogTemp, Warning, TEXT("UMusicDataLoader::LoadMusicDataFromFile: JSON 데이터 역직렬화 성공"));
        return true;
    }

    else
    {
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to deserialize JSON data JSON 데이터 역직렬화 실패: %s"), *FilePath);
        return false;
    }
}

void UMusicDataLoader::NormalizeIntensity(FMusicData& MusicData)
{
    // 강도 값 배열이 비어있는지 확인
    if (MusicData.Intensity.Num() == 0)
    {
        return; // 배열이 비어있다면 함수 종료
    }

    // 강도 값 배열의 첫 번째 값을 최소값 및 최대값으로 초기화
    float MinIntensity = MusicData.Intensity[0];
    float MaxIntensity = MusicData.Intensity[0];

    // 강도 값 배열을 순회하여 최소값 및 최대값 찾기
    for (float Intensity : MusicData.Intensity)
    {
        if (Intensity < MinIntensity)
        {
            MinIntensity = Intensity; // 최소값 업데이트
        }
        if (Intensity > MaxIntensity)
        {
            MaxIntensity = Intensity; // 최대값 업데이트
        }
    }

    // 각 강도 값을 0과 1 사이로 정규화
    for (float Intensity : MusicData.Intensity)
    {
        // 정규화된 값 계산: (현재 값 - 최소값) / (최대값 - 최소값)
        float Normalized = (Intensity - MinIntensity) / (MaxIntensity - MinIntensity);

        // 정규화된 값을 NormalizedIntensity 배열에 추가
        MusicData.NormalizedIntensity.Add(Normalized);
    }
}

void UMusicDataLoader::NormalizeFrequencyBands(FMusicData& MusicData)
{
    // 주파수 대역 데이터를 순회
    for (auto& Elem : MusicData.FrequencyBands)
    {
        // 주파수 대역의 값을 가져온다
        TArray<float>& BandValues = Elem.Value.Values;

        // 주파수 대역 값이 비어있는 경우 다음 대역으로
        if (BandValues.Num() == 0)
        {
            continue;
        }

        // 주파수 대역의 첫 번째 값을 최소값 및 최대값으로 초기화
        float MinValue = BandValues[0];
        float MaxValue = BandValues[0];

        // 주파수 대역의 최소값 및 최대값을 찾기
        for (float Value : BandValues)
        {
            if (Value < MinValue)
            {
                MinValue = Value;  // 최소값 업데이트
            }
            if (Value > MaxValue)
            {
                MaxValue = Value;  // 최대값 업데이트
            }
        }

        // 각 주파수 대역 값을 0과 1 사이로 정규화
        for (float& Value : BandValues)
        {
            // 정규화된 값 계산: (현재 값 - 최소값) / (최대값 - 최소값)
            float Normalized = (Value - MinValue) / (MaxValue - MinValue);

            // 정규화된 값으로 대체
            Value = Normalized;
        }
    }
}
