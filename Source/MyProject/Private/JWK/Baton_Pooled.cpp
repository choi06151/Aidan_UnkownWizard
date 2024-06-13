#include "JWK/Baton_Pooled.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "JWK/Boss.h"

ABaton_Pooled::ABaton_Pooled()
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
	movementComp->InitialSpeed = 0.0f;
	movementComp->MaxSpeed = 3000.0f;
	movementComp->bShouldBounce = false;
}

void ABaton_Pooled::BeginPlay()
{
	Super::BeginPlay();
}

void ABaton_Pooled::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 현재 Baton의 속도를 계산
	FVector BatonVelocity = GetActorForwardVector() * movementComp->InitialSpeed;

	FVector MoveDelta = BatonVelocity * DeltaTime + GetActorForwardVector() + GetActorUpVector();

	FVector NewLocation = GetActorLocation() + MoveDelta;
	
	// 새로운 위치로 총알 이동
	SetActorLocation(NewLocation);

	// // meshComponent 회전 로직 추가
	// FRotator CurrentRotation = meshComp->GetComponentRotation();
	// FRotator DeltaRotation(0, DeltaTime * 500, 0);  // Y축을 기준으로 회전 속도 설정
	// meshComp->SetWorldRotation(CurrentRotation + DeltaRotation);
}

// Baton 상태 활성화 or 비활성화
void ABaton_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABaton_Pooled::Deactivate, LifeSpan, false);
}

// Baton 비활성화
void ABaton_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBatonDespawn.Broadcast(this);
}

// Baton의 수명을 set
void ABaton_Pooled::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

// Baton의 Speed Set
void ABaton_Pooled::SetBatonSpeed(float Speed)
{
	movementComp->InitialSpeed = Speed;
}

// pool에서의 index 설정
void ABaton_Pooled::SetPoolIndex(int index)
{
	PoolIndex = index;
}

// 총알의 활성 or 비활성 bool 값 반환
bool ABaton_Pooled::IsActive()
{
	return Active;
}

// pool에서의 index 값 반환
int ABaton_Pooled::GetPoolIndex()
{
	return PoolIndex;
}


