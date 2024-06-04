#include "JWK/Anim_Boss.h"

#include "JWK/Boss.h"

void UAnim_Boss::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	auto owner = TryGetPawnOwner();
	if(nullptr == owner)
		return;
	
	ABoss* me = Cast<ABoss>(owner);

	bossFSM = me->bossFSM;
}

void UAnim_Boss::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == bossFSM)
		return;

	state = bossFSM->state;
}

void UAnim_Boss::AnimNotify_Boss_Throw()
{
}
