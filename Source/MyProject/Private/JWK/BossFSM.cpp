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

	case EBossState::COMMANDWAIT: TickCommandWait();
		break;
		
	case EBossState::COMMAND: TickCommand();
		break;

	case EBossState::PHASE_2: TickPhase_2();
		break;
	}

	if(me->bIsArrive)
		commandTime += DeltaTime;
}

// bIsGameStart, bIsWalk, bIsArrive, bIsAttack, bIsPhase, bIsDie

//////////////////////////////////////// Idle ////////////////////////////////////////
void UBossFSM::TickIdle()
{
	if (me->bIsGameStart && me->bIsWalk && !me->bIsArrive)
			SetState(EBossState::WALK);
	
	// if (me->bIsGameStart && me->bIsArrive)
	// {
	// 	// 도착하고 지휘대기
	// 	SetState(EBossState::COMMANDWAIT);
	// }
	UE_LOG(LogTemp, Error, TEXT("IDLE"));
}

//////////////////////////////////////// Walk ////////////////////////////////////////
void UBossFSM::TickWalk()
{
	ai->MoveToLocation(FVector(1480.000000, 650.000000, 169.999991), 0.1f);
	if(me->bIsArrive)
		SetState(EBossState::COMMANDWAIT);
	
	UE_LOG(LogTemp, Warning, TEXT("Walk"));
}

//////////////////////////////////////// CommandWait ////////////////////////////////////////
void UBossFSM::TickCommandWait()
{
	UE_LOG(LogTemp, Error, TEXT("COMMANDWAIT // COMMANDWAIT // COMMANDWAIT // COMMANDWAIT // COMMANDWAIT"))
	// 지휘대기 애니메이션이 끝날 경우
	
	if(commandTime >= 6.5f)
		SetState(EBossState::COMMAND);
}

//////////////////////////////////////// Command ////////////////////////////////////////
void UBossFSM::TickCommand()
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

//////////////////////////////////////// Damaged ////////////////////////////////////////
void UBossFSM::TakeDamaged(int damage)
{
	bossHP -= damage;

	if(bossHP <=0)
	{
		me->bIsStageEnd = true;
		/* 죽음 Montage, Sound 추가 */
		me->PlayAnimMontage(deadMontage, 1, FName("Dead"));
	}
}

void UBossFSM::SetState(EBossState next)
{
	state = next;
}
