#include "MyGameInstance.h"
#include "QuestSubsystem.h"
#include "QuestDataAsset.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UMyGameInstance::Init()
{
    Super::Init();

    // Get your QuestSubsystem
    if (UQuestSubsystem* QuestSubsystem = GetSubsystem<UQuestSubsystem>())
    {
        // Load a quest asset from the Content Browser
        // FStringAssetReference QuestAssetRef(TEXT("/Game/Quest/Quests/DA_Test_Quest_KillBandit.DA_Test_Quest_KillBandit"));

        //TSoftObjectPtr<UQuestDataAsset> QuestAssetRef(
        //    FSoftObjectPath(TEXT("/Game/Quest/Quests/DA_Test_KillBandit.DA_Test_KillBandit"))
        //);

        //UQuestDataAsset* QuestAsset = QuestAssetRef.LoadSynchronous();

        //if (QuestAsset)
        //{
        //    // Start the quest immediately (adds it to ActiveQuests)
        //    // QuestSubsystem->StartQuest(QuestAsset);
        //}
    }
}
