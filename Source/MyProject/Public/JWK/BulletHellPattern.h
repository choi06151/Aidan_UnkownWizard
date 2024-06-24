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
	Angel,
	// 하트
	Heart,
	// 민들레
	Dandelion,  
	// HA
	HA,      
	// 원형 이동
	CircularMoving,  
	// 바람개비
	Pinwheel
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

	// 민들레 패턴에 필요한 변수
	float SpreadDelay; // 퍼지기 시작하는 시간
	// 바람개비 패턴에 필요한 변수
	TArray<FVector> InitialPositions; // 초기 위치 배열
	TArray<float> OrbitSpeeds; // 궤도별 회전 속도
	TArray<float> OrbitRadii; // 궤도별 반지름

	// 원을 그리는 속도를 위한 멤버 변수 추가 0617
	float OrbitSpeed;
	// 추가: 초기 각도 배열
	TArray<float> InitialAngles; // 초기 각도 배열

	int32 FloatIntensity; // 둥실거림 강도 0619

	float CircleRadius; // 원형 패턴의 반지름 추가//////////0624
};
