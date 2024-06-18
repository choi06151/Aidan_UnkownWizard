#include "JWK/Anim_Boss.h"

#include "JWK/Boss.h"
#include "SEB/SpawnWidget.h"

void UAnim_Boss::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto owner = TryGetPawnOwner();
	if(nullptr == owner)
		return;
	
	me = Cast<ABoss>(owner);

	bossFSM = me->bossFSM;
}

void UAnim_Boss::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == bossFSM)
		return;

	state = bossFSM->state;
}

// 지휘 대기 후 공격 애니메이션
void UAnim_Boss::AnimNotify_Attack_Start()
{
	UE_LOG(LogTemp, Error, TEXT("AttackStart!!!!!!!!!AttackStart!!!!!!!!!"));
	me->bIsAttackStart = true;
}

// 지휘 대기 후 음악 재생
void UAnim_Boss::AnimNotify_Music_Start()
{
	UE_LOG(LogTemp, Warning, TEXT("MusicStart!!!!!!!!!MusicStart!!!!!!!!!"));
	me->bIsMusicStart = true;
}

void UAnim_Boss::AnimNotify_Throw_Baton()
{
	me->bThrowBaton = true;
}
