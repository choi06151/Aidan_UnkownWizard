// Fill out your copyright notice in the Description page of Project Settings.


#include "KJH/AudioAnalysis.h"
#include "AudioCaptureComponent.h"
#include "Sound/SoundWaveProcedural.h"
#include "AudioSynesthesiaNRT.h"
#include "Kismet/GameplayStatics.h"
#include "Audio.h"
#include "AudioAnalytics.h"
#include "Components/SynthComponent.h"
#include "AudioCapture.h"
#include "OnsetNRT.h"
#include "LoudnessNRT.h"
#include "ConstantQNRT.h"

// Sets default values for this component's properties
UAudioAnalysis::UAudioAnalysis()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ����� ĸó ������Ʈ ����
	AudioCaptureComponent = CreateDefaultSubobject<UAudioCaptureComponent>(TEXT("AudioCaptureComponent"));
}


// Called when the game starts
void UAudioAnalysis::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UAudioAnalysis::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// �� �����Ӹ��� ����� �����͸� ó��
	//ProcessAudioData();
}

//void UAudioAnalysis::InitializeAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // ����� �м� ������Ʈ�� �ʱ�ȭ
//        OnsetNRT = NewObject<UOnsetNRT>(this);
//        OnsetNRT->Settings = NewObject<UOnsetNRTSettings>(OnsetNRT);
//        OnsetNRT->Settings->AnalysisDuration = 10.0f;
//        OnsetNRT->Settings->AnalysisPeriod = 0.01f;
//        OnsetNRT->Settings->Sensitivity = 0.5f;
//
//        LoudnessNRT = NewObject<ULoudnessNRT>(this);
//        LoudnessNRT->Settings = NewObject<ULoudnessNRTSettings>(LoudnessNRT);
//        LoudnessNRT->Settings->DurationInSeconds = 10.0f;
//        LoudnessNRT->Settings->AnalysisPeriod = 0.01f;
//
//        ConstantQNRT = NewObject<UConstantQNRT>(this);
//        ConstantQNRT->Settings = NewObject<UConstantQNRTSettings>(ConstantQNRT);
//        ConstantQNRT->Settings->DurationInSeconds = 10.0f;
//        ConstantQNRT->Settings->AnalysisPeriod = 0.01f;
//    }
//}
//
//void UAudioAnalysis::StartAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // ����� ĸó ����
//        AudioCaptureComponent->OnBeginGenerate();
//    }
//}
//
//void UAudioAnalysis::StopAudioCapture()
//{
//    if (AudioCaptureComponent)
//    {
//        // ����� ĸó ����
//        AudioCaptureComponent->OnEndGenerate();
//    }
//}
//
//void UAudioAnalysis::ProcessAudioData()
//{
//    if (AudioCaptureComponent && AudioCaptureComponent->IsCapturing())
//    {
//        // ĸó�� ����� �����͸� ������
//        TArray<uint8> AudioData;
//        AudioCaptureComponent->GetCaptureAudioData(AudioData);
//        if (AudioData.Num() > 0)
//        {
//            // ����� �����͸� �м�
//            OnsetNRT->AnalyzeAudio(AudioData, 44100, 1024);
//            LoudnessNRT->AnalyzeAudio(AudioData, 44100, 1024);
//            ConstantQNRT->AnalyzeAudio(AudioData, 44100, 1024);
//
//            // �¼�(���� ����) ������ ó��
//            TArray<float> OnsetTimes;
//            OnsetNRT->GetNormalizedChannelOnsets(0, OnsetTimes);
//
//            // ���� ������ ó��
//            TArray<float> LoudnessValues;
//            LoudnessNRT->GetNormalizedChannelLoudness(0, LoudnessValues);
//            CurrentVolume = LoudnessValues.Last();
//
//            // ���ļ� ������ ó��
//            TArray<float> Magnitudes;
//            ConstantQNRT->GetNormalizedChannelConstantQ(0, 0.0f, Magnitudes);
//            FrequencyBands = Magnitudes;
//        }
//    }
//}

