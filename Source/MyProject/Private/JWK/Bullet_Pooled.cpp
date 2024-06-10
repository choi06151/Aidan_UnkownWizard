#include "JWK/Bullet_Pooled.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABullet_Pooled::ABullet_Pooled()
{
	PrimaryActorTick.bCanEverTick = true;

	// boxComponent
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetWorldScale3D(FVector(1.0f));
	boxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// meshComponent
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetWorldScale3D(FVector(0.025f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// movementComponent
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));
	movementComp->InitialSpeed = 0.0f;
	movementComp->MaxSpeed = 3000.0f;
	movementComp->bShouldBounce = false;

	// 추가된 변수들 초기화
	InitialRadius = 0.0f;        // 추가된 부분
	FinalRadius = 0.0f;          // 추가된 부분
	SizeChangeDistance = 0.0f;   // 추가된 부분
	bHasBroadcasted = false;     // 추가된 부분
}

void ABullet_Pooled::BeginPlay()
{
	Super::BeginPlay();

	movementComp->SetUpdatedComponent(boxComp);
	InitialLocation = GetActorLocation(); // 초기 위치 설정 //추가
}

void ABullet_Pooled::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//// 현재 총알의 속도를 계산
	//FVector BulletVelocity = GetActorForwardVector() * movementComp->InitialSpeed;

	//// 현재 시간을 기반으로한 진동 운동 계산
	//float OscillationDeltaX = FMath::Sin(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;
	//float OscillationDeltaY = FMath::Cos(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;

	//// 진동 운동을 적용한 이동 벡터 계산
	//FVector MoveDelta = BulletVelocity * DeltaTime + (GetActorForwardVector() * OscillationDeltaX) + (GetActorUpVector() * OscillationDeltaY);

	//// 현재 위치에서 이동 벡터를 더하여 새로운 위치 계산
	//FVector NewLocation = GetActorLocation() + MoveDelta;

	//// 새로운 위치로 총알 이동
	//SetActorLocation(NewLocation);

	///////기존 코드 move로 옯기고 수정
	if (Active)
	{
		MoveBullet(DeltaTime); // 총알 이동 
	}
}

// 총알 비활성화
void ABullet_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBulletDespawn.Broadcast(this);
}
///////////////////////////////////////////추가/////////////////////////////////////////////
void ABullet_Pooled::MoveBullet(float DeltaTime)
{
	// 기존 총알 이동 로직
	FVector BulletVelocity = GetActorForwardVector() * movementComp->InitialSpeed;

	// 현재 시간을 기반으로한 진동 운동 계산
	float OscillationDeltaX = FMath::Sin(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;
	float OscillationDeltaY = FMath::Cos(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;

	// 진동 운동을 적용한 이동 벡터 계산
	FVector MoveDelta = BulletVelocity * DeltaTime + (GetActorForwardVector() * OscillationDeltaX) + (GetActorUpVector() * OscillationDeltaY);

	// 현재 위치에서 이동 벡터를 더하여 새로운 위치 계산
	FVector NewLocation = GetActorLocation() + MoveDelta;

	// 새로운 위치로 총알 이동
	SetActorLocation(NewLocation);

	// 이동 거리 계산
	DistanceTraveled += MoveDelta.Size(); // 이동 거리 누적
	//UE_LOG(LogTemp, Warning, TEXT("ABullet_Pooled::MoveBullet: New Location: %s, Distance traveled: %f"), *NewLocation.ToString(), DistanceTraveled); // 디버깅 로그 추가

	// 이동 거리가 임계값을 초과할 때 이벤트 호출
	if (DistanceTraveled >= SomeThreshold && !bHasBroadcasted)
	{
		if (OnBulletTravelled.IsBound())
		{
			OnBulletTravelled.Broadcast(DistanceTraveled, this); // 이벤트 호출
			bHasBroadcasted = true; // 이벤트가 호출되었음을 표시
			//UE_LOG(LogTemp, Warning, TEXT("ABullet_Pooled::MoveBullet onbullet호출"));
		}
	}
}

void ABullet_Pooled::SetDistanceThreshold(float Threshold)
{
	SomeThreshold = Threshold;
}

// 총알의 패턴 타입 설정 함수
void ABullet_Pooled::SetPatternType(EPatternType Type)
{
	PatternType = Type;
}

// 총알의 패턴 타입 반환 함수
EPatternType ABullet_Pooled::GetPatternType() const
{
	return PatternType;
}
////////////////////////////////////////////////////////////////////////////////////////

// 총알 상태 활성화 or 비활성화
void ABullet_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABullet_Pooled::Deactivate, LifeSpan, false);
	//추가
	InitialLocation = GetActorLocation(); // 활성화 시 초기 위치 설정
	DistanceTraveled = 0.0f; // 이동 거리 초기화
	bHasBroadcasted = false; // 플래그 초기화
}

// bullet의 수명을 set
void ABullet_Pooled::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

// bullet의 Speed Set
void ABullet_Pooled::SetBulletSpeed(float Speed)
{
	movementComp->InitialSpeed = Speed;
}

// pool에서의 index 설정
void ABullet_Pooled::SetPoolIndex(int index)
{
	PoolIndex = index;
}

// 총알의 활성 or 비활성 bool 값 반환
bool ABullet_Pooled::IsActive()
{
	return Active;
}

// pool에서의 index 값 반환
int ABullet_Pooled::GetPoolIndex()
{
	return PoolIndex;
}
