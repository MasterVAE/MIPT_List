#ifndef LIST_MANAGER_H_
#define LIST_MANAGER_H_

enum ListErr
{
    LIST_CORRECT,
    LIST_NULL,
    LIST_FILE_ERROR,
    LIST_INCORRECT_INDEX,
    LIST_MEMORY_ERROR,
    LIST_INVALID_HEAD,
    LIST_INVALID_TAIL,
    LIST_INVALID_EMPTY,
    LIST_SHEILD_DAMAGED
};

typedef struct
{
    double value;
    int next;
    int previous;
} ListElement_t;

typedef struct
{
    size_t list_capacity;
    ListElement_t* elements;

    int head;
    int tail;
    int first_empty;

    FILE* log_file;
} List_t;

ListErr ListInit        (List_t* list);
void    ListDestroy     (List_t* list);
ListErr ListAddAfter    (List_t* list, int index, double value);
ListErr ListDel         (List_t* list, int index);
int     ListNext        (List_t* list, int index);
int     ListPrev        (List_t* list, int index);
ListErr ListVerify      (List_t* list);
void PrintError(ListErr errcode);

const size_t LIST_START_CAPACITY = 5;
const size_t LIST_MULTIPLIER_CAPACITY = 2;

const double SHIELD_VALUE = 0xF0F0F0F0;
const double POISON = 0xBADF00D;

#define CHECK(errcode) if(errcode != LIST_CORRECT)\
                        {\
                            PrintError(errcode);\
                            return errcode;\
                        }
#define VERIFY(list) {\
                        ListErr errr = ListVerify(list);\
                        if(errr != LIST_CORRECT)\
                        {\
                            PrintError(errr);\
                            ListDump(list);\
                            return errr;\
                        }\
                    }

#endif // LIST_MANAGER_H_