// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestObjective.h"
#include "QuestDataAsset.generated.h"

UCLASS(BlueprintType)
class MYPROJECT_API UQuestDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText QuestDescription;

    UPROPERTY(EditDefaultsOnly)
	FGameplayTag StartEventTag;

    UPROPERTY(EditDefaultsOnly)
    TArray<FQuestObjective> Objectives;
};
