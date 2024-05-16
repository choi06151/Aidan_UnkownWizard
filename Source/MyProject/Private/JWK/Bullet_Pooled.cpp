#include "JWK/Bullet_Pooled.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABullet_Pooled::ABullet_Pooled()
{
	PrimaryActorTick.bCanEverTick = true;

	// boxComponent
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	SetRootComponent(boxComp);
	boxComp->SetWorldScale3D(FVector(1.0f));
	boxComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// meshComponent
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetWorldScale3D(FVector(0.025f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// movementComponent
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

// 총알 비활성화
void ABullet_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBulletDespawn.Broadcast(this);
}

// 총알 상태 활성화 or 비활성화
void ABullet_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABullet_Pooled::Deactivate, LifeSpan, false);
}

// bullet의 수명을 set
void ABullet_Pooled::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

// pool에서의 index 설정
void ABullet_Pooled::SetPoolIndex(int index)
{
	PoolIndex = index;
}

// 총알의 활성 or 비활성 bool 값 반환
bool ABullet_Pooled::IsActive()
{
	return Active;
}

// pool에서의 index 값 반환
int ABullet_Pooled::GetPoolIndex()
{
	return PoolIndex;
}
