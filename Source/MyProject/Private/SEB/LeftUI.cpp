// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/LeftUI.h"

#include "Components/TextBlock.h"

void ULeftUI::NativeConstruct()
{
	Super::NativeConstruct();

	// �ð� ���� �ʱ�ȭ
	TimeElapsed = 0.0f;

	// Ÿ�̸� ����: 1�ʸ��� UpdateTimer �Լ��� ȣ��
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULeftUI::UpdateTImer, 0.01f, true);

}

void ULeftUI::UpdateTImer()
{
	// �ð� ����
	TimeElapsed += 0.01f;

	// Ÿ�̸� ������Ʈ ����
	if (PlayTime)
	{
		PlayTime->SetText(FText::FromString(GetFormattedTime()));
	}
}

FString ULeftUI::GetFormattedTime() const
{
	int32 TotalMilliseconds = static_cast<int32>(TimeElapsed * 1000.0f);
	int32 Minutes = TotalMilliseconds / 60000;
	TotalMilliseconds %= 60000;
	int32 Seconds = TotalMilliseconds / 1000;
	int32 Centiseconds = (TotalMilliseconds % 1000) / 10;  

	return FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Centiseconds);
}
