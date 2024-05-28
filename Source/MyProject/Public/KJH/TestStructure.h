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

	// ������ ����
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	float Tempo;

	// ������ ��Ʈ Ÿ�ӽ�����
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> Beats;

	// ������ ����
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> Intensity;

	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> NormalizedIntensity;

	// ����ȭ�� ���� ��


};
