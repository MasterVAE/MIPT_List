#include <stdio.h>

#include "list_manager.h"
#include "list_logger.h"

int main()
{
    // INIT
    List_t main_list;
    
    ListErr err = ListInit(&main_list);
    CHECK(err);

    err = ListDump(&main_list);
    CHECK(err);

    // PROCESS
    for(int i = 0; i < 10; i++)
    {
        err = ListAddAfter(&main_list, i, 1.0/(i+1)); 
        CHECK(err);
    }

    err = ListAddAfter(&main_list, 3, 666); 
    CHECK(err);

    err = ListDel(&main_list, 4);
    CHECK(err);

    err = ListDel(&main_list, 11);
    CHECK(err);

    // DECONSTRUCT
    ListDestroy(&main_list);

    return 0;
}