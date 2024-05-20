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
    // JSON ������ ������ ���ڿ��� �о�ɴϴ�.
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        return false; // ������ ���� ���ϸ� false�� ��ȯ
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

        // "frequency_bands" ��ü�� �Ľ��Ͽ� OutMusicData�� FrequencyBands �ʿ� ����
        const TSharedPtr<FJsonObject> FrequencyBandsObject = JsonObject->GetObjectField("frequency_bands");
        for (const auto& Elem : FrequencyBandsObject->Values)
        {
            TArray<TSharedPtr<FJsonValue>> BandArray = Elem.Value->AsArray(); // ���ļ� �뿪 �迭�� ������
            FFrequencyBand BandValues;
            for (const TSharedPtr<FJsonValue>& BandValue : BandArray)
            {
                BandValues.Values.Add(BandValue->AsNumber()); // �� ���ļ� �뿪 ���� �迭�� �߰�
            }
            OutMusicData.FrequencyBands.Add(Elem.Key, BandValues); // ���ļ� �뿪 �����͸� �ʿ� �߰�
        }

        return true; // ������ �ε� �� �Ľ� ���� �� true�� ��ȯ�մϴ�.
    }

    return false; // JSON �Ľ� ���� �� false�� ��ȯ�մϴ�.
}
