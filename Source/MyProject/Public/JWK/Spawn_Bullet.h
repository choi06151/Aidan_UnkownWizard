#pragma once

#include "CoreMinimal.h"
#include "Bullet_Pooled.h"
#include "Components/ActorComponent.h"
#include "Spawn_Bullet.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API USpawn_Bullet : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawn_Bullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Pooled Bullet 호출
	UFUNCTION(BlueprintCallable, Category = "Pool_Bullet")
	ABullet_Pooled* SpawnPooledBullet();
	
	UPROPERTY(EditAnywhere, Category = "Pool_Bullet")
	TSubclassOf<class ABullet_Pooled> PooledBulletSubclass;

	UPROPERTY(EditAnywhere, Category = "Pool_Bullet")
	int PoolSize = 20;
	
	UPROPERTY(EditAnywhere, Category = "Pool_Bullet")
	float PooledBulletLifeSpan = 0.0f;

	UFUNCTION()
	void OnPooledBulletDespawn(ABullet_Pooled* Bullet);

protected:
	TArray<ABullet_Pooled*> BulletPool;
	TArray<int> SpawnedPoolIndexes;
};
