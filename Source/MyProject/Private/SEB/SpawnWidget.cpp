// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SpawnWidget.h"

#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/GameOverUI.h"
#include "SEB/SelectStageUI.h"


// Sets default values
ASpawnWidget::ASpawnWidget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create the WidgetComponent and attach it to the root component
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	RootComponent = WidgetComponent;

	// Optionally set the WidgetComponent's properties
	WidgetComponent->SetDrawSize(FVector2D(1800, 1200));
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // Set to Screen Space if needed
	WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f)); // Center the pivot
}

// Called when the game starts or when spawned
void ASpawnWidget::BeginPlay()
{
	Super::BeginPlay();
	// Create and set the widget if the WidgetClass is set
	if (WidgetClass != nullptr)
	{
		WidgetComponent->SetWidgetClass(WidgetClass);
		WidgetComponent->InitWidget();
	}
	
	//Curtains Animation 
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("curtain"), Curtains);
	OpenCurtainAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/SEB/Anim/Curain/Anim_OpeningCurtains.Anim_OpeningCurtains'"));
	ShakeCurtainAnim = LoadObject<UAnimSequence>(nullptr, TEXT("/Script/Engine.AnimSequence'/Game/SEB/Anim/Curain/Anim_ShakingCurtains.Anim_ShakingCurtains'"));

	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
}	

// Called every frame
void ASpawnWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnWidget::MusicPlay()
{
	
	FString MusicFilePath = SpecificRow->MusicFilePath;
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + SpecificRow->JsonFilePath;
	FString MusicTitle = SpecificRow->MusicName;
	Boss->LoadMusicDataAndSetPatterns(MusicTitle, MusicFilePath);
	UE_LOG(LogTemp, Error, TEXT("ASpawnWidget::MusicPlay"));
}

void ASpawnWidget::MusicPlayOnly()
{
	
	FString MusicFilePath = SpecificRow->MusicFilePath;
	FString MusicTitle = SpecificRow->MusicName;
	Boss->PlayMusicOnly(MusicFilePath, MusicTitle);
	UE_LOG(LogTemp, Error, TEXT("sssssssssssssssssssssssssssss"));
	
}


void ASpawnWidget::CurtainOpenAnim()
{
	
	for(AActor* Curtain : Curtains)
	{
		USkeletalMeshComponent* SkeletalMesh = Curtain->FindComponentByClass<USkeletalMeshComponent>();
		if(SkeletalMesh && OpenCurtainAnim)
		{
			SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			SkeletalMesh->PlayAnimation(OpenCurtainAnim, false);
		}
	}
	
}

void ASpawnWidget::CurtainCloseAnim()
{
	for(AActor* Curtain : Curtains)
	{
		USkeletalMeshComponent* SkeletalMesh = Curtain->FindComponentByClass<USkeletalMeshComponent>();
		if(SkeletalMesh && OpenCurtainAnim)
		{
			SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			SkeletalMesh->PlayAnimation(OpenCurtainAnim, false);
			SkeletalMesh->SetPosition(OpenCurtainAnim->GetPlayLength(), false);
			SkeletalMesh->SetPlayRate(-1.0f);
		}
	}
}

void ASpawnWidget::CurtainShakeAnim()
{
	for(AActor* Curtain : Curtains)
	{
		USkeletalMeshComponent* SkeletalMesh = Curtain->FindComponentByClass<USkeletalMeshComponent>();
		if(SkeletalMesh && ShakeCurtainAnim)
		{
			SkeletalMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			SkeletalMesh->PlayAnimation(ShakeCurtainAnim, true);
		}
	}
}

