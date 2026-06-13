// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NBGameStateBase.generated.h"


UCLASS()
class NUMBERBASEBALL_API ANBGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ANBGameStateBase();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("xxxxx")));

	UPROPERTY(ReplicatedUsing = OnRep_ServerRemainingTime)
	int32 ServerTurnTimeLeft = 0;

	void ResetPlayersCurrentCount();

	UFUNCTION()
	void OnRep_ServerRemainingTime();

	void SetServerRemainingTime(int32 InRemainingTime);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
