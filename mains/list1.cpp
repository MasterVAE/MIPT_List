#include <stdio.h>      
#include <stdlib.h>
#include <string.h>                              

#include "../include/list_manager.h"
#include "../include/list_logger.h"

int main()
{
    List_t* main_list = (List_t*)calloc(1, sizeof(List_t));
    CHECK_MAIN(ListInit(main_list));

    for(size_t i = 0; i < 10; i++)
    {
        CHECK_MAIN(ListAddAfter(main_list, i, 1.0/((double)i+1)));
    }

    CHECK_MAIN(ListAddAfter(main_list, 3, 666));
    CHECK_MAIN(ListDel(main_list, 4));
    CHECK_MAIN(ListDel(main_list, 11));
    CHECK_MAIN(ListDel(main_list, 4));
    
    main_list->elements[3].previous = -7;

    CHECK_MAIN(ListVerify(main_list));  
    
    ListDestroy(main_list);

    return 0;
}