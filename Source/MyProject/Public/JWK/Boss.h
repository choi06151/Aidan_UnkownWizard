#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Boss.generated.h"

UCLASS()
class MYPROJECT_API ABoss : public ACharacter
{
	GENERATED_BODY()

public:
	ABoss();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UBossFSM* bossFSM;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* bossMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterMovementComponent* movementComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* batonMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABullet_Pooled> bulletFactory;

	// 변수
	UPROPERTY(EditAnywhere)
	int curTime = 0;
	UPROPERTY(EditAnywhere)
	int Time = 0;
	
	UPROPERTY(EditAnywhere)
	bool IsDie = false;
};
