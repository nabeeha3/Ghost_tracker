#include "defs.h"

int main()
{
    // Initialize the random number generator
    srand(time(NULL));

    // 1.1 Ask user for 4 hunter names
    char hunterNames[NUM_HUNTERS][MAX_STR];

    for (int i = 0; i < NUM_HUNTERS; ++i) {
        printf("Enter name for Hunter %d: ", i + 1);

        // Read the entire line, including spaces
        fgets(hunterNames[i], sizeof(hunterNames[i]), stdin);

        // Remove the trailing newline character if present
        size_t len = strlen(hunterNames[i]);
        if (len > 0 && hunterNames[i][len - 1] == '\n') {
            hunterNames[i][len - 1] = '\0';
        }
    }

    // Create the house: You may change this, but it's here for demonstration purposes
    // Note: This code will not compile until you have implemented the house functions and structures
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    placeHuntersInVan(&house, hunterNames);
    placeGhostRandomly(&house);

    for (int i = 0; i < NUM_HUNTERS; ++i) {
        pthread_create(&house.hunterThreads[i], NULL, hunterThread, (void*) &house.huntersInHouse[i]);
    }

    pthread_create(&house.ghost->threadId, NULL, ghostThread, (void*)&house.ghost);

    return 0;
}

