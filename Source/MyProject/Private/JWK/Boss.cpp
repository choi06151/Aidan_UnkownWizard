#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
		GetMesh()->SetWorldScale3D(FVector(2, 2, 2));
	}

	batonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("batonMesh"));
	batonMesh->SetupAttachment(GetMesh(), TEXT("Weapon_R"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBatonMesh(
		TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Baton/Baton.Baton'"));
	if (tempBatonMesh.Succeeded())
	{
		batonMesh->SetStaticMesh(tempBatonMesh.Object);
		batonMesh->SetRelativeLocationAndRotation(FVector(2, -1, 10), FRotator(-90, 65, -25));
		batonMesh->SetWorldScale3D(FVector(2, 2, 2));
	}

	BulletSpawner = CreateDefaultSubobject<USpawn_Bullet>(TEXT("BulletSpawner"));

	FireRate = 1.0f;

	CurrentPatternIndex = 0;
	CurrentTimeIndex = 0; // 추가: CurrentTimeIndex 초기화
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	if (BulletPatterns.Num() == 0)
		InitializeDefaultPatterns();

	/*StartFiring();*/
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//static float TimeElapsed = 0.0f;
	//TimeElapsed += DeltaTime;
	//
	//// 1.5초 ~ 3.0초 랜덤 탄막 패턴 발사
	//if (TimeElapsed > 1.5f && TimeElapsed <= 3.0f && !IsDie)
	//{
	//	StopFiring();
	//	ChangePattern();
	//	StartFiring();
	//}
	//// 3.0초 5.5초 발사 중지
	//if (TimeElapsed > 3.0f && TimeElapsed <= 5.5f && !IsDie)
	//{
	//	StopFiring();
	//}

	//// 5.5초 초과시 다음 탄막 패턴 발사
	//if (TimeElapsed > 5.5f && !IsDie)
	//{
	//	ChangePattern();
	//	StartFiring();

	//	// 타이머 리셋
	//	TimeElapsed = 0.0f;
	//}
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// JSON 데이터를 로드하고 패턴 조건을 설정하는 함수
void ABoss::LoadMusicDataAndSetPatterns(const FString& JsonFilePath, const FString& MusicFilePath)
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// 패턴 조건들을 초기화
			PatternConditions.Empty();
			CurrentTimeIndex = 0;

			float Tempo = JsonObject->GetNumberField(TEXT("tempo"));
			const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField(TEXT("beats"));
			const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField(TEXT("intensity"));
			const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField(TEXT("low"));
			const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField(TEXT("low_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField(TEXT("high_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField(TEXT("high"));

			for (int32 i = 0; i < IntensityArray.Num(); ++i)
			{
				FPatternConditions Conditions;

				Conditions.bIsHighIntensity = IntensityArray[i]->AsNumber() > 0.3f;
				Conditions.bIsLowFrequency = LowArray[i]->AsNumber() > 0.2f;
				Conditions.bIsLowMidFrequency = LowMidArray[i]->AsNumber() > 0.2f;
				Conditions.bIsHighMidFrequency = HighMidArray[i]->AsNumber() > 0.2f;
				Conditions.bIsHighFrequency = HighArray[i]->AsNumber() > 0.2f;

				for (const auto& BeatValue : BeatsArray)
				{
					float BeatTime = BeatValue->AsNumber();
					if (FMath::Abs(i - BeatTime) < 0.1f)
					{
						Conditions.bIsOnBeat = true;
						break;
					}
				}

				Conditions.bIsTempoAbove90 = Tempo > 90.0f;
				Conditions.bIsTempoAbove100 = Tempo > 100.0f;
				Conditions.bIsTempoAbove110 = Tempo > 110.0f;

				PatternConditions.Add(Conditions);
			}
			UE_LOG(LogTemp, Warning, TEXT("ABoss::Loaded JSON with %d conditions."), PatternConditions.Num());

			// 패턴 조건 업데이트 타이머 설정 (1초 간격으로 패턴 업데이트)
			GetWorldTimerManager().SetTimer(PatternUpdateTimerHandle, this, &ABoss::UpdatePatternConditions, 1.0f, true);

			// 음악 재생 시작
			UE_LOG(LogTemp, Warning, TEXT("ABoss::Playing music synchronized with pattern conditions."));
			if (USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
			{
				UGameplayStatics::PlaySound2D(this, Music);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ABoss::Failed to load music: %s"), *MusicFilePath);
			}

			// 탄막 발사 시작
			StartFiring();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::Failed to load JSON file: %s"), *JsonFilePath);
	}
}

// 패턴 조건을 업데이트하는 함수
void ABoss::UpdatePatternConditions()
{
	if (PatternConditions.IsValidIndex(CurrentTimeIndex))
	{
		FPatternConditions CurrentCondition = PatternConditions[CurrentTimeIndex];

		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Checking conditions at time index %d"), CurrentTimeIndex);

		// 패턴 조건을 기반으로 패턴 변경 로직을 추가합니다.
		if (CurrentCondition.bIsHighIntensity)
		{
			// 높은 강도 조건이 만족되면 패턴 속도를 높입니다.
			for (auto& Pattern : BulletPatterns)
			{
				Pattern.BulletSpeed += 50.0f; // 속도를 50만큼 증가
			}
			UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High intensity condition met. Increasing bullet speed."));
		}

		if (CurrentCondition.bIsLowFrequency)
		{
			// 저주파수 대역이 활성화될 때 패턴을 부채꼴로 변경
			CurrentPatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
				{
					return Pattern.PatternType == EPatternType::Fan;
				});
			UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Low frequency condition met. Changing pattern to Fan."));
		}

		if (CurrentCondition.bIsLowMidFrequency)
		{
			// 저중주파수 대역이 활성화될 때 패턴을 원형으로 변경
			CurrentPatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
				{
					return Pattern.PatternType == EPatternType::Circle;
				});
			UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Low-mid frequency condition met. Changing pattern to Circle."));
		}

		if (CurrentCondition.bIsHighMidFrequency)
		{
			// 중고주파수 대역이 활성화될 때 패턴을 나비로 변경
			CurrentPatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
				{
					return Pattern.PatternType == EPatternType::Butterfly;
				});
			UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High-mid frequency condition met. Changing pattern to Butterfly."));
		}

		if (CurrentCondition.bIsHighFrequency)
		{
			// 고주파수 대역이 활성화될 때 패턴을 나팔꽃으로 변경
			CurrentPatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
				{
					return Pattern.PatternType == EPatternType::TrumpetFlower;
				});
			UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: High frequency condition met. Changing pattern to TrumpetFlower."));
		}

		if (CurrentCondition.bIsOnBeat)
		{
			// 비트가 발생할 때 발사 주기를 줄입니다.
			FireRate = FMath::Max(0.1f, FireRate - 0.1f); // 최소 0.1초
		}

		CurrentTimeIndex++;
		//if (CurrentCondition.bIsTempoAbove110)
		//{
		//	// 템포가 110 이상일 때 패턴을 직선으로 변경
		//	CurrentPatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
		//		{
		//			return Pattern.PatternType == EPatternType::Straight;
		//		});
		//	UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Tempo above 110. Changing pattern to Straight."));
		//}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: No more conditions to process."));
		GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle); // Stop the timer if no more conditions
	}
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
			
		case EPatternType::Butterfly:
			FireButterflyPattern(CurrentPattern);
			break;

		case EPatternType::TrumpetFlower:
			FireTrumpetFlowerPattern(CurrentPattern);
			break;

		case EPatternType::Crescent:
			FireCrescentPattern(CurrentPattern);
			break;

		case EPatternType::Angel:
			FireAngelPattern(CurrentPattern);
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
		float RandomY = FMath::FRandRange(-400.0f, 400.0f);
		float RandomZ = FMath::FRandRange(-200.0f, 100.0f);
		FVector SpawnLocation = FVector(BossLocation.X,  BossLocation.Y + RandomY, BossLocation.Z + RandomZ);
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

void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	float Radius = Pattern.PatternSize / 3.0f;
	FRotator BossRotation = GetActorRotation();

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (360.0f / Pattern.NumberOfBullets);
		float Rad = FMath::DegreesToRadians(Angle);

		FVector Offset = FVector(0.0f, FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius);
		FVector SpawnLocation = BossLocation + Offset;

		FRotator SpawnRotation = BossRotation;

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Circle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::FireButterflyPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	float Radius = Pattern.PatternSize / 3.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (360.0f / Pattern.NumberOfBullets);
		float Rad = FMath::DegreesToRadians(Angle);
		FVector Offset = FVector(FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius, 0.0f);
		FVector SpawnLocation = BossLocation + Offset;
		// 정면 방향을 향하도록 회전
		FRotator SpawnRotation = (SpawnLocation - BossLocation).Rotation();
		SpawnRotation.Yaw += Angle;
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}
    
	UE_LOG(LogTemp, Warning, TEXT("Butterfly"));
	UE_LOG(LogTemp, Warning, TEXT("---------"));
}

void ABoss::FireTrumpetFlowerPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (Pattern.FanAngle / (Pattern.NumberOfBullets - 1)) - Pattern.FanAngle / 2;
		FRotator SpawnRotation = GetActorRotation();
		SpawnRotation.Yaw += Angle;
		// 정면 방향을 향하도록 회전
		BulletSpawner->SpawnPooledBullet(BossLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("TrumpetFlower"));
	UE_LOG(LogTemp, Warning, TEXT("-------------"));
}

void ABoss::FireCrescentPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float Angle = i * (Pattern.CrescentAngle / (Pattern.NumberOfBullets - 1)) - Pattern.CrescentAngle / 2;
		FRotator SpawnRotation = GetActorRotation();
		SpawnRotation.Yaw += Angle;
		// 정면 방향을 향하도록 회전
		BulletSpawner->SpawnPooledBullet(BossLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Crescent"));
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireAngelPattern(const FBulletHellPattern& Pattern)
{
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
	// Interval : 발사 주기 ( 높을수록 발사 빈도수 적어짐)	// PatternSize : 패턴의 크기
	// NumberOfBullets : 한 번에 발사되는 총알의 수		// BulletSpeed : 총알의 속도


	// 직선
	FBulletHellPattern StraightPattern;
	StraightPattern.PatternType = EPatternType::Straight;
	StraightPattern.Interval = 1.5f;
	StraightPattern.NumberOfBullets = 5;
	StraightPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(StraightPattern);

	// 랜덤 직선
	FBulletHellPattern RandomStraightPattern;
	RandomStraightPattern.PatternType = EPatternType::RandomStraight;
	RandomStraightPattern.Interval = 2.0f;
	RandomStraightPattern.NumberOfBullets = 6;
	RandomStraightPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(RandomStraightPattern);

	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.Interval = 1.5f;
	FanPattern.FanAngle = 90.0f;			// 부채꼴 패턴의 각도 설정
    FanPattern.NumberOfBullets = 7;			// 부채꼴 패턴에서 발사할 총알 수
	FanPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(FanPattern);

	// 원형 패턴
	FBulletHellPattern CirclePattern;
	CirclePattern.PatternType = EPatternType::Circle;
	CirclePattern.Interval = 2.0f;
    CirclePattern.PatternSize = 400.0f; // 원형 패턴의 크기 설정
	CirclePattern.NumberOfBullets = 12; // 총알의 수
	CirclePattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(CirclePattern);

    // 나팔꽃 패턴
    FBulletHellPattern TrumpetFlowerPattern;
    TrumpetFlowerPattern.PatternType = EPatternType::TrumpetFlower;
    TrumpetFlowerPattern.Interval = 1.0f;
    TrumpetFlowerPattern.NumberOfBullets = 30;
    TrumpetFlowerPattern.BulletSpeed = 300.0f;
    TrumpetFlowerPattern.FanAngle = 180.0f;
    BulletPatterns.Add(TrumpetFlowerPattern);
	
    // 나비 패턴
    FBulletHellPattern ButterflyPattern;
    ButterflyPattern.PatternType = EPatternType::Butterfly;
    ButterflyPattern.Interval = 1.0f;
    ButterflyPattern.NumberOfBullets = 20;
    ButterflyPattern.BulletSpeed = 350.0f;
    BulletPatterns.Add(ButterflyPattern);


    // 초승달 패턴
    FBulletHellPattern CrescentPattern;
    CrescentPattern.PatternType = EPatternType::Crescent;
    CrescentPattern.Interval = 1.5f;
    CrescentPattern.NumberOfBullets = 20;
    CrescentPattern.BulletSpeed = 300.0f;
    CrescentPattern.CrescentAngle = 120.0f;
    BulletPatterns.Add(CrescentPattern);
    
    // // 천사 패턴
    // FBulletHellPattern AngelPattern;
    // AngelPattern.PatternType = EPatternType::Angel;
    // AngelPattern.Interval = 2.0f;
    // AngelPattern.NumberOfBullets = 14;
    // AngelPattern.BulletSpeed = 250.0f;
    // BulletPatterns.Add(AngelPattern);
}
