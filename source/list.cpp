#include <stdio.h>      
#include <stdlib.h>
#include <string.h>                              

#include "../include/list_manager.h"
#include "../include/list_logger.h"

#define CHECK_MAIN(errcode) \
if(errcode != LIST_CORRECT)\
{\
    PrintError(errcode);\
    ListDestroy(main_list);\
    return errcode;\
}

int main()
{
    // INIT
    List_t* main_list = (List_t*)calloc(1, sizeof(List_t));
    
    ListErr err = ListInit(main_list);
    CHECK_MAIN(err);

    // PROCESS
    for(int i = 0; i < 10; i++)
    {
        err = ListAddAfter(main_list, i, 1.0/(i+1)); 
        CHECK_MAIN(err);
    }

    err = ListAddAfter(main_list, 3, 666); 
    CHECK_MAIN(err);

    err = ListDel(main_list, 4);
    CHECK_MAIN(err);

    err = ListDel(main_list, 11);
    CHECK_MAIN(err);
    
    err = ListDel(main_list, 4);
    CHECK_MAIN(err);

    // BREAK LIST
    // char* adress = (char*)main_list;
    // printf("ADRESS: %p\n", adress);
    // memset(adress + 3, 8, 8);

    VERIFY(main_list);

    ListDump(main_list);
    // DECONSTRUCT
    ListDestroy(main_list);

    return 0;
}