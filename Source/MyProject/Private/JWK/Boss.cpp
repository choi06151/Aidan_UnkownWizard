#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/Bullet_Pooled.h"
// #include "JWK/BossFSM.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	movementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movementComp"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetWorldScale3D(FVector(1));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	batonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("batonMesh"));
	batonMesh->SetupAttachment(GetMesh(), TEXT("BATON"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBatonMesh(
		TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Baton/Baton.Baton'"));
	if (tempBatonMesh.Succeeded())
	{
		batonMesh->SetStaticMesh(tempBatonMesh.Object);
		batonMesh->SetRelativeLocationAndRotation(FVector(2, -1, 10), FRotator(-90, 65, -25));
		batonMesh->SetWorldScale3D(FVector(1, 1, 1));
	}

	BulletSpawner = CreateDefaultSubobject<USpawn_Bullet>(TEXT("BulletSpawner"));

	FireRate = 1.0f;

	CurrentPatternIndex = 0;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	if (BulletPatterns.Num() == 0)
		InitializeDefaultPatterns();

	StartFiring();
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float TimeElapsed = 0.0f;
	TimeElapsed += DeltaTime;
	
	// 1.5초가 지났을 때 랜덤 탄막 패턴 발사
	if (TimeElapsed > 1.5f && TimeElapsed <= 3.0f && !IsDie)
	{
		StopFiring();
		ChangePattern();
		StartFiring();
	}
	// 6.5초가 지나면 발사 중지
	if (TimeElapsed > 3.0f && TimeElapsed <= 5.5f && !IsDie)
	{
		StopFiring();
	}

	// 9초가 지났을 때 다음 탄막 패턴 발사
	if (TimeElapsed > 5.5f && !IsDie)
	{
		ChangePattern();
		StartFiring();

		// 타이머 리셋
		TimeElapsed = 0.0f;
	}
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABoss::FireBullet()
{
	if (BulletSpawner != nullptr && BulletPatterns.Num() > 0)
	{
		FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorUpVector() * 100.0f;
		FRotator BossRotation = GetActorRotation();
		FBulletHellPattern CurrentPattern = BulletPatterns[CurrentPatternIndex];

		switch (CurrentPattern.PatternType)
		{
		case EPatternType::Straight:
			FireStraightPattern(CurrentPattern);
			break;
			
		case EPatternType::RandomStraight:
			FireRandomStraightPatter(CurrentPattern);
			break;

		case EPatternType::Fan:
			FireFanPattern(CurrentPattern);
			break;

		case EPatternType::Circle:
			FireCirclePattern(CurrentPattern);
			break;
			
		case EPatternType::RandomSpread:
			FireRandomSpreadPattern(CurrentPattern);
			break;

		default:
			break;
		}
	}
}

void ABoss::FireStraightPattern(const FBulletHellPattern& Pattern)
{
	// 직선 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();
	BulletSpawner->SpawnPooledBullet(BossLocation, BossRotation, 800.0f);

	UE_LOG(LogTemp, Warning, TEXT("Straight"));
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireRandomStraightPatter(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	// 총알 수 설정
	NumberOfBullets = 3;

	// Y축 범위 설정
	float YMin = -150.0f;
	float YMax = 150.0f;
	float Z = BossLocation.Z; // Z축 고정

	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		// Y축에서 랜덤 위치 설정
		float RandomY = FMath::FRandRange(YMin, YMax);

		// 발사 위치 설정
		FVector SpawnLocation = FVector(BossLocation.X, RandomY, Z);

		// 발사 각도 설정 (직선 발사)
		FRotator SpawnRotation = GetActorRotation();

		// 총알 발사
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomStraight"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireFanPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	// 부채꼴 패턴 각도와 총알 수 설정
	float FanAngle = 120.0f;
	NumberOfBullets = 3;
	AngleStep = FanAngle / (NumberOfBullets - 1);

	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		float AngleOffset = -FanAngle / 2 + AngleStep * i;
		FRotator SpawnRotation = BossRotation;
		SpawnRotation.Yaw += AngleOffset;

		BulletSpawner->SpawnPooledBullet(BossLocation, SpawnRotation, 800.0f);
	}
	UE_LOG(LogTemp, Warning, TEXT("Fan"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireRandomSpreadPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	// 스프레드 각도와 총알 수 설정
	float SpreadAngle = 30.0f; // 각도 설정
	NumberOfBullets = 5;

	// 랜덤 위치에서 발사
	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		// 랜덤한 각도와 거리 설정
		float RandomAngle = FMath::FRandRange(-SpreadAngle, SpreadAngle);
		float RandomY = FMath::FRandRange(-250.0f, 250.0f); // Y축 범위 설정
		float RandomZ = FMath::FRandRange(0.0f, 150.0f); // Z축 범위 설정

		// 발사 위치 설정
		FVector SpawnLocation = BossLocation + FVector(0.0f, RandomY, RandomZ);

		// 발사 각도 설정
		FRotator SpawnRotation = (SpawnLocation - BossLocation).Rotation();
		// SpawnRotation.Yaw += RandomAngle;

		// 총알 발사
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomSpread"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern)
{
	// 원형 패턴을 위해 총알의 위치를 계산합니다.
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	NumberOfBullets = Pattern.NumberOfBullets;
	float Radius = Pattern.PatternSize / 3.0f; // 원의 반지름


	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		float Angle = i * (360.0f / NumberOfBullets);
		float Rad = FMath::DegreesToRadians(Angle);

		// 원형 패턴에서 각 총알의 위치 계산
		FVector Offset = FVector(0.0f, FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius);
		FVector SpawnLocation = BossLocation + Offset;

		// 총알이 원 주위를 회전하도록 설정
		FRotator SpawnRotation = (Offset).Rotation();
        
		// 총알을 생성
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("Circle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::DefineCircleShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float Radius)
{
	AngleStep = 360.0f / NumberOfPoints;

	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		float Angle = i * AngleStep;
		float Radian = FMath::DegreesToRadians(Angle);
		float X = FMath::Cos(Radian) * Radius;
		float Y = FMath::Sin(Radian) * Radius;
		OutShape.Add(FVector(X, Y, 0.0f));
	}
}

void ABoss::StartFiring()
{
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABoss::FireBullet, FireRate, true);
}

void ABoss::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ABoss::ChangePattern()
{
	// CurrentPatternIndex = (CurrentPatternIndex + 1) % BulletPatterns.Num();
	int32 RandomIndex = FMath::RandRange(0, BulletPatterns.Num() - 1);
    
	// 현재 패턴 인덱스를 랜덤한 값으로 설정
	CurrentPatternIndex = RandomIndex;
}

void ABoss::InitializeDefaultPatterns()
{
	// 직선
	FBulletHellPattern StraightPattern;
	StraightPattern.PatternType = EPatternType::Straight;
	StraightPattern.Interval = 1.0f;
	BulletPatterns.Add(StraightPattern);

	// 직선
	FBulletHellPattern RandomStraightPattern;
	RandomStraightPattern.PatternType = EPatternType::RandomStraight;
	RandomStraightPattern.Interval = 1.0f;
	BulletPatterns.Add(RandomStraightPattern);

	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.Interval = 1.0f;
	FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
    FanPattern.NumberOfBullets = 20; // 부채꼴 패턴에서 발사할 총알 수
	BulletPatterns.Add(FanPattern);

	// 원형 패턴
	FBulletHellPattern CirclePattern;
	CirclePattern.PatternType = EPatternType::Circle;
	CirclePattern.Interval = 1.0f;
    CirclePattern.PatternSize = 300.0f; // 원형 패턴의 크기 설정
	CirclePattern.NumberOfBullets = 12; // 총알의 수
	BulletPatterns.Add(CirclePattern);

	// 랜덤 스프레드 패턴
	FBulletHellPattern RandomSpreadPattern;
	RandomSpreadPattern.PatternType = EPatternType::RandomSpread;
	RandomSpreadPattern.Interval = 1.0f;
	BulletPatterns.Add(RandomSpreadPattern);
}
