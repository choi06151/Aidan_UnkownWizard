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

	/* 별똥별 */
	PatternDelegates.Add(EPatternType::MeteorShower, FPatternDelegate::CreateUObject(this, &ABoss::FireMeteorShowerPattern));

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

			if (bIsGameStart)
				FireBullet();// 탄막 발사 시작

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
		FFinalPatternData CurrentData = FinalPatternData[CurrentTimeIndex];
		CurrentPatternIndex = CurrentData.PatternIndex;

		float LowEnergy = CurrentData.LowFrequencyEnergy;
		float LowMidEnergy = CurrentData.LowMidFrequencyEnergy;
		float HighMidEnergy = CurrentData.HighMidFrequencyEnergy;
		float HighEnergy = CurrentData.HighFrequencyEnergy;

		float MaxEnergy = FMath::Max(FMath::Max3(LowEnergy, LowMidEnergy, HighMidEnergy), HighEnergy);
		int FloatIntensity = 1;

		if (MaxEnergy == LowEnergy)
		{
			FloatIntensity = 1;
		}
		else if (MaxEnergy == LowMidEnergy)
		{
			FloatIntensity = 2;
		}
		else if (MaxEnergy == HighMidEnergy)
		{
			FloatIntensity = 3;
		}
		else if (MaxEnergy == HighEnergy)
		{
			FloatIntensity = 4;
		}

		// 둥실거림 강도를 각 총알에 적용
		for (auto& Bullet : BulletSpawner->GetBulletPool())
		{
			if (Bullet && Bullet->IsActive())
			{
				Bullet->SetFloatIntensity(FloatIntensity);
			}
		}

		if (PatternDelegates.Contains(BulletPatterns[CurrentPatternIndex].PatternType))
		{
			PatternDelegates[BulletPatterns[CurrentPatternIndex].PatternType].Execute(BulletPatterns[CurrentPatternIndex]);
		}

		CurrentTimeIndex++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABoss::UpdatePatternConditions: No more conditions to process."));
		GetWorldTimerManager().ClearTimer(PatternUpdateTimerHandle);
	}
}

// JSON 데이터를 로드하고 패턴 조건을 설정하는 함수
void ABoss::PreAnalyzeMusicData(const FString& MusicTitle, const FString& JsonFilePath)
{
	FString JsonString;
	if (FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			TArray<FFinalPatternData> FinalPatternDataArray;
			float Tempo = JsonObject->GetNumberField(TEXT("tempo"));
			float TotalDuration = JsonObject->GetNumberField(TEXT("total_duration"));

			const TArray<TSharedPtr<FJsonValue>> BeatsArray = JsonObject->GetArrayField(TEXT("beats"));
			const TArray<TSharedPtr<FJsonValue>> IntensityArray = JsonObject->GetArrayField(TEXT("intensity"));
			const TArray<TSharedPtr<FJsonValue>> LowArray = JsonObject->GetArrayField(TEXT("low"));
			const TArray<TSharedPtr<FJsonValue>> LowMidArray = JsonObject->GetArrayField(TEXT("low_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighMidArray = JsonObject->GetArrayField(TEXT("high_mid"));
			const TArray<TSharedPtr<FJsonValue>> HighArray = JsonObject->GetArrayField(TEXT("high"));

			for (int32 i = 0; i < IntensityArray.Num(); ++i)
			{
				FFinalPatternData FinalData;

				FinalData.Intensity = IntensityArray[i]->AsNumber();
				FinalData.LowFrequencyEnergy = LowArray[i]->AsNumber();
				FinalData.LowMidFrequencyEnergy = LowMidArray[i]->AsNumber();
				FinalData.HighMidFrequencyEnergy = HighMidArray[i]->AsNumber();
				FinalData.HighFrequencyEnergy = HighArray[i]->AsNumber();

				if (FinalData.Intensity >= 1.0f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.9f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.8f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.7f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.6f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.5f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.4f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.3f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.2f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else if (FinalData.Intensity >= 0.1f)
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}
				else
				{
					FinalData.PatternIndex = BulletPatterns.IndexOfByPredicate([](const FBulletHellPattern& Pattern)
						{
							return Pattern.PatternType == EPatternType::MeteorShower;
						});
				}

				FinalPatternDataArray.Add(FinalData);
			}

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
	FString BacklightJsonPath = ProjectDir + TEXT("Content/Data/Ado_backlight.json");
	FString NOTEJsonPath = ProjectDir + TEXT("Content/Data/AI_NOTE.json");
	FString UnrealJsonPath = ProjectDir + TEXT("Content/Data/AI_Unreal.json");
	FString WaitingForLoveJsonPath = ProjectDir + TEXT("Content/Data/Avicii_WaitingForLove.json");
	FString PonytailJsonPath = ProjectDir + TEXT("Content/Data/Badanamu_Ponytail.json");
	FString SymphonyNo_5JsonPath = ProjectDir + TEXT("Content/Data/Beethoven_Symphony No_5.json");
	FString RabelJsonPath = ProjectDir + TEXT("Content/Data/Boléro_Rabel.json");
	FString ButterflyJsonPath = ProjectDir + TEXT("Content/Data/butterfly.json");
	FString EliseJsonPath = ProjectDir + TEXT("Content/Data/Elise.json");
	FString FantaisieImpromptuJsonPath = ProjectDir + TEXT("Content/Data/Frédéric Chopin_Fantaisie Impromptu.json");
	FString OrpheusintheUnderworldJsonPath = ProjectDir + TEXT("Content/Data/Jacques Offenbach_Orpheus in the Underworld.json");
	FString LacrimosaJsonPath = ProjectDir + TEXT("Content/Data/Lacrimosa.json");
	FString NachtmusikJsonPath = ProjectDir + TEXT("Content/Data/Mozart_Eine Kleine Nachtmusik.json");
	FString SymphonyNo_25JsonPath = ProjectDir + TEXT("Content/Data/Mozart_Symphony No.json");
	FString TurkishMarchPath = ProjectDir + TEXT("Content/Data/Mozart_Turkish March.json");
	FString ETAJsonPath = ProjectDir + TEXT("Content/Data/NewJeans_ETA.json");
	FString HowSweetJsonPath = ProjectDir + TEXT("Content/Data/NewJeans_How Sweet.json");
	FString MelongtoyouJsonPath = ProjectDir + TEXT("Content/Data/Tenko Shibuki_Melong to you.json");
	FString AirplaneJsonPath = ProjectDir + TEXT("Content/Data/Turtles_Airplane.json");


	// 각 음악에 대해 미리 분석 수행
	PreAnalyzeMusicData(TEXT("Butter-Fly"), ButterflyJsonPath);
	PreAnalyzeMusicData(TEXT("Fur Elise"), EliseJsonPath);
	PreAnalyzeMusicData(TEXT("Requiem – Lacrimosa"), LacrimosaJsonPath);
	PreAnalyzeMusicData(TEXT("Backlight"), BacklightJsonPath);
	PreAnalyzeMusicData(TEXT("NOTE"), NOTEJsonPath);
	PreAnalyzeMusicData(TEXT("Unreal"), UnrealJsonPath);
	PreAnalyzeMusicData(TEXT("Waiting For Love"), WaitingForLoveJsonPath);
	PreAnalyzeMusicData(TEXT("Ponytail"), PonytailJsonPath);
	PreAnalyzeMusicData(TEXT("Symphony No_5"), SymphonyNo_5JsonPath);
	PreAnalyzeMusicData(TEXT("Bolero"), RabelJsonPath);
	PreAnalyzeMusicData(TEXT("Fantaisie Impromptu"), FantaisieImpromptuJsonPath);
	PreAnalyzeMusicData(TEXT("Orpheus in the Underworld"), OrpheusintheUnderworldJsonPath);
	PreAnalyzeMusicData(TEXT("Eine Kleine Nachtmusik"), NachtmusikJsonPath);
	PreAnalyzeMusicData(TEXT("Symphony No_25 In G Minor K"), SymphonyNo_25JsonPath);
	PreAnalyzeMusicData(TEXT("Turkish March"), TurkishMarchPath);
	PreAnalyzeMusicData(TEXT("ETA"), ETAJsonPath);
	PreAnalyzeMusicData(TEXT("How Sweet"), HowSweetJsonPath);
	PreAnalyzeMusicData(TEXT("Melong to you"), MelongtoyouJsonPath);
	PreAnalyzeMusicData(TEXT("Airplane"), AirplaneJsonPath);
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

void ABoss::PlayMusicOnly(const FString& MusicFilePath, const FString& MusicTitle)
{
	if (AnalyzedDataMap.Contains(MusicTitle))
	{
		// AnalyzedDataMap에서 분석된 데이터를 가져옴
		FMusicAnalysisData AnalyzedData = AnalyzedDataMap[MusicTitle];
		float Tempo = AnalyzedData.Tempo;
		float TotalDuration = AnalyzedData.TotalDuration;  // 노래의 전체 길이 (초 단위)
		FinalPatternData = AnalyzedData.FinalPatternDataArray;
		CurrentTimeIndex = 0;


		// 음악 설정 및 재생
		if (USoundBase* LoadedMusic = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath)))
		{
			// 노래 시작 시 변수 초기화
			bIsMusicFinished = false;

			// 음악을 AudioComponent에 설정
			MusicAudioComponent->SetSound(LoadedMusic);

			// 음악 재생 시작
			PlayMusic();

		}

		else
			UE_LOG(LogTemp, Warning, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load music: %s"), *MusicFilePath);
	}

	else
		UE_LOG(LogTemp, Error, TEXT("ABoss::LoadMusicDataAndSetPatterns: Failed to load pre-analyzed data for: %s"), *MusicTitle);


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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float RandomY = FMath::FRandRange(-400.0f, 400.0f);
		float RandomZ = FMath::FRandRange(-300, -200.0f);
		FVector SpawnLocation = FVector(BossLocation.X, BossLocation.Y + RandomY, BossLocation.Z + RandomZ);
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorUpVector() * -200;
	// BossLocation.Z = 500.0f;
	FRotator BossRotation = GetActorRotation();

	// 두 가지 다른 최대 각도 설정
	static bool bUseFirstPattern = true;
	float MaxFanAngle1 = 90.0f; // 첫 번째 패턴의 최대 각도
	float MaxFanAngle2 = 60.0f; // 두 번째 패턴의 최대 각도

	float MaxFanAngle = bUseFirstPattern ? MaxFanAngle1 : MaxFanAngle2;
	bUseFirstPattern = !bUseFirstPattern; // 다음 호출에서는 다른 각도를 사용하도록 토글

	float FanAngleStep = MaxFanAngle / (Pattern.NumberOfBullets - 1);

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		float AngleOffset = -MaxFanAngle / 2 + FanAngleStep * i;
		FRotator SpawnRotation = BossRotation;
		SpawnRotation.Yaw += AngleOffset;

		BulletSpawner->SpawnPooledBullet(BossLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
	}
	UE_LOG(LogTemp, Warning, TEXT("Fan"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}


// 유도 원형 : 발사될 때는 원형이다가 Player에게 다가올 때 쯤에는 한 점으로 모임 /////////////0624
void ABoss::FireTargetCirclePattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(-200, 200) + GetActorUpVector() * FMath::RandRange(-150, 150);
	float Radius = Pattern.CircleRadius; // 반지름을 Pattern.CircleRadius로 설정

	// 플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		// YZ 평면에서 균일한 위치 생성
		float Angle = (360.0f / Pattern.NumberOfBullets) * i;
		float Rad = FMath::DegreesToRadians(Angle);
		FVector Offset = FVector(0.0f, FMath::Cos(Rad) * Radius, FMath::Sin(Rad) * Radius);
		FVector SpawnLocation = BossLocation + Offset;

		// 플레이어를 향한 방향 계산
		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();

		// 총알 스폰
		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
		if (Bullet != nullptr)
		{
			Bullet->SetVelocity(Direction * Pattern.BulletSpeed);
			Bullet->SetBulletSpeed(Pattern.BulletSpeed);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetCircle"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorUpVector() * -200;
	TArray<FVector> CircleShape;
	// BossLocation.Z = 500.0f;

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
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
	}
	UE_LOG(LogTemp, Warning, TEXT("Swirl"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + GetActorRightVector() * FMath::RandRange(0, 400) + GetActorUpVector() * FMath::RandRange(0, 400) /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;
	// 보스의 전방 벡터
	FVector Forward = GetActorForwardVector();

	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	// 탄환 발사 위치 설정
	FVector CrossOffsets[] = {
        FVector(0, 0, 0), // 중심
        FVector(0, 200, 0), // 위쪽 중간
        FVector(0, 400, 0), // 위쪽
        FVector(0, -200, 0), // 아래쪽 중간
        FVector(0, -400, 0), // 아래쪽
        FVector(0, 0, 200), // 앞쪽 중간
        FVector(0, 0, 400), // 앞쪽
        FVector(0, 0, -200), // 뒤쪽 중간
        FVector(0, 0, -400) // 뒤쪽
	};

	for (int32 j = 0; j < 2; j++)
	{
		for (int32 i = 0; i < 9; ++i)
		{
			FVector SpawnLocation = BossLocation + Forward * CrossOffsets[i].X + FVector::RightVector * CrossOffsets[i].Y + FVector::UpVector * CrossOffsets[i].Z;
			// 플레이어를 향한 방향 계산
			FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
			FRotator SpawnRotation = Direction.Rotation();
			BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("TargetCross"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	FRotator BossRotation = GetActorRotation();
	// BossLocation.Z = 500.0f;
	// 탄환 발사 위치 설정
	FVector Wall[] = {
		FVector(0, 400, -600), FVector(0, 300, -600), FVector(0, 200, -600), FVector(0, 100, -600), FVector(0, 0, -600), FVector(0, -100, -600), FVector(0, -200, -600), FVector(0, -300, -600), FVector(0, -400, -600),
        FVector(0, 400, -400), FVector(0, 300, -400), FVector(0, 200, -400), FVector(0, 100, -400), FVector(0, 0, -400), FVector(0, -100, -400), FVector(0, -200, -400), FVector(0, -300, -400), FVector(0, -400, -400),
        FVector(0, 400, -200), FVector(0, 300, -200), FVector(0, 200, -200), FVector(0, 100, -200), FVector(0, 0, -200), FVector(0, -100, -200), FVector(0, -200, -200), FVector(0, -300, -200), FVector(0, -400, -200),
    };

	int WallSize = sizeof(Wall) / sizeof(Wall[0]); // 배열의 크기
	int WallWidth = 9; // YZ 평면에서 한 줄의 벽 개수

	for (int i = 0; i < WallSize; i++)
	{
		// 비어있는 열이 아닌 경우에만 벽 생성
		if (i % WallWidth != CurrentEmptyLine && i % WallWidth != CurrentEmptyLine + 1 && i % WallWidth != CurrentEmptyLine + 2)
		{
			FVector SpawnLocation = BossLocation + GetActorForwardVector() * Wall[i].X + GetActorRightVector() * Wall[i].Y + GetActorUpVector() * Wall[i].Z;

			BulletSpawner->SpawnPooledBullet(SpawnLocation, BossRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
		}
	}

	// 비어있는 열을 오른쪽으로 이동
	CurrentEmptyLine = (CurrentEmptyLine + 1) % WallWidth;
	UE_LOG(LogTemp, Warning, TEXT("Wall"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	FRotator BossRotation = GetActorRotation();
	// BossLocation.Z = 500.0f;
	// 탄환 발사 위치 설정
	FVector Octagon[] = {
		FVector(0.0f, 0.0f, 700.0f),
		FVector(0.0f, -158.51f * 2, 269.67f * 2),  // 간격을 두 배로 늘림 0624
		FVector(0.0f, -317.02f * 2, 169.67f * 2),
		FVector(0.0f, -475.53f * 2, 77.26f * 2),
		FVector(0.0f, -414.98f * 2, -31.83f * 2),
		FVector(0.0f, -354.44f * 2, -119.17f * 2),
		FVector(0.0f, -293.89f * 2, -202.26f * 2),
		FVector(0.0f, -97.96f * 2, -202.26f * 2),
		FVector(0.0f, 97.96f * 2, -202.26f * 2),
		FVector(0.0f, 293.89f * 2, -202.26f * 2),
		FVector(0.0f, 354.44f * 2, -119.17f * 2),
		FVector(0.0f, 414.98f * 2, -31.83f * 2),
		FVector(0.0f, 475.53f * 2, 77.26f * 2),
		FVector(0.0f, 317.02f * 2, 169.67f * 2),
		FVector(0.0f, 158.51f * 2, 269.67f * 2),
		FVector(0.0f, 0.0f, 700.0f)
	};


	//플레이어 위치
	FVector PlayerLocation = player->GetActorLocation();

	int StarSize = sizeof(Octagon) / sizeof(Octagon[0]); // 배열의 크기

	for (int i = 0; i < StarSize; i++)
	{
		FVector SpawnLocation = BossLocation + GetActorForwardVector() * Octagon[i].X + GetActorRightVector() * Octagon[i].Y + GetActorUpVector() * Octagon[i].Z;

		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();
		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
	}
	UE_LOG(LogTemp, Warning, TEXT("Octagon"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;

	TArray<FVector> HeartShape;
	DefineHeartShape(HeartShape, Pattern.NumberOfBullets, Pattern.PatternSize);

	for (const FVector& Offset : HeartShape)
	{
		FVector SpawnLocation = BossLocation + Offset;
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
	}
	UE_LOG(LogTemp, Warning, TEXT("Heart"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
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
		//OutShape.Add(FVector(0.0f, x * PatternSize / 8.0f, y * PatternSize / 6.5f));
		//OutShape.Add(FVector(0.0f, x * PatternSize / 6.0f, y * PatternSize / 5.5f));
		//OutShape.Add(FVector(0.0f, x * PatternSize / 4.0f, y * PatternSize / 3.5f));
		OutShape.Add(FVector(0.0f, x * PatternSize / 5.0f, y * PatternSize / 4.0f));
	
	}
}

void ABoss::FireDandelionPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;

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

			ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
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

	UE_LOG(LogTemp, Warning, TEXT("Dandelion"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}

void ABoss::FireHAPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;


	TArray<FVector> HAPatternShape = {


		// "H"
		FVector(0, 350, 400), FVector(0, 350, 250), FVector(0, 350, 100), FVector(0, 350, -50), FVector(0, 350, -200),
		FVector(0, 250, 100), FVector(0, 150, 100),
		FVector(0, 50, 400), FVector(0, 50, 250), FVector(0, 50, 100), FVector(0, 50, -50), FVector(0, 50, -200),

		// "A"
		FVector(0, -350, 400),
		FVector(0, -300, 250),FVector(0, -250, 100),
		FVector(0, -200, -50), FVector(0, -150, -200),
		FVector(0, -400, 250), FVector(0, -450, 100), FVector(0, -500, -50), FVector(0, -550, -200),
		FVector(0, -300, 100), FVector(0, -400, 100)


	};

	for (const FVector& Offset : HAPatternShape)
	{
		FVector SpawnLocation = BossLocation + Offset;

		// 보스의 정면을 향하는 방향으로 설정
		FRotator SpawnRotation = GetActorRotation();

		BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
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
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		FVector InitialOffset = Pattern.InitialPositions.IsValidIndex(i) ? Pattern.InitialPositions[i] : FVector::ZeroVector;
		FVector SpawnLocation = BossLocation + InitialOffset;

		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, GetActorRotation(), Pattern.BulletSpeed, Pattern.FloatIntensity);
		if (Bullet)
		{
			Bullet->SetPatternType(Pattern.PatternType);
			Bullet->CircularCenter = SpawnLocation; // 초기 위치를 중심으로 설정
			Bullet->CircularRadius = Pattern.PatternSize;
			Bullet->CircularSpeed = Pattern.OrbitSpeed; // 원을 그리는 속도 설정
			Bullet->CurrentAngle = 0.0f;
			Bullet->ForwardDirection = GetActorForwardVector(); // 보스의 전진 방향 설정
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("CircularMoving"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}

void ABoss::FirePinwheelPattern(const FBulletHellPattern& Pattern)
{
	FVector BossLocation = GetActorLocation() + GetActorForwardVector() * 100.0f /*+ GetActorUpVector() * 100*/;
	// BossLocation.Z = 500.0f;

	int32 NumBullets = Pattern.NumberOfBullets;

	for (int32 i = 0; i < NumBullets; ++i)
	{
		float InitialAngle = Pattern.InitialAngles.IsValidIndex(i) ? Pattern.InitialAngles[i] : 0.0f; // 초기 각도 설정

		// 각 총알의 궤도 반지름 및 초기 각도 설정
		FVector InitialOffset = FVector(Pattern.OrbitRadii.IsValidIndex(i) ? Pattern.OrbitRadii[i] : 0.0f,
			0.0f,
			Pattern.OrbitRadii.IsValidIndex(i) ? Pattern.OrbitRadii[i] : 0.0f);
		InitialOffset = InitialOffset.RotateAngleAxis(InitialAngle, FVector::UpVector);

		FVector SpawnLocation = BossLocation + InitialOffset;

		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, GetActorRotation(), Pattern.BulletSpeed, Pattern.FloatIntensity);
		if (Bullet)
		{
			Bullet->SetPatternType(Pattern.PatternType);
			Bullet->CircularCenter = BossLocation; // 중심점 설정
			Bullet->CircularRadius = Pattern.OrbitRadii.IsValidIndex(i) ? Pattern.OrbitRadii[i] : 0.0f;
			Bullet->CircularSpeed = Pattern.OrbitSpeeds.IsValidIndex(i) ? Pattern.OrbitSpeeds[i] : 0.0f; // 각 총알의 궤도 속도 설정
			Bullet->CurrentAngle = InitialAngle;
			Bullet->ForwardDirection = GetActorForwardVector();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Pinwheel"));
	UE_LOG(LogTemp, Warning, TEXT("-----------------------"));
}

void ABoss::FireMeteorShowerPattern(const FBulletHellPattern& Pattern)
{
	if (!player)
		return;

	FVector PlayerLocation = player->GetActorLocation();
	FVector BossLocation = GetActorLocation();

	// 보스 머리 위 약간 더 앞에서 시작 (높이를 적절히 조정)
	FVector StartLocation = BossLocation + FVector(800.0f, 0.0f, 800.0f); // 보스의 머리 위에서 더 앞쪽

	for (int32 i = 0; i < Pattern.NumberOfBullets; ++i)
	{
		// 각 탄막의 위치를 랜덤하게 조정하여 퍼짐 효과를 줌
		float RandomOffsetY = FMath::FRandRange(-300.0f, 300.0f);
		float RandomOffsetX = FMath::FRandRange(-300.0f, 300.0f);
		FVector SpawnLocation = StartLocation + FVector(RandomOffsetX, RandomOffsetY, 0.0f);

		// 탄막이 플레이어를 향해 아래로 내리꽂히도록 방향 설정
		FVector Direction = (PlayerLocation - SpawnLocation).GetSafeNormal();
		FRotator SpawnRotation = Direction.Rotation();

		// 탄막 생성 및 방향 설정
		ABullet_Pooled* Bullet = BulletSpawner->SpawnPooledBullet(SpawnLocation, SpawnRotation, Pattern.BulletSpeed, Pattern.FloatIntensity);
		if (Bullet)
		{
			Bullet->SetVelocity(Direction * Pattern.BulletSpeed); // 방향 설정
		}
	}
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
	RandomStraightPattern.NumberOfBullets = 15;
	RandomStraightPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(RandomStraightPattern);

	// 부채꼴
	FBulletHellPattern FanPattern;
	FanPattern.PatternType = EPatternType::Fan;
	FanPattern.FanAngle = 90.0f; // 부채꼴 패턴의 각도 설정
	FanPattern.NumberOfBullets = 20; // 부채꼴 패턴에서 발사할 총알 수
	FanPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(FanPattern);

	// 원형 패턴
	FBulletHellPattern TargetCirclePattern;
	TargetCirclePattern.PatternType = EPatternType::TargetCircle;
	TargetCirclePattern.PatternSize = 300.0f; // 원형 패턴의 크기 설정
	TargetCirclePattern.CircleRadius = 500.0f; // 원형 패턴의 반지름 설정 (예: 500.0f)
	TargetCirclePattern.NumberOfBullets = 12; // 총알의 수
	TargetCirclePattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(TargetCirclePattern);

	// 소용돌이(십자가) 
	FBulletHellPattern SwirlPattern;
	SwirlPattern.PatternType = EPatternType::Swirl;
	SwirlPattern.PatternSize = 150.0f; // 원형 패턴의 크기 설정
	SwirlPattern.NumberOfBullets = 4; // 총알의 수
	SwirlPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(SwirlPattern);

	// 유도 십자가
	FBulletHellPattern TargetCrossPattern;
	TargetCrossPattern.PatternType = EPatternType::TargetCross;
	TargetCrossPattern.NumberOfBullets = 20;
	TargetCrossPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(TargetCrossPattern);

	// 벽 패턴
	FBulletHellPattern WallPatter;
	WallPatter.PatternType = EPatternType::Wall;
	WallPatter.NumberOfBullets = 30;
	WallPatter.BulletSpeed = 600.0f;
	BulletPatterns.Add(WallPatter);

	// 유도 오각형
	FBulletHellPattern OctagonPattern;
	OctagonPattern.PatternType = EPatternType::Octagon;
	OctagonPattern.NumberOfBullets = 30;
	OctagonPattern.BulletSpeed = 600.0f;
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
	HeartPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(HeartPattern);

	// 민들레 패턴 
	FBulletHellPattern DandelionPattern;
	DandelionPattern.PatternType = EPatternType::Dandelion;
	DandelionPattern.NumberOfBullets = 30;
	DandelionPattern.BulletSpeed = 600.0f;
	DandelionPattern.SpreadDelay = 2.0f; // 몇 초 후에 퍼짐
	BulletPatterns.Add(DandelionPattern);

	// HA 패턴
	FBulletHellPattern HAPattern;
	HAPattern.PatternType = EPatternType::HA;
	HAPattern.NumberOfBullets = 23;
	HAPattern.BulletSpeed = 600.0f;
	BulletPatterns.Add(HAPattern);

	// CircularMoving 패턴 추가
	FBulletHellPattern CircularMovingPattern;
	CircularMovingPattern.PatternType = EPatternType::CircularMoving;
	CircularMovingPattern.NumberOfBullets = 12;
	CircularMovingPattern.BulletSpeed = 700.0f; // 앞으로 전진하는 속도
	CircularMovingPattern.OrbitSpeed = 150.0f; // 원을 그리는 속도
	CircularMovingPattern.PatternSize = 300.0f; // 원형 반지름
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
	PinwheelPattern.NumberOfBullets = 17;
	PinwheelPattern.BulletSpeed = 1000.0f; // 앞으로 전진하는 속도

	// 각 총알의 궤도 반지름과 속도를 배열로 설정
	PinwheelPattern.OrbitRadii = { 0.0f, 100.0f, 200.0f, 300.0f, 400.0f, -100.0f, -200.0f, -300.0f, -400.0f,100.0f, 200.0f, 300.0f, 400.0f, -100.0f, -200.0f, -300.0f, -400.0f }; // 궤도 반지름 배열
	PinwheelPattern.OrbitSpeeds = { 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f, 80.0f }; // 궤도 속도 배열
	//PinwheelPattern.InitialAngles = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 90.0f, 90.0f, 90.0f, 90.0f, 180.0f, 180.0f, 180.0f, 180.0f, 270.0f, 270.0f, 270.0f, 270.0f }; // 초기 각도 배열
	PinwheelPattern.InitialAngles = { 0.0f, 0.0f, 10.0f, 20.0f, 30.0f, 90.0f, 100.0f, 110.0f, 120.0f, 180.0f, 190.0f, 200.0f, 210.0f, 270.0f, 280.0f, 290.0f, 300.0f };//멋 부림

	BulletPatterns.Add(PinwheelPattern);

	// 별똥별 패턴
	FBulletHellPattern MeteorShowerPattern;
	MeteorShowerPattern.PatternType = EPatternType::MeteorShower;
	MeteorShowerPattern.NumberOfBullets = 20;
	MeteorShowerPattern.BulletSpeed = 800.0f; // 탄막 속도 조정
	BulletPatterns.Add(MeteorShowerPattern); 
}
