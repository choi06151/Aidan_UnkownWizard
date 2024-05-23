// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/BulletPatternManager.h"

void UBulletPatternManager::AnalyzeMusicData(const FMusicData& MusicData,
    TArray<FPatternConditions>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold,
    float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold)
{
    // 음악 데이터의 강도 배열 길이를 가져옴.
    // 이는 초 단위로 나뉜 음악 데이터의 길이로, 각 초마다의 패턴 조건을 평가하기 위해 사용됨.
    int32 NumSeconds = MusicData.NormalizedIntensity.Num();

    // 각 초에 대해 패턴 조건을 평가함.
    for (int32 i = 0; i < NumSeconds; ++i)
    {
        // 매 초마다 새로운 패턴 조건을 초기화함.
        FPatternConditions Conditions;

        // 강도 조건 평가
        // 현재 초에 해당하는 강도를 가져오고, 임계값(IntensityThreshold)과 비교하여 높은 강도 조건을 설정함.
        float Intensity = MusicData.NormalizedIntensity.IsValidIndex(i) ? MusicData.NormalizedIntensity[i] : 0.0f;
        Conditions.bIsHighIntensity = Intensity > IntensityThreshold;

        // 주파수 대역 조건 평가
        // 각 주파수 대역(low, low_mid, high_mid, high)에 대해 현재 초에 해당하는 값을 가져와 임계값과 비교하여 조건을 설정함.
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

        // 템포 조건 평가
        // 음악 데이터의 템포 값과 비교하여 조건을 설정함.
        // 조건은 상위 조건부터 평가하여 상위 조건이 충족되면 하위 조건은 평가하지 않음.
        Conditions.bIsTempoAbove110 = MusicData.Tempo > 110.0f;
        Conditions.bIsTempoAbove100 = !Conditions.bIsTempoAbove110 && MusicData.Tempo > 100.0f;
        Conditions.bIsTempoAbove90 = !Conditions.bIsTempoAbove100 && MusicData.Tempo > 90.0f;

        // 비트가 발생한 시점인지 확인
        // 현재 시간을 초 단위로 계산하고, 비트 타이밍과 비교하여 근사치로 비트가 발생한 시점인지 확인함.
        float CurrentTime = i; // 초 단위 시간
        for (float BeatTime : MusicData.Beats)
        {
            if (FMath::Abs(CurrentTime - BeatTime) < 0.1f) // 비트 발생 시점을 ±0.1초 이내로 확인
            {
                Conditions.bIsOnBeat = true;
                break;
            }
        }

        // 평가된 조건을 패턴 조건 배열에 추가함.
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
