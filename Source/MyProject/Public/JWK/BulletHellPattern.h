#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	// 직선
	Straight UMETA(DisplayName = "Straight"),
	// 파형
	Wave UMETA(DisplayName = "Wave"),
	// 부채꼴
	Fan UMETA(DisplayName = "Fan"),
	// 원
	Circle UMETA(DisplayName = "Circle"),
	// 렌덤 스프레드
	RandomSpread UMETA(DisplayName = "Spread"),
	// 팔각형
	Octagon UMETA(DisplayName = "Octagon"),
	// 별
	Star UMETA(DisplayName = "Star")
};

USTRUCT(BlueprintType)
struct FBulletHellPattern
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	EPatternType PatternType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float Interval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternSize;

	// 직선 탄막 특성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == EPatternType::Straight"))
	int32 NumberOfBullets;

	// 웨이브 탄막 특성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == EPatternType::Wave"))
	float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == EPatternType::Wave"))
	float Frequency;

	// 부채꼴 탄막 특성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == EPatternType::Fan"))
	float FanAngle;

	// 별 탄막 특성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == EPatternType::Star"))
	float BulletSpeed;


	FBulletHellPattern()
		: PatternType(EPatternType::Straight),
		  Interval(1.0f),
		  PatternSize(500.0f),
		  NumberOfBullets(10),
		  Amplitude(30.0f),
		  Frequency(3.0f),
		  FanAngle(90.0f),
		  BulletSpeed(800.0f)
	{
	}
};
