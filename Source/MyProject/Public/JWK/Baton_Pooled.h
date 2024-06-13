// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Baton_Pooled.generated.h"

// ���ֺ��� ��Ȱ��ȭ�� �� ȣ��Ǵ� ��������Ʈ ����
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

	// Baton�� ��Ȱ��ȭ�� �� ȣ��Ǵ� Event
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Events")
	FOnPooledBatonDespawn OnPooledBatonDespawn;

	// Baton�� Ȱ��ȭ �Ǵ� ��Ȱ��ȭ�ϴ� �Լ�
	void SetActive(bool IsActive);

	// Baton�� ������ set�ϴ� �Լ�
	void SetLifeSpan(float LifeTime);

	// Baton Speed�� set�ϴ� �Լ�
	void SetBatonSpeed(float Speed);

	// pool������ index �����ϴ� �Լ�
	void SetPoolIndex(int index);

	// �Ѿ��� Ȱ�� or ��Ȱ�� bool �� ��ȯ�ϴ� �Լ�
	bool IsActive();

	// pool������ index �� ��ȯ�ϴ� �Լ�
	int GetPoolIndex();

	// �Ѿ��� ��Ȱ��ȭ�ϴ� �Լ�
	void Deactivate();

protected:
	// �Ѿ��� Ȱ��ȭ ����
	bool Active;

	// �Ѿ��� ����
	float LifeSpan = 0.0f;

	// pool������ index
	int PoolIndex;

	// �Ѿ� ���� Ÿ�̸� �ڵ�
	FTimerHandle LifespanTimer;
};
