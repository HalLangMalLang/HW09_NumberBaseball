// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NBPlayerController.h"
#include "UI/ChatWidget.h"
#include "EngineUtils.h"
#include "Game/NBGameModeBase.h"
#include "NumberBaseball/NumberBaseball.h"
#include "Kismet\GameplayStatics.h"
#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerController::ANBPlayerController()
{
	bReplicates = true;
}

void ANBPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatWidgetClass))
	{
		ChatWidget = CreateWidget<UChatWidget>(this, ChatWidgetClass);
		if (IsValid(ChatWidget))
		{
			ChatWidget->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass))
	{
		NotificationTextWidget = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		if (IsValid(NotificationTextWidget))
		{
			NotificationTextWidget->AddToViewport();
		}
	}
}

void ANBPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ANBPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController())
	{
		if (ANBPlayerState* NBPS = GetPlayerState<ANBPlayerState>())
		{
			ServerRPCPrintChatMesaageString(ChatMessageString);
		}
	}
}

void ANBPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	ChatFunctionLibrary::PrintString(this, InChatMessageString, 10.f);
}

void ANBPlayerController::ServerRPCPrintChatMesaageString_Implementation(const FString& InchatMessageString)
{
	if (AGameModeBase* GM = UGameplayStatics::GetGameMode(this))
	{
		if (ANBGameModeBase* NBGM = Cast<ANBGameModeBase>(GM))
		{
			NBGM->ProcessPlayerInput(this, InchatMessageString);
		}
	}
}

void ANBPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}