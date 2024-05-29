// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/BulletPatternManager.h"

void UBulletPatternManager::AnalyzeMusicData(const FMusicData& MusicData,
    TArray<FPatternConditions>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold,
    float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold)
{
    // ���� �������� ���� �迭 ���̸� ������.
    // �̴� �� ������ ���� ���� �������� ���̷�, �� �ʸ����� ���� ������ ���ϱ� ���� ����.
    int32 NumSeconds = MusicData.Intensity.Num();

    // �� �ʿ� ���� ���� ������ ����.
    for (int32 i = 0; i < NumSeconds; ++i)
    {
        // �� �ʸ��� ���ο� ���� ������ �ʱ�ȭ��.
        FPatternConditions Conditions;

        // ���ļ� �뿪 ���� ��
        // �� ���ļ� �뿪(low, low_mid, high_mid, high)�� ���� ���� �ʿ� �ش��ϴ� ���� ������ �Ӱ谪�� ���Ͽ� ������ ������.
        float Intensity = MusicData.Intensity.IsValidIndex(i) ? MusicData.Intensity[i] : 0.0f;
        Conditions.bIsHighIntensity = Intensity > IntensityThreshold;

        // ���ļ� �뿪 ���� ��
        // �� ���ļ� �뿪(low, low_mid, high_mid, high)�� ���� ���� �ʿ� �ش��ϴ� ���� ������ �Ӱ谪�� ���Ͽ� ������ ������.
        float LowFrequency = MusicData.LowFrequencyBand.IsValidIndex(i) ? MusicData.LowFrequencyBand[i] : 0.0f;
        float LowMidFrequency = MusicData.LowMidFrequencyBand.IsValidIndex(i) ? MusicData.LowMidFrequencyBand[i] : 0.0f;
        float HighMidFrequency = MusicData.HighMidFrequencyBand.IsValidIndex(i) ? MusicData.HighMidFrequencyBand[i] : 0.0f;
        float HighFrequency = MusicData.HighFrequencyBand.IsValidIndex(i) ? MusicData.HighFrequencyBand[i] : 0.0f;

        Conditions.bIsLowFrequency = LowFrequency > LowFreqThreshold;
        Conditions.bIsLowMidFrequency = LowMidFrequency > LowMidFreqThreshold;
        Conditions.bIsHighMidFrequency = HighMidFrequency > HighMidFreqThreshold;
        Conditions.bIsHighFrequency = HighFrequency > HighFreqThreshold;

        // ���� ���� �� 
        // ���� �������� ���� ���� ���Ͽ� ������ ������.
        // ������ ���� ���Ǻ��� ���Ͽ� ���� ������ �����Ǹ� ���� ������ ������ ����.
        Conditions.bIsTempoAbove110 = MusicData.Tempo > 110.0f;
        Conditions.bIsTempoAbove100 = !Conditions.bIsTempoAbove110 && MusicData.Tempo > 100.0f;
        Conditions.bIsTempoAbove90 = !Conditions.bIsTempoAbove100 && MusicData.Tempo > 90.0f;

        // ��Ʈ�� �߻��� �������� Ȯ��
        // ���� �ð��� �� ������ ����ϰ�, ��Ʈ Ÿ�ְ̹� ���Ͽ� �ٻ�ġ�� ��Ʈ�� �߻��� �������� Ȯ����.
        float CurrentTime = i; // �� ���� �ð�
        for (float BeatTime : MusicData.Beats)
        {
            if (FMath::Abs(CurrentTime - BeatTime) < 0.1f) // ��Ʈ �߻� ������ ��0.1�� �̳��� Ȯ��
            {
                Conditions.bIsOnBeat = true;
                break;
            }
        }

        // �򰡵� ������ ���� ���� �迭�� �߰���.
        OutPatternConditions.Add(Conditions);
    }
}

// Ư�� ���� �����͸� �����ϴ� �Լ� ����
void UBulletPatternManager::ExtractDataAtSecond(const FMusicData& MusicData, int32 Second, float& OutIntensity,
    float& OutLowFrequency, float& OutLowMidFrequency, float& OutHighMidFrequency, float& OutHighFrequency,
    TArray<float>& OutBeats)
{
    // �־��� ��(Second)�� ���� �迭 ���� ���� �ִ��� Ȯ���ϰ�, �ش� ���� ���� ���� ��ȯ
    if (MusicData.Intensity.IsValidIndex(Second))
    {
        OutIntensity = MusicData.Intensity[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Intensity: %f"), Second, OutIntensity);
    }
    // �־��� ��(Second)�� �����ļ� �뿪 �迭 ���� ���� �ִ��� Ȯ���ϰ�, �ش� ���� �����ļ� �뿪 ���� ��ȯ
    if (MusicData.LowFrequencyBand.IsValidIndex(Second))
    {
        OutLowFrequency = MusicData.LowFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Low Frequency: %f"), Second, OutLowFrequency);
    }
    // �־��� ��(Second)�� �������ļ� �뿪 �迭 ���� ���� �ִ��� Ȯ���ϰ�, �ش� ���� �������ļ� �뿪 ���� ��ȯ
    if (MusicData.LowMidFrequencyBand.IsValidIndex(Second))
    {
        OutLowMidFrequency = MusicData.LowMidFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Low Mid Frequency: %f"), Second, OutLowMidFrequency);
    }
    // �־��� ��(Second)�� �߰����ļ� �뿪 �迭 ���� ���� �ִ��� Ȯ���ϰ�, �ش� ���� �߰����ļ� �뿪 ���� ��ȯ
    if (MusicData.HighMidFrequencyBand.IsValidIndex(Second))
    {
        OutHighMidFrequency = MusicData.HighMidFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, High Mid Frequency: %f"), Second, OutHighMidFrequency);
    }
    // �־��� ��(Second)�� �����ļ� �뿪 �迭 ���� ���� �ִ��� Ȯ���ϰ�, �ش� ���� �����ļ� �뿪 ���� ��ȯ
    if (MusicData.HighFrequencyBand.IsValidIndex(Second))
    {
        OutHighFrequency = MusicData.HighFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, High Frequency: %f"), Second, OutHighFrequency);
    }

    // ��Ʈ �߻� �ñ⸦ Ȯ���ϰ�, �־��� ��(Second) ��0.1�� �̳��� �߻��� ��Ʈ�� ��ȯ
    for (float BeatTime : MusicData.Beats)
    {
        if (FMath::Abs(Second - BeatTime) < 0.1f) // ��Ʈ �߻� ������ ��0.1�� �̳��� Ȯ��
        {
            OutBeats.Add(BeatTime);
            //UE_LOG(LogTemp, Log, TEXT("Second: %d, Beat Time: %f"), Second, BeatTime);
        }
    }
}