#include "defs.h"

/*
    Purpose: Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
  Params:
    out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    struct Room* van                = createRoom("Van");
    struct Room* hallway            = createRoom("Hallway");
    struct Room* master_bedroom     = createRoom("Master Bedroom");
    struct Room* boys_bedroom       = createRoom("Boy's Bedroom");
    struct Room* bathroom           = createRoom("Bathroom");
    struct Room* basement           = createRoom("Basement");
    struct Room* basement_hallway   = createRoom("Basement Hallway");
    struct Room* right_storage_room = createRoom("Right Storage Room");
    struct Room* left_storage_room  = createRoom("Left Storage Room");
    struct Room* kitchen            = createRoom("Kitchen");
    struct Room* living_room        = createRoom("Living Room");
    struct Room* garage             = createRoom("Garage");
    struct Room* utility_room       = createRoom("Utility Room");

    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}

/*
    Purpose: Initialize the house.
  Params:
    out: house - pointer to the HouseType structure to be initialized
*/
void initHouse(struct HouseType* house) {
    house->rooms = NULL;
    house->sharedEvidence = NULL;
}

/*
    Purpose: Create a room with the given name.
  Params:
    in: name - the name of the room
  Returns:
    out: Room* - pointer to the created room
*/
struct Room* createRoom(const char* name) {
    Room* newRoom = (struct Room*)malloc(sizeof(struct Room));

    strcpy(newRoom->name, name);
    newRoom->connectedRooms = NULL;
    newRoom->evidenceCollection = NULL;
    newRoom->ghostInRoom = NULL;
    sem_init(&newRoom->roomSemaphore, 0, 1);
    sem_init(&newRoom->evidenceSemaphore, 0, 1);

    return newRoom;
}

/*
    Purpose: Connect two rooms by creating two-way connections between them.
  Params:
    in: room1 - pointer to the first room to be connected
    in: room2 - pointer to the second room to be connected
*/
void connectRooms(struct Room* room1, struct Room* room2) {
    RoomNode* node1 = (RoomNode*)malloc(sizeof(RoomNode));
    RoomNode* node2 = (RoomNode*)malloc(sizeof(RoomNode));

    if (node1 != NULL && node2 != NULL) {
        node1->room = room2;
        node1->next = room1->connectedRooms;
        room1->connectedRooms = node1;

        node2->room = room1;
        node2->next = room2->connectedRooms;
        room2->connectedRooms = node2;
    }
}

/*
    Purpose: Add a room to the house's room list.
  Params:
    in: firstRoom - pointer to the pointer of the first room in the list
    in: newRoom - pointer to the room to be added
*/
void addRoom(struct RoomNode** firstRoom, struct Room* newRoom) {
    RoomNode* newNode = (RoomNode*)malloc(sizeof(RoomNode));

    newNode->room = newRoom;
    newNode->next = NULL;

    if (*firstRoom == NULL) {
        *firstRoom = newNode;
    } else {
        newNode->next = (*firstRoom)->next;
        (*firstRoom)->next = newNode;
    }
}
