#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.generated.h"

UENUM(BlueprintType)
enum class EPatternType : uint8
{
	RandomStraight,		// 랜덤 직선			O
	Fan,				// 부채꼴			O
	Circle,				// 원형				O
	Swirl,				// 소용돌이?			O
	TargetCross,		// 유도 십자가		O

	
	TrumpetFlower,		// 나팔꽃			수정
	Crescent,			// 초승달			수정
	Angel,				// 천사				수정

	Heart,               // 하트
	ExpandingSphere,      // 구 모양 변경되는

	Dandelion       // 민들레 패턴 추가
	// 새로운 패턴 추가시 이름 여기에 추가
	
};

USTRUCT(BlueprintType)
struct FBulletHellPattern
{
	GENERATED_BODY()

public:
	// 탄막의 패턴 타입 종류
	EPatternType PatternType;

	// 탄막 특성들
	float Interval;				// 발사 주기
	int32 NumberOfBullets;		// 한 번에 발사되는 총알의 수
	float BulletSpeed;			// 전체 탄막 속도
	float PatternSize;			// 패턴 크기

	float SpiralSpeed;			// 나선형
	
	float Amplitude;			// 웨이브 탄막 특성
	
	float FanAngle;				// 부채꼴 탄막 특성
	
	float ZigzagWidth;			// 지그재그
	float ZigzagFrequency;		// 지그재그
	
	float CrescentAngle;		// 반달

	// 추가된 변수들
	float InitialPatternSize; // 초기 패턴 크기
	float FinalPatternSize;   // 최종 패턴 크기
	float SizeChangeDistance; // 크기 변경이 일어나는 거리

	// 민들레 패턴에 필요한 변수
	float SpreadDelay; // 퍼지기 시작하는 시간
};
