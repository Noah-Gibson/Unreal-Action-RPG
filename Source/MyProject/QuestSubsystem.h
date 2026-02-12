// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "QuestObjective.h"
#include "QuestDataAsset.h"
#include "QuestRuntimeData.h"
#include "QuestSubsystem.generated.h"

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStarted, FGameplayTag, QuestID, const FQuestRuntimeData&, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestUpdated, FGameplayTag, QuestID, const FQuestRuntimeData&, QuestData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestCompleted, FGameplayTag, QuestID, const FQuestRuntimeData&, QuestData);

class UQuestDataAsset;

UCLASS()
class MYPROJECT_API UQuestSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Delegates
    UPROPERTY(BlueprintAssignable)
    FOnQuestStarted OnQuestStarted;

    UPROPERTY(BlueprintAssignable)
    FOnQuestUpdated OnQuestUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnQuestCompleted OnQuestCompleted;

    // Core API
    UFUNCTION(BlueprintCallable)
    void NotifyEvent(FGameplayTag EventTag);

    // Queries (UI-safe)
    UFUNCTION(BlueprintPure)
    bool GetActiveQuest(
        FGameplayTag& OutQuestID,
        FQuestRuntimeData& OutData
    ) const;

    UFUNCTION(BlueprintPure)
    bool GetCurrentObjective(
        FQuestObjective& OutObjective
    ) const;

    UFUNCTION(BlueprintCallable)
    bool IsQuestActive(FGameplayTag QuestID) const;

    UFUNCTION(BlueprintCallable)
    bool IsQuestNotStarted(FGameplayTag QuestID) const;

    UFUNCTION(BlueprintCallable)
    bool IsQuestCompleted(FGameplayTag QuestID) const;

protected:
    // All quests that the subsystem knows about
    UPROPERTY()
    TArray<UQuestDataAsset*> AllQuests;

    UPROPERTY()
    TMap<FGameplayTag, FQuestRuntimeData> ActiveQuests;

    UPROPERTY()
    TSet<FGameplayTag> CompletedQuests;

private:
    UFUNCTION(BlueprintCallable)
    bool StartQuest(UQuestDataAsset* QuestAsset);
};
