#pragma once

#include "CoreMinimal.h"
#include "Spawn_Bullet.h"
#include "GameFramework/Character.h"
#include "BulletHellPattern.h"
#include "Boss.generated.h"

DECLARE_DELEGATE_OneParam(FPatternDelegate, const FBulletHellPattern&);

// 패턴 조건을 정의하는 구조체
USTRUCT(BlueprintType)
struct FPatternConditions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsHighIntensity;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsLowFrequency;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsLowMidFrequency;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsHighMidFrequency;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsHighFrequency;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsOnBeat;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsTempoAbove90;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsTempoAbove100;

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Conditions")
	bool bIsTempoAbove110;

	FPatternConditions()
		: bIsHighIntensity(false)
		, bIsLowFrequency(false)
		, bIsLowMidFrequency(false)
		, bIsHighMidFrequency(false)
		, bIsHighFrequency(false)
		, bIsOnBeat(false)
		, bIsTempoAbove90(false)
		, bIsTempoAbove100(false)
		, bIsTempoAbove110(false)
	{
	}
};
// 각 시간 인덱스에서 사용할 탄막 패턴의 정보를 저장하는 구조체
USTRUCT(BlueprintType)
struct FFinalPatternData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Data")
	int32 PatternIndex; // 탄막 패턴의 인덱스

	UPROPERTY(BlueprintReadWrite, Category = "Pattern Data")
	float BulletSpeed; // 탄막의 총알 속도

	FFinalPatternData()
		: PatternIndex(0)
		, BulletSpeed(300.0f) // 기본 속도 값
	{
	}
};

UCLASS()
class MYPROJECT_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	ABoss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* bossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* bossEyeMesh_L;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* bossEyeMesh_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterMovementComponent* movementComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBossFSM* bossFSM;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* batonMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* capMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* tuxedoMesh;
	
	
	////////////////////////////////////// Boss Play 관련 bool //////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") // 게임이 시작되었는가?
	bool bIsGameStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") // 게임이 시작되고 움직이는가?
	bool bIsWalk = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") // 게임이 시작되고 목적지에 도달했는가?
	bool bIsArrive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") //목적지에 도달하고 공격을 시작했는가?
	bool bIsAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") // 일정 체력 이하로 떨어졌을 때 다음 페이즈로 진입했는가?
	bool bIsPhase = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "bool_State") // 체력이 0이 되었는가??
	bool bIsDie = false;


	//////////////////////////////////////// Play UI 관련 ////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "music")	// 음악이 시작되었는가??
	bool bIsMusicStart;

	UFUNCTION()
	void MusicStart();	// 음악이 시작되고 커튼 애니메이션 재생 및 보스 행동 시작

	float curTime;

	UPROPERTY(EditAnywhere, Category = "Spawn_Widget")
	class ASpawnWidget* SpawnWidget;
	
	//////////////////////////////////////// BulletHell 발사 관련 ////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") // 한 번에 발사되는 총알의 수
	int32 NumberOfBullets = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") // 총알의 각도
	float AngleStep = 360.0f / NumberOfBullets;

	
	//////////////////////////////////////// 음악분석 관련 ////////////////////////////////////////
	
    // JSON 데이터를 로드하고 패턴 조건을 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pattern Analysis")
	void LoadMusicDataAndSetPatterns(const FString& MusicTitle, const FString& MusicFilePath);

	// 패턴 조건을 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pattern Analysis")
	void UpdatePatternConditions();

	// 미리 분석된 결과를 저장하는 TMap
	//TMap<FString, TArray<FPatternConditions>> AnalyzedDataMap;
	TMap<FString, TPair<float, TArray<FFinalPatternData>>> AnalyzedDataMap; // 템포값까지 저장

	// PreAnalyzeMusicData 함수에서 저장된 최종 패턴 데이터를 저장할 변수
	TArray<FFinalPatternData> FinalPatternData;

	// 미리 분석하는 함수
	void PreAnalyzeMusicData(const FString& MusicTitle, const FString& JsonFilePath);

	// 모든 곡을 미리 분석하는 함수
	void PreAnalyzeAllMusicData();

private:
	//////////////////////////////////////// 총알 발사 관련 함수 ////////////////////////////////////////
	void FireBullet();
	void StartFiring();
	void StopFiring();
	void ChangePattern();
	void InitializeDefaultPatterns();

	
	//////////////////////////////////////// BulletHell 패턴 관련 ////////////////////////////////////////

	void FireRandomStraightPattern(const FBulletHellPattern& Pattern); // 랜덤 직선

	void FireFanPattern(const FBulletHellPattern& Pattern); // 부채꼴

	void FireCirclePattern(const FBulletHellPattern& Pattern); // 원

	void FireSwirlPattern(const FBulletHellPattern& Pattern); // 소용돌이
	void DefineSwirlShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float Radius, float RotationOffset); // 소용돌이


	void FireButterflyPattern(const FBulletHellPattern& Pattern); // 나비

	void FireTrumpetFlowerPattern(const FBulletHellPattern& Pattern); // 나팔꽃

	void FireCrescentPattern(const FBulletHellPattern& Pattern); // 초승달

	void FireAngelPattern(const FBulletHellPattern& Pattern); // 천사


	UPROPERTY(EditAnywhere, Category = "Combat") // 총알 생성
	USpawn_Bullet* BulletSpawner;

	UPROPERTY(EditAnywhere, Category = "Combat") // 패턴 종류 저장소
	TArray<FBulletHellPattern> BulletPatterns;

	UPROPERTY(EditAnywhere, Category = "Combat") // 발사 주기
	float FireRate;

	UPROPERTY(EditAnywhere, Category = "Combat") // 현재 패턴 인덱스
	int32 CurrentPatternIndex;

	FTimerHandle FireRateTimerHandle;


	// 부채꼴 관련 변수
	float FanAngleStep; // 각도 변경
	float FanStartAngle; // 초기 각도 변경


	// 기본 패턴 설정
	FBulletHellPattern DefaultRandomStraightPattern;

	// 패턴 델리게이트 맵
	TMap<EPatternType, FPatternDelegate> PatternDelegates;

	//////////////////////////////////////// 음악분석 관련 ////////////////////////////////////////
	FTimerHandle PatternUpdateTimerHandle; // 새로운 타이머 핸들 추가
	// 음악 데이터를 저장할 변수
	TArray<FPatternConditions> PatternConditions;
	// 패턴 업데이트를 위한 인덱스
	int32 CurrentTimeIndex;


	FTimerHandle TimerHandle;
	
	int cnt = 0;
	
	void HandleState();
	
};
