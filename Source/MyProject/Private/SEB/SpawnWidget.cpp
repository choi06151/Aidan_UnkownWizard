// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SpawnWidget.h"

#include "Components/WidgetComponent.h"

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
}

// Called every frame
void ASpawnWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

