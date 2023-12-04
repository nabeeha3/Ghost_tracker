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
    addHunterToRoom(&house->rooms->room->huntersInRoom, newHunter);

    l_hunterInit(newHunter->name, newHunter->equipmentType);
    return newHunter;
}

// Function to add a hunter to the given room's list of hunters
void addHunterToRoom(struct Hunter** hunterList, struct Hunter* newHunter) {
    newHunter->next = *hunterList;
    *hunterList = newHunter;
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

    while (1) {
        sem_wait(&hunter->hunterSemaphore);

        // Check if the hunter is in a room with a ghost
        if (hunter->currentRoom->ghostInRoom != NULL) {
            // Increase fear field by 1, reset boredom timer to 0
            hunter->fear++;
            hunter->boredom = 0;
        } else {
            // Not in the room with a ghost, increase boredom by 1
            hunter->boredom++;
        }

        // Randomly choose to collect evidence, move, or review evidence
        int action = randInt(0, 3);
        if (action == 0) {
            // Collect evidence
            collectEvidence(hunter);
        } else if (action == 1) {
            // Move to a random connected room
            moveHunterToRandomRoom(hunter);
        } else if (action == 2) {
            // Review evidence
            reviewEvidence(hunter);
        }

        // Check if fear is greater than or equal to FEAR_MAX, exit the thread
        if (hunter->fear >= FEAR_MAX) {
            l_hunterExit(hunter->name, LOG_FEAR);
            pthread_exit(NULL);
        }

        // Check if boredom is greater than or equal to BOREDOM_MAX, exit the thread
        if (hunter->boredom >= BOREDOM_MAX) {
            l_hunterExit(hunter->name, LOG_BORED);
            pthread_exit(NULL);
        }

        sem_post(&hunter->hunterSemaphore);

        // Sleep to simulate time passing
        usleep(HUNTER_WAIT);
    }
    // Shouldn't reach here
    pthread_exit(NULL);
}

// Function to move the hunter to a random connected room
void moveHunterToRandomRoom(struct Hunter* hunter) {
    sem_wait(&hunter->currentRoom->roomSemaphore);

    // Choose a random connected room
    struct RoomNode* connectedRoom = hunter->currentRoom->connectedRooms;
    int numConnectedRooms = countConnectedRooms(connectedRoom);

    if (numConnectedRooms > 0) {
        int randomIndex = randInt(0, numConnectedRooms);
        for (int i = 0; i < randomIndex; ++i) {
            connectedRoom = connectedRoom->next;
        }

        // Update room pointers
        hunter->currentRoom->huntersInRoom = removeHunterFromRoom(hunter->currentRoom->huntersInRoom, hunter);
        hunter->currentRoom = connectedRoom->room;
        addHunterToRoom(&hunter->currentRoom->huntersInRoom, hunter);

        // Log the hunter's movement
        l_hunterMove(hunter->name, hunter->currentRoom->name);
    }

    sem_post(&hunter->currentRoom->roomSemaphore);
}

// Function to review evidence for the hunter
void reviewEvidence(struct Hunter* hunter) {
    sem_wait(&hunter->hunterSemaphore);

    // Check if there is sufficient evidence to exit the simulation
    if (countUniqueEvidence(hunter->sharedEvidenceCollection) >= 3) {
        l_hunterReview(hunter->name, LOG_SUFFICIENT);
        l_hunterExit(hunter->name, LOG_EVIDENCE);
        pthread_exit(NULL);
    } else {
        l_hunterReview(hunter->name, LOG_INSUFFICIENT);
    }

    sem_post(&hunter->hunterSemaphore);
}

// Function to collect evidence for the hunter
void collectEvidence(struct Hunter* hunter) {
    sem_wait(&hunter->currentRoom->evidenceSemaphore);

    // Check each piece of evidence in the room's evidence collection
    struct EvidenceNode* evidenceNode = hunter->currentRoom->evidenceCollection;
    while (evidenceNode != NULL) {
        struct Evidence* evidence = evidenceNode->evidence;

        // Check if the hunter can detect this type of evidence
        if (hunterCanDetectEvidence(hunter, evidence->evidenceLeftByGhost)) {
            // Remove evidence from the room's collection
            hunter->currentRoom->evidenceCollection = removeEvidenceFromRoom(hunter->currentRoom->evidenceCollection, evidence);
            
            // Add evidence to the hunter's shared evidence collection
            addEvidenceToHunter(&hunter->sharedEvidenceCollection, evidence);
            
            // Log the hunter's collection
            l_hunterCollect(hunter->name, evidence->evidenceLeftByGhost, hunter->currentRoom->name);
        }

        evidenceNode = evidenceNode->next;
    }

    sem_post(&hunter->currentRoom->evidenceSemaphore);
}