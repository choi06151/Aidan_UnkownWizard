#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	// 직선
	Straight			UMETA(DisplayName = "Straight"),
	// 랜덤 직선
	RandomStraight		UMETA(DisplayName = "RandomStraight"),
	// 부채꼴
	Fan					UMETA(DisplayName = "Fan"),
	// 원
	Circle				UMETA(DisplayName = "Circle"),
	// 렌덤 스프레드
	RandomSpread		UMETA(DisplayName = "Spread"),
	// 팔각형
	Octagon				UMETA(DisplayName = "Octagon"),
	// 나선형
	Spiral				UMETA(DisplayName = "Spiral"),
	// 꽃
	Flower				UMETA(DisplayName = "Flower")
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

	// 전체 탄막 특성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern",
		meta = (EditCondition = "PatternType == All"))
	float BulletSpeed;


	FBulletHellPattern()
		: Interval(1.0f),		// 간격
		  PatternSize(500.0f),	// 패턴의 크기
		  Amplitude(0.0f),		// 진폭
		  Frequency(3.0f),		// 빈도
		  FanAngle(0.0f),		// 부채꼴 각도
		  NumberOfBullets(10),	// 총알 수
		  BulletSpeed(800.0f)	// 총알 속도
	{
	}
};
