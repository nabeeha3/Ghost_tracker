#include "defs.h"

// Function to create a new hunter with the given name
struct Hunter* createHunter(const char* name, struct HouseType* house) {
    struct Hunter* newHunter = malloc(sizeof(struct Hunter));

    // Initialize hunter fields
    strcpy(newHunter->name, name);
    newHunter->currentRoom = house->rooms->room;  // Set the current room to the Van room
    newHunter->equipmentType = randomEquipmentType();  // You need to define this function
    newHunter->sharedEvidenceCollection = NULL;  // Initialize as needed
    newHunter->fear = 0;
    newHunter->boredom = 0;
    newHunter->threadId = 0;  // Initialize as needed
    sem_init(&newHunter->hunterSemaphore, 0, 1);  // Initialize semaphore

    // Add the hunter to the head of the list in the Van room
    addHunterToRoom(&house->rooms->room->huntersInRoom, newHunter, &house->rooms->room);

    l_hunterInit(newHunter->name, newHunter->equipmentType);
    return newHunter;
}

// Function to add a hunter to the given room's list of hunters
void addHunterToRoom(struct Hunter** hunterList, struct Hunter* newHunter, struct Room* currentRoom) {
    newHunter->next = *hunterList;
    *hunterList = newHunter;
    newHunter->currentRoom = currentRoom;
}

// Function to find a room in the house by name
struct Room* findRoomByName(struct RoomNode** roomList, const char* targetName) {
    struct RoomNode* currentRoomNode = *roomList;
    while (currentRoomNode != NULL) {
        if (strcmp(currentRoomNode->room->name, targetName) == 0) {
            return currentRoomNode->room;
        }
        currentRoomNode = currentRoomNode->next;
    }
    return NULL;  // Room not found
}

// Function to place hunters in the Van room
void placeHuntersInVan(struct HouseType* house, char hunterNames[NUM_HUNTERS][MAX_STR]) {
    // Access the Van room directly as it's the head of the room list
    struct Room* vanRoom = house->rooms;

    // Create and place hunters in the Van room
    for (int i = 0; i < NUM_HUNTERS; ++i) {
        struct Hunter* newHunter = createHunter(hunterNames[i], house);
        // Place the hunter in the head of the room's hunters list
        newHunter->next = vanRoom->huntersInRoom;
        vanRoom->huntersInRoom = newHunter;
    }
}

enum EvidenceType randomEquipmentType() {
    return (enum EvidenceType) randInt(0, EV_COUNT);
}

void* hunterThread(void* arg) {
    struct Hunter* hunter = (struct Hunter*)arg;

    pthread_exit(NULL);
}