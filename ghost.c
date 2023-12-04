#include "defs.h"

// Place the ghost in a random room, not including the van
void placeGhostRandomly(HouseType* house) {
    
    // Get a random room from the house
    struct Room* randomRoom = getRandomRoom(house->rooms);


    // If the randomly selected room is the van, get another random room
    while (strcmp(randomRoom->name, "Van") == 0) {
        randomRoom = getRandomRoom(house->rooms);
    }

    // Place the ghost in the selected room
    house->ghost->currentRoom = randomRoom;
    l_ghostInit(house->ghost->ghostType, randomRoom->name);
    l_ghostMove(randomRoom->name);
}

// Helper function to get a random room from the list
struct Room* getRandomRoom(RoomNode* roomList) {
    // Count the number of rooms in the list
    int roomCount = 0;
    RoomNode* currentRoom = roomList;
    while (currentRoom != NULL) {
        roomCount++;
        currentRoom = currentRoom->next;
    }

    // Generate a random index within the range of available rooms
    int randomIndex = randInt(0, roomCount);

    // Traverse the list to the randomly selected room
    currentRoom = roomList;
    for (int i = 0; i < randomIndex; i++) {
        currentRoom = currentRoom->next;
    }

    // Return the randomly selected room
    return currentRoom->room;
}


void* ghostThread(void* arg) {
    Ghost* ghost = (Ghost*)arg;

    while (1) {
        sem_wait(&ghost->ghostSemaphore);

        // Check if the Ghost is in the room with a hunter
        if (ghost->currentRoom->huntersInRoom != NULL) {
            l_ghostMove(ghost->currentRoom->name);
            l_ghostEvidence(EV_UNKNOWN, ghost->currentRoom->name); // Ghost is leaving evidence or doing nothing randomly
            // Reset boredom timer to 0, and it cannot move
            ghost->boredomTimer = 0;
        } else {
            // Not in the room with a hunter
            l_ghostMove(ghost->currentRoom->name);

            // Increase boredom counter by 1
            ghost->boredomTimer++;

            // Randomly choose to move, leave evidence, or do nothing
            int action = randInt(0, 3);
            if (action == 0) {
                // Move to an adjacent room
                struct Room* newRoom = getRandomAdjacentRoom(ghost->currentRoom);
                if (newRoom != NULL) {
                    moveGhostToRoom(ghost, newRoom);
                    l_ghostMove(newRoom->name);
                }
            } else if (action == 1) {
                // Leave evidence
                EvidenceType evidenceType = getRandomEvidenceType();
                l_ghostEvidence(evidenceType, ghost->currentRoom->name);
                addEvidenceToRoom(&ghost->currentRoom->evidenceCollection, evidenceType);
            }
            // Do nothing if action is 2 (2/3 chance)
        }

        sem_post(&ghost->ghostSemaphore);

        // If boredom counter reaches BOREDOM_MAX, exit the thread
        if (ghost->boredomTimer >= BOREDOM_MAX) {
            l_ghostExit(LOG_BORED);
            pthread_exit(NULL);
        }

        // Sleep to simulate time passing
        usleep(GHOST_WAIT);
    }
    // Shouldn't reach here
    pthread_exit(NULL);
}
