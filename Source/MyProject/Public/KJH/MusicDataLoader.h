// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FrequencyBand.h"
#include "UObject/NoExportTypes.h"
#include "MusicDataLoader.generated.h"

// 음악 데이터 (템포, 비트, 강도, 주파수 대역)를 저장하는 구조체
USTRUCT(BlueprintType)
struct FMusicData
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

	// 정규화된 강도 값
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TArray<float> NormalizedIntensity;

	// 주파수 대역의 값
	UPROPERTY(BlueprintReadWrite, Category = "Music Data")
	TMap<FString, FFrequencyBand> FrequencyBands;

	FMusicData()
	{
		Tempo = 0.0f; // 기본값으로 템포를 0.0으로 초기화
	}
};

UCLASS()
class MYPROJECT_API UMusicDataLoader : public UObject
{
	GENERATED_BODY()


public:
	// JSON 파일에서 음악 데이터를 로드하는 함수
	// @param FilePath - 로드할 JSON 파일의 경로입니다.
	// @param OutMusicData - 로드된 음악 데이터가 저장될 구조체입니다.
	//@return - 파일 로드 및 파싱 성공 여부를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Music Data")
	static bool LoadMusicDataFromFile(const FString& FilePath, FMusicData& OutMusicData);

private:
	// 강도 값을 정규화하는 함수
	static void NormalizeIntensity(FMusicData& MusicData);

	// 주파수 대역 값을 정규화하는 함수
	static void NormalizeFrequencyBands(FMusicData& MusicData);
};
