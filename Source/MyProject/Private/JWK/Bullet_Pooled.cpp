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

	// 추가된 변수들
	// 민들레 패턴을 위한 변수 초기화
	bShouldSpread = false;
	SpreadDelay = 0.0f;
	TimeSinceFired = 0.0f;
	SpreadRotation = FRotator::ZeroRotator;
}

void ABullet_Pooled::BeginPlay()
{
	Super::BeginPlay();

	movementComp->SetUpdatedComponent(boxComp);
	//추가
	InitialLocation = GetActorLocation(); // 초기 위치 설정
	TimeSinceSpawned = 0.0f; // 생성된 이후 경과 시간 초기화
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

	//// meshComponent 회전 로직 추가
	//FRotator CurrentRotation = meshComp->GetComponentRotation();
	//FRotator DeltaRotation(0, DeltaTime * 100, 0);  // Y축을 기준으로 회전 속도 설정
	//meshComp->SetWorldRotation(CurrentRotation + DeltaRotation);

	if (Active)
	{
		TimeSinceFired += DeltaTime;
		MoveBullet(DeltaTime); // 총알 이동
	}
}

// 총알 상태 활성화 or 비활성화
void ABullet_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABullet_Pooled::Deactivate, LifeSpan, false);

	// 초기화
	InitialLocation = GetActorLocation(); // 활성화 시 초기 위치 설정
	DistanceTraveled = 0.0f; // 이동 거리 초기화
	ElapsedTime = 0.0f; // 경과 시간 초기화
	TimeSinceSpawned = 0.0f;

	// 민들레 패턴 변수 초기화
	bShouldSpread = false; // 초기에는 퍼지지 않도록 설정
	SpreadDelay = 0.0f; 
	TimeSinceFired = 0.0f;
	SpreadRotation = FRotator::ZeroRotator;
	InitialDirection = GetActorForwardVector();
	// 이동 속도 초기화
	movementComp->Velocity = InitialDirection * movementComp->InitialSpeed;
}

// 총알 비활성화
void ABullet_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBulletDespawn.Broadcast(this);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ABullet_Pooled::MoveBullet(float DeltaTime)
{
	// 바람개비 패턴
	if (PatternType == EPatternType::Pinwheel)
	{
		// 바람개비 패턴
		CurrentAngle += CircularSpeed * DeltaTime;
		float Rad = FMath::DegreesToRadians(CurrentAngle);
		FVector CircularOffset = FVector(0.0f, FMath::Cos(Rad) * CircularRadius, FMath::Sin(Rad) * CircularRadius);

		// 앞으로 나아가는 거리 계산
		FVector ForwardMovement = InitialDirection * movementComp->InitialSpeed * DeltaTime;

		// 새로운 위치 계산
		FVector NewLocation = CircularCenter + CircularOffset + ForwardMovement;
		SetActorLocation(NewLocation);

		// CircularCenter를 앞으로 나아가는 만큼 갱신
		CircularCenter += ForwardMovement;
	}
	// 원형 이동 패턴
	else if (PatternType == EPatternType::CircularMoving)
	{
		CurrentAngle += CircularSpeed * DeltaTime;
		float Rad = FMath::DegreesToRadians(CurrentAngle);
		FVector CircularOffset = FVector(0.0f, FMath::Cos(Rad) * CircularRadius, FMath::Sin(Rad) * CircularRadius);

		// 앞으로 나아가는 거리 계산
		FVector ForwardMovement = ForwardDirection * movementComp->InitialSpeed * DeltaTime;

		// 새로운 위치 계산
		FVector NewLocation = CircularCenter + CircularOffset + ForwardMovement;
		SetActorLocation(NewLocation);

		// CircularCenter를 앞으로 나아가는 만큼 갱신
		CircularCenter += ForwardMovement;
	}
	else
	{
		// 현재 총알의 속도를 계산
		FVector BulletVelocity = GetActorForwardVector() * movementComp->InitialSpeed;
		//FVector BulletVelocity = InitialDirection * movementComp->InitialSpeed;

		// 현재 시간을 기반으로한 진동 운동 계산
		float OscillationDeltaX = FMath::Sin(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;
		float OscillationDeltaY = FMath::Cos(GetGameTimeSinceCreation() * OscillationFrequency) * OscillationRadius;

		// 진동 운동을 적용한 이동 벡터 계산
		FVector MoveDelta = BulletVelocity * DeltaTime + (GetActorForwardVector() * OscillationDeltaX) + (GetActorUpVector() * OscillationDeltaY);

		// 현재 위치에서 이동 벡터를 더하여 새로운 위치 계산
		FVector NewLocation = GetActorLocation() + MoveDelta;

		// 새로운 위치로 총알 이동
		SetActorLocation(NewLocation);

		// meshComponent 회전 로직 추가
		FRotator CurrentRotation = meshComp->GetComponentRotation();
		FRotator DeltaRotation(0, DeltaTime * 100, 0);  // Y축을 기준으로 회전 속도 설정
		meshComp->SetWorldRotation(CurrentRotation + DeltaRotation);


		// 이동 거리 계산
		DistanceTraveled += BulletVelocity.Size() * DeltaTime; // 이동 거리 누적
		// 경과 시간 누적
		ElapsedTime += DeltaTime;

		// 일정 시간이 경과하면 퍼지는 로직 호출
		if (bShouldSpread && ElapsedTime >= SpreadDelay)
		{
			CheckAndSpread();
		}
	}
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

void ABullet_Pooled::SetSpreadParams(bool bSpread, float Delay, FRotator Rotation, EPatternType Type)
{
	bShouldSpread = bSpread;
	SpreadDelay = Delay;
	SpreadRotation = Rotation;
	PatternType = Type;
}

void ABullet_Pooled::CheckAndSpread()
{
	if (bShouldSpread && PatternType == EPatternType::Dandelion)
	{
		// 퍼지는 로직 (민들레 패턴에만 적용)
		InitialDirection = SpreadRotation.RotateVector(InitialDirection);
		movementComp->Velocity = InitialDirection * movementComp->InitialSpeed;

		// 퍼진 후 초기화
		bShouldSpread = false; // 한 번 퍼진 후에는 다시 퍼지지 않도록 설정
		ElapsedTime = 0.0f; // 경과 시간 초기화
	}
}

void ABullet_Pooled::SetCircularParams(const FVector& Center, float Radius, float Speed, const FVector& InitialDir)
{
	CircularCenter = Center;
	CircularRadius = Radius;
	CircularSpeed = OrbitSpeed; // 원을 그리는 속도
	InitialDirection = InitialDir;
	CurrentAngle = 0.0f;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
