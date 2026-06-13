// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NBPlayerState.h"
#include "Net/UnrealNetwork.h"

ANBPlayerState::ANBPlayerState() : PlayerNameString(TEXT("None")), CurrentGuessCount(0), MaxGuessCount(0)
{
	bReplicates = true;
}

void ANBPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ANBPlayerState::GetPlayerNameString()
{
	return PlayerNameString;
}

FString ANBPlayerState::GetAttemptsString()
{
	return TEXT("(남은 기회 : ") + FString::FromInt(CurrentGuessCount) + TEXT(" / ") + FString::FromInt(MaxGuessCount) + TEXT(")");
}

void ANBPlayerState::InitializeState(int32 InCurrentGuessCount, int32 InMaxGuessCount)
{
	CurrentGuessCount = InCurrentGuessCount;
	MaxGuessCount = InMaxGuessCount;
}
