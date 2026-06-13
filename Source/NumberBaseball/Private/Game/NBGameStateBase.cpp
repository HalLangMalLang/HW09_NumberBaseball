// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"
#include "Net/UnrealNetwork.h" 

ANBGameStateBase::ANBGameStateBase()
{
	bReplicates = true;
}

void ANBGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (!HasAuthority())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ANBPlayerController* NBPC = Cast<ANBPlayerController>(PC))
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				NBPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

void ANBGameStateBase::ResetPlayersCurrentCount()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (ANBPlayerState* NBPS = Cast<ANBPlayerState>(PS))
		{
			NBPS->CurrentGuessCount = 0;
		}
	}
}

void ANBGameStateBase::OnRep_ServerRemainingTime()
{
	if (ANBPlayerController* LocalPC = Cast<ANBPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		LocalPC->TimerText = FText::FromString(FString::Printf(TEXT("남은 시간 : %d"), ServerTurnTimeLeft));
	}
}

void ANBGameStateBase::SetServerRemainingTime(int32 InRemainingTime)
{
	ServerTurnTimeLeft = InRemainingTime;

	OnRep_ServerRemainingTime();
}

void ANBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ServerTurnTimeLeft);
}
