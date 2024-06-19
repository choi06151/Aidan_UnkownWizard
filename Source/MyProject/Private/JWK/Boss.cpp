#include "JWK/Boss.h"

#include "CJW/PlayerPawnCPP.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/BossFSM.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/SpawnWidget.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	movementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("movementComp"));

	// 보스
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/JWK/Asset/00_Boss/Boss/SK_skltnGent.SK_skltnGent'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -160), FRotator(0, -90, 0));
		GetMesh()->SetWorldScale3D(FVector(1.6f));
	}

	// 보스 눈
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

		bossEyeMesh_L->SetWorldScale3D(FVector(0.05));
		bossEyeMesh_R->SetWorldScale3D(FVector(0.05));
	}

	// 지휘봉
	batonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("batonMesh"));
	batonMesh->SetupAttachment(GetMesh(), TEXT("Weapon_R"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBatonMesh(
		TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Baton/Baton.Baton'"));
	if (tempBatonMesh.Succeeded())
	{
		batonMesh->SetStaticMesh(tempBatonMesh.Object);
		batonMesh->SetRelativeLocationAndRotation(FVector(2, -1, 10), FRotator(0, 0, 0));
		batonMesh->SetWorldScale3D(FVector(1.5f));
	}


	
	bossFSM = CreateDefaultSubobject<UBossFSM>(TEXT("bossFSM"));

	BulletSpawner = CreateDefaultSubobject<USpawn_Bullet>(TEXT("BulletSpawner"));

	BatonSpawner = CreateDefaultSubobject<USpawn_Baton>(TEXT("BatonSpawner"));

	FireRate = 1.0f;

	CurrentPatternIndex = 0;
	//////////////////////////////////////// 음악분석 관련 추가 ////////////////////////////////////////
	CurrentTimeIndex = 0; // CurrentTimeIndex 초기화

	// 추가
	bIsMusicFinished = false;
	MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudioComponent"));
	MusicAudioComponent->SetupAttachment(RootComponent);

	// 웃음 효과음
	static ConstructorHelpers::FObjectFinder<USoundWave> HaSoundAsset(TEXT("/Script/Engine.SoundWave'/Game/KJH/SFX/Ha.Ha'"));
	if (HaSoundAsset.Succeeded())
	{
		HaSound = HaSoundAsset.Object;
	}

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

	/* 하트) */
	PatternDelegates.Add(EPatternType::Heart, FPatternDelegate::CreateUObject(this, &ABoss::FireHeartPattern));

	/* 민들레 */
	PatternDelegates.Add(EPatternType::Dandelion, FPatternDelegate::CreateUObject(this, &ABoss::FireDandelionPattern));

	/* HA */
	PatternDelegates.Add(EPatternType::HA, FPatternDelegate::CreateUObject(this, &ABoss::FireHAPattern));

	/* 움직이는 원형 */
	PatternDelegates.Add(EPatternType::CircularMoving, FPatternDelegate::CreateUObject(this, &ABoss::FireCircularMovingPattern));

	/* 바람개비 */
	PatternDelegates.Add(EPatternType::Pinwheel, FPatternDelegate::CreateUObject(this, &ABoss::FirePinwheelPattern));

	//////////////////////////////////////// 보스 둥둥 효과 관련 ////////////////////////////////////////
	FloatingTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FloatingTimeline"));
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

	//////////////////////////////////////// 보스 둥둥 효과 관련 ////////////////////////////////////////
	InitialLocation = GetActorLocation();

	if (FloatCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleFloatProgress"));

		FloatingTimeline->AddInterpFloat(FloatCurve, ProgressFunction);
		FloatingTimeline->SetLooping(true);
		FloatingTimeline->PlayFromStart();
	}

	// boss의 하반신 안보이게
	USkeletalMeshComponent* meshComp = GetMesh();
	FName LeftLeg = TEXT("thigh_l");
	FName RightLeg = TEXT("thigh_R");
	meshComp->HideBoneByName(LeftLeg, PBO_None);
	meshComp->HideBoneByName(RightLeg, PBO_None);

	/////////////음악 재생을 위해
	if (Music)
		MusicAudioComponent->SetSound(Music);

	// StartFiring();
	// FireBullet();
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 음악 재생 시작
	if(bIsMusicStart)
	{
		bIsMusicStart = false;
		SpawnWidget->MusicPlay();
	}

	// Phase 진입하고 지휘봉 던지는 함수 호출
	if(bThrowBaton)
		ThrowBaton();

	// 중간에 GameOver시 보스 원위치
	if(bGameOver)
	{
		// boss 위치 초기화
		this->SetActorLocation(FVector(-190, 650, 1250));
		bGameOver = false;
		// 노래 및 총알 발사 중지
		StopFiring();
		StopMusic();
	}
	
	// 게임을 완전히 클리어 시 Boss의 Dead Montage 재생
	if(bClearGame)
		bossFSM->GameEnd();
		
	//////////////////////////////////////// 보스 둥둥 효과 관련 ////////////////////////////////////////
	FloatingTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	
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

	//////////////////////////////////////// Phase2 지휘봉 테스트용 코드 ////////////////////////////////////////
	// TimeElapsed += DeltaTime;
	//
	// if(TimeElapsed >= 5.0f && bTestFire)
	// {
	// 	bTestFire = false;
	// 	ThrowBaton();
	// }
	//////////////////////////////////////// Phase2 지휘봉 테스트용 코드 ////////////////////////////////////////
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


//////////////////////////////////////// 보스 둥둥 효과 관련 ////////////////////////////////////////
void ABoss::HandleFloatProgress(float Value)
{
	FVector NewBossLocation = InitialLocation;
	float Time = GetWorld()->GetTimeSeconds();
	float CurveValue = FMath::Sin(Time) * 100.0f;
	NewBossLocation.Z += Value * 20.0f; // Adjust the multiplier to control the float height
	SetActorLocation(NewBossLocation);
}

//////////////////////////////////////// 시작 커튼 애니메이션 관련 ////////////////////////////////////////
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
	case 0:		// 목적지까지 이동
		bIsWalk = true;
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 2.2f, false);
		break;

	case 1:		// 도착하고 커튼 닫히기까지
		bIsArrive = true;
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 1.5f, false);
		break;

	case 2:
		if (SpawnWidget != nullptr)
		{
			SpawnWidget->CurtainCloseAnim();
			// SpawnWidget->MusicPlay();		// 지휘 시작 시 음악 재생
		}
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 3.0f, false);
		break;
		
	case 3:
		bIsCommandWait = true;
		cnt++;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::HandleState, 3.0f, false);
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
		FireRate = BeatLength * 6; // 6/4 박자를 위해 4배로 설정

		UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Loaded pre-analyzed conditions for %s. Total Duration: %f seconds"), *MusicTitle, TotalDuration);

		// 패턴 조건 업데이트 타이머 설정 (한 바 단위로 패턴 업데이트)
		GetWorldTimerManager().SetTimer(PatternUpdateTimerHandle, this, &ABoss::UpdatePatternConditions, FireRate, true);

		// 노래가 끝났음을 감지하기 위한 타이머 설정
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABoss::OnMusicFinished, TotalDuration, false);

		// 음악 설정 및 재생
		if (USoundBase* LoadedMusic = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
		{
			// 노래 시작 시 변수 초기화
			bIsMusicFinished = false;

			// 음악을 AudioComponent에 설정
			MusicAudioComponent->SetSound(LoadedMusic);

			// 음악 재생 시작
			PlayMusic();
			
			if(bIsGameStart)
				FireBullet();// 탄막 발사 시작
		}
		
		else
			UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load music: %s"), *MusicFilePath);
	}
	
	else
		UE_LOG(LogTemp, Error, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load pre-analyzed data for: %s"), *MusicTitle);
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
							return Pattern.PatternType == EPatternType::CircularMoving;
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
	PreAnalyzeMusicData(TEXT("Butter-Fly"), ButterflyJsonPath);
	PreAnalyzeMusicData(TEXT("Fur Elise"), EliseJsonPath);
	PreAnalyzeMusicData(TEXT("Requiem – Lacrimosa"), LacrimosaJsonPath);
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
	StopMusic();

	// 여기 함수에 보스 죽는 bool값 넣으면 될 듯?
	bClearGame = true;
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

void ABoss::StopMusicAndFiringGameOver()
{
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->Stop();
		GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle);	// StopFiring
	}
}

void ABoss::StopMusic()
{
	if (MusicAudioComponent && MusicAudioComponent->IsPlaying())
	{
		MusicAudioComponent->Stop();
		bIsMusicFinished = true;
		StopFiring();
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
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ABoss::FireBullet, FireRate, true, 0.0f);
}

void ABoss::StopFiring()
{
	bIsMusicFinished = true;
	GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle);
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 ////////////////////////////////////////

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

	for (int32 j = 0; j < 2; j++)
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 ////////////////////////////////////////
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("bTestFire Is True!!!"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
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

	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	// FTimerHandle BulletTimer;
	// float BulletTime = 5.0f;
	//
	// GetWorld()->GetTimerManager().SetTimer(BulletTimer, FTimerDelegate::CreateLambda([ & ]()
	// {
	// 	// 실행할 내용
	// 	bTestFire = true;
	// 	TimeElapsed = 0.0f;
	// 	UE_LOG(LogTemp, Warning, TEXT("----TestFire Is True----"));
	// 	// TimerHandle 초기화
	// 	GetWorld()->GetTimerManager().ClearTimer(BulletTimer);
	// }), BulletTime, false);
	// //////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
}


void ABoss::FireAngelPattern(const FBulletHellPattern& Pattern)
{
}

void ABoss::FireHeartPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	TArray<FVector> HeartShape;
	DefineHeartShape(HeartShape, Pattern.NumberOfBullets, Pattern.PatternSize);

	for (const FVector& Offset : HeartShape)
	{
		FVector SpawnLocation = BossLocation + Offset;
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}
}

void ABoss::DefineHeartShape(TArray<FVector>& OutShape, int32 NumberOfPoints, float PatternSize)
{
	OutShape.Empty();

	// 하트 모양을 정의.
	for (int32 i = 0; i < NumberOfPoints; ++i)
	{
		float t = i * 2.0f * PI / NumberOfPoints;
		float x = 16 * FMath::Pow(FMath::Sin(t), 3);
		float y = 13 * FMath::Cos(t) - 5 * FMath::Cos(2 * t) - 2 * FMath::Cos(3 * t) - FMath::Cos(4 * t);

		// 여기서 하트 크기 조정 가능 같은 비율로 x랑y 나누는 수 변경해주기~
		OutShape.Add(FVector(0.0f, x * PatternSize / 8.0f, y * PatternSize / 6.5f));
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
				Bullet->SetSpreadParams(true, Pattern.SpreadDelay, SpreadRotation, Pattern.PatternType);
				BulletIndex++;
			}
		}
	}
}

void ABoss::FireHAPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;


	TArray<FVector> HAPatternShape = {


		// "H"
		FVector(0, 350, 400), FVector(0, 350, 250), FVector(0, 350, 100), FVector(0, 350, -50), FVector(0, 350, -200),
		FVector(0, 250, 100), FVector(0, 150, 100),
		FVector(0, 50, 400), FVector(0, 50, 250), FVector(0, 50, 100), FVector(0, 50, -50), FVector(0, 50, -200),

		// "A"
		FVector(0, -250, 400),FVector(0, -150, 250),
		FVector(0, -100, 100), FVector(0, -100, -50), FVector(0, -100, -200),
		FVector(0, -350, 250),
		FVector(0, -400, 100), FVector(0, -400, -50), FVector(0, -400, -200),
		FVector(0, -200, 100), FVector(0, -300, 100)


	};

	for (const FVector& Offset : HAPatternShape)
	{
		FVector SpawnLocation = BossLocation + Offset;

		// 보스의 정면을 향하는 방향으로 설정
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed);
	}

	// 웃음 효과음 재생
	if (HaSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HaSound, BossLocation);
	}

	UE_LOG(LogTemp, Warning, TEXT("HA Pattern"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}

void ABoss::FireCircularMovingPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		FVector InitialOffset = Pattern.InitialPositions.IsValidIndex(i) ? Pattern.InitialPositions[i] : FVector::ZeroVector;
		FVector SpawnLocation = BossLocation + InitialOffset;

		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, GetActorRotation(), Pattern.BulletSpeed);
		if (Bullet)
		{
			Bullet->SetPatternType(Pattern.PatternType);
			Bullet->CircularCenter = SpawnLocation; // 초기 위치를 중심으로 설정
			Bullet->CircularRadius = Pattern.PatternSize;
			Bullet->CircularSpeed = Pattern.BulletSpeed;
			Bullet->CurrentAngle = 0.0f;
			Bullet->ForwardDirection = GetActorForwardVector(); // 보스의 전진 방향 설정
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("CircularMoving"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}

void ABoss::FirePinwheelPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	BossLocation.Z = 300.0f;

	// 궤도 속도와 반지름 배열의 유효성을 확인
	if (Pattern.OrbitSpeeds.Num() == 0 || Pattern.OrbitRadii.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("OrbitSpeeds or OrbitRadii arrays are empty!"));
		return;
	}

	// 궤도 속도와 반지름 배열의 크기가 동일한지 확인
	if (Pattern.OrbitSpeeds.Num() != Pattern.OrbitRadii.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("OrbitSpeeds and OrbitRadii size mismatch!"));
		return;
	}

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		FVector InitialOffset = Pattern.InitialPositions.IsValidIndex(i) ? Pattern.InitialPositions[i] : FVector::ZeroVector;
		FVector SpawnLocation = BossLocation + InitialOffset;

		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, GetActorRotation(), Pattern.BulletSpeed);
		if (Bullet)
		{
			Bullet->SetPatternType(Pattern.PatternType);

			// 궤도 속도와 반지름 설정
			int OrbitIndex = FMath::Clamp(i / 4, 0, Pattern.OrbitSpeeds.Num() - 1);
			if (Pattern.OrbitSpeeds.IsValidIndex(OrbitIndex) && Pattern.OrbitRadii.IsValidIndex(OrbitIndex))
			{
				Bullet->SetCircularParams(BossLocation, Pattern.OrbitRadii[OrbitIndex], Pattern.OrbitSpeeds[OrbitIndex], GetActorForwardVector());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Invalid OrbitIndex: %d"), OrbitIndex);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("FirePinwheel"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}


//////////////////////////////////////// AnimNotify에서 페이즈 2일 때 호출 ////////////////////////////////////////
void ABoss::ThrowBaton()
{
	bThrowBaton = false;
	
	// FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100;
	
	// USkeletalMeshComponent* MeshComponent = GetMesh();
	FVector BossLocation = GetMesh()->GetSocketLocation(FName("Weapon_R"));
	
	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation() + player->GetActorUpVector() * -500;
	FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();
	FRotator SpawnRotation = Direction.Rotation();

	BatonSpawner->SpawnPooledBaton(BossLocation, SpawnRotation, 500);

	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
	FTimerHandle BatonTimer;
	float BatonTime = 5.0f;

	GetWorld()->GetTimerManager().SetTimer(BatonTimer, FTimerDelegate::CreateLambda([ & ]()
	{
		// 실행할 내용
		bTestFire = true;
		TimeElapsed = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("----TestFire Is True----"));
		// TimerHandle 초기화
		GetWorld()->GetTimerManager().ClearTimer(BatonTimer);
	}), BatonTime, false);
	//////////////////////////////////////// 탄막 테스트용 코드 //////////////////////////////////////// 
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

	// 하트 모양 패턴
	FBulletHellPattern HeartPattern;
	HeartPattern.PatternType = EPatternType::Heart;
	HeartPattern.PatternSize = 100.0f; // 하트 모양 패턴의 크기 설정
	HeartPattern.NumberOfBullets = 30; // 총알의 수
	HeartPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(HeartPattern);

	// 민들레 패턴 
	FBulletHellPattern DandelionPattern;
	DandelionPattern.PatternType = EPatternType::Dandelion;
	DandelionPattern.NumberOfBullets = 30;
	DandelionPattern.BulletSpeed = 300.0f;
	DandelionPattern.SpreadDelay = 2.0f; // 2초 후에 퍼짐
	BulletPatterns.Add(DandelionPattern);

	// HA 패턴
	FBulletHellPattern HAPattern;
	HAPattern.PatternType = EPatternType::HA;
	HAPattern.NumberOfBullets = 23; // "HA" 패턴의 점 수에 맞춰서 설정
	HAPattern.BulletSpeed = 300.0f;
	BulletPatterns.Add(HAPattern);

	// CircularMoving 패턴 추가
	FBulletHellPattern CircularMovingPattern;
	CircularMovingPattern.PatternType = EPatternType::CircularMoving;
	CircularMovingPattern.NumberOfBullets = 12; // 예시로 3개의 총알을 발사
	CircularMovingPattern.BulletSpeed = 200.0f; // 원형 이동의 속도
	CircularMovingPattern.PatternSize = 200.0f; // 원형 반지름
	// 초기 위치 설정
	CircularMovingPattern.InitialPositions = {
		FVector(0.0f, -150.0f, 0.0f),
		FVector(0.0f, 0.0f, -150.0f),
		FVector(0.0f, 0.0f, 150.0f),
		FVector(0.0f, 150.0f, 0.0f),
		FVector(0.0f, -75.0f, 130.0f),
		FVector(0.0f, -75.0f, -130.0f),
		FVector(0.0f, 75.0f, 130.0f),
		FVector(0.0f, 75.0f, -130.0f),
		FVector(0.0f, -130.0f, 75.0f),
		FVector(0.0f, -130.0f, -75.0f),
		FVector(0.0f, 130.0f, 75.0f),
		FVector(0.0f, 130.0f, -75.0f)
	};
	BulletPatterns.Add(CircularMovingPattern);

	// 바람개비 패턴
	FBulletHellPattern PinwheelPattern;
	PinwheelPattern.PatternType = EPatternType::Pinwheel;
	PinwheelPattern.NumberOfBullets = 13;
	PinwheelPattern.BulletSpeed = 200.0f;
	PinwheelPattern.PatternSize = 300.0f;
	PinwheelPattern.InitialPositions = {
		FVector(0.0f, 0.0f, 0.0f),
		FVector(0.0f, -100.0f, 0.0f),
		FVector(0.0f, 100.0f, 0.0f),
		FVector(0.0f, 0.0f, 100.0f),
		FVector(0.0f, 0.0f, -100.0f),
		FVector(0.0f, -200.0f, 0.0f),
		FVector(0.0f, 200.0f, 0.0f),
		FVector(0.0f, 0.0f, 200.0f),
		FVector(0.0f, 0.0f, -200.0f),
		FVector(0.0f, -300.0f, 0.0f),
		FVector(0.0f, 300.0f, 0.0f),
		FVector(0.0f, 0.0f, 300.0f),
		FVector(0.0f, 0.0f, -300.0f)
	};
	PinwheelPattern.OrbitSpeeds = {
		0.0f,   // 중심 총알은 움직이지 않음
		0.0f,  // 첫 번째 궤도 속도
		0.0f,  // 두 번째 궤도 속도
		0.0f   // 세 번째 궤도 속도
	};
	PinwheelPattern.OrbitRadii = {
		0.0f,   // 중심 총알 반지름
		100.0f, // 첫 번째 궤도 반지름
		200.0f, // 두 번째 궤도 반지름
		300.0f  // 세 번째 궤도 반지름
	};
	BulletPatterns.Add(PinwheelPattern);
}
