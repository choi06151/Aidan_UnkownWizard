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

	if (TimeElapsed > 2.0f)
	{
		ChangePattern();
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
		FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
		FRotator BossRotation = GetActorRotation();
		FBulletHellPattern CurrentPattern = BulletPatterns[CurrentPatternIndex];

		switch (CurrentPattern.PatternType)
		{
		case EPatternType::Straight:
			FireStraightPattern(CurrentPattern);
			break;

		case EPatternType::Wave:
			FireWavePattern(CurrentPattern);
			break;

		case EPatternType::Fan:
			FireFanPattern(CurrentPattern);
			break;

		case EPatternType::Circle:
			FireCirclePattern(CurrentPattern);
			break;

		case EPatternType::Spread:
			FireSpreadPattern(CurrentPattern);
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

void ABoss::FireWavePattern(const FBulletHellPattern& Pattern)
{
	// 파형 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	for (int32 i = 0; i < 10; ++i)
	{
		float Offset = FMath::Sin(GetWorld()->GetTimeSeconds() + i * 10.0f) * 10.0f;
		FVector SpawnLocation = BossLocation + BossRotation.RotateVector(FVector(0.0f, Offset, 0.0f));
		BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("Wave"));
	UE_LOG(LogTemp, Warning, TEXT("----"));
}

void ABoss::FireFanPattern(const FBulletHellPattern& Pattern)
{
	// 부채꼴 패턴 발사
	FVector BossLocation = GetActorLocation();
	FRotator BossRotation = GetActorRotation();

	const float FanAngle = Pattern.PatternSize; // 부채꼴 패턴의 각도 설정
	NumberOfBullets = 6; // 부채꼴 패턴의 총알 수
	AngleStep = FanAngle / (NumberOfBullets - 1);
	const float StartAngle = -FanAngle / 2.0f;

	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		float Angle = StartAngle + i * AngleStep;
		FRotator SpawnRotation = BossRotation + FRotator(0, Angle, 0);
		FVector Direction = FRotationMatrix(SpawnRotation).GetUnitAxis(EAxis::X);
		FVector SpawnLocation = BossLocation + Direction * 100.0f;
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("Fan"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireSpreadPattern(const FBulletHellPattern& Pattern)
{
	FVector EnemyLocation = GetActorLocation();
	FRotator EnemyRotation = GetActorRotation();

	// 스프레드 각도와 총알 수 설정
	float SpreadAngle = 30.0f; // 각도 설정
	float BulletSpacing = 100.0f; // 총알 간격 설정
	NumberOfBullets = 5;

	// 총알 생성 및 발사
	for (int32 i = 0; i < NumberOfBullets; ++i)
    {
        float Angle = SpreadAngle * (i - (NumberOfBullets - 1) / 2);
        FRotator SpawnRotation = EnemyRotation + FRotator(0, Angle, 0);
        FVector SpawnLocation = EnemyLocation + SpawnRotation.Vector() * BulletSpacing * i; // 발사 위치 설정
        BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
    }

	UE_LOG(LogTemp, Warning, TEXT("Spread"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireRandomSpreadPattern(const FBulletHellPattern& Pattern)
{
	FVector EnemyLocation = GetActorLocation();
	FRotator EnemyRotation = GetActorRotation();

	// 스프레드 각도와 총알 수 설정
	float SpreadAngle = 80.0f; // 각도 설정
	NumberOfBullets = 5;

	// 랜덤 위치에서 발사
	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		// 랜덤한 각도와 거리 설정
		float RandomAngle = FMath::FRandRange(-SpreadAngle, SpreadAngle);
		float RandomDistance = FMath::FRandRange(200.0f, 1000.0f);

		// 발사 위치 설정
		FVector SpawnLocation = EnemyLocation + EnemyRotation.RotateVector(FVector(RandomDistance, 0.0f, 0.0f));

		// 발사 각도 설정
		FRotator SpawnRotation = (SpawnLocation - EnemyLocation).Rotation();
		SpawnRotation.Yaw += RandomAngle;

		// 총알 발사
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, 800.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomSpread"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern)
{
	// 원형 패턴을 위해 총알의 위치를 계산합니다.
	FVector BossLocation = GetActorLocation();
	FRotator BossRotation = GetActorRotation();
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

	// 웨이브
	FBulletHellPattern WavePattern;
	WavePattern.PatternType = EPatternType::Wave;
	WavePattern.Interval = 0.5f;
	WavePattern.Amplitude = 150.0f; // 웨이브 진폭 설정
	WavePattern.Frequency = 1.0f; // 웨이브 주파수 설정
	BulletPatterns.Add(WavePattern);

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

	// 스프레드 패턴
	FBulletHellPattern SpreadPattern;
	SpreadPattern.PatternType = EPatternType::Spread;
	SpreadPattern.Interval = 1.0f;
	BulletPatterns.Add(SpreadPattern);

	// 랜덤 스프레드 패턴
	FBulletHellPattern RandomSpreadPattern;
	RandomSpreadPattern.PatternType = EPatternType::RandomSpread;
	RandomSpreadPattern.Interval = 1.0f;
	BulletPatterns.Add(RandomSpreadPattern);
}
