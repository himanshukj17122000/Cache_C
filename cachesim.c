#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//method to get the number of bits
unsigned int log2(int n)
{
    int r = 0;
    while (n >>= 1)
        r++;
    return r;
}
// creates a masking
int ones(int n)
{
    return (1 << n) - 1;
}
//gets the last bits for manipulations
int bottomN(int search, int n)
{
    int onnestring = ones(n);
    return search & onnestring;
}
//the structure of every frame in the cache
typedef struct
{
    int valid;
    int address;
    int tag;
    unsigned char data[512];
    int dirty;
} Block;
//all the global variables have been declared below
int sizeofCache;
int ways;
int sizeofBlock;
int numberofFrames;
int numberofSets;
int sizeofBlockOffset;
int sizeofSetIndex;
int sizeofTag;
Block **cache;
unsigned char *Memory;
//calculates all the terms required for building the cache
void calculations()
{
    numberofFrames = sizeofCache / sizeofBlock;
    numberofSets = numberofFrames / ways;
    sizeofBlockOffset = log2(sizeofBlock);
    sizeofSetIndex = log2(numberofSets);
    sizeofTag = 24 - sizeofSetIndex - sizeofBlockOffset;
}
//builds and initialises the cache and the main memory
void buildingthecache()
{
    cache = (Block **)malloc(sizeof(Block *) * numberofSets);
    Memory = (unsigned char *)malloc(16 * 1024 * 1024);

    for (int i = 0; i < numberofSets; i++)
    {

        cache[i] = (Block *)malloc(sizeof(Block) * ways);
        for (int j = 0; j < ways; j++)
        {
            cache[i][j].valid = -1;
            cache[i][j].dirty = 0;
            cache[i][j].tag = -1;
        }
    }

    for (int i = 0; i < 16 * 1024 * 1024; i++)
    {
        Memory[i] = 0;
    }
}

int shifter(int a, int b)
{
    return a >> b;
}

int main(int argc, char *argv[])
{

    FILE *testFile = fopen(argv[1], "r");

    sizeofCache = atoi(argv[2]) * 1024;
    ways = atoi(argv[3]);

    sizeofBlock = atoi(argv[4]);
    calculations();
    buildingthecache();
    char ins[24];

    while (fscanf(testFile, "%s\n", ins) != EOF)
    {
        if (strcmp(ins, "store") == 0)
        {
            int memAdd, sizeofAccess;

            fscanf(testFile, "0x%x\n", &memAdd);
            fscanf(testFile, "%d\n", &sizeofAccess);

            int temp = memAdd;

            int block = bottomN(temp, sizeofBlockOffset);

            temp = shifter(temp, sizeofBlockOffset);

            int set = bottomN(temp, sizeofSetIndex);

            int tag = shifter(temp, sizeofSetIndex);

            int Position = -1;
            int hitschecked = -1;

            for (int i = 0; i < ways; i++)
            {
                if (cache[set][i].tag == tag)
                {
                    printf("%s 0x%x hit\n", ins, memAdd);
                    hitschecked = 1;
                    Position = i;
                    for (int j = Position; j > 0; j--)
                    {

                        cache[set][j] = cache[set][j - 1];
                    }
                    cache[set][0].tag = tag;
                    cache[set][0].address = memAdd;
                    // printf("%x", cache[set][0].address);
                    cache[set][0].valid = 1;
                    cache[set][0].dirty = 1;
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        fscanf(testFile, "%02hhx", &cache[set][0].data[block + j]);
                    }
                }
            }
            if (hitschecked != 1)
            {

                if (ways != 1)
                {
                    int b = ways - 1;
                    if (cache[set][b].dirty == 1)
                    {
                        for (int k = 0; k < strlen((char *)cache[set][b].data); k++)
                        {

                            int c = cache[set][b].address + k;

                            Memory[c] = cache[set][b].data[k];
                        }
                        cache[set][b].dirty = 0;
                    }
                    for (int j = (ways - 1); j > 0; j--)
                    {
                        cache[set][j] = cache[set][j - 1];
                    }

                    cache[set][0].tag = tag;
                    cache[set][0].address = memAdd;
                    cache[set][0].valid = 1;
                    cache[set][0].dirty = 1;
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        fscanf(testFile, "%02hhx", &cache[set][0].data[block + j]);
                    }
                    printf("%s 0x%x miss\n", ins, memAdd);
                }
                else if (ways == 1)
                {
                    int s = 0;
                    if (cache[set][s].dirty == 1)
                    {

                        for (int k = 0; k < strlen((char *)cache[set][s].data); k++)
                        {

                            int c = cache[set][s].address + k;

                            Memory[c] = cache[set][s].data[k];
                        }
                        cache[set][s].dirty = 0;
                    }
                    cache[set][0].tag = tag;
                    cache[set][0].address = memAdd;
                    cache[set][0].valid = 1;
                    cache[set][0].dirty = 1;
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        fscanf(testFile, "%02hhx", &cache[set][0].data[block + j]);
                    }
                    printf("%s 0x%x miss\n", ins, memAdd);
                }
            }
        }

        if (strcmp(ins, "load") == 0)
        {
            int memAdd, sizeofAccess;

            fscanf(testFile, "0x%x\n", &memAdd);
            fscanf(testFile, "%d\n", &sizeofAccess);

            int temp = memAdd;

            int block = bottomN(temp, sizeofBlockOffset);

            temp = shifter(temp, sizeofBlockOffset);

            int set = bottomN(temp, sizeofSetIndex);

            int tag = shifter(temp, sizeofSetIndex);

            int Position = -1;
            int hitschecked = -1;
            for (int i = 0; i < ways; i++)
            {
                if (cache[set][i].tag == tag)
                {
                    hitschecked = 1;
                    Position = i;
                    unsigned char buffer[512];
                    strcpy((char *)buffer, (char *)cache[set][i].data);
                    for (int j = Position; j > 0; j--)
                    {
                        if (j == Position && cache[set][j].dirty == 1)
                        {
                            for (int k = 0; k < strlen((char *)cache[set][j].data); k++)
                            {
                                int d = cache[set][j].address + k;
                                Memory[d] = cache[set][j].data[k];
                            }
                        }
                        cache[set][j] = cache[set][j - 1];
                    }
                    cache[set][0].tag = tag;
                    cache[set][0].valid = 1;
                    cache[set][0].address = memAdd;
                    strcpy((char *)cache[set][0].data, (char *)buffer);
                    printf("%s 0x%x hit ", ins, memAdd);
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        printf("%02hhx", cache[set][0].data[block + j]);
                    }
                    printf("\n");
                }
            }
            if (hitschecked != 1)
            {
                if (ways != 1)
                {
                    for (int j = (ways - 1); j > 0; j--)
                    {
                        if ((j == (ways - 1)) && (cache[set][j].dirty == 1))
                        {
                            for (int k = 0; k < strlen((char *)cache[set][j].data); k++)
                            {

                                int e = cache[set][j].address + k;
                                // printf("%x \n", Memory[e] );
                                // printf("%x \n", e );
                                //printf("%x \n", e);
                                // printf("%x \n", cache[set][j].data[k]);

                                //printf("%x", Memory[e]);
                                //printf("%x", cache[set][j].data[k]);
                                Memory[e] = cache[set][j].data[k];
                                //printf("%x", Memory[e]);
                            }
                        }
                        cache[set][j] = cache[set][j - 1];
                    }
                    cache[set][0].tag = tag;
                    cache[set][0].valid = 1;
                    cache[set][0].address = memAdd;
                    for (int k = 0; k < sizeofAccess; k++)
                    {
                        int f = block + k;
                        int g = memAdd + k;
                        //printf("%x \n", g);
                        cache[set][0].data[f] = Memory[g];
                    }
                    printf("%s 0x%x miss ", ins, memAdd);
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        int h = block + j;
                        printf("%02hhx", cache[set][0].data[h]);
                        //printf("%x", cache[set][0].data[h]);
                    }
                    printf("\n");
                }
                else if (ways == 1)
                {
                    int j = 0;
                    if ((cache[set][j].dirty == 1))
                    {
                        for (int k = 0; k < strlen((char *)cache[set][j].data); k++)
                        {

                            int e = cache[set][j].address + k;
                            // printf("%x \n", Memory[e] );
                            // printf("%x \n", e );
                            //printf("%x \n", e);
                            // printf("%x \n", cache[set][j].data[k]);

                            //printf("%x", Memory[e]);
                            //printf("%x", cache[set][j].data[k]);
                            Memory[e] = cache[set][j].data[k];
                            //printf("%x", Memory[e]);
                        }
                    }
                    cache[set][0].tag = tag;
                    cache[set][0].valid = 1;
                    cache[set][0].address = memAdd;
                    for (int k = 0; k < sizeofAccess; k++)
                    {
                        int f = block + k;
                        int g = memAdd + k;
                        //printf("%x \n", g);
                        cache[set][0].data[f] = Memory[g];
                    }
                    printf("%s 0x%x miss ", ins, memAdd);
                    for (int j = 0; j < sizeofAccess; j++)
                    {
                        int h = block + j;
                        printf("%02hhx", cache[set][0].data[h]);
                        //printf("%x", cache[set][0].data[h]);
                    }
                    printf("\n");
                }
            }
        }
    }
    fclose(testFile);
    free(Memory);
    free(cache);
    return EXIT_SUCCESS;
}