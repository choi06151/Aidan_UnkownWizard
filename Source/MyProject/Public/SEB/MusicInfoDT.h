// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "MusicInfoDT.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMusicInfoDT : public FTableRowBase
{
	GENERATED_BODY()

	//가수 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ArtistName;
	//노래 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MusicName;
	//난이도
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Difficulty;
	//썸네일
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Thumbnail;
	//최고 점수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BestScore;
	//음악
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MusicFilePath;
	//json
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString JsonFilePath;
};
