// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/BulletPatternManager.h"

void UBulletPatternManager::AnalyzeMusicData(const FMusicData& MusicData,
    TArray<FPatternConditions>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold,
    float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold)
{
    // ���� �������� ���� �迭 ���̸� ������.
    // �̴� �� ������ ���� ���� �������� ���̷�, �� �ʸ����� ���� ������ ���ϱ� ���� ����.
    int32 NumSeconds = MusicData.NormalizedIntensity.Num();

    // �� �ʿ� ���� ���� ������ ����.
    for (int32 i = 0; i < NumSeconds; ++i)
    {
        // �� �ʸ��� ���ο� ���� ������ �ʱ�ȭ��.
        FPatternConditions Conditions;

        // ���� ���� ��
        // ���� �ʿ� �ش��ϴ� ������ ��������, �Ӱ谪(IntensityThreshold)�� ���Ͽ� ���� ���� ������ ������.
        float Intensity = MusicData.NormalizedIntensity.IsValidIndex(i) ? MusicData.NormalizedIntensity[i] : 0.0f;
        Conditions.bIsHighIntensity = Intensity > IntensityThreshold;

        // ���ļ� �뿪 ���� ��
        // �� ���ļ� �뿪(low, low_mid, high_mid, high)�� ���� ���� �ʿ� �ش��ϴ� ���� ������ �Ӱ谪�� ���Ͽ� ������ ������.
        for (const auto& Elem : MusicData.FrequencyBands)
        {
            const FString& BandName = Elem.Key;
            const TArray<float>& BandValues = Elem.Value.Values;

            float BandValue = BandValues.IsValidIndex(i) ? BandValues[i] : 0.0f;
            if (BandName == "low")
            {
                Conditions.bIsLowFrequency = BandValue > LowFreqThreshold;
            }
            else if (BandName == "low_mid")
            {
                Conditions.bIsLowMidFrequency = BandValue > LowMidFreqThreshold;
            }
            else if (BandName == "high_mid")
            {
                Conditions.bIsHighMidFrequency = BandValue > HighMidFreqThreshold;
            }
            else if (BandName == "high")
            {
                Conditions.bIsHighFrequency = BandValue > HighFreqThreshold;
            }
        }

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

void UBulletPatternManager::CreatePatternsFromConditions(const TArray<FPatternConditions>& PatternConditions,
	TArray<FBulletHellPattern>& OutBulletPatterns)
{
    for (const FPatternConditions& Condition : PatternConditions)
    {
        FBulletHellPattern Pattern;

        if (Condition.bIsHighIntensity)
        {
            Pattern.PatternType = EPatternType::Circle;
            Pattern.NumberOfBullets = 20;
            Pattern.PatternSize = 500.0f;
        }
        else if (Condition.bIsLowFrequency)
        {
            Pattern.PatternType = EPatternType::Straight;
            Pattern.NumberOfBullets = 5;
            Pattern.Interval = 1.0f;
        }
        else if (Condition.bIsLowMidFrequency)
        {
            Pattern.PatternType = EPatternType::Fan;
            Pattern.FanAngle = 90.0f;
            Pattern.NumberOfBullets = 15;
        }
        else if (Condition.bIsHighMidFrequency)
        {
            Pattern.PatternType = EPatternType::Wave;
            Pattern.Amplitude = 150.0f;
            Pattern.Frequency = 1.0f;
        }
        else if (Condition.bIsHighFrequency)
        {
            Pattern.PatternType = EPatternType::Spread;
            Pattern.NumberOfBullets = 10;
        }
        else if (Condition.bIsOnBeat)
        {
            Pattern.PatternType = EPatternType::RandomSpread;
            Pattern.NumberOfBullets = 20;
        }
        else if (Condition.bIsTempoAbove110)
        {
            Pattern.PatternType = EPatternType::Circle;
            Pattern.NumberOfBullets = 30;
            Pattern.PatternSize = 600.0f;
        }
        else if (Condition.bIsTempoAbove100)
        {
            Pattern.PatternType = EPatternType::Fan;
            Pattern.FanAngle = 120.0f;
            Pattern.NumberOfBullets = 25;
        }
        else if (Condition.bIsTempoAbove90)
        {
            Pattern.PatternType = EPatternType::Wave;
            Pattern.Amplitude = 200.0f;
            Pattern.Frequency = 1.5f;
        }

        OutBulletPatterns.Add(Pattern);
    }
}
