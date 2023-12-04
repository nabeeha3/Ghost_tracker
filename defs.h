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

typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

// Forward declarations
struct Room;
struct RoomNode;
struct Evidence;
struct EvidenceNode;
struct Hunter;
struct Ghost;
struct HouseType;

// Definitions of your structures

typedef struct RoomNode {
    struct Room* room;
    struct RoomNode* next;
} RoomNode;

typedef struct Room {
    char name[MAX_STR];
    struct RoomNode* connectedRooms;
    struct EvidenceNode* evidenceCollection;
    struct Hunter* huntersInRoom;
    struct Ghost* ghostInRoom;
    sem_t roomSemaphore;
    sem_t evidenceSemaphore;
} Room;

typedef struct EvidenceNode {
    struct Evidence* evidence;
    struct EvidenceNode* next;
} EvidenceNode;

typedef struct Evidence {
    EvidenceType evidenceLeftByGhost;
    sem_t evidenceSemaphore;
} Evidence;

typedef struct Hunter {
    struct Room* currentRoom;
    EvidenceType equipmentType;
    char name[MAX_STR];
    struct Evidence* sharedEvidenceCollection;
    int fear;
    int boredom;
    pthread_t threadId;
    sem_t hunterSemaphore;
    struct Hunter* next;  // Pointer to the next hunter in the list
} Hunter;

typedef struct Ghost {
    GhostClass ghostType;
    Room* currentRoom;
    int boredomTimer;
    pthread_t threadId;
    sem_t ghostSemaphore;
} Ghost;

typedef struct HouseType {
    RoomNode* rooms;
    struct Room* roomsInHouse;  // Changed from "Room* roomsInHouse" to "struct Room* roomsInHouse"
    struct Hunter* huntersInHouse;  // Changed from "Hunter* huntersInHouse" to "struct Hunter* huntersInHouse"
    struct Evidence* sharedEvidence;
    Ghost* ghost;
    pthread_t hunterThreads[NUM_HUNTERS];
    sem_t evidenceSemaphore;
} HouseType;


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

// House.c functions
void initHouse(struct HouseType* house);
struct Room* createRoom(const char* name);
void connectRooms(struct Room* room1, struct Room* room2);
void addRoom(struct RoomNode** roomList, struct Room* room);
void populateRooms(struct HouseType* house);

//Hunter.c functions
void placeHuntersInVan(struct HouseType* house, char hunterNames[NUM_HUNTERS][MAX_STR]);
enum EvidenceType randomEquipmentType();
void* hunterThread(void* arg);
void performHunterAction(Hunter* hunter, HouseType* house);
void moveHunterToRandomRoom(Hunter* hunter, HouseType* house);
void collectEvidence(Hunter* hunter, HouseType* house);
void reviewEvidence(Hunter* hunter, HouseType* house);
void exitHunterThread(Hunter* hunter, HouseType* house, enum LoggerDetails reason);
void* hunterThread(void* arg);

//Ghost.c functions
void placeGhostRandomly(HouseType* house);
struct Room* getRandomRoom(RoomNode* roomList);
void* ghostThread(void* arg);