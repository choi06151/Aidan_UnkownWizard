#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/Bullet_Pooled.h"
// #include "JWK/BossFSM.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	movementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movementComp"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/JWK/Skeleton_Boss/ToonSkeleton/Characters/Meshes/SKM_ToonSkeleton.SKM_ToonSkeleton'"));
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

		case EPatternType::Flower:
			FireFlowerPattern(CurrentPattern);
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
	BulletSpawner->SpawnPooledBullet(BossLocation, BossRotation, Pattern.BulletSpeed);

	UE_LOG(LogTemp, Warning, TEXT("Straight"));
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireRandomStraightPatter(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float RandomY = FMath::FRandRange(-150.0f, 150.0f);
		FVector SpawnLocation = FVector(BossLocation.X, RandomY, BossLocation.Z);
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomStraight"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireFanPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	AngleStep = Pattern.FanAngle / (Pattern.NumberOfBullets - 1);

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float AngleOffset = -Pattern.FanAngle / 2 + AngleStep * i;
		FRotator SpawnRotation = BossRotation;
		SpawnRotation.Yaw += AngleOffset;

		BulletSpawner->SpawnPooledBullet(BossLocation, SpawnRotation, Pattern.BulletSpeed);
	}
	UE_LOG(LogTemp, Warning, TEXT("Fan"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireRandomSpreadPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	FireRate = 0.6;
	
	// 랜덤 위치에서 발사
	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		// 랜덤한 각도와 거리 설정
		float RandomY = FMath::FRandRange(-250.0f, 250.0f); // Y축 범위 설정
		float RandomZ = FMath::FRandRange(10.0f, 120.0f); // Z축 범위 설정

		// 발사 위치 설정
		FVector SpawnLocation = BossLocation + FVector(0.0f, RandomY, RandomZ);

		// 발사 각도 설정
		FRotator SpawnRotation = (SpawnLocation - BossLocation).Rotation();
		// SpawnRotation.Yaw += RandomAngle;

		// 총알 발사
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomSpread"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	float Radius = Pattern.PatternSize / 3.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (360.0f / Pattern.NumberOfBullets);
		float Rad = FMath::DegreesToRadians(Angle);

		FVector Offset = FVector(0.0f, FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius);
		FVector SpawnLocation = BossLocation + Offset;

		FRotator SpawnRotation = (Offset).Rotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Circle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireFlowerPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	float Radius = Pattern.PatternSize / 3.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (360.0f / Pattern.NumberOfBullets);
		float Rad = FMath::DegreesToRadians(Angle);

		float X = FMath::Cos(Rad) * Radius * FMath::Sin(Angle);
		float Y = FMath::Sin(Rad) * Radius * FMath::Sin(Angle);
		FVector Offset = FVector(X, Y, 0.0f);
		FVector SpawnLocation = BossLocation + Offset;

		FRotator SpawnRotation = Offset.Rotation();
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Flower"));
	UE_LOG(LogTemp, Warning, TEXT("------"));
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
	StraightPattern.Interval = 5.0f;
	StraightPattern.FanAngle = 0.0f;
	StraightPattern.NumberOfBullets = 1;
	StraightPattern.BulletSpeed = 100.0f;
	BulletPatterns.Add(StraightPattern);

	// 랜덤 직선
	FBulletHellPattern RandomStraightPattern;
	RandomStraightPattern.PatternType = EPatternType::RandomStraight;
	RandomStraightPattern.Interval = 5.0f;
	RandomStraightPattern.FanAngle = 0.0f;
	RandomStraightPattern.NumberOfBullets = 1;
	RandomStraightPattern.BulletSpeed = 1000.0f;
	BulletPatterns.Add(RandomStraightPattern);

	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.Interval = 1.0f;
	FanPattern.PatternSize = 600.0f;
	FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
    FanPattern.NumberOfBullets = 1; // 부채꼴 패턴에서 발사할 총알 수
	FanPattern.BulletSpeed = 3000.0f;
	BulletPatterns.Add(FanPattern);

	// 원형 패턴
	FBulletHellPattern CirclePattern;
	CirclePattern.PatternType = EPatternType::Circle;
	CirclePattern.Interval = 10.0f;
    CirclePattern.PatternSize = 300.0f; // 원형 패턴의 크기 설정
	CirclePattern.FanAngle = 0;
	CirclePattern.NumberOfBullets = 1; // 총알의 수
	CirclePattern.BulletSpeed = 10000.0f;
	BulletPatterns.Add(CirclePattern);

	// 랜덤 스프레드 패턴
	FBulletHellPattern RandomSpreadPattern;
	RandomSpreadPattern.PatternType = EPatternType::RandomSpread;
	RandomSpreadPattern.Interval = 100.0f;
	RandomSpreadPattern.NumberOfBullets = 11;
	RandomStraightPattern.BulletSpeed = 500.0f;
	BulletPatterns.Add(RandomSpreadPattern);

	// 꽃 패턴
	FBulletHellPattern RandomFlowerPattern;
	RandomFlowerPattern.PatternType = EPatternType::Flower;
	RandomFlowerPattern.Interval = 1.0f;
	RandomStraightPattern.PatternSize = 600.0f;
	RandomStraightPattern.Amplitude = 50.0f;
	RandomFlowerPattern.FanAngle = 0;
	RandomStraightPattern.NumberOfBullets = 1;
	RandomFlowerPattern.BulletSpeed = 500.0f;
	BulletPatterns.Add(RandomFlowerPattern);
}
