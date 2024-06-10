// Fill out your copyright notice in the Description page of Project Settings.


#include "SEB/SelectStageUI.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"
#include "SEB/GameStartUI.h"
#include "SEB/MusicInfoDT.h"
#include "SEB/StageUI.h"
#include "Components/WidgetComponent.h"
#include "JWK/Boss.h"
#include "SEB/GameOverUI.h"

void USelectStageUI::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	SpawnWidget = Cast<ASpawnWidget>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnWidget::StaticClass()));
	
	// DataTable 초기화
	static const FSoftObjectPath DataTablePath(TEXT("/Game/SEB/Blueprints/DT_MusicInfo.DT_MusicInfo"));
	UDataTable* MusicInfoDataObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *DataTablePath.ToString()));

	if(MusicInfoDataObject)
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable Succeed!!!"));
		MusicDataTable = MusicInfoDataObject;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DataTable Load Failed"));
		return;
	}
	
	if (!StageUIClass || !MainScroll || !SpawnWidget) return;

	MainScroll->ClearChildren();

	TArray<FName> RowNames = MusicDataTable->GetRowNames();
	int32 RowCount = RowNames.Num();

	for(int i=0; i<RowCount; i++)
	{
		UStageUI* StageUI = CreateWidget<UStageUI>(this, StageUIClass);
		FMusicInfoDT* Row = MusicDataTable->FindRow<FMusicInfoDT>(RowNames[i], TEXT(""));
		
		if (StageUI && Row)
		{
			StageUI->SetArtistText(FText::FromString(Row->ArtistName));
			StageUI->SetMusicText(FText::FromString(Row->MusicName));
			StageUI->SetDifficultyImage(Row->Difficulty);
			StageUI->ParentSelectStageUI = this;  
			MainScroll->AddChild(StageUI);
		}
	}
	//배열의 첫번째 값들로 정보 초기화
	FMusicInfoDT* FirstRow = MusicDataTable->FindRow<FMusicInfoDT>(RowNames[0], TEXT(""));

	if(nullptr == SpawnWidget->SpecificRow)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnWidget->SpecificRow is Null"));
		SpawnWidget->SpecificRow = FirstRow;
		
	}
	ChangeStageName(FText::FromString(SpawnWidget->SpecificRow->ArtistName), FText::FromString(SpawnWidget->SpecificRow->MusicName));
	
	if(SpawnWidget)
		SpawnWidget->SpecificRow = FindRowByColumnValue("ArtistName", FirstRow->ArtistName, "MusicName", FirstRow->MusicName);
}

void USelectStageUI::NativeConstruct()
{
	Super::NativeConstruct();
	UpArrowBtn->OnPressed.AddDynamic(this, &USelectStageUI::OnUPArrowClicked);
	DownArrowBtn->OnPressed.AddDynamic(this, &USelectStageUI::OnDownArrowClicked);
	BackBtn->OnPressed.AddDynamic(this, &USelectStageUI::OnBackClicked);
	PlayBtn->OnPressed.AddDynamic(this, &USelectStageUI::OnPlayClicked);
	
}


void USelectStageUI::OnUPArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset - 300.0f);
	}
}

void USelectStageUI::OnDownArrowClicked()
{
	if (MainScroll)
	{
		float CurrentOffset = MainScroll->GetScrollOffset();
		MainScroll->SetScrollOffset(CurrentOffset + 300.0f);
	}
}

void USelectStageUI::OnBackClicked()
{
	// SpawnWidget의 WidgetClass를 변경
	UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(GameStartUIClass);
}

void USelectStageUI::OnPlayClicked()
{
	// UI 숨김
	SetVisibility(ESlateVisibility::Hidden);
	// 게임 시작 
	ABoss* Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	if (Boss)
	{
		Boss->bIsGameStart = true;
		Boss->MusicStart();
		//이건 보스에서 공격 시작할때 하도록 해야함.
		//커튼이 열리고 -> 보스가 걸어나오고 -> 커튼이 닫히고 -> 커튼 흔들리는 애니메이션으로 변경
		//SpawnWidget->CurtainOpenAnim();
		//SpawnWidget->CurtainCloseAnim();
		//SpawnWidget->CurtainShakeAnim();
		//SpawnWidget->MusicPlay();
	}
	//우선은 GameOverUI로 넘어가도록 함. 
	/*UWidgetComponent* WidgetComponent = SpawnWidget->FindComponentByClass<UWidgetComponent>();
	WidgetComponent->SetWidgetClass(GameOverUIClass);*/
}


//버튼 클릭 시 좌측 정보 업데이트
void USelectStageUI::ChangeStageName(const FText& NewText, const FText& NewInfoText)
{
	int32 ScrollCount = MainScroll->GetChildrenCount();
	FColor ClickColor = FColor(184, 121, 16, 255);
	
	SetSelectUIColor(ScrollCount, ClickColor);
	
	//NewText라는 ArtistName열을 가진 행 탐색
	FMusicInfoDT* SpecificRow = FindRowByColumnValue("ArtistName", NewText.ToString(), "MusicName", NewInfoText.ToString());
	if (!SpecificRow)
	{
		UE_LOG(LogTemp, Error, TEXT("SpecificRow not found"));
		return;
	}
	if(SpawnWidget)
		SpawnWidget->SpecificRow = SpecificRow;

	//SpawnWidget->MusicPlay();
	
	ArtistName->SetText(NewText);
	MusicName->SetText(NewInfoText);
	
	//BestScore 변경
	BestScore->SetText(FText::AsNumber(SpecificRow->BestScore));

	// 썸네일 변경
	ThumbnailPath = new FText(FText::FromString(SpecificRow->Thumbnail));
	UTexture2D* ThumbnailTexture = LoadObject<UTexture2D>(nullptr, *ThumbnailPath->ToString());
	if (ThumbnailTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(ThumbnailTexture);
		Thumbnail->SetBrush(NewBrush);
	}

	//난이도 별 이미지 변경
	int32 num = SpecificRow->Difficulty;
	if(DifficultyGridPanel)
	{
		for(int i=0; i<DifficultyGridPanel->GetChildrenCount(); i++)
		{
			UWidget* ChildWidget = DifficultyGridPanel->GetChildAt(i);
			UImage* ImageWidget = Cast<UImage>(ChildWidget);
			
			if(i>=num) {
				if(ImageWidget)
				{
					FText* StarPath = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star.Star'")));
					SetStarFill(ImageWidget, StarPath);
				}
			}
			else
			{
				if(ImageWidget)
				{
					FText* StarFillPath = new FText(FText::FromString(TEXT("/Script/Engine.Texture2D'/Game/SEB/UI/Resources/Star_fill.Star_fill'")));
					SetStarFill(ImageWidget, StarFillPath);
				}
			}

			
		}
	}

	
	
}


void USelectStageUI::SetStarFill(UImage* ImageWidget, FText* Path)
{
	const TCHAR* MyCharArr = *Path->ToString();
	UTexture2D* NewTexture = LoadObject<UTexture2D>(nullptr, MyCharArr);
	if (NewTexture)
	{
		FSlateBrush NewBrush;
		NewBrush.SetResourceObject(NewTexture);
		ImageWidget->SetBrush(NewBrush);
	}
}

FMusicInfoDT* USelectStageUI::FindRowByColumnValue(const FString& ColumnName1, const FString& ColumnValue1, const FString& ColumnName2, const FString& ColumnValue2)
{
	if (!MusicDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("MusicDataTable읎다"));
		return nullptr;
	}
	static const FString ContextString(TEXT("FindRowByColumnValue"));
	TArray<FName> RowNames = MusicDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FMusicInfoDT* Row = MusicDataTable->FindRow<FMusicInfoDT>(RowName, ContextString);
		if (Row)
		{
			// ArtistName과 MusicName 모두 일치할 경우
			if ((ColumnName1 == "ArtistName" && Row->ArtistName == ColumnValue1) &&
				(ColumnName2 == "MusicName" && Row->MusicName == ColumnValue2))
			{
				return Row;
			}
		}
	}

	return nullptr;
}

void USelectStageUI::SetSelectUIColor(int32 ScrollCount, const FColor& ClickColor)
{
	for (int32 i = 0; i < ScrollCount; ++i)
	{
		UWidget* ChildWidget = MainScroll->GetChildAt(i);
		if (UStageUI* StageUI = Cast<UStageUI>(ChildWidget))
		{
			if (StageUI->SelectStageBtn)
			{
				FButtonStyle ButtonStyle = StageUI->SelectStageBtn->WidgetStyle;

				FSlateBrush NormalBrush = ButtonStyle.Normal;
				NormalBrush.TintColor = FSlateColor(ClickColor);
				ButtonStyle.SetNormal(NormalBrush);


				StageUI->SelectStageBtn->SetStyle(ButtonStyle);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Child widget is not of type UStageUI"));
		}
	}
}
