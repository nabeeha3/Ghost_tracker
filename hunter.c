
#include "defs.h"

void initHunter(Hunter* newHunter, char* name, HouseType* house, int index, EvidenceType equipment) {
    newHunter->currentRoom = house->rooms->room;
    newHunter->equipmentType = equipment;
    strcpy(newHunter->name, name);
    newHunter->sharedEvidenceCollection = house->sharedEvidence;
    newHunter->fear = 0;
    newHunter->boredom = 0;
    sem_init(&newHunter->hunterSemaphore, 0, 1);

    house->rooms->room->huntersInRoom[index] = newHunter;
    house->huntersInHouse[index] = newHunter;

    l_hunterInit(newHunter->name, newHunter->equipmentType);
}

void* hunterThread(void* arg) {
    HouseType* house = (HouseType*)arg;
    Ghost* ghost = house->ghost;

    Hunter* hunter;
    int index;
    for(int i = 0; i < NUM_HUNTERS; ++i) {
        if(pthread_self() == house->hunterThreads[i]){
            hunter = house->huntersInHouse[i];
            index = i;
            break;
        }
    }

    while(hunter->boredom < BOREDOM_MAX && hunter->fear < FEAR_MAX){
        sem_wait(&hunter->hunterSemaphore);

        if(strcmp(hunter->currentRoom->name, ghost->currentRoom->name) == 0) {
            hunter->fear += FEAR_INCREMENT;
            hunter->boredom = 0;
        } else {
            hunter->boredom++;
        }

        int action = randInt(0, 3);

        if(action == 0) {
            EvidenceNode* currentEvidence = hunter->currentRoom->evidenceCollection;
            EvidenceNode* previousEvidence = NULL;

            while(currentEvidence != NULL){
                if(currentEvidence->evidence.evidenceLeftByGhost == hunter->equipmentType){
                    if(previousEvidence != NULL){
                        previousEvidence->next = currentEvidence->next;
                        break;
                    }
                    currentEvidence->next = house->sharedEvidence;
                    house->sharedEvidence = currentEvidence;

                    previousEvidence = currentEvidence;
                }
            }
        } else if(action == 1){
            Room* newRoom = getRandomRoom(hunter->currentRoom->connectedRooms);
            hunter->currentRoom->huntersInRoom[index] = NULL;
            hunter->currentRoom = newRoom;
            hunter->currentRoom->huntersInRoom[index] = hunter;

            l_hunterMove(hunter->name, hunter->currentRoom->name);
        } else {
            EvidenceNode* sharedEvidence = hunter->sharedEvidenceCollection;

            enum EvidenceType uniqueTypes[EV_COUNT];
            int uniques = 0;

            while(sharedEvidence != NULL){
                enum EvidenceType evidenceTypeNumber = sharedEvidence->evidence.evidenceLeftByGhost;
                if(uniqueTypes[evidenceTypeNumber] != evidenceTypeNumber){
                    uniqueTypes[evidenceTypeNumber] == evidenceTypeNumber;
                    uniques++;
                }
                if(uniques == 3){
                    l_hunterExit(hunter->name, LOG_SUFFICIENT);
                    for(int i = 0; i < NUM_HUNTERS; ++i){
                        pthread_cancel(house->hunterThreads[i]);
                    }
                    pthread_cancel(ghost->threadId);
                    pthread_exit(NULL);
                }
                sharedEvidence = sharedEvidence->next;
            }
        }
        sem_post(&hunter->hunterSemaphore);
    
        usleep(HUNTER_WAIT);

    }

    if(hunter->boredom >= BOREDOM_MAX) {
        l_hunterExit(hunter->name, LOG_BORED);
    } else {
        l_hunterExit(hunter->name, LOG_FEAR);
    }

    pthread_exit(NULL);
}

