#include "defs.h"

/*
    Purpose: create a new list of rooms excluding a specified room.
  Params:
    in: rooms - the original list of rooms
    in: excludedRoom - the room to be excluded from the new list
*/
RoomNode* createRoomListExcluding(RoomNode* rooms, Room* excludedRoom) 
{
    RoomNode* newRoomList = NULL;

    //traverse original list of rooms
    RoomNode* currentRoom = rooms;
    while (currentRoom != NULL) 
    {
        // check if the current room is not the room we don't want
        if (currentRoom->room != excludedRoom) 
        {
            //create new node for room, add to new list
            RoomNode* newNode = (RoomNode*)malloc(sizeof(RoomNode));
            newNode->room = currentRoom->room;
            newNode->next = newRoomList;
            newRoomList = newNode;
        }
        currentRoom = currentRoom->next;
    }
    return newRoomList;
}

/*
    Purpose: assign a random room to the ghost from the provided list.
  Params:
    in: rooms - the list of rooms to choose from
*/
Room* assignGhostRoom(RoomNode* rooms) 
{
    int numRooms = countRooms(rooms);
    //random index to select a room
    int randomIndex = randInt(0, numRooms);

    //find room at random index
    RoomNode* currentRoom = rooms;
    for (int i = 0; i < randomIndex; ++i) 
    {
        currentRoom = currentRoom->next;
    }
    return currentRoom->room;
}

//function to get a room by its name
Room* getRoomByName(RoomNode* rooms, const char* targetName) 
{
    RoomNode* currentRoomNode = rooms;

    while (currentRoomNode != NULL) 
    {
        if (strcmp(currentRoomNode->room->name, targetName) == 0) 
        {
            return currentRoomNode->room;
        }
        currentRoomNode = currentRoomNode->next;
    }
    return NULL;
}

//function to check if the ghost is in the same room as a hunter
int isGhostInSameRoomAsHunter(Ghost* ghost, Hunter* hunters[NUM_HUNTERS]) 
{
    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        return (ghost->currentRoom == hunters[i]->currentRoom);
    }
}

//function to check what evidence types a specific ghost can leave
Evidence* getGhostEvidence(Ghost* ghost)
{
    EvidenceType* typesOfEvidence = (EvidenceType*)malloc(GHOST_EVIDENCE * sizeof(EvidenceType));
    Evidence pieceOfEvidence;

    switch(ghost->ghostType) 
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
        default:
            for (int i = 0; i < GHOST_EVIDENCE; ++i) 
            {
                typesOfEvidence[i] = EV_UNKNOWN;
            }
            break;
    }

    int evidenceType = randInt(0, GHOST_EVIDENCE);

    return pieceOfEvidence.evidenceLeftByGhost = typesOfEvidence[evidenceType];
}

//function to add the evidence that each ghost leaves behind to shared evidence list
void addEvidenceToRoom(Room* room, Evidence* evidenceToAdd) 
{

    EvidenceNode* newEvidenceNode = (EvidenceNode*)malloc(sizeof(EvidenceNode));

    if (newEvidenceNode == NULL) 
    {
        perror("Nothing to add...");
        exit(EXIT_FAILURE);
    }

    //assign evidence to new node
    newEvidenceNode->evidence = *evidenceToAdd;
    newEvidenceNode->next = NULL;

    sem_wait(&room->evidenceSemaphore);

    //if shared evidence list is empty, set new node as the head
    if (room->evidenceCollection == NULL) 
    {
        room->evidenceCollection = newEvidenceNode;
    } 
    else 
    {
        //append the new node
        EvidenceNode* currentEvidence = room->evidenceCollection;
        while (currentEvidence->next != NULL) 
        {
            currentEvidence = currentEvidence->next;
        }
        currentEvidence->next = newEvidenceNode;
    }
    sem_post(&room->evidenceSemaphore);
}

//
Room* getRandomConnectedRoom(Room* room) 
{
    RoomNode* connectedRooms = room->connectedRooms;

    int numConnectedRooms = 0;
    while (connectedRooms != NULL) 
    {
        numConnectedRooms++;
        connectedRooms = connectedRooms->next;
    }

    if (numConnectedRooms == 0) 
    {
        return NULL;
    }

    int randomIndex = randInt(0, numConnectedRooms - 1);

    connectedRooms = room->connectedRooms;
    for (int i = 0; i < randomIndex; ++i) {
        connectedRooms = connectedRooms->next;
    }

    return connectedRooms->room;
}

//ghost thread 
void* runGhostThread(Ghost* a, Hunter* b[NUM_HUNTERS], HouseType* c) 
{
    Ghost* ghost = a;
    Hunter* hunters[NUM_HUNTERS] = b[NUM_HUNTERS];
    HouseType* house = c;

    while (ghost->boredomTimer < BOREDOM_MAX) 
    {
        //wait on the semaphore
        sem_wait(&ghost->ghostSemaphore);

        //check if ghost in room with a hunter
        if (isGhostInSameRoomAsHunter(&ghost, &hunters)) 
        {
            ghost->boredomTimer = 0; 

            //randomly choose to leave evidence or do nothing
            if (randInt(0, 2) == 0) 
            {
                Evidence* evidenceLeftByGhost = getGhostEvidence(ghost);

                //wait on the evidence semaphore
                sem_wait(&ghost->currentRoom->evidenceSemaphore);
                addEvidenceToRoom(ghost->currentRoom, &evidenceLeftByGhost);
                sem_post(&ghost->currentRoom->evidenceSemaphore);

                l_ghostEvidence(evidenceLeftByGhost, ghost->currentRoom->name);
            }
        } 
        else 
        {
            ghost->boredomTimer++; 
            int action = randInt(0, 3);

            if (action == 0) 
            {
                //do nothing
            } 
            else if (action == 1) 
            {
                //randomly choose to move to a connected room
                Room* nextRoom = getRandomConnectedRoom(ghost->currentRoom);
                if (nextRoom != NULL) 
                {
                    //move to next room
                    ghost->currentRoom->ghostInRoom = NULL;
                    ghost->currentRoom = nextRoom;
                    ghost->currentRoom->ghostInRoom = ghost;

                    l_ghostMove(ghost->currentRoom->name);
                }
            } 
            else if (action == 2) 
            {
                Evidence* evidenceLeftByGhost = getGhostEvidence(ghost);

                //wait on the evidence semaphore
                sem_wait(&ghost->currentRoom->evidenceSemaphore);
                addEvidenceToRoom(ghost->currentRoom, evidenceLeftByGhost);
                sem_post(&ghost->currentRoom->evidenceSemaphore);

                l_ghostEvidence(evidenceLeftByGhost, ghost->currentRoom->name);
            }
        }
        //post to the semaphore
        sem_post(&ghost->ghostSemaphore);
        //sleep for a short duration before the next iteration
        usleep(GHOST_WAIT);
    }

    //ghost exits due to boredom
    l_ghostExit(LOG_BORED);
    pthread_exit(NULL);
}
