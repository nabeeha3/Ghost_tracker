#include "defs.h"

/*
    Purpose: Check and print the emotion levels of all hunters.
  Params:
    in: hunters - array of pointers to Hunter structures
*/
void huntersEmotionLevel(Hunter* hunters[])
{
    for (int i=0; i<NUM_HUNTERS; i++)
    {
        if (hunters[i]->fear >= FEAR_MAX)
        {
            printf("%s reached max fear!", hunters[i]->name);
        }
        else if (hunters[i]->boredom >= BOREDOM_MAX)
        {
            printf("%s reached max boredom!", hunters[i]->name);
        }
    }
}

/*
    Purpose: Check if the ghost has won by causing all hunters to reach max boredom or max fear.
  Params:
    in: hunters - array of pointers to Hunter structures
*/

void didGhostWin(Hunter* hunters[])
{
    int numBoredHunters = 0;
    int numScaredHunters = 0;

    for (int i=0; i<NUM_HUNTERS; i++)
    {
        if (hunters[i]->boredom >= BOREDOM_MAX)
        {
            numBoredHunters++;
        }
        if (hunters[i]->fear >= FEAR_MAX)
        {
            numScaredHunters++;
        }
    }
    if (numBoredHunters == NUM_HUNTERS || numScaredHunters == NUM_HUNTERS)
    {
        printf("The Ghost has won!");
    }
}



