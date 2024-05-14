#include "JWK/Bullet_Pooled.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABullet_Pooled::ABullet_Pooled()
{
	PrimaryActorTick.bCanEverTick = true;

	// boxComp 생성해서 RootComponent에 붙이기
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetWorldScale3D(FVector(1.0f));
	boxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetWorldScale3D(FVector(0.025f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));

	movementComp->InitialSpeed = 800.0f;
	movementComp->MaxSpeed = 8000.0f;
	movementComp->bShouldBounce = false;
}

void ABullet_Pooled::BeginPlay()
{
	Super::BeginPlay();

	movementComp->SetUpdatedComponent(boxComp);
}

void ABullet_Pooled::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABullet_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBulletDespawn.Broadcast(this);
}

void ABullet_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABullet_Pooled::Deactivate, LifeSpan, false);
}

void ABullet_Pooled::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

void ABullet_Pooled::SetPoolIndex(int index)
{
	PoolIndex = index;
}

bool ABullet_Pooled::IsActive()
{
	return Active;
}

int ABullet_Pooled::GetPoolIndex()
{
	return PoolIndex;
}
