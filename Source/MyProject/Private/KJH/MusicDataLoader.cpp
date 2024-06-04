// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/MusicDataLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Json.h"
#include "JsonUtilities.h"

// �־��� ���� ��ο��� JSON ������ ���� �����͸� �о�� FMusicData ����ü�� ����
bool UMusicDataLoader::LoadMusicDataFromFile(const FString& FilePath, FMusicData& OutMusicData)
{
    FString JsonString;
    // ���� ��� ���
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to load file to string: %s"), *FilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject; // JSON ��ü�� ���� ������ ����
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString); // JSON ���ڿ��� �б� ���� ���� ����

    // "tempo" �ʵ��� ���� �о�ͼ� OutMusicData�� Tempo�� ����
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // ���� ���� �Ľ��Ͽ� ����
        OutMusicData.Tempo = JsonObject->GetNumberField("tempo");

        // "beats" �迭�� �Ľ��Ͽ� OutMusicData�� Beats �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField("beats");
        for (const TSharedPtr<FJsonValue>& Value : BeatsArray)
        {
            OutMusicData.Beats.Add(Value->AsNumber()); // �� ��Ʈ ���� �迭�� �߰�
        }

        // "intensity" �迭�� �Ľ��Ͽ� OutMusicData�� Intensity �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField("intensity");
        for (const TSharedPtr<FJsonValue>& Value : IntensityArray)
        {
            OutMusicData.Intensity.Add(Value->AsNumber()); // �� ���� ���� �迭�� �߰�
        }

        // "low" �迭�� �Ľ��Ͽ� OutMusicData�� LowFrequencyBand �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField("low");
        for (const TSharedPtr<FJsonValue>& Value : LowArray)
        {
            OutMusicData.LowFrequencyBand.Add(Value->AsNumber()); // �� �����ļ� �뿪 ���� �迭�� �߰�
        }

        // "low_mid" �迭�� �Ľ��Ͽ� OutMusicData�� LowMidFrequencyBand �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField("low_mid");
        for (const TSharedPtr<FJsonValue>& Value : LowMidArray)
        {
            OutMusicData.LowMidFrequencyBand.Add(Value->AsNumber()); // �� �������ļ� �뿪 ���� �迭�� �߰�
        }

        // "high_mid" �迭�� �Ľ��Ͽ� OutMusicData�� HighMidFrequencyBand �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField("high_mid");
        for (const TSharedPtr<FJsonValue>& Value : HighMidArray)
        {
            OutMusicData.HighMidFrequencyBand.Add(Value->AsNumber()); // �� �߰����ļ� �뿪 ���� �迭�� �߰�
        }

        // "high" �迭�� �Ľ��Ͽ� OutMusicData�� HighFrequencyBand �迭�� ����
        const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField("high");
        for (const TSharedPtr<FJsonValue>& Value : HighArray)
        {
            OutMusicData.HighFrequencyBand.Add(Value->AsNumber()); // �� �����ļ� �뿪 ���� �迭�� �߰�
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


