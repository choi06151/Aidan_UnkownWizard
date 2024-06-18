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

	default:
		break;
	}

	// if(me->bIsArrive)
	// 	commandTime += DeltaTime;
}

// bIsGameStart, bIsWalk, bIsArrive, bIsAttack, bIsPhase, bIsDie

//////////////////////////////////////// Idle ////////////////////////////////////////
void UBossFSM::TickIdle()
{
	if (me->bIsGameStart && me->bIsWalk && !me->bIsArrive)
			SetState(EBossState::WALK);
	
	if (me->bIsGameStart && me->bIsArrive && me->bIsCommandWait)
		SetState(EBossState::COMMANDWAIT);// 도착하고 지휘대기

	// UE_LOG(LogTemp, Error, TEXT("IDLE"));
}

//////////////////////////////////////// Walk ////////////////////////////////////////
void UBossFSM::TickWalk()
{
	ai->MoveToLocation(FVector(1480.000000, 650.000000, 170), 0.1f);
	if(me->bIsArrive)
		SetState(EBossState::IDLE);
	
	// UE_LOG(LogTemp, Warning, TEXT("Walk"));
}

//////////////////////////////////////// CommandWait ////////////////////////////////////////
void UBossFSM::TickCommandWait()
{
	// 지휘대기 애니메이션이 끝날 경우
	if(me->bIsAttackStart)
		SetState(EBossState::COMMAND);
	
	// UE_LOG(LogTemp, Error, TEXT("COMMANDWAIT // COMMANDWAIT // COMMANDWAIT // COMMANDWAIT // COMMANDWAIT"));
}

//////////////////////////////////////// Command ////////////////////////////////////////
void UBossFSM::TickCommand()
{
	// 일정 조건일 때 Phase2 진입
	if (me->bIsPhase)
		SetState(EBossState::PHASE_2);

	// UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

//////////////////////////////////////// Phase_2 ////////////////////////////////////////
void UBossFSM::TickPhase_2()
{
	// UE_LOG(LogTemp, Warning, TEXT("Phase"));
}

//////////////////////////////////////// GmaeClear ////////////////////////////////////////
void UBossFSM::GameEnd()
{
	me->bClearGame = false;
	/* 죽음 Montage, Sound 추가 */
	me->PlayAnimMontage(deadMontage, 1, FName("Dead"));
}

void UBossFSM::SetState(EBossState next)
{
	state = next;
}
