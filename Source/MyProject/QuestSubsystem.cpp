#include "QuestSubsystem.h"
#include "QuestDataAsset.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"

bool UQuestSubsystem::StartQuest(UQuestDataAsset* QuestAsset, FGameplayTag QuestID)
{
    if (!QuestAsset || !QuestID.IsValid())
    {
        return false;
    }

    if (ActiveQuests.Contains(QuestID) || CompletedQuests.Contains(QuestID))
    {
        return false;
    }

    FQuestRuntimeData RuntimeData;
    RuntimeData.QuestAsset = QuestAsset;
    RuntimeData.CurrentObjectiveIndex = 0;

    ActiveQuests.Add(QuestID, RuntimeData);

    UE_LOG(LogTemp, Log, TEXT("Quest Started: %s"), *QuestID.ToString());

    OnQuestStarted.Broadcast(QuestID, RuntimeData);

    return true;
}

bool UQuestSubsystem::AdvanceObjective(FGameplayTag QuestID)
{
    FQuestRuntimeData* QuestData = ActiveQuests.Find(QuestID);
    if (!QuestData || !QuestData->QuestAsset)
    {
        return false;
    }

    QuestData->CurrentObjectiveIndex++;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Quest Advanced: %s -> Objective %d"),
        *QuestID.ToString(),
        QuestData->CurrentObjectiveIndex
    );

    OnQuestUpdated.Broadcast(QuestID, *QuestData);

    const int32 TotalObjectives = QuestData->QuestAsset->Objectives.Num();
    if (QuestData->CurrentObjectiveIndex >= TotalObjectives)
    {
        ActiveQuests.Remove(QuestID);
        CompletedQuests.Add(QuestID);

        UE_LOG(LogTemp, Log, TEXT("Quest Completed: %s"), *QuestID.ToString());

        OnQuestCompleted.Broadcast(QuestID);
    }

    return true;
}

void UQuestSubsystem::NotifyEvent(FGameplayTag EventTag, AActor* InstigatorActor)
{
    if (!EventTag.IsValid())
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Quest Event Fired: %s"), *EventTag.ToString());

    // Iterate over all active quests
    for (auto& QuestPair : ActiveQuests)
    {
        FGameplayTag QuestID = QuestPair.Key;
        FQuestRuntimeData& QuestData = QuestPair.Value;

        if (!QuestData.QuestAsset || QuestData.CurrentObjectiveIndex >= QuestData.QuestAsset->Objectives.Num())
        {
            continue; // Skip invalid or completed objectives
        }

        FQuestObjective& CurrentObjective = QuestData.QuestAsset->Objectives[QuestData.CurrentObjectiveIndex];

        // Check if the event matches the current objective's required tag
        if (CurrentObjective.RequiredEventTag == EventTag)
        {
            UE_LOG(LogTemp, Log, TEXT("Advancing Quest %s: Objective %d completed by event %s"),
                *QuestID.ToString(), QuestData.CurrentObjectiveIndex, *EventTag.ToString());

            AdvanceObjective(QuestID);
            break; // Only advance one quest per event (optional)
        }
    }
}

bool UQuestSubsystem::IsQuestActive(FGameplayTag QuestID) const
{
    return ActiveQuests.Contains(QuestID);
}
