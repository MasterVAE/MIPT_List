#include <stdio.h>                                    

#include "list_manager.h"
#include "list_logger.h"

#define CHECK_MAIN(errcode) if(errcode != LIST_CORRECT)\
                        {\
                            PrintError(errcode);\
                            ListDestroy(&main_list);\
                            return errcode;\
                        }

int main()
{
    // INIT
    List_t main_list;
    
    ListErr err = ListInit(&main_list);
    CHECK_MAIN(err);

    err = ListDump(&main_list);
    CHECK_MAIN(err);

    // PROCESS
    for(int i = 0; i < 10; i++)
    {
        err = ListAddAfter(&main_list, i, 1.0/(i+1)); 
        CHECK_MAIN(err);
    }

    err = ListAddAfter(&main_list, 3, 666); 
    CHECK_MAIN(err);

    err = ListDel(&main_list, 4);
    CHECK_MAIN(err);

    err = ListDel(&main_list, 11);
    CHECK_MAIN(err);

    // DECONSTRUCT
    ListDestroy(&main_list);

    return 0;
}