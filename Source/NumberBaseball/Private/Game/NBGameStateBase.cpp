// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NBGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/NBPlayerState.h"
#include "Player/NBPlayerController.h"
#include "Game/NBGameModeBase.h"
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

void ANBGameStateBase::OnTurnTimerTick()
{
	TurnTimeLeft--;

	if (TurnTimeLeft <= 0)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		if (ANBGameModeBase* GM = GetWorld()->GetAuthGameMode<ANBGameModeBase>())
		{
			GM->OnTurnTimeout();
		}
	}
}

void ANBGameStateBase::OnGameStartTimer()
{
	if (ANBGameModeBase* GM = GetWorld()->GetAuthGameMode<ANBGameModeBase>())
	{
		GM->GameStart();
	}
}

void ANBGameStateBase::OnGameEndTimer()
{
	if (ANBGameModeBase* GM = GetWorld()->GetAuthGameMode<ANBGameModeBase>())
	{
		GM->ResetGame();
	}
}

void ANBGameStateBase::OnRep_TurnTimeLeft()
{
	if (ANBPlayerController* LocalPC = Cast<ANBPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		LocalPC->TimerText = FText::FromString(FString::Printf(TEXT("남은 시간 : %d"), TurnTimeLeft));
	}
}

void ANBGameStateBase::StartTurnTimer(int SetTimerValue)
{
	if (HasAuthority() && !GetWorldTimerManager().IsTimerActive(TurnTimerHandle))
	{
		TurnTimeLeft = SetTimerValue;
		GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &ANBGameStateBase::OnTurnTimerTick, 1.f, true);
	}
}

void ANBGameStateBase::ClearTurnTimer()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
}

void ANBGameStateBase::GameStartTimer(int SetTimerValue)
{
	if (HasAuthority() && !GetWorldTimerManager().IsTimerActive(GameStartTimerHandle))
	{
		GetWorldTimerManager().SetTimer(GameStartTimerHandle, this, &ANBGameStateBase::OnGameStartTimer, SetTimerValue, false);
	}
}

void ANBGameStateBase::GameEndTimer(int SetTimerValue)
{
	if (HasAuthority() && !GetWorldTimerManager().IsTimerActive(GameEndTimerHandle))
	{
		GetWorldTimerManager().SetTimer(GameEndTimerHandle, this, &ANBGameStateBase::OnGameEndTimer, SetTimerValue, false);
	}
}

void ANBGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TurnTimeLeft);
}
