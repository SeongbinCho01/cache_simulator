#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void parsing(char buf[], char *tokens[]) {
    char *temp = strtok(buf, " \n");

    for(int i = 0; i < 3; i++) {
        tokens[i] = temp;
        temp = strtok(NULL, " \n");
    }
}

void ordering(char **arr, int height, int width, int index) {
    int temp = 0;
    if(arr[height][index] == NULL) {
        temp = width;
    }
    else {
        temp = arr[height][index];
    }
    int i = 0;
    while(i < width) {
        if((arr[height][i] != NULL) && (arr[height][i] < temp)) {
            arr[height][i]++;
        }
        i++;
    }
    arr[height][index] = 1;
}

int findIndex(char **arr, int height, int width) {
    for(int i = 0; i < width; i++) {
        if(arr[height][i] == width) {
            ordering(arr, height, width, i);

            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]) {

    if(argc != 8) {
        printf("인자의 개수가 알맞지 않음");
        return 0;
    }

    int Total_Loads = 0, Total_Stores = 0, Load_Hits = 0, Load_Misses = 0, Store_Hits = 0, Store_Misses = 0, Total_Cycles = 0;
    char buf[1024];
    char *tokens[3];

    int number_of_sets = atoi(argv[1]);
    int associativity = atoi(argv[2]);
    int size_of_block = atoi(argv[3]);

    int how = 0;

    int **arrayForOrder_Load = (int **)malloc(sizeof(int *) * number_of_sets);
    arrayForOrder_Load[0] = (int *)malloc(sizeof(int) * associativity * number_of_sets);
    for(int i = 1; i < number_of_sets; i++) {
        arrayForOrder_Load[i] = arrayForOrder_Load[i - 1] + associativity;
    }

    int **arrayForOrder_Store = (int **)malloc(sizeof(int *) * number_of_sets);
    arrayForOrder_Store[0] = (int *)malloc(sizeof(int) * associativity * number_of_sets);
    for(int i = 1; i < number_of_sets; i++) {
        arrayForOrder_Store[i] = arrayForOrder_Store[i - 1] + associativity;
    }

    for(int i = 0; i < number_of_sets; i++) {
        for(int j = 0; j < associativity; j++) {
            arrayForOrder_Load[i][j] = NULL;
            arrayForOrder_Store[i][j] = NULL;
        }
    }

    if(!strcmp(argv[6], "lru")) {
        how = 1;
    }
    else if(!strcmp(argv[6], "fifo")) {
        how = 0;
    }
    else if(!strcmp(argv[6], "random")) {
        how = 2;
    }

    if(number_of_sets == 1) {
        int way = 0;
    }
    else if(associativity == 1) {
        int way = 1;
    }
    else {
        int way = -1;
    }

    int number_of_blocks = number_of_sets * associativity;

    int **arrayOfCacheBlock_load = (int **)malloc(sizeof(int *) * number_of_sets);
    arrayOfCacheBlock_load[0] = (int *)malloc(sizeof(int) * associativity * number_of_sets);
    for(int i = 1; i < number_of_sets; i++) {
        arrayOfCacheBlock_load[i] = arrayOfCacheBlock_load[i - 1] + associativity;
    }
    int **arrayOfCacheBlock_store = (int **)malloc(sizeof(int *) * number_of_sets);
    arrayOfCacheBlock_store[0] = (int *)malloc(sizeof(int) * associativity * number_of_sets);
    for(int i = 1; i < number_of_sets; i++) {
        arrayOfCacheBlock_store[i] = arrayOfCacheBlock_store[i - 1] + associativity;
    }

    for(int i = 0; i < number_of_sets; i++) {
        for(int j = 0; j < associativity; j++) {
            arrayOfCacheBlock_load[i][j] = NULL;
            arrayOfCacheBlock_store[i][j] = NULL;
        }
    }

    FILE *fp = fopen(argv[7], "r");

    while(fgets(buf, sizeof(buf), fp)) {
        parsing(buf, tokens);

        int address = strtol(tokens[1], NULL, 16);

        int set = address % number_of_sets;

        if(tokens[0][0] == 'l') {
            Total_Loads++;

            int isMiss = 0;

            if(how == 1) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_load[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        ordering(arrayForOrder_Load, set, associativity, i);
                        break;
                    }
                    else if(arrayOfCacheBlock_load[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_load[set][i] = address;
                        ordering(arrayForOrder_Load, set, associativity, i);
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    int target = findIndex(arrayForOrder_Load, set, associativity);
                    arrayOfCacheBlock_load[set][target] = address;
                }

                if(isMiss)
                    Load_Misses++;
                else
                    Load_Hits++;
            }
            else if(how == 2) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_load[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        break;
                    }
                    else if(arrayOfCacheBlock_load[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_load[set][i] = address;
                        ordering(arrayForOrder_Load, set, associativity, i);
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    int target = findIndex(arrayForOrder_Load, set, associativity);
                    arrayOfCacheBlock_load[set][target] = address;
                }

                if(isMiss)
                    Load_Misses++;
                else
                    Load_Hits++;
            }
            else if(how == 0) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_load[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        break;
                    }
                    else if(arrayOfCacheBlock_load[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_load[set][i] = address;
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    srand(time(NULL));
                    int target = rand() % associativity;
                    arrayOfCacheBlock_load[set][target] = address;
                }

                if(isMiss)
                    Load_Misses++;
                else
                    Load_Hits++;
            }
        }
        else if (tokens[0][0] == 's')
        {
            Total_Stores++;

            int isMiss = 0;

            if(how == 1) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_store[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        ordering(arrayForOrder_Store, set, associativity, i);
                        break;
                    }
                    else if(arrayOfCacheBlock_store[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_store[set][i] = address;
                        ordering(arrayForOrder_Store, set, associativity, i);
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    int target = findIndex(arrayForOrder_Store, set, associativity);
                    arrayOfCacheBlock_store[set][target] = address;
                }

                if(isMiss)
                    Store_Misses++;
                else
                    Store_Hits++;
            }
            else if(how == 2) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_store[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        break;
                    }
                    else if(arrayOfCacheBlock_store[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_store[set][i] = address;
                        ordering(arrayForOrder_Store, set, associativity, i);
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    int target = findIndex(arrayForOrder_Store, set, associativity);
                    arrayOfCacheBlock_store[set][target] = address;
                }

                if(isMiss)
                    Store_Misses++;
                else
                    Store_Hits++;
            }
            else if(how == 0) {
                int fail = 1;
                int i = 0;
                while(i < associativity) {
                    if(arrayOfCacheBlock_store[set][i] == address) {
                        isMiss = 0;
                        fail = 0;
                        break;
                    }
                    else if(arrayOfCacheBlock_store[set][i] == NULL) {
                        isMiss = 1;
                        fail = 0;
                        arrayOfCacheBlock_store[set][i] = address;
                        break;
                    }
                    i++;
                }
                if(fail) {
                    isMiss = 1;
                    srand(time(NULL));
                    int target = rand() % associativity;
                    arrayOfCacheBlock_store[set][target] = address;
                }

                if(isMiss)
                    Store_Misses++;
                else
                    Store_Hits++;
            }
        }
    }
    Total_Cycles = Total_Loads + Total_Stores + 100 * (Load_Misses + Store_Misses) * size_of_block / 4;

    printf("Total loads: %d\nTotal stores: %d\nLoad hits: %d\nLoad misses: %d\nStore hits: %d\nStore misses: %d\nTotal cycles: %d\n", Total_Loads, Total_Stores, Load_Hits, Load_Misses, Store_Hits, Store_Misses, Total_Cycles);
    fclose(fp);
    return 0;
}
