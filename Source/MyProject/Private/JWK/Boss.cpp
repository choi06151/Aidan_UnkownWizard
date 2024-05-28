#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/Bullet_Pooled.h"
#include "KJH/BulletPatternManager.h"
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
	TimeElapsed = 0.0f; // 추가
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	if (BulletSpawner == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::BeginPlay: BulletSpawner is not initialized"));
	}

	//if (BulletPatterns.Num() == 0)
	//	InitializeDefaultPatterns(); 음악 데이터 기반으로 패턴을 설정하기 때문에 기본 패턴을 초기화할 필요 없다

	// 탄막 발사 타이머 설정
	StartFiring();

}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeElapsed += DeltaTime;
	float CurrentFPS = 1.0f / DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("ABoss::Tick: TimeElapsed = %f, FPS = %f"), TimeElapsed, CurrentFPS);

}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// bullet spawner와 현재패턴 (currentpatternindex에 의해 결정된 패턴)을 사용하여 탄막 발사
void ABoss::FireBullet()
{
	if (BulletSpawner != nullptr && BulletPatterns.Num() > 0)
	{
		FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
		FRotator BossRotation = GetActorRotation();
		FBulletHellPattern CurrentPattern = BulletPatterns[CurrentPatternIndex];

		// 탄막 속도 설정 // 로그에는 속도가 바뀌는걸로 찍히는데 실제로는 속도가 바뀌지 않는다. 확인 필요
		float BulletSpeed = 800.0f; // 기본 속도
		if (CurrentConditions.bIsTempoAbove110)
		{
			BulletSpeed = 5000.0f; // 템포가 110 이상일 때 속도
		}
		else if (CurrentConditions.bIsTempoAbove100)
		{
			BulletSpeed = 1000.0f; // 템포가 100 이상일 때 속도
		}
		else if (CurrentConditions.bIsTempoAbove90)
		{
			BulletSpeed = 800.0f; // 템포가 90 이상일 때 속도
		}

		//  현재 패턴과 속도 출력
		UE_LOG(LogTemp, Warning, TEXT("ABoss::FireBullet: Firing pattern %d of type %d at speed %f"), CurrentPatternIndex, (int32)CurrentPattern.PatternType, BulletSpeed);

		switch (CurrentPattern.PatternType)
		{
		case EPatternType::Straight:
			FireStraightPattern(CurrentPattern, BulletSpeed);
			break;

		case EPatternType::Wave:
			FireWavePattern(CurrentPattern, BulletSpeed);
			break;

		case EPatternType::Fan:
			FireFanPattern(CurrentPattern, BulletSpeed);
			break;

		case EPatternType::Circle:
			FireCirclePattern(CurrentPattern, BulletSpeed);
			break;

		case EPatternType::Spread:
			FireSpreadPattern(CurrentPattern, BulletSpeed);
			break;
			
		case EPatternType::RandomSpread:
			FireRandomSpreadPattern(CurrentPattern, BulletSpeed);
			break;

		default:
			UE_LOG(LogTemp, Error, TEXT("ABoss::FireBullet: Unknown pattern type %d"), (int32)CurrentPattern.PatternType);
			break;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::FireBullet: No BulletSpawner or BulletPatterns available."));
	}
}

// 각 패턴별 탄막 발사 함수들에 속도 인자 추가
void ABoss::FireStraightPattern(const FBulletHellPattern& Pattern, float BulletSpeed)
{
	// 직선 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();
	BulletSpawner->SpawnPooledBullet(BossLocation, BossRotation, BulletSpeed);

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("Straight: Speed = %f"), BulletSpeed);
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireWavePattern(const FBulletHellPattern& Pattern, float BulletSpeed)
{
	// 파형 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	for (int32 i = 0; i < 10; ++i)
	{
		float Offset = FMath::Sin(GetWorld()->GetTimeSeconds() + i * 10.0f) * 10.0f;
		FVector SpawnLocation = BossLocation + BossRotation.RotateVector(FVector(0.0f, Offset, 0.0f));
		BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation, BulletSpeed);
	}

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("Wave: Speed = %f"), BulletSpeed);
	UE_LOG(LogTemp, Warning, TEXT("----"));
}

void ABoss::FireFanPattern(const FBulletHellPattern& Pattern, float BulletSpeed)
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
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, BulletSpeed);
	}

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("Fan: Speed = %f"), BulletSpeed);
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireSpreadPattern(const FBulletHellPattern& Pattern, float BulletSpeed)
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
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, BulletSpeed);
	}

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("Spread: Speed = %f"), BulletSpeed);
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireRandomSpreadPattern(const FBulletHellPattern& Pattern, float BulletSpeed)
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
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, BulletSpeed);
	}

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("RandomSpread: Speed = %f"), BulletSpeed);
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern, float BulletSpeed)
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
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, BulletSpeed);
	}

	// 로그에 속도 출력
	UE_LOG(LogTemp, Warning, TEXT("Circle: Speed = %f"), BulletSpeed);
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

void ABoss::LoadMusicDataAndSetPatterns(const FString& FilePath)
{
	FMusicData MusicData;

	UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Loading music data from: %s"), *FilePath);

	if (UMusicDataLoader::LoadMusicDataFromFile(FilePath, MusicData))
	{
		UBulletPatternManager::AnalyzeMusicData(MusicData, PatternConditions, 0.3f, 0.2f, 0.2f, 0.2f, 0.2f);
		CurrentTimeIndex = 0; // 현재 시간을 초기화
		UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Successfully loaded music data and set patterns. Total Conditions: %d"), PatternConditions.Num());

		if (BulletPatterns.Num() == 0)
		{
			InitializeDefaultPatterns(); // 기본 패턴을 설정하여 빈 패턴 배열을 방지합니다.
			UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Initialized default bullet patterns."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load music data from: %s"), *FilePath);
	}
}

// 매 1초마다 호출되어 현재 시간에 맞는 패턴 조건을 업데이트
void ABoss::UpdatePatternConditions()
{
	static float LastUpdateTime = 0.0f;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float TimeSinceLastUpdate = CurrentTime - LastUpdateTime;
	LastUpdateTime = CurrentTime;

	UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: TimeSinceLastUpdate = %f"), TimeSinceLastUpdate);

	// 패턴 조건 업데이트
	if (PatternConditions.IsValidIndex(CurrentTimeIndex))
	{
		CurrentConditions = PatternConditions[CurrentTimeIndex];
		CurrentTimeIndex++;
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Updated CurrentConditions. TimeIndex: %d"), CurrentTimeIndex);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: No more conditions available."));
	}

	// 조건에 따른 패턴 변경
	if (CurrentConditions.bIsHighIntensity)
	{
		CurrentPatternIndex = GetHighIntensityPatternIndex();
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High Intensity detected. Pattern Index: %d"), CurrentPatternIndex);
	}
	else if (CurrentConditions.bIsLowFrequency)
	{
		CurrentPatternIndex = GetLowFrequencyPatternIndex();
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Low Frequency detected. Pattern Index: %d"), CurrentPatternIndex);
	}
	else if (CurrentConditions.bIsLowMidFrequency)
	{
		CurrentPatternIndex = GetLowMidFrequencyPatternIndex();
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Low Mid Frequency detected. Pattern Index: %d"), CurrentPatternIndex);
	}
	else if (CurrentConditions.bIsHighMidFrequency)
	{
		CurrentPatternIndex = GetHighMidFrequencyPatternIndex();
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High Mid Frequency detected. Pattern Index: %d"), CurrentPatternIndex);
	}
	else if (CurrentConditions.bIsHighFrequency)
	{
		CurrentPatternIndex = GetHighFrequencyPatternIndex();
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High Frequency detected. Pattern Index: %d"), CurrentPatternIndex);
	}
}

int32 ABoss::GetHighIntensityPatternIndex() const
{
	for (int32 i = 0; i < BulletPatterns.Num(); ++i)
	{
		if (BulletPatterns[i].PatternType == EPatternType::Circle) // 예시: 고강도 패턴은 Circle로 설정
		{
			return i;
		}
	}
	return 0; // 기본값
}

int32 ABoss::GetLowFrequencyPatternIndex() const
{
	for (int32 i = 0; i < BulletPatterns.Num(); ++i)
	{
		if (BulletPatterns[i].PatternType == EPatternType::Straight) // 예시: 저주파수 패턴은 Straight로 설정
		{
			return i;
		}
	}
	return 0; // 기본값
}

int32 ABoss::GetLowMidFrequencyPatternIndex() const
{
	for (int32 i = 0; i < BulletPatterns.Num(); ++i)
	{
		if (BulletPatterns[i].PatternType == EPatternType::Wave) // 예시: LowMid 주파수 패턴은 Wave로 설정
		{
			return i;
		}
	}
	return 0; // 기본값
}

int32 ABoss::GetHighMidFrequencyPatternIndex() const
{
	for (int32 i = 0; i < BulletPatterns.Num(); ++i)
	{
		if (BulletPatterns[i].PatternType == EPatternType::Fan) // 예시: HighMid 주파수 패턴은 Fan으로 설정
		{
			return i;
		}
	}
	return 0; // 기본값
}

int32 ABoss::GetHighFrequencyPatternIndex() const
{
	for (int32 i = 0; i < BulletPatterns.Num(); ++i)
	{
		if (BulletPatterns[i].PatternType == EPatternType::RandomSpread) // 예시: High 주파수 패턴은 RandomSpread로 설정
		{
			return i;
		}
	}
	return 0; // 기본값
}

void ABoss::StartFiring()
{
	if (!GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABoss::FireBullet, FireRate, true);
	}

	if (!GetWorldTimerManager().IsTimerActive(PatternUpdateTimerHandle))
	{
		GetWorldTimerManager().SetTimer(PatternUpdateTimerHandle, this, &ABoss::UpdatePatternConditions, FireRate, true);
	}
}

void ABoss::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle);
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
