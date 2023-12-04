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
    Ghost* ghost = (struct Ghost*)arg;

    pthread_exit(NULL);
}