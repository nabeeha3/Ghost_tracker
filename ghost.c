#include "defs.h"

// Function to initialize the house
void initGhost(HouseType* house) {
    Ghost* ghost = house->ghost;
    ghost->ghostType = randomGhost();
    ghost->currentRoom = placeGhostRandomly(house);
    ghost->boredomTimer = 0;
    sem_init(&ghost->ghostSemaphore, 0, 1);

    ghost->currentRoom->ghostInRoom = ghost;

    l_ghostInit(ghost->ghostType, ghost->currentRoom->name);
}

// Place the ghost in a random room, not including the van
Room* placeGhostRandomly(HouseType* house) {
    
    // Get a random room from the house
    struct Room* randomRoom = getRandomRoom(house->rooms);


    // If the randomly selected room is the van, get another random room
    while (strcmp(randomRoom->name, "Van") == 0) {
        randomRoom = getRandomRoom(house->rooms);
    }

    return randomRoom;
}

void leaveEvidence(HouseType* house) {
    EvidenceType* typesOfEvidence = (EvidenceType*)malloc(GHOST_EVIDENCE * sizeof(EvidenceType));
    Evidence evidence;

    switch(house->ghost->ghostType) 
    {
        case BANSHEE:
            typesOfEvidence[0] = EMF;
            typesOfEvidence[1] = TEMPERATURE;
            typesOfEvidence[2] = SOUND;
            break;
        case BULLIES:
            typesOfEvidence[0] = EMF;
            typesOfEvidence[1] = FINGERPRINTS;
            typesOfEvidence[2] = SOUND;
            break;
        case PHANTOM:
            typesOfEvidence[0] = FINGERPRINTS;
            typesOfEvidence[1] = TEMPERATURE;
            typesOfEvidence[2] = SOUND;
            break;
        case POLTERGEIST:
            typesOfEvidence[0] = EMF;
            typesOfEvidence[1] = TEMPERATURE;
            typesOfEvidence[2] = FINGERPRINTS;
            break;
    }

    int evidenceType = randInt(0, GHOST_EVIDENCE);
    evidence.evidenceLeftByGhost = typesOfEvidence[evidenceType];
    sem_init(&evidence.evidenceSemaphore, 0, 1);

    EvidenceNode* sharedEvidence = house->sharedEvidence;
    EvidenceNode* roomEvidence = house->ghost->currentRoom->evidenceCollection;

    EvidenceNode* newSharedNode = (EvidenceNode*)malloc(sizeof(EvidenceNode));
    newSharedNode->evidence = evidence;
    newSharedNode->next = sharedEvidence;
    sharedEvidence = newSharedNode;

    EvidenceNode* newRoomNode = (EvidenceNode*)malloc(sizeof(EvidenceNode));
    newRoomNode->evidence = evidence;
    newRoomNode->next = roomEvidence;
    roomEvidence = newRoomNode;
    
    l_ghostEvidence(evidence.evidenceLeftByGhost, house->ghost->currentRoom->name);
}

void* ghostThread(void* arg) {
    HouseType* house = (HouseType*)arg;
    Ghost* ghost = house->ghost;

    while(ghost->boredomTimer < BOREDOM_MAX){
        sem_wait(&ghost->ghostSemaphore);

        if(ghost->currentRoom->huntersInRoom[0] != NULL) {
            ghost->boredomTimer = 0;
            int action = randInt(0, 2);

            if(action == 1) {
                leaveEvidence(house);
            }
        } else {
            ghost->boredomTimer++;
            int action = randInt(0, 3);

            if(action == 1) {
                leaveEvidence(house);
            } else if(action == 2){
                Room* newRoom = getRandomRoom(ghost->currentRoom->connectedRooms);
                ghost->currentRoom->ghostInRoom = NULL;
                ghost->currentRoom = newRoom;
                ghost->currentRoom->ghostInRoom = ghost;

                l_ghostMove(ghost->currentRoom->name);
            }
        }
        sem_post(&ghost->ghostSemaphore);
        
        usleep(GHOST_WAIT);
    }
    l_ghostExit(LOG_BORED);
    pthread_exit(NULL);
}