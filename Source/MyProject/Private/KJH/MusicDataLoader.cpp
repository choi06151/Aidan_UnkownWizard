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
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to load file to string������ ���ڿ��� �ε��ϴ� �� �����߽��ϴ�: %s"), *FilePath);
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

        NormalizeIntensity(OutMusicData);
        NormalizeFrequencyBands(OutMusicData);
        UE_LOG(LogTemp, Warning, TEXT("UMusicDataLoader::LoadMusicDataFromFile: JSON ������ ������ȭ ����"));
        return true;
    }

    else
    {
        UE_LOG(LogTemp, Error, TEXT("UMusicDataLoader::LoadMusicDataFromFile: Failed to deserialize JSON data JSON ������ ������ȭ ����: %s"), *FilePath);
        return false;
    }
}

void UMusicDataLoader::NormalizeIntensity(FMusicData& MusicData)
{
    // ���� �� �迭�� ����ִ��� Ȯ��
    if (MusicData.Intensity.Num() == 0)
    {
        return; // �迭�� ����ִٸ� �Լ� ����
    }

    // ���� �� �迭�� ù ��° ���� �ּҰ� �� �ִ밪���� �ʱ�ȭ
    float MinIntensity = MusicData.Intensity[0];
    float MaxIntensity = MusicData.Intensity[0];

    // ���� �� �迭�� ��ȸ�Ͽ� �ּҰ� �� �ִ밪 ã��
    for (float Intensity : MusicData.Intensity)
    {
        if (Intensity < MinIntensity)
        {
            MinIntensity = Intensity; // �ּҰ� ������Ʈ
        }
        if (Intensity > MaxIntensity)
        {
            MaxIntensity = Intensity; // �ִ밪 ������Ʈ
        }
    }

    // �� ���� ���� 0�� 1 ���̷� ����ȭ
    for (float Intensity : MusicData.Intensity)
    {
        // ����ȭ�� �� ���: (���� �� - �ּҰ�) / (�ִ밪 - �ּҰ�)
        float Normalized = (Intensity - MinIntensity) / (MaxIntensity - MinIntensity);

        // ����ȭ�� ���� NormalizedIntensity �迭�� �߰�
        MusicData.NormalizedIntensity.Add(Normalized);
    }
}

void UMusicDataLoader::NormalizeFrequencyBands(FMusicData& MusicData)
{
    // ���ļ� �뿪 �����͸� ��ȸ
    for (auto& Elem : MusicData.FrequencyBands)
    {
        // ���ļ� �뿪�� ���� �����´�
        TArray<float>& BandValues = Elem.Value.Values;

        // ���ļ� �뿪 ���� ����ִ� ��� ���� �뿪����
        if (BandValues.Num() == 0)
        {
            continue;
        }

        // ���ļ� �뿪�� ù ��° ���� �ּҰ� �� �ִ밪���� �ʱ�ȭ
        float MinValue = BandValues[0];
        float MaxValue = BandValues[0];

        // ���ļ� �뿪�� �ּҰ� �� �ִ밪�� ã��
        for (float Value : BandValues)
        {
            if (Value < MinValue)
            {
                MinValue = Value;  // �ּҰ� ������Ʈ
            }
            if (Value > MaxValue)
            {
                MaxValue = Value;  // �ִ밪 ������Ʈ
            }
        }

        // �� ���ļ� �뿪 ���� 0�� 1 ���̷� ����ȭ
        for (float& Value : BandValues)
        {
            // ����ȭ�� �� ���: (���� �� - �ּҰ�) / (�ִ밪 - �ּҰ�)
            float Normalized = (Value - MinValue) / (MaxValue - MinValue);

            // ����ȭ�� ������ ��ü
            Value = Normalized;
        }
    }
}
