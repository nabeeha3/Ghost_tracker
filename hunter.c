#include "defs.h"

/*
    Purpose: Initialize a hunter with the provided parameters.
    Params:
        in/out: newHunter - pointer to the hunter structure to initialize.
        in: name - the name of the hunter.
        in: house - pointer to the house structure.
*/

void initHunter(Hunter* newHunter, char* name, HouseType* house) {
    newHunter->currentRoom = house->rooms->room;
    newHunter->equipmentType = randomEquipmentType();
    strcpy(newHunter->name, name);
    newHunter->sharedEvidenceCollection = NULL;
    newHunter->fear = 0;
    newHunter->boredom = 0;
    sem_init(&newHunter->hunterSemaphore, 0, 1);

    l_hunterInit(newHunter->name, newHunter->equipmentType);
}

/*
    Purpose: Generate a random evidence type for a hunter's equipment.
    Returns: A random evidence type.
*/
enum EvidenceType randomEquipmentType() {
    return (enum EvidenceType) randInt(0, EV_COUNT);
}


int isHunterInRoomWithGhost(Hunter* hunter, Ghost* ghost)
{
    if (hunter->currentRoom == ghost->currentRoom)
    {
        return C_TRUE;
    }
    return C_FALSE;
}

/*
    Purpose: Get a randomly connected room from the provided room.
  Params:
    in: room - the room for which to get a connected room
*/
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

void hunterCollectsEvidence(Hunter* hunter)
{
    Room* currentRoom = hunter->currentRoom;

    //this should technicallyy traverse the list of evidence in a room not just this
    if (currentRoom->evidenceCollection->evidenceLeftByGhost == hunter->equipmentType)
    {
        //remove evidence .. obvs not done here, need to actually remove from list
        currentRoom->evidenceCollection->evidenceLeftByGhost;
        //supposedly.... should actually be adding the evidence to the list of shared evidence
        hunter->sharedEvidenceCollection->evidenceToCollection = currentRoom->evidenceCollection->evidenceLeftByGhost;

        l_hunterCollect(hunter, currentRoom->evidenceCollection->evidenceLeftByGhost, currentRoom);
    }
}

void hunterMovesToDifferentRoom(Hunter* hunter)
{
    Room* currentRoom = hunter->currentRoom;
    Room* newRoom = getRandomConnectedRoom(currentRoom);
    hunter->currentRoom = newRoom;

    //not supposed to be NULL as you can have more than one hunter in a room, should remove current hunter
    currentRoom->huntersInRoom = NULL;
    //shouldn't just be equal to hunter but rather adding the hunter to the list
    newRoom->huntersInRoom = hunter;

    l_ghostMove(newRoom);
}

void hunterReviewsEvidence(Hunter* hunter)
{
    //should be going through the list looking at all pieces 
    hunter->sharedEvidenceCollection->evidenceToCollection;
    //traverse through the list of evidence and if there are 3 distinct types of evidence left, exit

    //l_hunterReview(hunter, result);
}

void hunterPossibleActions(Hunter* hunter)
{
    int decision = randInt(1,3);
    
    if (decision == 1) //collect evidence
    {
        hunterCollectsEvidence(&hunter);
    }
    else if (decision == 2) // move
    {
        hunterMovesToDifferentRoom(&hunter);
    }
    else //review evidencec
    {
        hunterReviewsEvidence(&hunter);
    }
}



// the hunter function without threading or what not
void* hunterFunction(Hunter hunter, Ghost ghost) 
{
    while (hunter.boredom <= BOREDOM_MAX && hunter.fear <= FEAR_MAX)
    {
        if (isHunterInRoomWithGhost)
        {
            hunter.fear += INCREASE_EMOTION;
            hunter.boredom = 0;
        }
        else
        {
            hunter.boredom += INCREASE_EMOTION;
        }
        hunterPossibleActions(&hunter);
    }

}



































// // Function to add a hunter to the given room's list of hunters
// void addHunterToRoom(struct Hunter** hunterList, struct Hunter* newHunter) {
//     newHunter->next = *hunterList;
//     *hunterList = newHunter;
// }

// // Function to find a room in the house by name
// struct Room* findRoomByName(struct RoomNode** roomList, const char* targetName) {
//     struct RoomNode* currentRoomNode = *roomList;
//     while (currentRoomNode != NULL) {
//         if (strcmp(currentRoomNode->room->name, targetName) == 0) {
//             return currentRoomNode->room;
//         }
//         currentRoomNode = currentRoomNode->next;
//     }
//     return NULL;  // Room not found
// }

// // Function to place hunters in the Van room
// void placeHuntersInVan(struct HouseType* house, char hunterNames[NUM_HUNTERS][MAX_STR]) {
//     // Access the Van room directly as it's the head of the room list
//     struct Room* vanRoom = house->rooms;

//     // Create and place hunters in the Van room
//     for (int i = 0; i < NUM_HUNTERS; ++i) {
//         struct Hunter* newHunter = createHunter(hunterNames[i], house);
//         // Place the hunter in the head of the room's hunters list
//         newHunter->next = vanRoom->huntersInRoom;
//         vanRoom->huntersInRoom = newHunter;
//     }
// }

void* hunterThread(void* arg){
    
}
// void* hunterThread(void* arg) {
//     struct Hunter* hunter = (struct Hunter*)arg;

//     while (1) {
//         sem_wait(&hunter->hunterSemaphore);

//         // Check if the hunter is in a room with a ghost
//         if (hunter->currentRoom->ghostInRoom != NULL) {
//             // Increase fear field by 1, reset boredom timer to 0
//             hunter->fear++;
//             hunter->boredom = 0;
//         } else {
//             // Not in the room with a ghost, increase boredom by 1
//             hunter->boredom++;
//         }

//         // Randomly choose to collect evidence, move, or review evidence
//         int action = randInt(0, 3);
//         if (action == 0) {
//             // Collect evidence
//             collectEvidence(hunter);
//         } else if (action == 1) {
//             // Move to a random connected room
//             moveHunterToRandomRoom(hunter);
//         } else if (action == 2) {
//             // Review evidence
//             reviewEvidence(hunter);
//         }

//         // Check if fear is greater than or equal to FEAR_MAX, exit the thread
//         if (hunter->fear >= FEAR_MAX) {
//             l_hunterExit(hunter->name, LOG_FEAR);
//             pthread_exit(NULL);
//         }

//         // Check if boredom is greater than or equal to BOREDOM_MAX, exit the thread
//         if (hunter->boredom >= BOREDOM_MAX) {
//             l_hunterExit(hunter->name, LOG_BORED);
//             pthread_exit(NULL);
//         }

//         sem_post(&hunter->hunterSemaphore);

//         // Sleep to simulate time passing
//         usleep(HUNTER_WAIT);
//     }
//     // Shouldn't reach here
//     pthread_exit(NULL);
// }

// // Function to move the hunter to a random connected room
// void moveHunterToRandomRoom(struct Hunter* hunter) {
//     sem_wait(&hunter->currentRoom->roomSemaphore);

//     // Choose a random connected room
//     struct RoomNode* connectedRoom = hunter->currentRoom->connectedRooms;
//     int numConnectedRooms = countConnectedRooms(connectedRoom);

//     if (numConnectedRooms > 0) {
//         int randomIndex = randInt(0, numConnectedRooms);
//         for (int i = 0; i < randomIndex; ++i) {
//             connectedRoom = connectedRoom->next;
//         }

//         // Update room pointers
//         hunter->currentRoom->huntersInRoom = removeHunterFromRoom(hunter->currentRoom->huntersInRoom, hunter);
//         hunter->currentRoom = connectedRoom->room;
//         addHunterToRoom(&hunter->currentRoom->huntersInRoom, hunter);

//         // Log the hunter's movement
//         l_hunterMove(hunter->name, hunter->currentRoom->name);
//     }

//     sem_post(&hunter->currentRoom->roomSemaphore);
// }

// // Function to review evidence for the hunter
// void reviewEvidence(struct Hunter* hunter) {
//     sem_wait(&hunter->hunterSemaphore);

//     // Check if there is sufficient evidence to exit the simulation
//     if (countUniqueEvidence(hunter->sharedEvidenceCollection) >= 3) {
//         l_hunterReview(hunter->name, LOG_SUFFICIENT);
//         l_hunterExit(hunter->name, LOG_EVIDENCE);
//         pthread_exit(NULL);
//     } else {
//         l_hunterReview(hunter->name, LOG_INSUFFICIENT);
//     }

//     sem_post(&hunter->hunterSemaphore);
// }

// // Function to collect evidence for the hunter
// void collectEvidence(struct Hunter* hunter) {
//     sem_wait(&hunter->currentRoom->evidenceSemaphore);

//     // Check each piece of evidence in the room's evidence collection
//     struct EvidenceNode* evidenceNode = hunter->currentRoom->evidenceCollection;
//     while (evidenceNode != NULL) {
//         struct Evidence* evidence = evidenceNode->evidence;

//         // Check if the hunter can detect this type of evidence
//         if (hunterCanDetectEvidence(hunter, evidence->evidenceLeftByGhost)) {
//             // Remove evidence from the room's collection
//             hunter->currentRoom->evidenceCollection = removeEvidenceFromRoom(hunter->currentRoom->evidenceCollection, evidence);
            
//             // Add evidence to the hunter's shared evidence collection
//             addEvidenceToHunter(&hunter->sharedEvidenceCollection, evidence);
            
//             // Log the hunter's collection
//             l_hunterCollect(hunter->name, evidence->evidenceLeftByGhost, hunter->currentRoom->name);
//         }

//         evidenceNode = evidenceNode->next;
//     }

//     sem_post(&hunter->currentRoom->evidenceSemaphore);
// }
