#pragma once

#include "CoreMinimal.h"
#include "Spawn_Bullet.h"
#include "GameFramework/Character.h"
#include "BulletHellPattern.h"
#include "Boss.generated.h"

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

	
private:
	// 총알 발사 관련 함수
	void FireBullet();
	void StartFiring();
	void StopFiring();
	void ChangePattern();
	void InitializeDefaultPatterns();

	// 직선
	void FireStraightPattern(const FBulletHellPattern& Pattern);
	// 웨이브
	void FireWavePattern(const FBulletHellPattern& Pattern);
	// 부채꼴
	void FireFanPattern(const FBulletHellPattern& Pattern);
	// 랜덤 스프레드
	void FireRandomSpreadPattern(const FBulletHellPattern& Pattern);
	// 원
	void FireCirclePattern(const FBulletHellPattern& Pattern);
	void DefineCircleShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float Radius);

	
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
	FBulletHellPattern DefaultWavePattern;
	FBulletHellPattern DefaultFanPattern;
	FBulletHellPattern DefaultCirlPattern;
	FBulletHellPattern DefaultSpreadPattern;
	FBulletHellPattern DefaultRandomSpreadPattern;
};
