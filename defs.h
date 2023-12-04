#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STR         64
#define MAX_RUNS        50
#define BOREDOM_MAX     100
#define C_TRUE          1
#define C_FALSE         0
#define HUNTER_WAIT     5000
#define GHOST_WAIT      600
#define NUM_HUNTERS     4
#define FEAR_MAX        10
#define LOGGING         C_TRUE
#define GHOST_EVIDENCE  3
#define MAX_EVIDENCE    10
#define NUM_ROOMS       13


typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

//make structures

typedef struct EvidenceNode EvidenceNode;
typedef struct RoomNode RoomNode;
typedef struct Room Room;
typedef struct Ghost Ghost;
typedef struct Hunter Hunter;
typedef struct HouseType HouseType;
typedef struct Evidence Evidence;
typedef struct GhostThreadArgs GhostThreadArgs;

struct Evidence {
    EvidenceNode* evidenceToCollection;
    enum EvidenceType evidenceLeftByGhost;
    sem_t evidenceSemaphore;
};

struct EvidenceNode {
    Evidence evidence;
    EvidenceNode* next;
};

struct RoomNode {
    Room* room;
    RoomNode* next;
};

struct GhostThreadArgs {
    Ghost* ghost;
    Hunter* hunters[NUM_HUNTERS];
    HouseType* house;
};

struct Room {
    char name[MAX_STR];
    RoomNode* connectedRooms;
    Evidence* evidenceCollection;
    Hunter* huntersInRoom;
    Ghost* ghostInRoom;
    sem_t roomSemaphore;
    sem_t evidenceSemaphore;
};

struct Ghost {
    GhostClass ghostType;
    Room* currentRoom;
    int boredomTimer;
    pthread_t threadId;
    sem_t ghostSemaphore;
};

struct Hunter {
    Room* currentRoom;
    EvidenceType equipmentType;
    char name[MAX_STR];
    Evidence* sharedEvidenceCollection;
    int fear;
    int boredom;
    pthread_t threadId;
    sem_t hunterSemaphore;
};

struct HouseType {
    RoomNode* rooms;
    Hunter* huntersInHouse;
    Evidence* sharedEvidence;
    pthread_t hunterThreads[NUM_HUNTERS];
};


// Helper Utilies
int randInt(int,int);        // Pseudo-random number generator function
float randFloat(float, float);  // Pseudo-random float generator function
enum GhostClass randomGhost();  // Return a randomly selected a ghost type
void ghostToString(enum GhostClass, char*); // Convert a ghost type to a string, stored in output paremeter
void evidenceToString(enum EvidenceType, char*); // Convert an evidence type to a string, stored in output parameter

// Logging Utilities
void l_hunterInit(char* name, enum EvidenceType equipment);
void l_hunterMove(char* name, char* room);
void l_hunterReview(char* name, enum LoggerDetails reviewResult);
void l_hunterCollect(char* name, enum EvidenceType evidence, char* room);
void l_hunterExit(char* name, enum LoggerDetails reason);
void l_ghostInit(enum GhostClass type, char* room);
void l_ghostMove(char* room);
void l_ghostEvidence(enum EvidenceType evidence, char* room);
void l_ghostExit(enum LoggerDetails reason);

//helper functions
// RoomNode* createRoomListExcluding(RoomNode* rooms, Room* excludedRoom);
// Room* assignGhostRoom(RoomNode* rooms);
// Room* getRoomByName(RoomNode* rooms, const char* targetName);
// int isGhostInSameRoomAsHunter(Ghost* ghost, Hunter* hunters[NUM_HUNTERS]);
// enum EvidenceType getGhostEvidence(Ghost* ghost);
// void addEvidenceToRoom(Room* room, enum EvidenceType evidenceToAdd);
// Room* getRandomConnectedRoom(Room* room);

// Ghost.c functions
void* ghostThread(void* arg);
void initGhost(Ghost* ghost, HouseType* house);
Room* placeGhostRandomly(HouseType* house);
Room* getRandomRoom(RoomNode* roomList);

// Hunter.c functions
void* hunterThread(void* arg);
void initHunter(Hunter* newHunter, char* name, HouseType* house);
enum EvidenceType randomEquipmentType();

// House.c functions
void initHouse(struct HouseType* house);
struct Room* createRoom(const char* name);
void connectRooms(struct Room* room1, struct Room* room2);
void addRoom(struct RoomNode** roomList, struct Room* room);
void populateRooms(HouseType* house);