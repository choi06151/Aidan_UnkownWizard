#pragma once

#include "CoreMinimal.h"
#include "Spawn_Bullet.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* bossMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCharacterMovementComponent* movementComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* batonMesh;


	// Bullet 발사 관련
	void FireBullet();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USpawn_Bullet* BulletSpawner;

	
	// Boss의 상태
	UPROPERTY(EditAnywhere)
	bool IsDie = false;

	
private:
	FTimerHandle FireRateTimerHandle;

	// 탄막 발사 타이머 설정 함수
	void StartFiring();
	void StopFiring();
};
