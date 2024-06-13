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
	
	// ���� Baton�� �ӵ��� ���
	FVector BatonVelocity = GetActorForwardVector() * movementComp->InitialSpeed;

	FVector MoveDelta = BatonVelocity * DeltaTime + GetActorForwardVector() + GetActorUpVector();

	FVector NewLocation = GetActorLocation() + MoveDelta;
	
	// ���ο� ��ġ�� �Ѿ� �̵�
	SetActorLocation(NewLocation);

	// // meshComponent ȸ�� ���� �߰�
	// FRotator CurrentRotation = meshComp->GetComponentRotation();
	// FRotator DeltaRotation(0, DeltaTime * 500, 0);  // Y���� �������� ȸ�� �ӵ� ����
	// meshComp->SetWorldRotation(CurrentRotation + DeltaRotation);
}

// Baton ���� Ȱ��ȭ or ��Ȱ��ȭ
void ABaton_Pooled::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifespanTimer, this, &ABaton_Pooled::Deactivate, LifeSpan, false);
}

// Baton ��Ȱ��ȭ
void ABaton_Pooled::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledBatonDespawn.Broadcast(this);
}

// Baton�� ������ set
void ABaton_Pooled::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

// Baton�� Speed Set
void ABaton_Pooled::SetBatonSpeed(float Speed)
{
	movementComp->InitialSpeed = Speed;
}

// pool������ index ����
void ABaton_Pooled::SetPoolIndex(int index)
{
	PoolIndex = index;
}

// �Ѿ��� Ȱ�� or ��Ȱ�� bool �� ��ȯ
bool ABaton_Pooled::IsActive()
{
	return Active;
}

// pool������ index �� ��ȯ
int ABaton_Pooled::GetPoolIndex()
{
	return PoolIndex;
}


