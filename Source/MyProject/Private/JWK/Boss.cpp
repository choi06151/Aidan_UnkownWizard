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

	if (TimeElapsed > 12.0f)
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
	BulletSpawner->SpawnPooledBullet(BossLocation, BossRotation);

	UE_LOG(LogTemp, Warning, TEXT("Straight"));
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireWavePattern(const FBulletHellPattern& Pattern)
{
	// 파형 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	for (int32 i = 0; i < 5; ++i)
	{
		float Offset = FMath::Sin(GetWorld()->GetTimeSeconds() + i * 10.0f) * 2.5f;
		FVector SpawnLocation = BossLocation + BossRotation.RotateVector(FVector(0.0f, Offset, 0.0f));
		BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation);
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
	NumberOfBullets = 5; // 부채꼴 패턴의 총알 수
	AngleStep = FanAngle / (NumberOfBullets - 1);
	const float StartAngle = -FanAngle / 2.0f;

	for (int32 i = 0; i < NumberOfBullets; ++i)
	{
		float Angle = StartAngle + i * AngleStep;
		FRotator SpawnRotation = BossRotation + FRotator(0, Angle, 0);
		FVector Direction = FRotationMatrix(SpawnRotation).GetUnitAxis(EAxis::X);
		FVector SpawnLocation = BossLocation + Direction * 100.0f;
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation);
	}

	UE_LOG(LogTemp, Warning, TEXT("Fan"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
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
	CurrentPatternIndex = (CurrentPatternIndex + 1) % BulletPatterns.Num();
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
	WavePattern.Interval = 1.0f;
	BulletPatterns.Add(WavePattern);

	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.Interval = 1.0f;
	FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
	BulletPatterns.Add(FanPattern);
}
