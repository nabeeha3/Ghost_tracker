#include "defs.h"

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

    // Initialize hunters and create hunter thread
    for(int i = 0; i < NUM_HUNTERS; ++i) {
        Hunter newHunter;
        house.huntersInHouse[i] = &newHunter;
        initHunter(&newHunter, hunterNames[i], &house);
        pthread_create(&house.hunterThreads[i], NULL, hunterThread, (void*)&house);
    }

    pthread_create(&(ghost.threadId), NULL, ghostThread, (void*)&house);

    pthread_join(house.ghost->threadId, NULL);

    return 0;
}

