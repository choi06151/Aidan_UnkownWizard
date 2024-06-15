#pragma once

#include "CoreMinimal.h"
#include "BulletHellPattern.h"
#include "GameFramework/Actor.h"
#include "Bullet_Pooled.generated.h"

// 총알이 비활성화될 때 호출되는 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledBulletDespawn, ABullet_Pooled*, Bullet);
// 총알이 이동한 거리를 계산할 때 호출되는 델리게이트 선언 ////추가
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBulletTravelled, float, DistanceTraveled, ABullet_Pooled*, PooledBullet);

UCLASS()
class MYPROJECT_API ABullet_Pooled : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet_Pooled();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* movementComp;

	// 총알이 비활성화될 때 호출되는 Event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
	FOnPooledBulletDespawn OnPooledBulletDespawn;

	//////////추가
	// 총알이 이동한 거리를 계산할 때 호출되는 Event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
	FOnBulletTravelled OnBulletTravelled;
	
	// 총알을 활성화 또는 비활성화하는 함수
	void SetActive(bool IsActive);
	
	// bullet의 수명을 set하는 함수
	void SetLifeSpan(float LifeTime);

	// Bullet의 Speed를 set하는 함수
	void SetBulletSpeed(float Speed);
	
	// pool에서의 index 설정하는 함수
	void SetPoolIndex(int index);
	
	// 총알의 활성 or 비활성 bool 값 반환하는 함수
	bool IsActive();
	
	// pool에서의 index 값 반환하는 함수
	int GetPoolIndex();

	// 총알을 비활성화하는 함수
	void Deactivate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletHP")
	int bulletHP;

	//////// 추가
	// 총알의 패턴 타입 설정 함수 
	void SetPatternType(EPatternType Type);

	// 총알의 패턴 타입 반환 함수
	EPatternType GetPatternType() const;

	// 총알을 이동시키는 함수
	void MoveBullet(float DeltaTime);

	// 패턴 변화하는 거리 정하는 함수
	void SetDistanceThreshold(float Threshold);

	// 패턴 생성된 시간 가져오는 함수
	float GetElapsedTime() const;

	float InitialRadius; // 총알이 처음 생성될 때의 초기 크기
	float FinalRadius; // 총알이 이동하면서 도달할 최종 크기
	float SizeChangeDistance; // 총알의 크기가 변하기 시작하는 거리 기준
	float SizeChangeTime; // 크기 변화에 걸리는 시간
	float TimeSinceSpawned; // 생성된 이후 경과 시간
	int32 FrameCounter; //  프레임 카운터
	
	// 민들레 패턴에 필요한 변수들
	FVector InitialDirection; // 초기 방향
	bool bShouldSpread; // 퍼지기 시작할지 여부
	float SpreadDelay; // 퍼지기 시작하는 시간
	float TimeSinceFired; // 발사된 후 경과 시간
	FRotator SpreadRotation; // 퍼질 때 사용할 회전 각도

	void SetSpreadParams(bool bSpread, float Delay, FRotator Rotation, EPatternType Type);
	void CheckAndSpread(); // 퍼지는 로직을 처리하는 함수
	
protected:
	// 총알의 활성화 상태
	bool Active;

	// 총알의 수명
	float LifeSpan = 0.0f;

	// pool에서의 index
	int PoolIndex;

	// 총알 수명 타이머 핸들
	FTimerHandle LifespanTimer;

	////////추가
	// 총알의 초기 위치
	FVector InitialLocation;

	// 총알의 패턴 타입
	EPatternType PatternType;

	UPROPERTY(EditAnywhere)
	float DistanceTraveled = 0.0f; // 이동 거리
	float SomeThreshold; // 임계값 (이 거리에 다다르면 패턴 변화
	float ElapsedTime; // 경과 시간을 저장하는 변수
	bool bHasBroadcasted;



private:
	// 움직임 패턴 관련 변수 초기화
	UPROPERTY(EditAnywhere)
	float OscillationFrequency = 5.0f;	// 진동주파수
	UPROPERTY(EditAnywhere)
	float OscillationRadius = 1.0f;		// 진동반경
	UPROPERTY(VisibleAnywhere)
	class URotatingMovementComponent* movement;

	
};
