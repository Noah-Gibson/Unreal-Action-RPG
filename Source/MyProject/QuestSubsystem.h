// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

#include "QuestSubsystem.generated.h"

class UQuestDataAsset;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FQuestRuntimeData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UQuestDataAsset> QuestAsset = nullptr;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentObjectiveIndex = 0;
};

// Quest lifecycle delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnQuestStarted,
    FGameplayTag, QuestID,
    const FQuestRuntimeData&, QuestData
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnQuestUpdated,
    FGameplayTag, QuestID,
    const FQuestRuntimeData&, QuestData
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnQuestCompleted,
    FGameplayTag, QuestID
);



UCLASS()
class MYPROJECT_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool StartQuest(UQuestDataAsset* QuestAsset, FGameplayTag QuestID);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    bool AdvanceObjective(FGameplayTag QuestID);

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void NotifyEvent(FGameplayTag EventTag, AActor* InstigatorActor);

    UFUNCTION(BlueprintPure, Category = "Quest")
    bool IsQuestActive(FGameplayTag QuestID) const;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestStarted OnQuestStarted;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestUpdated OnQuestUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnQuestCompleted OnQuestCompleted;



private:
    // Active quests currently in progress
    UPROPERTY()
    TMap<FGameplayTag, FQuestRuntimeData> ActiveQuests;

    // Completed quests
    UPROPERTY()
    TSet<FGameplayTag> CompletedQuests;
};

