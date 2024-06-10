#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/BossFSM.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"

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

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempEyeMesh(
		TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (tempEyeMesh.Succeeded())
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

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempCapMesh(
		TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Hat/CapCartola.CapCartola'"));
	if (tempCapMesh.Succeeded())
	{
		capMesh->SetStaticMesh(tempCapMesh.Object);
		capMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 90));
		capMesh->SetWorldScale3D(FVector(1, 1, 1));
	}

	bossFSM = CreateDefaultSubobject<UBossFSM>(TEXT("bossFSM"));

	BulletSpawner = CreateDefaultSubobject<USpawn_Bullet>(TEXT("BulletSpawner"));

	FireRate = 1.0f;

	CurrentPatternIndex = 0;
	//////////////////////////////////////// 음악분석 관련 추가 ////////////////////////////////////////
	CurrentTimeIndex = 0; // CurrentTimeIndex 초기화

	// DELEGATE Map 초기화
	/* 랜덤직선 */
	PatternDelegates.Add(EPatternType::RandomStraight,
	                     FPatternDelegate::CreateUObject(this, &ABoss::FireRandomStraightPattern));

	/* 부채꼴 */
	PatternDelegates.Add(EPatternType::Fan, FPatternDelegate::CreateUObject(this, &ABoss::FireFanPattern));

	/* 타겟 원형 */
	PatternDelegates.Add(EPatternType::TargetCircle,
	                     FPatternDelegate::CreateUObject(this, &ABoss::FireTargetCirclePattern));

	/* 소용돌이(십자가) */
	PatternDelegates.Add(EPatternType::Swirl, FPatternDelegate::CreateUObject(this, &ABoss::FireSwirlPattern));

	/* 타겟 십자가 */
	PatternDelegates.Add(EPatternType::TargetCross,
	                     FPatternDelegate::CreateUObject(this, &ABoss::FireTargetCrossPattern));

	/* 벽 */
	PatternDelegates.Add(EPatternType::Wall, FPatternDelegate::CreateUObject(this, &ABoss::FireWallPattern));

	/* 오각형 */
	PatternDelegates.Add(EPatternType::Octagon,
	                     FPatternDelegate::CreateUObject(this, &ABoss::FireTargetOctagonPattern));

	/* 추가 예정 */
	PatternDelegates.Add(EPatternType::Angel, FPatternDelegate::CreateUObject(this, &ABoss::FireAngelPattern));
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	if (BulletPatterns.Num() == 0)
		InitializeDefaultPatterns();

	player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found"));
		return;
	}

	// FireBullet();
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if(bIsGameStart)		// GameStart 버튼이 눌리고
		MusicStart();*/

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// TimeElapsed += DeltaTime;
	//
	// // 5초 간격으로 저장된 패턴 발사
	// if (TimeElapsed >= 5.0f && bTestFire)
	// {
	// 	bTestFire = false;
	// 	FireBullet();
	// }
	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABoss::MusicStart()
{
	if (SpawnWidget != nullptr)
	{
		SpawnWidget->CurtainOpenAnim();
		cnt = 0;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 2.0f, false);
	}
}

void ABoss::HandleState()
{
	switch (cnt)
	{
	case 0:
		bIsWalk = true;
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 2.5f, false); // 7.5 - 5 = 2.5
		break;
	case 1:
		bIsArrive = true;
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 1.5f, false); // 10 - 7.5 = 2.5
		break;
	case 2:
		if (SpawnWidget != nullptr)
		{
			SpawnWidget->CurtainCloseAnim();
		}
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 3.0f, false); // 20 - 10 = 10
		break;
	case 3:
		if (SpawnWidget != nullptr)
		{
			SpawnWidget->CurtainShakeAnim();
			SpawnWidget->MusicPlay();
		}
		bIsAttack = true;
		cnt++;
		break;
	default:
		break;
	}
}

//////////////////////////////////////// 음악분석 관련 ////////////////////////////////////////
void ABoss::LoadMusicDataAndSetPatterns(const FString& MusicTitle, const FString& MusicFilePath)
{
	if (AnalyzedDataMap.Contains(MusicTitle))
	{
		auto AnalyzedData = AnalyzedDataMap[MusicTitle];
		float Tempo = AnalyzedData.Key;
		FinalPatternData = AnalyzedData.Value;
		CurrentTimeIndex = 0;

		// FireRate 설정 (템포에 맞춰 조정)
		float BeatLength = 60.0f / Tempo; // 한 비트의 길이
		FireRate = BeatLength * 4; // 4/4 박자를 위해 4배로 설정

		UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Loaded pre-analyzed conditions for %s."),
		       *MusicTitle);

		// 패턴 조건 업데이트 타이머 설정 (한 바 단위로 패턴 업데이트)
		GetWorldTimerManager().SetTimer(PatternUpdateTimerHandle, this, &ABoss::UpdatePatternConditions, FireRate, true);

		// 음악 재생 시작
		UE_LOG(LogTemp, Warning,
		       TEXT("ABoss::LoadMusicDataAndSetPatterns: Playing music synchronized with pattern conditions."));
		if (USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
		{
			UGameplayStatics::PlaySound2D(this, Music);

			// // 탄막 발사 시작
			// StartFiring();
			FireBullet();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load music: %s"),
			       *MusicFilePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load pre-analyzed data for: %s"),
		       *MusicTitle);
	}
}

// 패턴 조건을 업데이트하는 함수
void ABoss::UpdatePatternConditions()
{
	if (FinalPatternData.IsValidIndex(CurrentTimeIndex))
	{
		// 현재 시간 인덱스에 해당하는 패턴 데이터를 가져옴
		FFinalPatternData CurrentData = FinalPatternData[CurrentTimeIndex];

		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Checking conditions at time index %d"),
		       CurrentTimeIndex);

		// 패턴 인덱스를 설정
		CurrentPatternIndex = CurrentData.PatternIndex;

		// 모든 탄막 패턴의 속도를 현재 데이터의 속도로 설정
		for (auto& Pattern : BulletPatterns)
		{
			Pattern.BulletSpeed = CurrentData.BulletSpeed;
		}

		// 패턴 델리게이트가 존재하는지 확인하고, 존재하면 실행
		if (PatternDelegates.Contains(BulletPatterns[CurrentPatternIndex].PatternType))
		{
			PatternDelegates[BulletPatterns[CurrentPatternIndex].PatternType].Execute(
				BulletPatterns[CurrentPatternIndex]);
		}

		UE_LOG(LogTemp, Warning,
		       TEXT("ABoss::UpdatePatternConditions: Applying pattern index %d with bullet speed %f at time index %d"),
		       CurrentData.PatternIndex, CurrentData.BulletSpeed, CurrentTimeIndex);

		// 4/4 박자 단위로 인덱스를 증가시켜 다음 조건을 확인하도록 함
		CurrentTimeIndex++; // 4 비트마다 한 번씩 업데이트
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: No more conditions to process."));
		GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle); // 타이머를 중지하여 패턴 업데이트를 멈춤
		StopFiring(); // 탄막 발사 중지
	}
}

// JSON 데이터를 로드하고 패턴 조건을 설정하는 함수
void ABoss::PreAnalyzeMusicData(const FString& MusicTitle, const FString& JsonFilePath)
{
	// JSON 파일을 문자열로 로드
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		// JSON 파서를 위한 객체 생성
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		// JSON 문자열을 파싱하여 JsonObject로 변환
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// 최종 패턴 데이터를 저장할 배열
			TArray<FFinalPatternData> FinalPatternDataArray;

			// JSON 객체에서 템포, 비트, 강도, 주파수 대역별 데이터 값을 가져옴
			float Tempo = JsonObject->GetNumberField(TEXT("tempo"));
			const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField(TEXT("beats"));
			const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField(TEXT("intensity"));
			const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField(TEXT("low"));
			const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField(TEXT("low_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField(TEXT("high_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField(TEXT("high"));

			// 각 강도 값에 대해 반복하여 패턴 데이터를 생성
			for (int32 i = 0; i < IntensityArray.Num(); ++i)
			{
				FFinalPatternData FinalData;

				// 강도 값이 0.3 이상인 경우 탄막 속도를 증가시킴
				if (IntensityArray[i]->AsNumber() > 0.3f)
				{
					FinalData.BulletSpeed += 50.0f;
				}

				// 주파수 대역별 값에 따라 패턴 인덱스를 설정
				if (LowArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
					{
						return Pattern.PatternType == EPatternType::TargetCross;
					});
				}

				else if (LowMidArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
					{
						return Pattern.PatternType == EPatternType::TargetCircle;
					});
				}

				else if (HighMidArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
					{
						return Pattern.PatternType == EPatternType::TargetCross;
					});
				}

				else if (HighArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
					{
						return Pattern.PatternType == EPatternType::Octagon;
					});
				}

				// 비트 시간과 현재 인덱스가 가까운 경우 탄막 속도를 감소시킴
				// 탄막 발사 타이밍을 음악의 비트와 동기화하여 더 리드미컬하게 만들기 위함임
				// 제대로 되는지는 모르겠음
				for (const auto& BeatValue : BeatsArray)
				{
					float BeatTime = BeatValue->AsNumber();
					if (FMath::Abs(i * 4 * (60.0 / Tempo) - BeatTime) < 0.1f)
					{
						// 현재 인덱스가 비트 시간과 매우 가까운 경우 탄막 속도를 감소시킴
						FinalData.BulletSpeed = FMath::Max(0.1f, FinalData.BulletSpeed - 0.1f);
						break;
					}
				}

				// 생성된 패턴 데이터를 배열에 추가
				FinalPatternDataArray.Add(FinalData);
			}
			// Tempo 값을 TMap에 함께 저장합니다.
			AnalyzedDataMap.Add(MusicTitle, TPair<float, TArray<FFinalPatternData>>(Tempo, FinalPatternDataArray));

			UE_LOG(LogTemp, Warning, TEXT("ABoss::PreAnalyzeMusicData: Pre-analyzed %d conditions for %s."),
			       FinalPatternDataArray.Num(), *MusicTitle);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::Failed to load JSON file: %s"), *JsonFilePath);
	}
}

void ABoss::PreAnalyzeAllMusicData()
{
	// 각 음악에 대한 JSON 파일 경로 설정
	FString ProjectDir = UKismetSystemLibrary::GetProjectDirectory();
	FString ButterflyJsonPath = ProjectDir + TEXT("Content/Data/butterfly.json");
	FString EliseJsonPath = ProjectDir + TEXT("Content/Data/Elise.json");
	FString LacrimosaJsonPath = ProjectDir + TEXT("Content/Data/Lacrimosa.json");

	// 각 음악에 대해 미리 분석 수행
	PreAnalyzeMusicData(TEXT("butterfly"), ButterflyJsonPath);
	PreAnalyzeMusicData(TEXT("Elise"), EliseJsonPath);
	PreAnalyzeMusicData(TEXT("Lacrimosa"), LacrimosaJsonPath);
}

////////////////////////////////////////////////// 발사 관련 함수 //////////////////////////////////////////////////
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


////////////////////////////////////////////////// 탄막 패턴들 //////////////////////////////////////////////////

/*
//플레이어 위치
FVector PlayerLocation = player->GetActorLocation();

// 플레이어를 향한 방향 계산
FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
FRotator BossRotation = Direction.Rotation();

// 총알 스폰
BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation, Pattern.BulletSpeed);

// 랜덤 범위
GetActorRightVector() * FMath::RandRange(-200, 200);
*/

// 랜덤 직선 : YZ 평면 랜덤좌표에서 총알들이 발사됨
void ABoss::FireRandomStraightPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorUpVector() * -550;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float RandomY = FMath::FRandRange(-400.0f, 400.0f);
		float RandomZ = FMath::FRandRange(0, 150.0f);
		FVector SpawnLocation = FVector(BossLocation.X, BossLocation.Y + RandomY, BossLocation.Z + RandomZ);
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	
	UE_LOG(LogTemp, Warning, TEXT("RandomStraight"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}


// 부채꼴 : 부채꼴 모양으로 퍼지며 발사됨
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


// 유도 원형 : 발사될 때는 원형이다가 Player에게 다가올 때 쯤에는 한 점으로 모임
void ABoss::FireTargetCirclePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(-200, 200) + GetActorUpVector() *
		FMath::RandRange(-150, 150);
	float Radius = Pattern.PatternSize / 3.0f; // 패턴 크기에 따른 반지름

	/////////////// 플레이어 위치 ///////////////
	FVector PlayerLocation = player->GetActorLocation();

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		// YZ 평면에서 랜덤 위치 생성
		float Angle = FMath::FRandRange(0.0f, 360.0f);
		float Rad = FMath::DegreesToRadians(Angle);
		FVector Offset = FVector(0.0f, FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius);
		FVector SpawnLocation = BossLocation + Offset;

		// 플레이어를 향한 방향 계산
		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();

		// 총알 스폰
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 

	UE_LOG(LogTemp, Warning, TEXT("Circle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}


// 소용돌이? : 한 번만 호출될 경우 그냥 +(십자) 모양임. 발사 횟수가 누적될 때 마다 시계방향으로 조금씩 회전되어 발사됨
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

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 ////////////////////////////////////////
	
	UE_LOG(LogTemp, Warning, TEXT("TargetCircle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

// 소용돌이 패턴 정의
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


// 유도 십자가 : + 모양의 탄막이 플레이어의 위치로 모임
void ABoss::FireTargetCrossPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(0, 400) + GetActorUpVector() * FMath::RandRange(0, 400);

	// 보스의 전방 벡터
	FVector Forward = GetActorForwardVector();

	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	// 탄환 발사 위치 설정
	FVector CrossOffsets[] = {
		FVector(0, 0, 0), // 중심
		FVector(0, 100, 0), // 위쪽 중간
		FVector(0, 200, 0), // 위쪽
		FVector(0, -100, 0), // 아래쪽 중간
		FVector(0, -200, 0), // 아래쪽
		FVector(0, 0, 100), // 앞쪽 중간
		FVector(0, 0, 200), // 앞쪽
		FVector(0, 0, -100), // 뒤쪽 중간
		FVector(0, 0, -200) // 뒤쪽
	};

	for(int32 j=0; j<2; j++)
	{
		for (int32 i = 0; i < 9; ++i)
		{
			FVector SpawnLocation = BossLocation + Forward * CrossOffsets[i].X + FVector::RightVector * CrossOffsets[i].Y + FVector::UpVector * CrossOffsets[i].Z;
			// 플레이어를 향한 방향 계산
			FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
			FRotator SpawnRotation = Direction.Rotation();
			BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
		}
	}

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 ////////////////////////////////////////
	
	UE_LOG(LogTemp, Warning, TEXT("Cross"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}


// 벽 : 3 X 9 (가로 X 세로)의 벽 중에서 세로 2줄의 빈 공간이 생기고 발사. 누적될 때 마다 빈 공간(세로 2줄)이 옆으로 이동함
void ABoss::FireWallPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	// 탄환 발사 위치 설정
	FVector Wall[] = {
		FVector(0, 200, -300),
		FVector(0, 150, -300),
		FVector(0, 100, -300),
		FVector(0, 50, -300),
		FVector(0, 0, -300),
		FVector(0, -50, -300),
		FVector(0, -100, -300),
		FVector(0, -150, -300),
		FVector(0, -200, -300),

		FVector(0, 200, -200),
		FVector(0, 150, -200),
		FVector(0, 100, -200),
		FVector(0, 50, -200),
		FVector(0, 0, -200),
		FVector(0, -50, -200),
		FVector(0, -100, -200),
		FVector(0, -150, -200),
		FVector(0, -200, -200),

		FVector(0, 200, -100),
		FVector(0, 150, -100),
		FVector(0, 100, -100),
		FVector(0, 50, -100),
		FVector(0, 0, -100),
		FVector(0, -50, -100),
		FVector(0, -100, -100),
		FVector(0, -150, -100),
		FVector(0, -200, -100),
	};

	int WallSize = sizeof(Wall) / sizeof(Wall[0]); // 배열의 크기
	int WallWidth = 9; // YZ 평면에서 한 줄의 벽 개수

	for (int i = 0; i < WallSize; i++)
	{
		// 비어있는 열이 아닌 경우에만 벽 생성
		if (i % WallWidth != CurrentEmptyLine && i % WallWidth != CurrentEmptyLine + 1 && i % WallWidth != CurrentEmptyLine + 2)
		{
			FVector SpawnLocation = BossLocation + GetActorForwardVector() * Wall[i].X + GetActorRightVector() * Wall[i].Y + GetActorUpVector() * Wall[i].Z;

			BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation, Pattern.BulletSpeed);
		}
	}

	// 비어있는 열을 오른쪽으로 이동
	CurrentEmptyLine = (CurrentEmptyLine + 1) % WallWidth;

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 

	UE_LOG(LogTemp, Warning, TEXT("Wall"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}


void ABoss::FireTargetOctagonPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator BossRotation = GetActorRotation();

	// 탄환 발사 위치 설정
	FVector Octagon[] = {
		FVector(0.0f, 0.0f, 250.0f),
		FVector(0.0f, -79.255f, 192.42f),
		FVector(0.0f, -158.51f, 134.835f),
		FVector(0.0f, -237.765f, 77.255f),
		FVector(0.0f, -207.49f, -15.915f),
		FVector(0.0f, -177.22f, -109.085f),
		FVector(0.0f, -146.945f, -202.255f),
		FVector(0.0f, -48.98f, -202.255f),
		FVector(0.0f, 48.98f, -202.255f),
		FVector(0.0f, 146.945f, -202.255f),
		FVector(0.0f, 177.22f, -109.085f),
		FVector(0.0f, 207.49f, -15.915f),
		FVector(0.0f, 237.765f, 77.255f),
		FVector(0.0f, 158.51f, 134.835f),
		FVector(0.0f, 79.255f, 192.42f),
		FVector(0.0f, 0.0f, 250.0f)
	};

	// 보스의 전방 벡터
	FVector Forward = GetActorForwardVector();

	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	int StarSize = sizeof(Octagon) / sizeof(Octagon[0]); // 배열의 크기

	for (int i = 0; i < StarSize; i++)
	{
		FVector SpawnLocation = BossLocation + GetActorForwardVector() * Octagon[i].X + GetActorRightVector() * Octagon[i].Y + GetActorUpVector() * Octagon[i].Z;

		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BulletTimer;
	float BulletTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("----TestFire Is True----"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	}), BulletTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 

	UE_LOG(LogTemp, Warning, TEXT("Octagon"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}


void ABoss::FireAngelPattern(const FBulletHellPattern& Pattern)
{
}


////////////////////////////////////////////////// 패턴들 특성 //////////////////////////////////////////////////
void ABoss::InitializeDefaultPatterns()
{
	// Interval : 발사 주기 ( 높을수록 발사 빈도수 적어짐)	// PatternSize : 패턴의 크기
	// NumberOfBullets : 한 번에 발사되는 총알의 수		// BulletSpeed : 총알의 속도

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
	FBulletHellPattern TargetCirclePattern;
	TargetCirclePattern.PatternType = EPatternType::TargetCircle;
	TargetCirclePattern.PatternSize = 300.0f; // 원형 패턴의 크기 설정
	TargetCirclePattern.NumberOfBullets = 12; // 총알의 수
	TargetCirclePattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(TargetCirclePattern);
	
	// 소용돌이(십자가) 
	FBulletHellPattern SwirlPattern;
	SwirlPattern.PatternType = EPatternType::Swirl;
	SwirlPattern.PatternSize = 150.0f; // 원형 패턴의 크기 설정
	SwirlPattern.NumberOfBullets = 4; // 총알의 수
	SwirlPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(SwirlPattern);
	
	// 유도 십자가
	FBulletHellPattern TargetCrossPattern;
	TargetCrossPattern.PatternType = EPatternType::TargetCross;
	TargetCrossPattern.NumberOfBullets = 20;
	TargetCrossPattern.BulletSpeed = 350.0f;
	BulletPatterns.Add(TargetCrossPattern);
	
	// 벽 패턴
	FBulletHellPattern WallPatter;
	WallPatter.PatternType = EPatternType::Wall;
	WallPatter.NumberOfBullets = 30;
	WallPatter.BulletSpeed = 300.0f;
	BulletPatterns.Add(WallPatter);
	
	// 유도 오각형
	FBulletHellPattern OctagonPattern;
	OctagonPattern.PatternType = EPatternType::Octagon;
	OctagonPattern.NumberOfBullets = 30;
	OctagonPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(OctagonPattern);

	// // 천사 패턴
	// FBulletHellPattern AngelPattern;
	// AngelPattern.PatternType = EPatternType::Angel;
	// AngelPattern.NumberOfBullets = 14;
	// AngelPattern.BulletSpeed = 250.0f;
	// BulletPatterns.Add(AngelPattern);
}
