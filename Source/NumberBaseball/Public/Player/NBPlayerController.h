// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NBPlayerController.generated.h"

class UChatWidget;

UCLASS()
class NUMBERBASEBALL_API ANBPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UChatWidget> ChatWidgetClass;

	UPROPERTY()
	TObjectPtr<UChatWidget> ChatWidget;

	FString ChatMessageString;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> NotificationTextWidget;

public:
	ANBPlayerController();
	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMesaageString(const FString& InchatMessageString);

	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

	UPROPERTY(BlueprintReadOnly)
	FText TimerText;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
