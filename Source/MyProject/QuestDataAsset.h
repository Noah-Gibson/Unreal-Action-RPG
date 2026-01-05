// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

#include "QuestDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    // Event required to complete this objective
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag RequiredEventTag;

    // UI text
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText ObjectiveText;
};

UCLASS(BlueprintType)
class MYPROJECT_API UQuestDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    // Unique identifier for this quest
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTag QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText QuestName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText QuestDescription;

    // Ordered list of objectives
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FQuestObjective> Objectives;
};