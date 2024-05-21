#pragma once

#include "CoreMinimal.h"
#include "BulletHellPatern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	// 직선
	Straight	UMETA(DisplayName = "Straight"),
	// 웨이브
	Wave		UMETA(DisplayName = "Wave"),
	// 부채꼴
	Fan			UMETA(DisplayName = "Fan")
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


	FBulletHellPattern()
		: PatternType(EPatternType::Straight),
		  Interval(1.0f),
		  PatternSize(500.0f),
		  PatternSpeed(200.0f),
		  PatternDuration(5.0f),
		  PatternRotation(FRotator::ZeroRotator),
		  PatternSpacing(50.0f),
		  PatternDirection(FVector::ForwardVector),
		  PatternRotationSpeed(90.0f),
		  PatternCurve(0.0f)
	{
	}
};
