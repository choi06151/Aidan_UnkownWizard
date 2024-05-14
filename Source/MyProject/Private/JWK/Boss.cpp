#include "JWK/Boss.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/Bullet_Pooled.h"
// #include "JWK/BossFSM.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	// bossFSM = CreateDefaultSubobject<UBossFSM>(TEXT("bossFSM"));

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

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempBatonMesh(TEXT("/Script/Engine.StaticMesh'/Game/JWK/Asset/Baton/Baton.Baton'"));
	if(tempBatonMesh.Succeeded())
	{
		batonMesh->SetStaticMesh(tempBatonMesh.Object);
		batonMesh->SetRelativeLocationAndRotation(FVector(2, -1, 10), FRotator(-90, 65, -25));
		batonMesh->SetWorldScale3D(FVector(1, 1, 1));
	}
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FTransform t = bossMesh->GetSocketTransform(TEXT("BulletFire"));

	curTime += DeltaTime;
	Time += DeltaTime;
	
	if (curTime >= 2 && Time <=20)
	{
		curTime = 0;
		// GetWorld()->SpawnActor<ABullet_Pooled>(bulletFactory, t);
	}
}

void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
