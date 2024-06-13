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

void UAnim_Boss::AnimNotify_Attack_Start()
{
	UE_LOG(LogTemp, Error, TEXT("CommandStart!!!!!!!!!CommandStart!!!!!!!!!"));
	me->bIsAttack = true;
	SpawnWidget->MusicPlay();
}

void UAnim_Boss::AnimNotify_Boss_Throw()
{
	me->ThrowBaton();
}
