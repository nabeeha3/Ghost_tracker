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


typedef enum EvidenceType EvidenceType;
typedef enum GhostClass GhostClass;

enum EvidenceType { EMF, TEMPERATURE, FINGERPRINTS, SOUND, EV_COUNT, EV_UNKNOWN };
enum GhostClass { POLTERGEIST, BANSHEE, BULLIES, PHANTOM, GHOST_COUNT, GH_UNKNOWN };
enum LoggerDetails { LOG_FEAR, LOG_BORED, LOG_EVIDENCE, LOG_SUFFICIENT, LOG_INSUFFICIENT, LOG_UNKNOWN };

//make structures

typedef struct 
{
    Evidence       evidence;
    EvidenceNode*  next;
} EvidenceNode;

typedef struct
{
    Room*      room;
    RoomNode*  next;
} RoomNode;

typedef struct 
{
    char             name[MAX_STR];
    RoomNode*        connectedRooms;  
    Evidence*        evidenceCollection;    
    Hunter*          huntersInRoom;
    Ghost*           ghostInRoom; 
    sem_t            roomSemaphore;  
    sem_t            evidenceSemaphore; 
} Room;

typedef struct 
{
    GhostClass       ghostType;
    Room*            currentRoom;
    int              boredomTimer;
    pthread_t        threadId;
    sem_t            ghostSemaphore;  
} Ghost;

typedef struct 
{
    RoomNode*         rooms;
    Hunter*           huntersInHouse; 
    Evidence*         sharedEvidence;
    pthread_t         hunterThreads[NUM_HUNTERS]; 
} HouseType;

typedef struct 
{
    Room*            currentRoom;
    EvidenceType     equipmentType;
    char             name[MAX_STR];
    Evidence*        sharedEvidenceCollection;
    int              fear;
    int              boredom;
    pthread_t        threadId;  
    sem_t            hunterSemaphore;  
} Hunter;

typedef struct 
{
    EvidenceNode*  evidenceToCollection;
    EvidenceType   evidenceLeftByGhost;
    sem_t          evidenceSemaphore;   
} Evidence;


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
RoomNode* createRoomListExcluding(RoomNode* rooms, Room* excludedRoom);
Room* assignGhostRoom(RoomNode* rooms);
Room* getRoomByName(RoomNode* rooms, const char* targetName);
int isGhostInSameRoomAsHunter(Ghost* ghost, Hunter* hunters[NUM_HUNTERS]);
Evidence* getGhostEvidence(Ghost* ghost);
void addEvidenceToRoom(Room* room, Evidence* evidenceToAdd);
Room* getRandomConnectedRoom(Room* room);
void* runGhostThread(Ghost* a, Hunter* b[NUM_HUNTERS], HouseType* c);

