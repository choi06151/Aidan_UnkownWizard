#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	// 직선
	Straight	UMETA(DisplayName = "Straight"),
	// 파형
	Wave		UMETA(DisplayName = "Wave"),
	// 부채꼴
	Fan			UMETA(DisplayName = "Fan"),
	// 원
	Circle		UMETA(DisplayName = "Circle")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	FRotator PatternRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternSpacing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	FVector PatternDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")
	float PatternCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern")	// 부채꼴 패턴의 각도
	float FanAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Pattern") // 총알 수
	float NumberOfBullets;
	

	FBulletHellPattern()
		: PatternType(EPatternType::Straight),
		  Interval(1.0f),
		  PatternSize(500.0f),
		  PatternSpeed(100.0f),
		  PatternDuration(5.0f),
		  PatternRotation(FRotator::ZeroRotator),
		  PatternSpacing(100.0f),
		  PatternDirection(FVector::ForwardVector),
		  PatternRotationSpeed(90.0f),
		  PatternCurve(0.0f),
	      NumberOfBullets(10)
	{
	}
};
