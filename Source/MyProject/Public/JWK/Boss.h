#pragma once

#include "CoreMinimal.h"
#include "Spawn_Bullet.h"
#include "GameFramework/Character.h"
#include "BulletHellPattern.h"
#include "Boss.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterMovementComponent* movementComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* batonMesh;


	// Bullet 발사 관련
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")	// 한 번에 발사되는 총알의 수
	int32 NumberOfBullets = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")	// 총알의 각도
	float AngleStep = 360.0f / NumberOfBullets;

	
	
	// Boss의 상태
	UPROPERTY(EditAnywhere)
	bool IsDie = false;

	//----------------------------------------------------------------
	// 추가해야하는 
	// JSON 데이터를 로드하고 패턴 조건을 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pattern Analysis")
	void LoadMusicDataAndSetPatterns(const FString& JsonFilePath, const FString& MusicFilePath);

	// 패턴 조건을 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pattern Analysis")
	void UpdatePatternConditions();
	
private:
	// 총알 발사 관련 함수
	void FireBullet();
	void StartFiring();
	void StopFiring();
	void ChangePattern();
	void InitializeDefaultPatterns();

	// 직선
	void FireStraightPattern(const FBulletHellPattern& Pattern);
	// 랜덤 직선
	void FireRandomStraightPatter(const FBulletHellPattern& Pattern);
	// 부채꼴
	void FireFanPattern(const FBulletHellPattern& Pattern);
	// 원
	void FireCirclePattern(const FBulletHellPattern& Pattern);
	// 나비
	void FireButterflyPattern(const FBulletHellPattern& Pattern);
	// 나팔꽃
	void FireTrumpetFlowerPattern(const FBulletHellPattern& Pattern);
	// 초승달
	void FireCrescentPattern(const FBulletHellPattern& Pattern);
	// 천사
	void FireAngelPattern(const FBulletHellPattern& Pattern);
	




	
	
	
	UPROPERTY(EditAnywhere, Category = "Combat")	// 총알 생성
	USpawn_Bullet* BulletSpawner;

	UPROPERTY(EditAnywhere, Category = "Combat")	// 패턴 종류
	TArray<FBulletHellPattern> BulletPatterns;

	UPROPERTY(EditAnywhere, Category = "Combat")	// 발사 주기
	float FireRate;
	
	UPROPERTY(EditAnywhere, Category = "Combat")	// 현재 패턴 인덱스
	int32 CurrentPatternIndex;
	
	FTimerHandle FireRateTimerHandle;

	
	// 부채꼴 관련 변수
	float FanAngleStep;		// 각도 변경
	float FanStartAngle;	// 초기 각도 변경


	// 기본 패턴 설정
	FBulletHellPattern DefaultStraightPattern;


	//-------------------------------------------
	// 추가
	FTimerHandle PatternUpdateTimerHandle; // 새로운 타이머 핸들 추가
	// 음악 데이터를 저장할 변수
	TArray<FPatternConditions> PatternConditions;
	// 패턴 업데이트를 위한 인덱스
	int32 CurrentTimeIndex;
};
