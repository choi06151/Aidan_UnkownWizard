#include "JWK/BossFSM.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "JWK/Anim_Boss.h"
#include "JWK/Boss.h"

UBossFSM::UBossFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	state = EBoss_state::IDLE;
}


void UBossFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ABoss>(GetOwner());

	bossAnim = Cast<UAnim_Boss>(me->GetMesh()->GetAnimInstance());

}


void UBossFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (state)
	{
	case EBoss_state::IDLE:
		TickIdle();
		break;

	case EBoss_state::WALK:
		TickWalk();
		break;
		
	case EBoss_state::ATTACK:
		TickAttack();
		break;
		
	case EBoss_state::DEAD:
		TickDead();
		break;
	}

	// 만약 보스가 죽었다면 state를 dead로 set
	if(state != EBoss_state::DEAD && me->bIsDie == true)
		SetState(EBoss_state::DEAD);
	
}

// bIsGameStart, bIsWalk, bIsArrive, bIsAttack, bIsPhase, bIsDie

//////////////////////////////////////// Idle ////////////////////////////////////////
void UBossFSM::TickIdle()
{
	if(me->bIsGameStart)
	{
		me->bIsWalk = true;
		SetState(EBoss_state::WALK);
	}

	if(me->bIsAttack == true && me->bIsWalk == false)
		SetState(EBoss_state::ATTACK);
	
	UE_LOG(LogTemp, Warning, TEXT("IDLE"));
}

//////////////////////////////////////// Walk ////////////////////////////////////////
void UBossFSM::TickWalk()
{
	curTime += GetWorld()->GetDeltaSeconds();

	bossAnim->speed = 800;
	
	if(curTime >= 5)
	{
		bossAnim->speed = 0;
		me->bIsGameStart = false;
		me->bIsWalk = false;
		SetState(EBoss_state::IDLE);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Walk"));
}

//////////////////////////////////////// Attack ////////////////////////////////////////
void UBossFSM::TickAttack()
{
	// 일정 체력 이하일 때 Phase2 진입
	if(me->bIsPhase)
		SetState(EBoss_state::Phase);

	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

//////////////////////////////////////// Phase ////////////////////////////////////////
void UBossFSM::TickPhase()
{
	// 체력이 0이 되었을 때 죽음
	if(me->bIsDie)
		SetState(EBoss_state::DEAD);

	UE_LOG(LogTemp, Warning, TEXT("Phase"));
}

//////////////////////////////////////// Dead ////////////////////////////////////////
void UBossFSM::TickDead()
{
	me->GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	UE_LOG(LogTemp, Warning, TEXT("DEAD"));
}

void UBossFSM::SetState(EBoss_state nextState)
{
	state = nextState;
	curTime = 0;
}

