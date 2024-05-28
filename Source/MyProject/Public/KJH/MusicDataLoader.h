// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrequencyBand.h"
#include "UObject/NoExportTypes.h"
#include "MusicDataLoader.generated.h"

// ���� ������ (����, ��Ʈ, ����, ���ļ� �뿪)�� �����ϴ� ����ü
USTRUCT(BlueprintType)
struct FMusicData
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

	// ����ȭ�� ���� ��
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> NormalizedIntensity;

	// ���ļ� �뿪�� ��
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TMap<FString, FFrequencyBand> FrequencyBands;

	FMusicData()
	{
		Tempo = 0.0f; // �⺻������ ������ 0.0���� �ʱ�ȭ
	}
};

UCLASS()
class MYPROJECT_API UMusicDataLoader : public UObject
{
	GENERATED_BODY()


public:
	// JSON ���Ͽ��� ���� �����͸� �ε��ϴ� �Լ�
	// @param FilePath - �ε��� JSON ������ ����Դϴ�.
	// @param OutMusicData - �ε�� ���� �����Ͱ� ����� ����ü�Դϴ�.
	//@return - ���� �ε� �� �Ľ� ���� ���θ� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable, Category = "Music Data")
	static bool LoadMusicDataFromFile(const FString& FilePath, FMusicData& OutMusicData);

private:
	// ���� ���� ����ȭ�ϴ� �Լ�
	static void NormalizeIntensity(FMusicData& MusicData);

	// ���ļ� �뿪 ���� ����ȭ�ϴ� �Լ�
	static void NormalizeFrequencyBands(FMusicData& MusicData);
};
