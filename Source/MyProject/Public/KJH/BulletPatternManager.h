// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KJH/MusicDataLoader.h"
#include "JWK/BulletHellPattern.h"
#include "BulletPatternManager.generated.h"

// 패턴 조건을 정의하는 구조체
USTRUCT(BlueprintType)
struct FPatternConditions22
{
    GENERATED_BODY()

    // 높은 강도일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighIntensity;

    // 낮은 주파수 대역일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsLowFrequency;

    // 낮은 중간 주파수 대역일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsLowMidFrequency;

    // 높은 중간 주파수 대역일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighMidFrequency;

    // 높은 주파수 대역일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighFrequency;

    // 비트가 발생했을 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsOnBeat;

    // 템포가 90 이상일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove90;

    // 템포가 100 이상일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove100;

    // 템포가 110 이상일 때 참
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove110;

    // 기본 생성자
    FPatternConditions22()
        : bIsHighIntensity(false)
        , bIsLowFrequency(false)
        , bIsLowMidFrequency(false)
        , bIsHighMidFrequency(false)
        , bIsHighFrequency(false)
        , bIsOnBeat(false)
        , bIsTempoAbove90(false)
        , bIsTempoAbove100(false)
        , bIsTempoAbove110(false)
    {
    }
};

UCLASS(Blueprintable)
class MYPROJECT_API UBulletPatternManager : public UObject
{
    GENERATED_BODY()

public:
    // 음악 데이터를 기반으로 패턴 조건을 생성하는 함수
    UFUNCTION(BlueprintCallable, Category = "Bullet Pattern")
    static void AnalyzeMusicData(const FMusicData& MusicData, TArray<FPatternConditions22>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold, float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold);

    void ExtractDataAtSecond(const FMusicData& MusicData, int32 Second, float& OutIntensity, float& OutLowFrequency, float& OutLowMidFrequency, float& OutHighMidFrequency, float& OutHighFrequency, TArray<float>& OutBeats);
};