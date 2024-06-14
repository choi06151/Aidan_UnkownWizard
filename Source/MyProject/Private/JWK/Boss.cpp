#include "JWK/Boss.h"

#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/BossFSM.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"
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

	// 0613
	bIsMusicFinished = false;
	MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudioComponent"));
	MusicAudioComponent->SetupAttachment(RootComponent);

	// DELEGATE Map 초기화
	PatternDelegates.Add(EPatternType::RandomStraight,
		FPatternDelegate::CreateUObject(this, &ABoss::FireRandomStraightPattern));
	PatternDelegates.Add(EPatternType::Fan, FPatternDelegate::CreateUObject(this, &ABoss::FireFanPattern));
	PatternDelegates.Add(EPatternType::Circle, FPatternDelegate::CreateUObject(this, &ABoss::FireCirclePattern));
	PatternDelegates.Add(EPatternType::Swirl, FPatternDelegate::CreateUObject(this, &ABoss::FireSwirlPattern));
	PatternDelegates.Add(EPatternType::TargetCross, FPatternDelegate::CreateUObject(this, &ABoss::FireTargetCrossPattern));
	PatternDelegates.Add(EPatternType::TrumpetFlower,
		FPatternDelegate::CreateUObject(this, &ABoss::FireTrumpetFlowerPattern));
	PatternDelegates.Add(EPatternType::Crescent, FPatternDelegate::CreateUObject(this, &ABoss::FireCrescentPattern));
	PatternDelegates.Add(EPatternType::Angel, FPatternDelegate::CreateUObject(this, &ABoss::FireAngelPattern));
	////// 0613
	PatternDelegates.Add(EPatternType::Heart, FPatternDelegate::CreateUObject(this, &ABoss::FireHeartPattern));
	PatternDelegates.Add(EPatternType::ExpandingSphere, FPatternDelegate::CreateUObject(this, &ABoss::FireExpandingSpherePattern));
	PatternDelegates.Add(EPatternType::Dandelion, FPatternDelegate::CreateUObject(this, &ABoss::FireDandelionPattern));

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

	//StartFiring(); 0613//0614
	if (Music)
	{
		MusicAudioComponent->SetSound(Music);
		
	}
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if(bIsGameStart)		// GameStart 버튼이 눌리고
		MusicStart();*/

	static float TimeElapsed = 0.0f;
	TimeElapsed += DeltaTime;

	//// 1.5초 ~ 3.0초 랜덤 탄막 패턴 발사
	//if (TimeElapsed > 1.5f && TimeElapsed <= 3.0f && !bIsDie)
	//{
	//	StopFiring();
	//	ChangePattern();
	//	StartFiring();
	//}
	//// 3.0초 5.5초 발사 중지
	//if (TimeElapsed > 3.0f && TimeElapsed <= 5.5f && !bIsDie)
	//{
	//	StopFiring();
	//}

	//// 5.5초 초과시 다음 탄막 패턴 발사
	//if (TimeElapsed > 5.5f && !bIsDie)
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
		// AnalyzedDataMap에서 분석된 데이터를 가져옴
		FMusicAnalysisData AnalyzedData = AnalyzedDataMap[MusicTitle];
		float Tempo = AnalyzedData.Tempo;
		float TotalDuration = AnalyzedData.TotalDuration;  // 노래의 전체 길이 (초 단위)
		FinalPatternData = AnalyzedData.FinalPatternDataArray;
		CurrentTimeIndex = 0;

		// FireRate 설정 (템포에 맞춰 조정)
		float BeatLength = 60.0f / Tempo; // 한 비트의 길이
		FireRate = BeatLength * 4; // 4/4 박자를 위해 4배로 설정

		UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Loaded pre-analyzed conditions for %s. Total Duration: %f seconds"), *MusicTitle, TotalDuration);

		// 패턴 조건 업데이트 타이머 설정 (한 바 단위로 패턴 업데이트)
		GetWorldTimerManager().SetTimer(PatternUpdateTimerHandle, this, &ABoss::UpdatePatternConditions, FireRate, true);

		// 노래가 끝났음을 감지하기 위한 타이머 설정
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABoss::OnMusicFinished, TotalDuration, false);

		//// 음악 재생 시작
		//UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Playing music synchronized with pattern conditions."));
		//if (USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
		//{
		//	// 노래 시작 시 변수 초기화
		//	bIsMusicFinished = false;

		//	//UGameplayStatics::PlaySound2D(this, Music); 0614
		//	UGameplayStatics::SpawnSound2D(this, Music);
		//	
		//	// 탄막 발사 시작
		//	StartFiring();
		//}
		// 음악 설정 및 재생
		if (USoundBase* LoadedMusic = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
		{
			// 노래 시작 시 변수 초기화
			bIsMusicFinished = false;

			// 음악을 AudioComponent에 설정
			MusicAudioComponent->SetSound(LoadedMusic);

			// 음악 재생 시작
			PlayMusic();

			// 탄막 발사 시작
			StartFiring();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load music: %s"), *MusicFilePath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load pre-analyzed data for: %s"), *MusicTitle);
	}
}

// 패턴 조건을 업데이트하는 함수
void ABoss::UpdatePatternConditions()
{
	if (FinalPatternData.IsValidIndex(CurrentTimeIndex))
	{
		// 현재 시간 인덱스에 해당하는 패턴 데이터를 가져옴
		FFinalPatternData CurrentData = FinalPatternData[CurrentTimeIndex];

		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Checking conditions at time index %d"), CurrentTimeIndex);

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
			PatternDelegates[BulletPatterns[CurrentPatternIndex].PatternType].Execute(BulletPatterns[CurrentPatternIndex]);
		}

		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: Applying pattern index %d with bullet speed %f at time index %d"), CurrentData.PatternIndex, CurrentData.BulletSpeed, CurrentTimeIndex);

		// 4/4 박자 단위로 인덱스를 증가시켜 다음 조건을 확인하도록 함
		CurrentTimeIndex++; // 4 비트마다 한 번씩 업데이트
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: No more conditions to process."));
		GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle); // 타이머를 중지하여 패턴 업데이트를 멈춤
	}
}

// JSON 데이터를 로드하고 패턴 조건을 설정하는 함수 0613
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
			float TotalDuration = JsonObject->GetNumberField(TEXT("total_duration"));  // 전체 길이 (초 단위)

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
							return Pattern.PatternType == EPatternType::Dandelion;
						});
				}
				else if (LowMidArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::Dandelion;
						});
				}
				else if (HighMidArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::Dandelion;
						});
				}
				else if (HighArray[i]->AsNumber() > 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::Dandelion;
						});
				}

				// 비트 시간과 현재 인덱스가 가까운 경우 탄막 속도를 감소시킴
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

			// 분석된 데이터 구조체 생성 및 저장
			FMusicAnalysisData AnalysisData;
			AnalysisData.Tempo = Tempo;
			AnalysisData.FinalPatternDataArray = FinalPatternDataArray;
			AnalysisData.TotalDuration = TotalDuration;

			AnalyzedDataMap.Add(MusicTitle, AnalysisData);

			UE_LOG(LogTemp, Warning, TEXT("ABoss::PreAnalyzeMusicData: Pre-analyzed %d conditions for %s."), FinalPatternDataArray.Num(), *MusicTitle);
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
	// MusicTitle 여기에서 수정 가능!!
	PreAnalyzeMusicData(TEXT("dragonfly"), ButterflyJsonPath);
	PreAnalyzeMusicData(TEXT("Elise"), EliseJsonPath);
	PreAnalyzeMusicData(TEXT("Lacrimosa"), LacrimosaJsonPath);
}

float ABoss::GetTotalDuration(const FString& MusicTitle) const
{
	if (AnalyzedDataMap.Contains(MusicTitle))
	{
		const FMusicAnalysisData& AnalyzedData = AnalyzedDataMap[MusicTitle];
		return AnalyzedData.TotalDuration;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABoss::GetTotalDuration: Failed to find music data for: %s"), *MusicTitle);
		return -1.0f; // 실패를 나타내는 값
	}
}

void ABoss::OnMusicFinished()
{
	bIsMusicFinished = true;
	UE_LOG(LogTemp, Warning, TEXT("ABoss::OnMusicFinished: Music has finished."));

	// 노래가 끝나면 탄막 발사 중지 
	StopFiring();

	// 여기 함수에 보스 죽는 bool값 넣으면 될 듯?
}

void ABoss::SetMusicVolume(float Volume)
{
	if (MusicAudioComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::SetMusicVolume: Setting volume to: %f"), Volume); // 볼륨 값 로그 출력
		MusicAudioComponent->SetVolumeMultiplier(Volume);
	}
}

void ABoss::SetMusicSpeed(float Speed)
{
	if (MusicAudioComponent)
	{
		MusicAudioComponent->SetPitchMultiplier(Speed);
	}
}

void ABoss::PlayMusic()
{
	if (MusicAudioComponent && !MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->Play();
	}
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
	if (!bIsMusicFinished)  // 노래가 끝나지 않았을 때만 탄막 발사를 시작 0613
	{
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABoss::FireBullet, FireRate, true, 0.0f);
	}
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

/* 랜덤 범위 : GetActorRightVector() * FMath::RandRange(-200, 200);*/

// 랜덤 직선
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

// 부채꼴
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

// 타겟 원형
void ABoss::FireCirclePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(-200, 200) + GetActorUpVector() * FMath::RandRange(0, 150);
	BossLocation.Z = 300.0f;  // 보스의 높이를 설정
	float Radius = Pattern.PatternSize / 3.0f;  // 패턴 크기에 따른 반지름
	FRotator BossRotation = GetActorRotation();  // 보스의 현재 회전 값


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

	UE_LOG(LogTemp, Warning, TEXT("Circle"));
	UE_LOG(LogTemp, Warning, TEXT("---"));
}

// 소용돌이?
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

// 유도 십자가
void ABoss::FireTargetCrossPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(-200, 200);
	BossLocation.Z = 300.0f;

	// 보스의 전방 벡터
	FVector Forward = GetActorForwardVector();

	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	// 탄환 발사 위치 설정
	FVector CrossOffsets[] = {
		FVector(0, 0, 0),		// 중심
		FVector(0, 100, 0),		// 위쪽 중간
		FVector(0, 200, 0),		// 위쪽
		FVector(0, -100, 0),	// 아래쪽 중간
		FVector(0, -200, 0),	// 아래쪽
		FVector(0, 0, 100),		// 앞쪽 중간
		FVector(0, 0, 200),		// 앞쪽
		FVector(0, 0, -100),	// 뒤쪽 중간
		FVector(0, 0, -200)		// 뒤쪽
	};

	for (int32 i = 0; i < 9; ++i)
	{
		FVector SpawnLocation = BossLocation + Forward * CrossOffsets[i].X + FVector::RightVector * CrossOffsets[i].Y + FVector::UpVector * CrossOffsets[i].Z;
		// 플레이어를 향한 방향 계산
		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Cross"));
	UE_LOG(LogTemp, Warning, TEXT("------------"));
}

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


/* 랜덤 범위 : GetActorRightVector() * FMath::RandRange(-200, 200);*/
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

/* 랜덤 범위 : GetActorRightVector() * FMath::RandRange(-200, 200);*/
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

/* 랜덤 범위 : GetActorRightVector() * FMath::RandRange(-200, 200);*/
void ABoss::FireAngelPattern(const FBulletHellPattern& Pattern)
{
}

// 하트
void ABoss::FireHeartPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	TArray<FVector> HeartShape;
	DefineHeartShape(HeartShape, Pattern.NumberOfBullets, Pattern.PatternSize);

	for (const FVector& Offset : HeartShape)
	{
		FVector SpawnLocation = BossLocation + Offset;

		// 플레이어를 향한 방향 계산
		FVector Direction = (player->GetActorLocation() - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	UE_LOG(LogTemp, Warning, TEXT("Heart"));
	UE_LOG(LogTemp, Warning, TEXT("-----"));
}

void ABoss::DefineHeartShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float PatternSize)
{
	OutShape.Empty();

	// 하트 모양을 정의합니다.
	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		float t = i * 2.0f * PI / NumberOfPoints;
		float x = 16 * FMath::Pow(FMath::Sin(t), 3);
		float y = 13 * FMath::Cos(t) - 5 * FMath::Cos(2 * t) - 2 * FMath::Cos(3 * t) - FMath::Cos(4 * t);

		OutShape.Add(FVector(0.0f, x * PatternSize / 16.0f, y * PatternSize / 13.0f));
	}
}
//////////// 커지는 구
void ABoss::FireExpandingSpherePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	TArray<FVector> SphereShape;
	float CurrentRadius = Pattern.InitialPatternSize;

	DefineExpandingSphereShape(SphereShape, Pattern.NumberOfBullets, CurrentRadius);

	for (const FVector& Offset : SphereShape)
	{
		FVector SpawnLocation = BossLocation + Offset;
		FRotator SpawnRotation = GetActorForwardVector().Rotation();
		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
		if (Bullet)
		{
			Bullet->SetPatternType(Pattern.PatternType);
			Bullet->SetDistanceThreshold(Pattern.SizeChangeDistance);
			Bullet->InitialRadius = Pattern.InitialPatternSize; // 초기 크기 설정
			Bullet->FinalRadius = Pattern.FinalPatternSize; // 최종 크기 설정
			Bullet->SizeChangeDistance = Pattern.SizeChangeDistance; // 크기 변화 거리 설정

			// 이미 바인딩되어 있는지 확인한 후에 이벤트 추가
			if (!Bullet->OnBulletTravelled.IsAlreadyBound(this, &ABoss::OnBulletTravelled))
			{
				Bullet->OnBulletTravelled.AddDynamic(this, &ABoss::OnBulletTravelled);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ExpandingSphere"));
	UE_LOG(LogTemp, Warning, TEXT("---------------"));
}

void ABoss::DefineExpandingSphereShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float Radius)
{
	// 기존에 저장된 점들을 초기화합니다.
	OutShape.Empty();

	// Sin 및 Cos 값을 미리 계산하여 저장할 배열을 선언합니다.
	TArray<float> SinPhi, CosPhi, SinTheta, CosTheta;

	// 위도와 경도 각도를 계산할 때 사용할 간격을 설정합니다.
	float PhiStep = PI / NumberOfPoints;       // 위도 각도 간격
	float ThetaStep = 2 * PI / NumberOfPoints; // 경도 각도 간격

	// 위도(Phi)에 대한 Sin, Cos 값을 미리 계산하여 저장합니다.
	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		float Phi = i * PhiStep;              // 현재 위도 각도 계산
		SinPhi.Add(FMath::Sin(Phi));          // 위도 각도의 Sine 값을 배열에 추가
		CosPhi.Add(FMath::Cos(Phi));          // 위도 각도의 Cosine 값을 배열에 추가
	}

	// 경도(Theta)에 대한 Sin, Cos 값을 미리 계산하여 저장합니다.
	for (int32 j = 0; j < NumberOfPoints; ++j)
	{
		float Theta = j * ThetaStep;          // 현재 경도 각도 계산
		SinTheta.Add(FMath::Sin(Theta));      // 경도 각도의 Sine 값을 배열에 추가
		CosTheta.Add(FMath::Cos(Theta));      // 경도 각도의 Cosine 값을 배열에 추가
	}

	// 미리 계산한 Sin, Cos 값을 사용하여 구의 표면 점들을 계산합니다.
	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		for (int32 j = 0; j < NumberOfPoints; ++j)
		{
			// 구 표면의 X, Y, Z 좌표를 계산합니다.
			float X = Radius * SinPhi[i] * CosTheta[j]; // X 좌표 계산
			float Y = Radius * SinPhi[i] * SinTheta[j]; // Y 좌표 계산
			float Z = Radius * CosPhi[i];               // Z 좌표 계산

			// 계산된 점을 결과 배열에 추가합니다.
			OutShape.Add(FVector(X, Y, Z));
		}
	}
}

void ABoss::OnBulletTravelled(float DistanceTraveled, ABullet_Pooled* PooledBullet)
{
	UE_LOG(LogTemp, Warning, TEXT("ABoss::OnBulletTravelled: Bullet travelled distance: %f"), DistanceTraveled);
	// 총알과 관련된 패턴 찾기
	const FBulletHellPattern* Pattern = BulletPatterns.FindByPredicate([PooledBullet](const FBulletHellPattern& Pattern)
		{
			return PooledBullet->GetPatternType() == Pattern.PatternType;
		});

	if (Pattern)
	{
		float TotalTime = Pattern->SizeChangeDistance; // 크기 변화가 일어나는 총 시간 (초 단위)
		
		// 경과 시간에 따라 새로운 반지름 계산
		float NewRadius = FMath::Lerp(PooledBullet->InitialRadius, PooledBullet->FinalRadius, PooledBullet->GetElapsedTime() / TotalTime);

		TArray<FVector> SphereShape;
		DefineExpandingSphereShape(SphereShape, Pattern->NumberOfBullets, NewRadius);

		FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
		BossLocation.Z = 300.0f;

		if (SphereShape.Num() > 0)
		{
			for (int32 i = 0; i < SphereShape.Num(); ++i)
			{
				// 총알의 위치를 업데이트하여 구의 크기를 변화시킴
				PooledBullet->SetActorLocation(BossLocation + SphereShape[i]);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::OnBulletTravelled: Pattern not found for bullet"));
	}
}

void ABoss::FireDandelionPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	// 각도 지정 
	TArray<float> VerticalAngles = { -30.0f, -15.0f, 0.0f, 15.0f, 30.0f }; // 세로 각도
	TArray<float> HorizontalAngles = { -45.0f, -27.0f, -9.0f, 9.0f, 27.0f, 45.0f }; // 가로 각도

	int32 BulletIndex = 0;

	// 총알을 발사하고 퍼질 각도를 설정
	for (float VerticalAngle : VerticalAngles)
	{
		for (float HorizontalAngle : HorizontalAngles)
		{
			FVector SpawnLocation = BossLocation;
			FRotator SpawnRotation = GetActorRotation();

			if (BulletIndex >= Pattern.NumberOfBullets)
				break;

			ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
			if (Bullet)
			{
				Bullet->InitialDirection = GetActorForwardVector(); // 올바른 방향 설정

				// 퍼질 각도를 계산
				FRotator SpreadRotation = FRotator(VerticalAngle, HorizontalAngle, 0);

				// bShouldSpread를 true로 설정
				Bullet->SetSpreadParams(true, Pattern.SpreadDelay, SpreadRotation);
				BulletIndex++;

				// 로그 추가
				/*UE_LOG(LogTemp, Warning, TEXT("ABoss::FireDandelionPattern:Bullet Fired. InitialDirection: %s, SpreadRotation: %s"),
					*Bullet->InitialDirection.ToString(),
					*SpreadRotation.ToString());*/
			}
		}
	}

    UE_LOG(LogTemp, Warning, TEXT("Dandelion Pattern Fired"));
    UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}


////////////////////////////////////////////////// 패턴들 특성 //////////////////////////////////////////////////
void ABoss::InitializeDefaultPatterns()
{
	// Interval : 발사 주기 ( 높을수록 발사 빈도수 적어짐)	// PatternSize : 패턴의 크기
	// NumberOfBullets : 한 번에 발사되는 총알의 수		// BulletSpeed : 총알의 속도

	//// 랜덤 직선
	//FBulletHellPattern RandomStraightPattern;
	//RandomStraightPattern.PatternType = EPatternType::RandomStraight;
	//RandomStraightPattern.NumberOfBullets = 6;
	//RandomStraightPattern.BulletSpeed = 300.0f;
	//BulletPatterns.Add(RandomStraightPattern);
	//
	//// 부채꼴
	//FBulletHellPattern FanPattern;
	//FanPattern.PatternType = EPatternType::Fan;
	//FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
	//FanPattern.NumberOfBullets = 7; // 부채꼴 패턴에서 발사할 총알 수
	//FanPattern.BulletSpeed = 300.0f;
	//BulletPatterns.Add(FanPattern);
	//
	//// 원형 패턴
	//FBulletHellPattern CirclePattern;
	//CirclePattern.PatternType = EPatternType::Circle;
	//CirclePattern.PatternSize = 400.0f; // 원형 패턴의 크기 설정
	//CirclePattern.NumberOfBullets = 12; // 총알의 수
	//CirclePattern.BulletSpeed = 300.0f;
	//BulletPatterns.Add(CirclePattern);
	//
	//// 유도 원형 패턴
	//FBulletHellPattern TargetSwirlPattern;
	//TargetSwirlPattern.PatternType = EPatternType::Swirl;
	//TargetSwirlPattern.PatternSize = 150.0f; // 원형 패턴의 크기 설정
	//TargetSwirlPattern.NumberOfBullets = 4; // 총알의 수
	//TargetSwirlPattern.BulletSpeed = 300.0f;
	//BulletPatterns.Add(TargetSwirlPattern);
	//
	//// 나팔꽃 패턴
	//FBulletHellPattern TrumpetFlowerPattern;
	//TrumpetFlowerPattern.PatternType = EPatternType::TrumpetFlower;
	//TrumpetFlowerPattern.NumberOfBullets = 30;
	//TrumpetFlowerPattern.BulletSpeed = 300.0f;
	//TrumpetFlowerPattern.FanAngle = 180.0f;
	//BulletPatterns.Add(TrumpetFlowerPattern);

	//// 유도 십자가형
	//FBulletHellPattern TargetCrossPattern;
	//TargetCrossPattern.PatternType = EPatternType::TargetCross;
	//TargetCrossPattern.NumberOfBullets = 20;
	//TargetCrossPattern.BulletSpeed = 350.0f;
	//BulletPatterns.Add(TargetCrossPattern);


	// // 초승달 패턴
	// FBulletHellPattern CrescentPattern;
	// CrescentPattern.PatternType = EPatternType::Crescent;
	// CrescentPattern.NumberOfBullets = 20;
	// CrescentPattern.BulletSpeed = 300.0f;
	// CrescentPattern.CrescentAngle = 120.0f;
	// BulletPatterns.Add(CrescentPattern);

	// // 천사 패턴
	// FBulletHellPattern AngelPattern;
	// AngelPattern.PatternType = EPatternType::Angel;
	// AngelPattern.NumberOfBullets = 14;
	// AngelPattern.BulletSpeed = 250.0f;
	// BulletPatterns.Add(AngelPattern);

	//// 하트 모양 패턴
	//FBulletHellPattern HeartPattern;
	//HeartPattern.PatternType = EPatternType::Heart;
	//HeartPattern.PatternSize = 100.0f; // 하트 모양 패턴의 크기 설정
	//HeartPattern.NumberOfBullets = 30; // 총알의 수
	//HeartPattern.BulletSpeed = 300.0f;
	//BulletPatterns.Add(HeartPattern);

	//// 구의 크기가 커지는 패턴
	//FBulletHellPattern ExpandingSpherePattern;
	//ExpandingSpherePattern.PatternType = EPatternType::ExpandingSphere;
	//ExpandingSpherePattern.InitialPatternSize = 100.0f; // 초기 구 크기 설정
	//ExpandingSpherePattern.FinalPatternSize = 500.0f; // 최종 구 크기 설정
	//ExpandingSpherePattern.SizeChangeDistance = 3.0f; // 크기 변화가 일어나는 거리 설정
	//ExpandingSpherePattern.NumberOfBullets = 30; // 총알의 수
	//ExpandingSpherePattern.BulletSpeed = 600.0f;
	//BulletPatterns.Add(ExpandingSpherePattern);

	// 민들레 패턴 초기화
	FBulletHellPattern DandelionPattern;
	DandelionPattern.PatternType = EPatternType::Dandelion;
	DandelionPattern.NumberOfBullets = 30;
	DandelionPattern.BulletSpeed = 300.0f;
	DandelionPattern.SpreadDelay = 2.0f; // 2초 후에 퍼짐
	BulletPatterns.Add(DandelionPattern);
}
