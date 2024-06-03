#include "JWK/BossFSM.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	case EBossState::DEAD: TickDead();
		break;
	}

	// 만약 보스가 죽었다면 state를 dead로 set
	if (state != EBossState::DEAD && me->bIsDie == true)
		SetState(EBossState::DEAD);
}

// bIsGameStart, bIsWalk, bIsArrive, bIsAttack, bIsPhase, bIsDie

//////////////////////////////////////// Idle ////////////////////////////////////////
void UBossFSM::TickIdle()
{
	if (me->bIsGameStart && !me->bIsWalk)
		SetState(EBossState::WALK);

	UE_LOG(LogTemp, Warning, TEXT("IDLE"));
}

//////////////////////////////////////// Walk ////////////////////////////////////////
void UBossFSM::TickWalk()
{
	ai->MoveToLocation(FVector(-500, 0, 0), 5.0f);
	if(me->bIsWalk)
		SetState(EBossState::IDLE);
	
	UE_LOG(LogTemp, Warning, TEXT("Walk"));
}

//////////////////////////////////////// Attack ////////////////////////////////////////
void UBossFSM::TickAttack()
{
	// 일정 체력 이하일 때 Phase2 진입
	if (me->bIsPhase)
		SetState(EBossState::PHASE_2);

	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

//////////////////////////////////////// Phase ////////////////////////////////////////
void UBossFSM::TickPhase_2()
{
	// 체력이 0이 되었을 때 죽음
	if (me->bIsDie)
		SetState(EBossState::DEAD);

	UE_LOG(LogTemp, Warning, TEXT("Phase"));
}

//////////////////////////////////////// Dead ////////////////////////////////////////
void UBossFSM::TickDead()
{
	me->GetCharacterMovement()->SetMovementMode(MOVE_None);

	UE_LOG(LogTemp, Warning, TEXT("DEAD"));
}

void UBossFSM::SetState(EBossState next)
{
	state = next;
}
