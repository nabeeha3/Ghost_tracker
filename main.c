#include "defs.h"

/*
    Purpose: Main function to run the house simulation.
*/
int main()
{
    // Initialize the random number generator
    srand(time(NULL));

    // Initialize house and populate rooms
    HouseType house;
    initHouse(&house);
    populateRooms(&house);

    // Initialize ghost
    Ghost ghost;
    house.ghost = &ghost;
    initGhost(&house);


    // Get hunter names
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

    for (int i = 0; i < NUM_HUNTERS; ++i) {
        house.huntersInHouse[i] = (Hunter*)malloc(sizeof(Hunter));
        house.huntersInHouse[i]->equipmentType = (EvidenceType)i;
        initHunter(house.huntersInHouse[i], hunterNames[i], &house, i, house.huntersInHouse[i]->equipmentType);
        pthread_create(&house.hunterThreads[i], NULL, hunterThread, (void*)&house);
    }


    pthread_create(&(ghost.threadId), NULL, ghostThread, (void*)&house);

    pthread_join(house.ghost->threadId, NULL);

    return 0;
}


