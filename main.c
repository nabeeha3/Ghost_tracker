#include "defs.h"

int main()
{
    // Initialize the random number generator
    srand(time(NULL));

    //1.1 Ask user for 4 hunter names
    char hunterNames[NUM_HUNTERS][MAX_STR];
    // int equipmentName [NUM_HUNTERS];

    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        printf("Enter name for Hunter %d: ", i + 1);
        scanf("%s", hunterNames[i]);
        // printf("Enter a number for the equipment type to assign to the hunter \n (0 = EMF, 1 = TEMPERATURE, 2 = FINGERPRINTS, 3 = SOUND): ");
        // scanf("%d", equipmentName[i]);
    }

    // 1.2 Create the house: & populate rooms
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    // 1.3 create 4 hunters and start them in the van
    Hunter hunters[NUM_HUNTERS];
    pthread_t hunterThreads[NUM_HUNTERS];
    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        hunters[i].currentRoom = getRoomByName(&house.rooms, "Van");
        hunters[i].equipmentType = (EvidenceType)i;  //base evidence collect type on # in index
        strncpy(hunters[i].name, hunterNames[i], MAX_STR);
        hunters[i].fear = 0;
        hunters[i].boredom = 0;
        sem_init(&hunters[i].hunterSemaphore, 0, 1);

        l_hunterInit(hunters[i].name, hunters[i].equipmentType);

        //1.5 create threads for hunters 
        pthread_create(&hunterThreads[i], NULL, NULL, NULL);
    }

    //1.4 place the ghost in a random room, that is not the van
    Ghost ghost;
    pthread_t ghostThread;
    RoomNode* roomsExcludingVan = createRoomListExcluding(house.rooms, getRoomByName(&house.rooms, "Van"));
    Room* ghostStartingRoom = assignGhostRoom(roomsExcludingVan);
    ghost.currentRoom = ghostStartingRoom;
    ghost.ghostType = randomGhost();
    ghost.boredomTimer = 0;

    l_ghostInit(ghost.ghostType, ghost.currentRoom);
    //freeRoomList(roomsExcludingVan);

    //1.5 create thread for ghost
    //create semaphore for the ghost
    sem_t ghostSemaphore;
    sem_init(&ghostSemaphore, 0, 1);

    pthread_create(&ghostThread, NULL, NULL, NULL);


    return 0;
}

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
Room* getRoomByName(RoomNode* rooms, const char* targetName) {
    RoomNode* currentRoomNode = rooms;

    while (currentRoomNode != NULL) {
        if (strcmp(currentRoomNode->room->name, targetName) == 0) {
            return currentRoomNode->room;
        }
        currentRoomNode = currentRoomNode->next;
    }
    return NULL;
}


//function to check if the ghost is in the same room as a hunter
int isGhostInSameRoomAsHunter(Ghost* ghost, Hunter* hunter) 
{
    return (ghost->currentRoom == hunter->currentRoom);
}

//ghost thread 
void* runGhostThread(Ghost* a, Hunter* b[NUM_HUNTERS], HouseType* c) 
{
    Ghost* ghost = a;
    Hunter* hunters[NUM_HUNTERS] = b[NUM_HUNTERS];
    HouseType* house = c;

    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        while (ghost->boredomTimer < BOREDOM_MAX) 
        {
            //wait on the semaphore
            sem_wait(&ghost->ghostSemaphore);

            //check if ghost in room with a hunter
            if (isGhostInSameRoomAsHunter(&ghost, &hunters[i])) 
            {
                ghost->boredomTimer = 0; 

                //randomly choose to leave evidence or do nothing
                if (randInt(0, 2) == 0) 
                {
                    enum EvidenceType evidenceType = randomGhostEvidenceType();

                    //wait on the evidence semaphore
                    sem_wait(&ghost->currentRoom->evidenceSemaphore);
                    //addEvidenceToRoom(ghost->currentRoom, evidenceType);
                    sem_post(&ghost->currentRoom->evidenceSemaphore);

                    l_ghostEvidence(evidenceType, ghost->currentRoom->name);
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
                    //randomly choose to move to an adjacent room
                    Room* nextRoom = getRandomAdjacentRoom(ghost->currentRoom);
                    if (nextRoom != NULL) 
                    {
                        //move to next room
                        //ghost->room->ghostInRoom = NULL;
                        ghost->currentRoom = nextRoom;
                        //nextRoom->ghostInRoom = ghost;

                        l_ghostMove(ghost->currentRoom->name);
                    }
                } 
                else if (action == 2) 
                {
                    //randomly choose to leave evidence
                    enum EvidenceType evidenceType = randomGhostEvidenceType();

                    //wait on the evidence semaphore
                    sem_wait(&ghost->currentRoom->evidenceSemaphore);
                    //addEvidenceToRoom(ghost->currentRoom, evidenceType);
                    sem_post(&ghost->currentRoom->evidenceSemaphore);

                    l_ghostEvidence(evidenceType, ghost->currentRoom->name);
                }
            }
            //post to the semaphore
            sem_post(&ghost->ghostSemaphore);
            //sleep for a short duration before the next iteration
            usleep(GHOST_WAIT);
        }
    }
    //ghost exits due to boredom
    l_ghostExit(LOG_BORED);
    pthread_exit(NULL);
}

