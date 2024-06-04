#pragma once

#include "CoreMinimal.h"
#include "Spawn_Bullet.h"
#include "GameFramework/Character.h"
#include "BulletHellPattern.h"
#include "Boss.generated.h"

DECLARE_DELEGATE_OneParam(FPatternDelegate, const FBulletHellPattern&);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float curTime = 0;

	

	
	//////////////////////////////////////// Boss HP 관련 ////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int bossMaxHP = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int bossHP = bossMaxHP;

	UFUNCTION(BlueprintCallable)
	void TakeDamaged(int damage);

	//////////////////////////////////////// BulletHell 발사 관련 ////////////////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") // 한 번에 발사되는 총알의 수
	int32 NumberOfBullets = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") // 총알의 각도
	float AngleStep = 360.0f / NumberOfBullets;

private:
	// 총알 발사 관련 함수
	void FireBullet();
	void StartFiring();
	void StopFiring();
	void ChangePattern();
	void InitializeDefaultPatterns();

	//////////////////////////////////////// BulletHell 패턴 관련 ////////////////////////////////////////

	void FireStraightPattern(const FBulletHellPattern& Pattern); // 직선

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
	FBulletHellPattern DefaultStraightPattern;

	// 패턴 델리게이트 맵
	TMap<EPatternType, FPatternDelegate> PatternDelegates;
};
