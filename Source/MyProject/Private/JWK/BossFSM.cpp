#include "JWK/BossFSM.h"

#include "AIController.h"
#include "JWK/Anim_Boss.h"
#include "JWK/Boss.h"

UBossFSM::UBossFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBossFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ABoss>(GetOwner());

	bossAnim = Cast<UAnim_Boss>(me->GetMesh()->GetAnimInstance());

	ai = Cast<AAIController>(me->GetController());
}


void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (state)
	{
	case EBossState::IDLE: TickIdle();
		break;

	case EBossState::WALK: TickWalk();
		break;

	case EBossState::ATTACK: TickAttack();
		break;

	case EBossState::PHASE_2: TickPhase_2();
		break;
	}
}

// bIsGameStart, bIsWalk, bIsArrive, bIsAttack, bIsPhase, bIsDie

//////////////////////////////////////// Idle ////////////////////////////////////////
void UBossFSM::TickIdle()
{
	if (me->bIsGameStart && me->bIsWalk && !me->bIsArrive)
		SetState(EBossState::WALK);

	if(me->bIsArrive && me->bIsAttack)
		SetState(EBossState::ATTACK);
	
	// UE_LOG(LogTemp, Warning, TEXT("IDLE"));
}

//////////////////////////////////////// Walk ////////////////////////////////////////
void UBossFSM::TickWalk()
{
	ai->MoveToLocation(FVector(1480.000000, 650.000000, 169.999991), 0.1f);
	if(me->bIsArrive)
		SetState(EBossState::IDLE);
	
	// UE_LOG(LogTemp, Warning, TEXT("Walk"));
}

//////////////////////////////////////// Attack ////////////////////////////////////////
void UBossFSM::TickAttack()
{
	// 일정 체력 이하일 때 Phase2 진입
	if (me->bIsPhase)
		SetState(EBossState::PHASE_2);

	// UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

//////////////////////////////////////// Phase_2 ////////////////////////////////////////
void UBossFSM::TickPhase_2()
{
	// UE_LOG(LogTemp, Warning, TEXT("Phase"));
}

void UBossFSM::TakeDamaged(int damage)
{
	bossHP -= damage;

	if(bossHP <=0)
	{
		me->bIsDie = true;
		/* 죽음 Montage, Sound 추가 */
		me->PlayAnimMontage(deadMontage, 1, FName("Dead"));
	}
}

void UBossFSM::SetState(EBossState next)
{
	state = next;
}
