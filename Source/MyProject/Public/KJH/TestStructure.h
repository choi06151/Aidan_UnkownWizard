// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrequencyBand.h"
#include "TestStructure.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)

struct MYPROJECT_API FTestStructure : public FTableRowBase
{
	GENERATED_BODY()

	// 음악의 템포
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	float Tempo;

	// 음악의 비트 타임스탬프
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> Beats;

	// 음악의 강도
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> Intensity;

	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> NormalizedIntensity;

	// 정규화된 강도 값


};
