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
    //pthread_t hunterThreads[NUM_HUNTERS];
    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        hunters[i].currentRoom = getRoomByName(house.rooms, "Van");
        hunters[i].equipmentType = (EvidenceType)i;  //base evidence collect type on # in index
        strncpy(hunters[i].name, hunterNames[i], MAX_STR);
        hunters[i].fear = 0;
        hunters[i].boredom = 0;
        sem_init(&hunters[i].hunterSemaphore, 0, 1);

        l_hunterInit(hunters[i].name, hunters[i].equipmentType);

        //1.5 create threads for hunters 
        //pthread_create(&hunterThreads[i], NULL, NULL, NULL);
    }

    //1.4 place the ghost in a random room, that is not the van
    Ghost ghost;
    pthread_t ghostThread;
    RoomNode* roomsExcludingVan = createRoomListExcluding(house.rooms, getRoomByName(house.rooms, "Van"));
    Room* ghostStartingRoom = assignGhostRoom(roomsExcludingVan);
    ghost.currentRoom = ghostStartingRoom;
    ghost.ghostType = randomGhost();
    ghost.boredomTimer = 0;

    l_ghostInit(ghost.ghostType, ghost.currentRoom->name);
    //freeRoomList(roomsExcludingVan);

    //create ghost thread
    GhostThreadArgs ghostThreadArg;
    ghostThreadArg.ghost = &ghost;
    for (int i = 0; i < NUM_HUNTERS; ++i) 
    {
        ghostThreadArg.hunters[i] = &hunters[i];
    }
    ghostThreadArg.house = &house;


    //1.5 create thread for ghost
    //create semaphore for the ghost
    sem_t ghostSemaphore;
    sem_init(&ghostSemaphore, 0, 1);
    pthread_create(&ghostThread, NULL, runGhostThread, (void*)&ghostThreadArg);

    pthread_join(ghostThread, NULL);

    return 0;
}
