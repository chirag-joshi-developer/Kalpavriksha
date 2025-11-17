#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Players_data.h"

#define NUMBER_OF_TEAMS 10
#define MINIMUM_PLAYER_ID 1
#define MAXIMUM_PLAYER_ID 1500
#define MINIMUM_PLAYER_PER_TEAM 11
#define MAXIMUM_PLAYER_PER_TEAM 50
#define MAXIMUM_PLAYER_NAME_LENGTH 51
#define MAXIMUM_TEAM_NAME_LENGTH 51
#define MAXIMUM_ROLE_NAME_LENGTH 21

typedef enum {
    UNKNOWN = 0,
    BATSMAN = 1,
    ALLROUNDER = 2,
    BOWLER = 3
} PlayerType;

typedef struct Player {
    int playerId;
    char playerName[MAXIMUM_PLAYER_NAME_LENGTH];
    char playerTeamName[MAXIMUM_TEAM_NAME_LENGTH];

    PlayerType playerType; 
    
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int totalWickets;
    float economyRate;
    float performanceIndex;

    struct Player* next;
} Player ;

typedef struct Team {
    int teamId;
    char teamName[MAXIMUM_TEAM_NAME_LENGTH];
    int playerCount;
    float strikeRateSum;
    int strikeRateCount;
    float teamAverageBattingStrikeRate;

    struct Player* playerHead;
} Team ;

typedef struct HeapNode {
    Player* playerPointer;
    int teamIndex;
} HeapNode;

Team teamList[NUMBER_OF_TEAMS];
int* allPlayersId;
int currentPlayerCount = 0;

static void initializeAllPlayersIdTracker() {
    allPlayersId = malloc((MAXIMUM_PLAYER_ID + 1) * sizeof *allPlayersId);
    if (!allPlayersId) {
        fprintf(stderr, "Allocation failure for allPlayersId\n");
        exit(EXIT_FAILURE);
    }
    currentPlayerCount = 0;
}

static float computePerformanceIndexFromFields(PlayerType playerType, float battingAverage, float strikeRate, int wickets, float economyRate) {
    if (playerType == BATSMAN) {
        return (battingAverage * strikeRate) / 100.0f;
    } else if (playerType == ALLROUNDER) {
        return (wickets * 2.0f) + (100.0f - economyRate);
    } else {
        return ((battingAverage * strikeRate) / 100.0f) + (wickets * 2.0f);
    }
}

static int roleNameToType(const char* roleName) {
    if (roleName == NULL) return UNKNOWN;
    if (strcasecmp(roleName, "Batsman") == 0) return BATSMAN;
    if (strcasecmp(roleName, "All-rounder") == 0 ) return ALLROUNDER;
    if (strcasecmp(roleName, "Bowler") == 0) return BOWLER;
    return 0;
}

static int binarySearchForPlayerId(int *allPlayersId,int target ,int currentPlayerCount){
    int low = 0;
    int high = currentPlayerCount-1;

    while(low <= high){
        int mid = low + (high - low)/2;
        if(allPlayersId[mid] == target){
            return mid;
        }
        if(allPlayersId[mid]<target){
            low = mid+1;
        }
        else{
            high = mid-1;
        }
    }
    return -1;
}

static const char* roleTypeToName(PlayerType roleType) {
    switch (roleType) {
        case BATSMAN: 
            return "Batsman";
        case ALLROUNDER: 
            return "All-rounder";
        case BOWLER: 
            return "Bowler";
        default: 
            return "Unknown";
    }
}

static void initializeTeamsListFromHeader() {
    for (int index = 0; index < NUMBER_OF_TEAMS && index < teamCount; ++index) {
        teamList[index].teamId = index + 1;
        if (teams == NULL || teams[index] == NULL) {
            fprintf(stderr, "teams data missing ");
            exit(EXIT_FAILURE);
        }
        
        strncpy(teamList[index].teamName, teams[index], MAXIMUM_TEAM_NAME_LENGTH - 1);
        
        if (teamList[index].teamName[0] == '\0') {
            fprintf(stderr, "team name copy produced empty ");
            exit(EXIT_FAILURE);
        }
        teamList[index].teamName[MAXIMUM_TEAM_NAME_LENGTH - 1] = '\0';

        teamList[index].playerCount = 0;
        teamList[index].strikeRateSum = 0.0f;
        teamList[index].strikeRateCount = 0;
        teamList[index].teamAverageBattingStrikeRate = 0.0f;
        teamList[index].playerHead = NULL;
    }
}

static void insertPlayerAndSortByPerformanceIndex(Team* destinationTeam, Player* newPlayer) {
    
    if (destinationTeam == NULL || newPlayer == NULL) return;
    Player** indirect = &destinationTeam->playerHead;
    while (*indirect != NULL && (*indirect)->performanceIndex > newPlayer->performanceIndex) {
        indirect = &(*indirect)->next;
    }
    newPlayer->next = *indirect;
    *indirect = newPlayer;
    destinationTeam->playerCount += 1;
    if (newPlayer->playerType == BATSMAN || newPlayer->playerType == ALLROUNDER) {
        destinationTeam->strikeRateSum += newPlayer->strikeRate;
        destinationTeam->strikeRateCount += 1;
        destinationTeam->teamAverageBattingStrikeRate = destinationTeam->strikeRateSum / (float)destinationTeam->strikeRateCount;
    }
}

Team* findTeamById(int teamId) {
    if (teamId < 1 || teamId > NUMBER_OF_TEAMS) {
        return NULL;
    }
    return &teamList[teamId - 1];
}

static int addPlayerToTeam(int playerId, const char* playerNameValue, const char* playerTeamNameValue, PlayerType playerTypeValue, int totalRunsValue, float battingAverageValue, float strikeRateValue, int totalWicketsValue, float economyRateValue) {
    if (playerId < MINIMUM_PLAYER_ID || playerId > MAXIMUM_PLAYER_ID) return -1;
    if (playerTypeValue == UNKNOWN) return -2;
    Team* destinationTeam = NULL;
    for (int index = 0; index < NUMBER_OF_TEAMS; ++index) {
        if (strcmp(teamList[index].teamName, playerTeamNameValue) == 0) {
            destinationTeam = &teamList[index];
            break;
        }
    }
    if(binarySearchForPlayerId( allPlayersId, playerId, currentPlayerCount)>-1){
        printf("player of this id already exists in database ");
        return -1;
    }

    if (destinationTeam == NULL) return -3;

    Player* createdPlayer = (Player*) malloc(sizeof(Player));
    if (createdPlayer == NULL) return -4;

    createdPlayer->playerId = playerId;

    strncpy(createdPlayer->playerName, playerNameValue, MAXIMUM_PLAYER_NAME_LENGTH - 1);
    createdPlayer->playerName[MAXIMUM_PLAYER_NAME_LENGTH - 1] = '\0';

    if (createdPlayer->playerName[0] == '\0') {
        free(createdPlayer);
        return -5;
    }

    strncpy(createdPlayer->playerTeamName, playerTeamNameValue, MAXIMUM_TEAM_NAME_LENGTH - 1);
    createdPlayer->playerTeamName[MAXIMUM_TEAM_NAME_LENGTH - 1] = '\0';

    if (createdPlayer->playerTeamName[0] == '\0') {
        free(createdPlayer);
        return -6;
    }  

    createdPlayer->playerType = playerTypeValue;
    createdPlayer->totalRuns = totalRunsValue;
    createdPlayer->battingAverage = battingAverageValue;
    createdPlayer->strikeRate = strikeRateValue;
    createdPlayer->totalWickets = totalWicketsValue;
    createdPlayer->economyRate = economyRateValue;
    createdPlayer->performanceIndex = computePerformanceIndexFromFields(playerTypeValue, battingAverageValue, strikeRateValue, totalWicketsValue, economyRateValue);
    createdPlayer->next = NULL;

    insertPlayerAndSortByPerformanceIndex(destinationTeam, createdPlayer);
    int index = currentPlayerCount;
    while(index >= 0 && allPlayersId[index] > playerId){
        allPlayersId[index+1] = allPlayersId[index];
        index--;
    }
    allPlayersId[index+1] = playerId;
    
    currentPlayerCount++;
    return 0;
}

static void initializePlayersFromHeader() {
    for (int index = 0; index < playerCount; ++index) {
        PlayerData headerPlayer = players[index];

        if (headerPlayer.name == NULL || headerPlayer.team == NULL || headerPlayer.role == NULL) {
            fprintf(stderr, "Warning: skipping malformed player entry at index %d\n", index);
            continue;
        }

        PlayerType roleType = roleNameToType(headerPlayer.role);
        if (roleType == 0) roleType = 1;
        int result = addPlayerToTeam(headerPlayer.id, headerPlayer.name, headerPlayer.team, roleType,
                                     headerPlayer.totalRuns, headerPlayer.battingAverage,
                                     headerPlayer.strikeRate, headerPlayer.wickets, headerPlayer.economyRate);
        if (result != 0) {
            fprintf(stderr, "Warning: failed to add header player id %d (error %d). Skipping.\n", headerPlayer.id, result);
        }
    }
}

static void displayPlayersOfSpecificTeamById() {
    printf("Enter Team ID: ");
    int userTeamId = 0;
    if (scanf("%d", &userTeamId) != 1) {
        printf("Invalid numeric input for Team ID.\n");
        while (getchar() != '\n');
        return;
    }

    Team* selectedTeam = findTeamById(userTeamId);
    if (selectedTeam == NULL) {
        printf("Team with ID %d not found.\n", userTeamId);
        return;
    }

    printf("\nPlayers of Team %s:\n", selectedTeam->teamName);
    printf("=========================================================================================================\n");
    printf("%-6s %-25s %-14s %-6s %-7s %-7s %-6s %-6s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=========================================================================================================\n");

    Player* iterator = selectedTeam->playerHead;
    if (iterator == NULL) {
        printf("No players in this team yet.\n");
    } else {
        while (iterator != NULL) {
            printf("%-6d %-25s %-14s %-6d %-7.2f %-7.2f %-6d %-6.2f %-12.2f\n",
                   iterator->playerId,
                   iterator->playerName,
                   roleTypeToName(iterator->playerType),
                   iterator->totalRuns,
                   iterator->battingAverage,
                   iterator->strikeRate,
                   iterator->totalWickets,
                   iterator->economyRate,
                   iterator->performanceIndex);
            iterator = iterator->next;
        }
    }

    printf("=========================================================================================================\n");
    printf("Total Players: %d\n", selectedTeam->playerCount);
    printf("Average Batting Strike Rate: %.2f\n", selectedTeam->teamAverageBattingStrikeRate);
}

static int teamCompareForQsort(const void* a, const void* b) {
    const Team* teamA = (const Team*)a;
    const Team* teamB = (const Team*)b;

    if (teamA->teamAverageBattingStrikeRate < teamB->teamAverageBattingStrikeRate) return 1;
    if (teamA->teamAverageBattingStrikeRate > teamB->teamAverageBattingStrikeRate) return -1;
    return 0;
}

static void displayTeamsByAverageStrikeRate() {
    Team tempArray[NUMBER_OF_TEAMS];
    for (int index = 0; index < NUMBER_OF_TEAMS; ++index) {
        tempArray[index] = teamList[index];
    }

    qsort(tempArray, NUMBER_OF_TEAMS, sizeof(Team), teamCompareForQsort);

    printf("\nTeams Sorted by Average Batting Strike Rate (descending):\n");
    printf("===========================================================\n");
    printf("%-4s %-25s %-12s %-6s\n", "ID", "Team Name", "Avg Bat SR", "Players");
    printf("===========================================================\n");

    for (int index = 0; index < NUMBER_OF_TEAMS; ++index) {
        printf("%-4d %-25s %-12.2f %-6d\n",
               tempArray[index].teamId,
               tempArray[index].teamName,
               tempArray[index].teamAverageBattingStrikeRate,
               tempArray[index].playerCount);
    }
    printf("===========================================================\n");
}

static void displayTopKPlayersOfTeamByRole() {
    printf("Enter Team ID: ");
    int userTeamId = 0;
    if (scanf("%d", &userTeamId) != 1) {
        printf("Invalid numeric input for Team ID.\n");
        while (getchar() != '\n');
        return;
    }

    Team* selectedTeam = findTeamById(userTeamId);
    if (selectedTeam == NULL) {
        printf("Team with ID %d not found.\n", userTeamId);
        return;
    }

    printf("Role (1-Batsman, 2-All-Rounder, 3-Bowler): ");
    int userRole = 0;
    if (scanf("%d", &userRole) != 1 || userRole < 1 || userRole > 3) {
        printf("Invalid role selection.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Enter number of players (K): ");
    int requestedCount = 0;
    if (scanf("%d", &requestedCount) != 1 || requestedCount <= 0) {
        printf("Invalid K value.\n");
        while (getchar() != '\n');
        return;
    }

    printf("\nTop %d %s of Team %s:\n", requestedCount, roleTypeToName(userRole), selectedTeam->teamName);
    printf("=========================================================================================================\n");
    printf("%-6s %-25s %-14s %-6s %-7s %-7s %-6s %-6s %-12s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=========================================================================================================\n");

    int printedCount = 0;
    Player* iterator = selectedTeam->playerHead;
    while (iterator != NULL && printedCount < requestedCount) {
        if (iterator->playerType == userRole) {
            printf("%-6d %-25s %-14s %-6d %-7.2f %-7.2f %-6d %-6.2f %-12.2f\n",
                   iterator->playerId,
                   iterator->playerName,
                   roleTypeToName(iterator->playerType),
                   iterator->totalRuns,
                   iterator->battingAverage,
                   iterator->strikeRate,
                   iterator->totalWickets,
                   iterator->economyRate,
                   iterator->performanceIndex);
            ++printedCount;
        }
        iterator = iterator->next;
    }

    if (printedCount == 0) {
        printf("No players of role %s found in team %s.\n", roleTypeToName(userRole), selectedTeam->teamName);
    }

    printf("=========================================================================================================\n");
}

static void swapHeapNodes(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

static void heapifyDown(HeapNode heapArray[], int heapSize, int rootIndex) {
    int largestIndex = rootIndex;
    int leftChildIndex = 2 * rootIndex + 1;
    int rightChildIndex = 2 * rootIndex + 2;

    if (leftChildIndex < heapSize && heapArray[leftChildIndex].playerPointer->performanceIndex > heapArray[largestIndex].playerPointer->performanceIndex) {
        largestIndex = leftChildIndex;
    }
    if (rightChildIndex < heapSize && heapArray[rightChildIndex].playerPointer->performanceIndex > heapArray[largestIndex].playerPointer->performanceIndex) {
        largestIndex = rightChildIndex;
    }
    if (largestIndex != rootIndex) {
        swapHeapNodes(&heapArray[rootIndex], &heapArray[largestIndex]);
        heapifyDown(heapArray, heapSize, largestIndex);
    }
}

static void displayAllPlayersOfRoleAcrossTeams() {
    printf("Role (1-Batsman, 2-All-Rounder, 3-Bowler): ");
    int userRole = 0;
    if (scanf("%d", &userRole) != 1 || userRole < 1 || userRole > 3) {
        printf("Invalid role selection.\n");
        while (getchar() != '\n');
        return;
    }

    HeapNode initialHeap[NUMBER_OF_TEAMS];
    int heapSize = 0;
    for (int index = 0; index < NUMBER_OF_TEAMS; ++index) {
        Player* candidate = teamList[index].playerHead;
        while (candidate != NULL && candidate->playerType != userRole) {
            candidate = candidate->next;
        }
        if (candidate != NULL) {
            initialHeap[heapSize].playerPointer = candidate;
            initialHeap[heapSize].teamIndex = index;
            ++heapSize;
        }
    }

    if (heapSize == 0) {
        printf("No players of the chosen role across all teams.\n");
        return;
    }

    for (int index = heapSize / 2 - 1; index >= 0; --index) {
        heapifyDown(initialHeap, heapSize, index);
    }

    printf("\nAll %s across all teams sorted by performance index (desc):\n", roleTypeToName(userRole));
    printf("=========================================================================================================\n");
    printf("%-6s %-25s %-14s %-6s %-7s %-7s %-6s %-6s %-12s\n", "ID", "Name", "Team", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("=========================================================================================================\n");

    while (heapSize > 0) {
        HeapNode topNode = initialHeap[0];
        Player* printedPlayer = topNode.playerPointer;
        int teamIdx = topNode.teamIndex;

        printf("%-6d %-25s %-14s %-6d %-7.2f %-7.2f %-6d %-6.2f %-12.2f\n",
               printedPlayer->playerId,
               printedPlayer->playerName,
               teamList[teamIdx].teamName,
               printedPlayer->totalRuns,
               printedPlayer->battingAverage,
               printedPlayer->strikeRate,
               printedPlayer->totalWickets,
               printedPlayer->economyRate,
               printedPlayer->performanceIndex);

        Player* advancePointer = printedPlayer->next;
        while (advancePointer != NULL && advancePointer->playerType != userRole) {
            advancePointer = advancePointer->next;
        }

        if (advancePointer != NULL) {
            initialHeap[0].playerPointer = advancePointer;
            initialHeap[0].teamIndex = teamIdx;
        } else {
            initialHeap[0] = initialHeap[heapSize - 1];
            --heapSize;
        }

        if (heapSize > 0) heapifyDown(initialHeap, heapSize, 0);
    }

    printf("=========================================================================================================\n");
}

static void interactiveAddNewPlayer() {
    printf("Enter Team ID to add player: ");
    int teamIdInput = 0;
    if (scanf("%d", &teamIdInput) != 1) {
        printf("Team ID must be numeric.\n");
        while (getchar() != '\n');
        return;
    }

    Team* selectedTeam = findTeamById(teamIdInput);
    if (selectedTeam == NULL) {
        printf("Team not found for ID %d.\n", teamIdInput);
        return;
    }

    while (getchar() != '\n'); 

    char inputPlayerName[MAXIMUM_PLAYER_NAME_LENGTH];
    printf("Enter Player Name: ");
    if (fgets(inputPlayerName, sizeof(inputPlayerName), stdin) == NULL) {
        printf("Failed to read name. Aborting add operation.\n");
        return;
    }
    inputPlayerName[strcspn(inputPlayerName, "\n")] = '\0';

    
    int newPlayerId;
    printf("Enter Player id: ");
    if(scanf("%d",&newPlayerId)!=1){
        printf("Failed to read palyer id. Aborting add operation.\n");
        return;
    }

    if (inputPlayerName[0] == '\0') {
        printf("Player name cannot be empty. Aborting add operation.\n");
        return;
    }

    printf("Role (1-Batsman, 2-All-Rounder, 3-Bowler): ");
    int roleInput = 0;
    if (scanf("%d", &roleInput) != 1 || roleInput < 1 || roleInput > 3) {
        printf("Invalid role selection.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Total Runs: ");
    int totalRunsInput = 0;
    if (scanf("%d", &totalRunsInput) != 1) {
        printf("Invalid numeric input for runs.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Batting Average: ");
    float battingAverageInput = 0.0f;
    if (scanf("%f", &battingAverageInput) != 1) {
        printf("Invalid numeric input for batting average.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Strike Rate: ");
    float strikeRateInput = 0.0f;
    if (scanf("%f", &strikeRateInput) != 1) {
        printf("Invalid numeric input for strike rate.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Wickets: ");
    int wicketsInput = 0;
    if (scanf("%d", &wicketsInput) != 1) {
        printf("Invalid numeric input for wickets.\n");
        while (getchar() != '\n');
        return;
    }

    printf("Economy Rate: ");
    float economyRateInput = 0.0f;
    if (scanf("%f", &economyRateInput) != 1) {
        printf("Invalid numeric input for economy rate.\n");
        while (getchar() != '\n');
        return;
    }

    int addResult = addPlayerToTeam(newPlayerId,
                                    inputPlayerName,
                                    selectedTeam->teamName,
                                    roleInput,
                                    totalRunsInput,
                                    battingAverageInput,
                                    strikeRateInput,
                                    wicketsInput,
                                    economyRateInput);
    if (addResult == 0) {
        printf("Player added successfully to team %s.\n", selectedTeam->teamName);
    } else {
        printf("Failed to add player (error code %d).\n", addResult);
    }
}

static void freeAllPlayers(Player* head) {
    while (head != NULL) {
        Player* nextNode = head->next;
        free(head);
        head = nextNode;
    }
}

static void freeAllAllocatedMemory() {
    for (int index = 0; index < NUMBER_OF_TEAMS; ++index) {
        freeAllPlayers(teamList[index].playerHead);
        teamList[index].playerHead = NULL;
        teamList[index].playerCount = 0;
        teamList[index].strikeRateSum = 0.0f;
        teamList[index].strikeRateCount = 0;
        teamList[index].teamAverageBattingStrikeRate = 0.0f;
    }
}

int main() {
    initializeAllPlayersIdTracker();
    initializeTeamsListFromHeader();
    initializePlayersFromHeader();

    while (1) {
        printf("\n=============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("=============================================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display All Players of Specific Role Across All Teams by Performance Index\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        int menuChoice = 0;
        if (scanf("%d", &menuChoice) != 1) {
            printf("Invalid input. Please enter a number between 1 and 6.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (menuChoice) {
            case 1:
                interactiveAddNewPlayer();
                break;
            case 2:
                displayPlayersOfSpecificTeamById();
                break;
            case 3:
                displayTeamsByAverageStrikeRate();
                break;
            case 4:
                displayTopKPlayersOfTeamByRole();
                break;
            case 5:
                displayAllPlayersOfRoleAcrossTeams();
                break;
            case 6:
                freeAllAllocatedMemory();
                printf("Exiting program. \n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}