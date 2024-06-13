// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Baton_Pooled.generated.h"

// 지휘봉이 비활성화될 때 호출되는 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledBatonDespawn, ABaton_Pooled*, Baton);

UCLASS()
class MYPROJECT_API ABaton_Pooled : public AActor
{
	GENERATED_BODY()

public:
	ABaton_Pooled();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class AActor* player;

	UPROPERTY()
	class ABoss* boss;

	
	//Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UProjectileMovementComponent* movementComp;

	// Baton이 비활성화될 때 호출되는 Event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
	FOnPooledBatonDespawn OnPooledBatonDespawn;

	// Baton을 활성화 또는 비활성화하는 함수
	void SetActive(bool IsActive);

	// Baton의 수명을 set하는 함수
	void SetLifeSpan(float LifeTime);

	// Baton Speed를 set하는 함수
	void SetBatonSpeed(float Speed);

	// pool에서의 index 설정하는 함수
	void SetPoolIndex(int index);

	// 총알의 활성 or 비활성 bool 값 반환하는 함수
	bool IsActive();

	// pool에서의 index 값 반환하는 함수
	int GetPoolIndex();

	// 총알을 비활성화하는 함수
	void Deactivate();

protected:
	// 총알의 활성화 상태
	bool Active;

	// 총알의 수명
	float LifeSpan = 0.0f;

	// pool에서의 index
	int PoolIndex;

	// 총알 수명 타이머 핸들
	FTimerHandle LifespanTimer;
};
