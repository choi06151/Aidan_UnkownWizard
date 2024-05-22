// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KJH/MusicDataLoader.h"
#include "JWK/BulletHellPattern.h"
#include "BulletPatternManager.generated.h"

// ���� ������ �����ϴ� ����ü
USTRUCT(BlueprintType)
struct FPatternConditions
{
    GENERATED_BODY()

    // ���� ������ �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighIntensity;

    // ���� ���ļ� �뿪�� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsLowFrequency;

    // ���� �߰� ���ļ� �뿪�� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsLowMidFrequency;

    // ���� �߰� ���ļ� �뿪�� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighMidFrequency;

    // ���� ���ļ� �뿪�� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsHighFrequency;

    // ��Ʈ�� �߻����� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsOnBeat;

    // ������ 90 �̻��� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove90;

    // ������ 100 �̻��� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove100;

    // ������ 110 �̻��� �� ��
    UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
    bool bIsTempoAbove110;

    // �⺻ ������
    FPatternConditions()
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
    // ���� �����͸� ������� ���� ������ �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Bullet Pattern")
    static void AnalyzeMusicData(const FMusicData& MusicData, TArray<FPatternConditions>& OutPatternConditions, float IntensityThreshold, float LowFreqThreshold, float LowMidFreqThreshold, float HighMidFreqThreshold, float HighFreqThreshold);
};
