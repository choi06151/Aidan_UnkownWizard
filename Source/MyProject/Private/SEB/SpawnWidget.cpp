// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SpawnWidget.h"

#include "Components/WidgetComponent.h"
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
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World); // Set to Screen Space if needed
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

}

// Called every frame
void ASpawnWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnWidget::PlayMusicAndLoadData(const FString& MusicFilePath, const FString& JsonFilePath)
{
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Loading JSON from: %s"), *JsonFilePath);
	/*if (Boss)
	{
		//Boss->LoadMusicDataAndSetPatterns(JsonFilePath); // JSON 파일 경로 전달
	}*/

	// 음악 재생
	UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Playing music from: %s"), *MusicFilePath);
	USoundBase* Music = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, *MusicFilePath));
	if (Music)
	{
		UGameplayStatics::PlaySound2D(this, Music);
		//GetWorld()->GetTimerManager().SetTimer(Boss->PatternUpdateTimerHandle, Boss, &ABoss::UpdatePatternConditions, 1.0f, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTestUI::PlayMusicAndLoadData: Failed to load music: %s"), *MusicFilePath);
	}
}

void ASpawnWidget::MusicPlay()
{
	FString MusicFilePath = SpecificRow->MusicFilePath;
	FString JsonFilePath = UKismetSystemLibrary::GetProjectDirectory() + SpecificRow->JsonFilePath;
	PlayMusicAndLoadData(MusicFilePath, JsonFilePath);
}

