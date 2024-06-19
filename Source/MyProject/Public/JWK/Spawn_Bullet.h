#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Bullet_Pooled.h"
#include "Spawn_Bullet.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYPROJECT_API USpawn_Bullet : public UActorComponent
{
	GENERATED_BODY()

public:
	USpawn_Bullet();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Bullet을 Pool에서 Spawn하는 함수
	ABullet_Pooled* SpawnPooledBullet(FVector SpawnLocation, FRotator SpawnRotation, float BulletSpeed, int FloatIntensity);

	// Bullet이 비활성화될 때 호출되는 함수
	UFUNCTION()
	void OnPooledBulletDespawn(ABullet_Pooled* Bullet);

	// 모든 활성화된 Bullet 비활성화
	void DeactivateAllBullets();

	// Bullet Pool을 반환하는 함수 추가////0619
	const TArray<ABullet_Pooled*>& GetBulletPool() const;

private:
	UPROPERTY(EditAnywhere, Category="Bullet Pool")
	TSubclassOf<ABullet_Pooled> PooledBulletSubclass;

	UPROPERTY(EditAnywhere, Category="Bullet Pool")
	int32 PoolSize;

	UPROPERTY(EditAnywhere, Category="Bullet Pool")
	float PooledBulletLifeSpan;

	TArray<ABullet_Pooled*> BulletPool;
	TArray<int32> SpawnedPoolIndexes;
};
