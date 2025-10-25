#include <stdio.h>

#include "list_manager.h"
#include "list_logger.h"

int main()
{
    // INIT
    List_t main_list;
    
    ListInit(&main_list);
    ListDump(&main_list);

    // PROCESS
    for(int i = 0; i < 10; i++)
    {
        ListAddAfter(&main_list, i, 1.0/(i+1)); 
    }

    ListAddAfter(&main_list, 3, 666); 

    ListDel(&main_list, 4);
    ListDel(&main_list, 11);

    // DECONSTRUCT
    ListDestroy(&main_list);

    return 0;
}