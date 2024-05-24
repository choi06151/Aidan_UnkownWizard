// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioAnalysis.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAudioAnalysis : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAudioAnalysis();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // ����� ĸó�� �ʱ�ȭ�ϴ� �Լ�
    void InitializeAudioCapture();

    // ����� ĸó�� �����ϴ� �Լ�
    void StartAudioCapture();

    // ����� ĸó�� �����ϴ� �Լ�
    void StopAudioCapture();

private:
    // ����� ĸó ������Ʈ
    UPROPERTY()
    class UAudioCaptureComponent* AudioCaptureComponent;

    // ����� �м� ������Ʈ��
    UPROPERTY()
    class UOnsetNRT* OnsetNRT;

    UPROPERTY()
    class ULoudnessNRT* LoudnessNRT;

    UPROPERTY()
    class UConstantQNRT* ConstantQNRT;

    // ����� �����͸� ó���ϴ� �Լ�
    void ProcessAudioData();

public:
    // ���� ������ �����ϴ� ����
    float CurrentVolume;

    // ���ļ� �뿪 �����͸� �����ϴ� �迭
    TArray<float> FrequencyBands;
};


