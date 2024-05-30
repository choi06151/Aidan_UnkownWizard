#include "JWK/Anim_Boss.h"

#include "JWK/Boss.h"
#include "Kismet/KismetMathLibrary.h"

void UAnim_Boss::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	me = Cast<ABoss>(TryGetPawnOwner());
	
}

void UAnim_Boss::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr != me)
		speed = UKismetMathLibrary::VSize(me->GetVelocity());
}
