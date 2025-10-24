#include <stdio.h>

#include "list_manager.h"

int main()
{
    List_t main_list;

    ListInit(&main_list);
    ListDump(&main_list);
    for(int i = 0; i < 15; i++)
    {
        ListAddAfter(&main_list, i, 1.0/(i+1)); 
    }
    ListDestroy(&main_list);

    return 0;
}