#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	// 랜덤 직선
	RandomStraight,
	// 부채꼴
	Fan,
	// 유도 원형	
	TargetCircle,
	// 소용돌이(십자가)
	Swirl,
	// 유도 십자가
	TargetCross,
	// 벽
	Wall,
	// 유도 오각형
	Octagon,
	// 천사 - 수정
	Angel 
};

USTRUCT(BlueprintType)
struct FBulletHellPattern
{
	GENERATED_BODY()

public:
	// 탄막의 패턴 타입 종류
	EPatternType PatternType;

	// 탄막 특성들
	float Interval; // 발사 주기
	int32 NumberOfBullets; // 한 번에 발사되는 총알의 수
	float BulletSpeed; // 전체 탄막 속도
	float PatternSize; // 패턴 크기

	float SpiralSpeed; // 나선형

	float Amplitude; // 웨이브 탄막 특성

	float FanAngle; // 부채꼴 탄막 특성

	float ZigzagWidth; // 지그재그
	float ZigzagFrequency; // 지그재그

	float CrescentAngle; // 반달
};
