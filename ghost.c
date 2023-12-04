#include "defs.h"

/*
    Purpose: Initialize the ghost in the house
  Params:
    in/out: house - pointer to the HouseType structure containing ghost and house information
*/
void initGhost(HouseType* house) 
{
    Ghost* ghost = house->ghost;
    ghost->ghostType = randomGhost();
    ghost->currentRoom = placeGhostRandomly(house);
    ghost->boredomTimer = 0;
    sem_init(&ghost->ghostSemaphore, 0, 1);

    ghost->currentRoom->ghostInRoom = ghost;

    l_ghostInit(ghost->ghostType, ghost->currentRoom->name);
}

/*
    Purpose: Place the ghost in a random room, excluding the van
  Params:
    in: house - pointer to the HouseType structure containing room information
  Returns:
    out: Room* - pointer to the randomly selected room for the ghost
*/
Room* placeGhostRandomly(HouseType* house) 
{   
    // Get a random room from the house
    struct Room* randomRoom = getRandomRoom(house->rooms);

    // If the randomly selected room is the van, get another random room
    while (strcmp(randomRoom->name, "Van") == 0) 
    {
        randomRoom = getRandomRoom(house->rooms);
    }

    return randomRoom;
}

/*
    Purpose: Get a random room from the list of rooms
  Params:
    in: roomList - pointer to the RoomNode structure containing the list of rooms
  Returns:
    out: Room* - pointer to the randomly selected room
*/
Room* getRandomRoom(RoomNode* roomList) 
{
    // Count the number of rooms in the list
    int roomCount = 0;
    RoomNode* currentRoom = roomList;
    while (currentRoom != NULL) 
    {
        roomCount++;
        currentRoom = currentRoom->next;
    }

    // Generate a random index within the range of available rooms
    int randomIndex = randInt(0, roomCount);

    // Traverse the list to the randomly selected room
    currentRoom = roomList;
    for (int i = 0; i < randomIndex; i++) 
    {
        currentRoom = currentRoom->next;
    }

    // Return the randomly selected room
    return currentRoom->room;
}

/*
    Purpose: Leave evidence in the current room based on the type of ghost.
  Params:
    in/out: house - pointer to the HouseType structure containing ghost and room information
*/
void leaveEvidence(HouseType* house) 
{
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
        default:
            for (int i = 0; i < GHOST_EVIDENCE; ++i) 
            {
                typesOfEvidence[i] = EV_UNKNOWN;
            }
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

/*
    Purpose: Simulate the behavior of the ghost in the haunted house.
  Params:
    in: arg - pointer to the HouseType structure containing ghost and house information
*/
void* ghostThread(void* arg) 
{
    HouseType* house = (HouseType*)arg;
    Ghost* ghost = house->ghost;

    while(ghost->boredomTimer < BOREDOM_MAX)
    {
        sem_wait(&ghost->ghostSemaphore);

        if(ghost->currentRoom->huntersInRoom[0] != NULL) 
        {
            ghost->boredomTimer = 0;
            int action = randInt(0, 2);

            if(action == 1) 
            {
                leaveEvidence(house);
            }
        } else {
            ghost->boredomTimer++;
            int action = randInt(0, 3);

            if(action == 1) 
            {
                leaveEvidence(house);
            } 
            else if(action == 2)
            {
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
