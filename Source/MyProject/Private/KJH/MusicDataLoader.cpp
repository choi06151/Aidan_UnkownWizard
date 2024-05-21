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
    // JSON 파일의 내용을 문자열로 읽어옵니다.
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        return false; // 파일을 읽지 못하면 false를 반환
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

        return true; // 데이터 로드 및 파싱 성공 시 true를 반환합니다.
    }

    return false; // JSON 파싱 실패 시 false를 반환합니다.
}
