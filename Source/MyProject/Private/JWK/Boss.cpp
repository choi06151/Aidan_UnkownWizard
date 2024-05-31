#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
// #include "JWK/BossFSM.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	movementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movementComp"));

	// 보스
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT(
		"/Script/Engine.SkeletalMesh'/Game/JWK/Skeleton_Boss/ToonSkeleton/Characters/Meshes/SKM_ToonSkeleton.SKM_ToonSkeleton'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetWorldScale3D(FVector(1));
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
		GetMesh()->SetWorldScale3D(FVector(2, 2, 2));
	}

	bossEyeMesh_L = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bossEyeMesh_L"));
	bossEyeMesh_R = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bossEyeMesh_R"));
	bossEyeMesh_L->SetupAttachment(GetMesh(), TEXT("BossEye_L"));
	bossEyeMesh_R->SetupAttachment(GetMesh(), TEXT("BossEye_R"));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempEyeMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if(tempEyeMesh.Succeeded())
	{
		bossEyeMesh_L->SetStaticMesh(tempEyeMesh.Object);
		bossEyeMesh_R->SetStaticMesh(tempEyeMesh.Object);
		bossEyeMesh_L->SetRelativeLocation(FVector(3, 12, -5));
		bossEyeMesh_R->SetRelativeLocation(FVector(3, 12, 5));
		bossEyeMesh_L->SetWorldScale3D(FVector(0.075));
		bossEyeMesh_R->SetWorldScale3D(FVector(0.075));
	}
	
	// 지휘봉
	batonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("batonMesh"));
	batonMesh->SetupAttachment(GetMesh(), TEXT("Weapon_R"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBatonMesh(
		TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Baton/Baton.Baton'"));
	if (tempBatonMesh.Succeeded())
	{
		batonMesh->SetStaticMesh(tempBatonMesh.Object);
		batonMesh->SetRelativeLocationAndRotation(FVector(2, -1, 10), FRotator(-90, 65, -25));
		batonMesh->SetWorldScale3D(FVector(2));
	}

	capMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("capMesh"));
	capMesh->SetupAttachment(GetMesh(), TEXT("Hat"));
	
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempCapMesh(TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Hat/CapCartola.CapCartola'"));
	if(tempCapMesh.Succeeded())
	{
		capMesh->SetStaticMesh(tempCapMesh.Object);
		capMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 90));
		capMesh->SetWorldScale3D(FVector(1, 1, 1));
	}
	BulletSpawner = CreateDefaultSubobject<USpawn_Bullet>(TEXT("BulletSpawner"));

	FireRate = 1.0f;

	CurrentPatternIndex = 0;

	// DELEGATE Map 초기화
	PatternDelegates.Add(EPatternType::Straight, FPatternDelegate::CreateUObject(this, &ABoss::FireStraightPattern));
	PatternDelegates.Add(EPatternType::RandomStraight, FPatternDelegate::CreateUObject(this, &ABoss::FireRandomStraightPattern));
	PatternDelegates.Add(EPatternType::Fan, FPatternDelegate::CreateUObject(this, &ABoss::FireFanPattern));
	PatternDelegates.Add(EPatternType::Circle, FPatternDelegate::CreateUObject(this, &ABoss::FireCirclePattern));
	PatternDelegates.Add(EPatternType::Swirl, FPatternDelegate::CreateUObject(this, &ABoss::FireSwirlPattern));
	PatternDelegates.Add(EPatternType::Butterfly, FPatternDelegate::CreateUObject(this, &ABoss::FireButterflyPattern));
	PatternDelegates.Add(EPatternType::TrumpetFlower, FPatternDelegate::CreateUObject(this, &ABoss::FireTrumpetFlowerPattern));
	PatternDelegates.Add(EPatternType::Crescent, FPatternDelegate::CreateUObject(this, &ABoss::FireCrescentPattern));
	PatternDelegates.Add(EPatternType::Angel, FPatternDelegate::CreateUObject(this, &ABoss::FireAngelPattern));
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	if (BulletPatterns.Num() == 0)
		InitializeDefaultPatterns();
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float TimeElapsed = 0.0f;
	TimeElapsed += DeltaTime;

	// 1.5초 ~ 5.0초 랜덤 탄막 패턴 발사
	if (TimeElapsed > 1.5f && TimeElapsed <= 5.0f && !bIsDie)
	{
		StopFiring();
		ChangePattern();
		StartFiring();
	}
	// 5.0초 7.5초 발사 중지
	if (TimeElapsed > 5.0f && TimeElapsed <= 7.5f && !bIsDie)
	{
		StopFiring();
	}

	// 7.5초 초과시 다음 탄막 패턴 발사
	if (TimeElapsed > 7.5f && !bIsDie)
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
		const FBulletHellPattern& CurrentPattern = BulletPatterns[CurrentPatternIndex];
		// 현재 패턴 타입에 대한 델리게이트가 존재하는지 확인하고, 존재하면 해당 패턴 타입에 매핑된 메서드를 호출
		if (PatternDelegates.Contains(CurrentPattern.PatternType))
			PatternDelegates[CurrentPattern.PatternType].Execute(CurrentPattern);
	}
}

void ABoss::FireStraightPattern(const FBulletHellPattern& Pattern)
{
	// 직선 패턴 발사
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;
	FRotator BossRotation = GetActorRotation();
	BulletSpawner->SpawnPooledBullet(BossLocation, BossRotation, Pattern.BulletSpeed);

	UE_LOG(LogTemp, Warning, TEXT("Straight"));
	UE_LOG(LogTemp, Warning, TEXT("--------"));
}

void ABoss::FireRandomStraightPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;
	
	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float RandomY = FMath::FRandRange(-400.0f, 400.0f);
		float RandomZ = FMath::FRandRange(-200.0f, 100.0f);
		FVector SpawnLocation = FVector(BossLocation.X, BossLocation.Y + RandomY, BossLocation.Z + RandomZ);
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("RandomStraight"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

void ABoss::FireFanPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;
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
	BossLocation.Z = 300.0f;
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

void ABoss::FireSwirlPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	TArray<FVector> CircleShape;

	// 시계방향 회전을 위한 현재 시간
	float CurrentTime = GetWorld()->GetTimeSeconds();
	int32 NumberOfCircles = 4; // 원하는 원의 수

	for (int32 i = 0; i < NumberOfCircles; ++i)
	{
		float Radius = Pattern.PatternSize / 3.0f * (i + 1); // 각 원의 반지름
		float RotationOffset = (360.0f / NumberOfCircles) * i + (CurrentTime * 50.0f); // 각 원의 회전 오프셋, 50.0f는 회전 속도
		DefineSwirlShape(CircleShape, Pattern.NumberOfBullets, Radius, RotationOffset);
	}

	for (const FVector& Offset : CircleShape)
	{
		FVector SpawnLocation = BossLocation + Offset;

		// 총알이 보스의 정면을 향하도록 회전 설정
		FRotator SpawnRotation = GetActorForwardVector().Rotation();

		// 총알을 생성
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetCircle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

void ABoss::DefineSwirlShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float Radius, float RotationOffset)
{
	AngleStep = 360.0f / NumberOfPoints;

	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		float Angle = i * AngleStep + RotationOffset;
		float Radian = FMath::DegreesToRadians(Angle);
		float Y = FMath::Cos(Radian) * Radius;
		float Z = FMath::Sin(Radian) * Radius;
		OutShape.Add(FVector(0.0f, Y, Z));
	}
}

void ABoss::FireButterflyPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;
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
	BossLocation.Z = 300.0f;

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
	BossLocation.Z = 300.0f;

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
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABoss::FireBullet, FireRate, true, 0.0f);
}

void ABoss::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ABoss::ChangePattern()
{
	CurrentPatternIndex = (CurrentPatternIndex + 1) % BulletPatterns.Num();
	// int32 RandomIndex = FMath::RandRange(0, BulletPatterns.Num() - 1);

	// // 현재 패턴 인덱스를 랜덤한 값으로 설정
	// CurrentPatternIndex = RandomIndex;
}

void ABoss::InitializeDefaultPatterns()
{
	// Interval : 발사 주기 ( 높을수록 발사 빈도수 적어짐)	// PatternSize : 패턴의 크기
	// NumberOfBullets : 한 번에 발사되는 총알의 수		// BulletSpeed : 총알의 속도


	// 직선
	FBulletHellPattern StraightPattern;
	StraightPattern.PatternType = EPatternType::Straight;
	StraightPattern.NumberOfBullets = 5;
	StraightPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(StraightPattern);
	
	// 랜덤 직선
	FBulletHellPattern RandomStraightPattern;
	RandomStraightPattern.PatternType = EPatternType::RandomStraight;
	RandomStraightPattern.NumberOfBullets = 6;
	RandomStraightPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(RandomStraightPattern);
	
	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
	FanPattern.NumberOfBullets = 7; // 부채꼴 패턴에서 발사할 총알 수
	FanPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(FanPattern);
	
	// 원형 패턴
	FBulletHellPattern CirclePattern;
	CirclePattern.PatternType = EPatternType::Circle;
	CirclePattern.PatternSize = 400.0f; // 원형 패턴의 크기 설정
	CirclePattern.NumberOfBullets = 12; // 총알의 수
	CirclePattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(CirclePattern);

	// 유도 원형 패턴
	FBulletHellPattern TargetSwirlPattern;
	TargetSwirlPattern.PatternType = EPatternType::Swirl;
	TargetSwirlPattern.PatternSize = 150.0f;	// 원형 패턴의 크기 설정
	TargetSwirlPattern.NumberOfBullets = 4;	// 총알의 수
	TargetSwirlPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(TargetSwirlPattern);
	
	// 나팔꽃 패턴
	FBulletHellPattern TrumpetFlowerPattern;
	TrumpetFlowerPattern.PatternType = EPatternType::TrumpetFlower;
	TrumpetFlowerPattern.NumberOfBullets = 30;
	TrumpetFlowerPattern.BulletSpeed = 300.0f;
	TrumpetFlowerPattern.FanAngle = 180.0f;
	BulletPatterns.Add(TrumpetFlowerPattern);
	
	// 나비 패턴
	FBulletHellPattern ButterflyPattern;
	ButterflyPattern.PatternType = EPatternType::Butterfly;
	ButterflyPattern.NumberOfBullets = 20;
	ButterflyPattern.BulletSpeed = 350.0f;
	BulletPatterns.Add(ButterflyPattern);
	
	
	// 초승달 패턴
	FBulletHellPattern CrescentPattern;
	CrescentPattern.PatternType = EPatternType::Crescent;
	CrescentPattern.NumberOfBullets = 20;
	CrescentPattern.BulletSpeed = 300.0f;
	CrescentPattern.CrescentAngle = 120.0f;
	BulletPatterns.Add(CrescentPattern);

	// // 천사 패턴
	// FBulletHellPattern AngelPattern;
	// AngelPattern.PatternType = EPatternType::Angel;
	// AngelPattern.NumberOfBullets = 14;
	// AngelPattern.BulletSpeed = 250.0f;
	// BulletPatterns.Add(AngelPattern);
}
