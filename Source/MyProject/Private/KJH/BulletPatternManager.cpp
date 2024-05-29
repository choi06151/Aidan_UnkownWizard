// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/BulletPatternManager.h"

void UBulletPatternManager::AnalyzeMusicData(const FMusicData& MusicData,
    TArray<FPatternConditions>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold,
    float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold)
{
    // 음악 데이터의 강도 배열 길이를 가져옴.
    // 이는 초 단위로 나뉜 음악 데이터의 길이로, 각 초마다의 패턴 조건을 평가하기 위해 사용됨.
    int32 NumSeconds = MusicData.Intensity.Num();

    // 각 초에 대해 패턴 조건을 평가함.
    for (int32 i = 0; i < NumSeconds; ++i)
    {
        // 매 초마다 새로운 패턴 조건을 초기화함.
        FPatternConditions Conditions;

        // 주파수 대역 조건 평가
        // 각 주파수 대역(low, low_mid, high_mid, high)에 대해 현재 초에 해당하는 값을 가져와 임계값과 비교하여 조건을 설정함.
        float Intensity = MusicData.Intensity.IsValidIndex(i) ? MusicData.Intensity[i] : 0.0f;
        Conditions.bIsHighIntensity = Intensity > IntensityThreshold;

        // 주파수 대역 조건 평가
        // 각 주파수 대역(low, low_mid, high_mid, high)에 대해 현재 초에 해당하는 값을 가져와 임계값과 비교하여 조건을 설정함.
        float LowFrequency = MusicData.LowFrequencyBand.IsValidIndex(i) ? MusicData.LowFrequencyBand[i] : 0.0f;
        float LowMidFrequency = MusicData.LowMidFrequencyBand.IsValidIndex(i) ? MusicData.LowMidFrequencyBand[i] : 0.0f;
        float HighMidFrequency = MusicData.HighMidFrequencyBand.IsValidIndex(i) ? MusicData.HighMidFrequencyBand[i] : 0.0f;
        float HighFrequency = MusicData.HighFrequencyBand.IsValidIndex(i) ? MusicData.HighFrequencyBand[i] : 0.0f;

        Conditions.bIsLowFrequency = LowFrequency > LowFreqThreshold;
        Conditions.bIsLowMidFrequency = LowMidFrequency > LowMidFreqThreshold;
        Conditions.bIsHighMidFrequency = HighMidFrequency > HighMidFreqThreshold;
        Conditions.bIsHighFrequency = HighFrequency > HighFreqThreshold;

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

// 특정 초의 데이터를 추출하는 함수 정의
void UBulletPatternManager::ExtractDataAtSecond(const FMusicData& MusicData, int32 Second, float& OutIntensity,
    float& OutLowFrequency, float& OutLowMidFrequency, float& OutHighMidFrequency, float& OutHighFrequency,
    TArray<float>& OutBeats)
{
    // 주어진 초(Second)가 강도 배열 범위 내에 있는지 확인하고, 해당 초의 강도 값을 반환
    if (MusicData.Intensity.IsValidIndex(Second))
    {
        OutIntensity = MusicData.Intensity[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Intensity: %f"), Second, OutIntensity);
    }
    // 주어진 초(Second)가 저주파수 대역 배열 범위 내에 있는지 확인하고, 해당 초의 저주파수 대역 값을 반환
    if (MusicData.LowFrequencyBand.IsValidIndex(Second))
    {
        OutLowFrequency = MusicData.LowFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Low Frequency: %f"), Second, OutLowFrequency);
    }
    // 주어진 초(Second)가 저중주파수 대역 배열 범위 내에 있는지 확인하고, 해당 초의 저중주파수 대역 값을 반환
    if (MusicData.LowMidFrequencyBand.IsValidIndex(Second))
    {
        OutLowMidFrequency = MusicData.LowMidFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, Low Mid Frequency: %f"), Second, OutLowMidFrequency);
    }
    // 주어진 초(Second)가 중고주파수 대역 배열 범위 내에 있는지 확인하고, 해당 초의 중고주파수 대역 값을 반환
    if (MusicData.HighMidFrequencyBand.IsValidIndex(Second))
    {
        OutHighMidFrequency = MusicData.HighMidFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, High Mid Frequency: %f"), Second, OutHighMidFrequency);
    }
    // 주어진 초(Second)가 고주파수 대역 배열 범위 내에 있는지 확인하고, 해당 초의 고주파수 대역 값을 반환
    if (MusicData.HighFrequencyBand.IsValidIndex(Second))
    {
        OutHighFrequency = MusicData.HighFrequencyBand[Second];
        //UE_LOG(LogTemp, Log, TEXT("Second: %d, High Frequency: %f"), Second, OutHighFrequency);
    }

    // 비트 발생 시기를 확인하고, 주어진 초(Second) ±0.1초 이내에 발생한 비트를 반환
    for (float BeatTime : MusicData.Beats)
    {
        if (FMath::Abs(Second - BeatTime) < 0.1f) // 비트 발생 시점을 ±0.1초 이내로 확인
        {
            OutBeats.Add(BeatTime);
            //UE_LOG(LogTemp, Log, TEXT("Second: %d, Beat Time: %f"), Second, BeatTime);
        }
    }
}