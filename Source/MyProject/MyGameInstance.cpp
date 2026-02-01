#include "MyGameInstance.h"
#include "QuestSubsystem.h"
#include "QuestData.h"

void UMyGameInstance::Init()
{
    Super::Init();

    // Here is where you access your QuestSubsystem
    if (UQuestSubsystem* QuestSubsystem = GetSubsystem<UQuestSubsystem>())
    {
        // Example: manually assign quest assets
        QuestSubsystem->AllQuests = {
            LoadObject<UQuestData>(nullptr, TEXT("/Game/Quest/Quests/DA_Test_Quest_KillBandit")),
        };
    }
}
