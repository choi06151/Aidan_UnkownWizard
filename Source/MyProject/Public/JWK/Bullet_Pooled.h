#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet_Pooled.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledBulletDespawn, ABullet_Pooled*, Bullet);

UCLASS()
class MYPROJECT_API ABullet_Pooled : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet_Pooled();

	FOnPooledBulletDespawn OnPooledBulletDespawn;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//////////////////////////// Component ////////////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	class UProjectileMovementComponent* movementComp;
	
	//////////////////////////// Pool ////////////////////////////
	// TimerHandle이 만료될 때 호출 될 함수
	UFUNCTION(BlueprintCallable, Category = "Bullet_Pooled")
	void Deactivate();

	void SetActive(bool IsActive);
	// bullet의 수명
	void SetLifeSpan(float LifeTime);
	// index 정수 할당
	void SetPoolIndex(int index);
	// bool Active 값만 반환
	bool IsActive();
	// int PoolIndex 값만 반환
	int GetPoolIndex();

protected:
	bool Active;
	float LifeSpan = 0.0f;
	int PoolIndex;

	FTimerHandle LifespanTimer;
};
