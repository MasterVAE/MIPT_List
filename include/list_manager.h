#ifndef LIST_MANAGER_H_
#define LIST_MANAGER_H_

typedef double list_type;

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
    LIST_SHEILD_DAMAGED,
    LIST_INVALID_SIZE
};

enum ReallocType
{
    REALLOC_UP,
    REALLOC_DOWN
};

typedef struct
{
    list_type value;
    int next;
    int previous;
} ListElement_t;

typedef struct
{
    size_t list_capacity;
    size_t list_size;
    ListElement_t* elements;

    int first_empty;

    FILE* log_file;
} List_t;

size_t      ListHead        (List_t* list);
size_t      ListTail        (List_t* list);
ListErr     ListAddFront    (List_t* list, list_type value);
ListErr     ListAddBack     (List_t* list, list_type value);
list_type   ListGetOnIndex  (List_t* list, int index);
ListErr     ListSetOnIndex  (List_t* list, int index, list_type value);

ListErr     ListInit        (List_t* list);
void        ListDestroy     (List_t* list);
ListErr     ListAddAfter    (List_t* list, int index, list_type value);
ListErr     ListAddBefore   (List_t* list, int index, list_type value);
ListErr     ListDel         (List_t* list, int index);
size_t      ListNext        (List_t* list, int index);
size_t      ListPrev        (List_t* list, int index);
ListErr     ListVerify      (List_t* list);
void PrintError(ListErr errcode);
int ValueEquality(list_type a, list_type b);

const size_t LIST_START_CAPACITY = 5;
const size_t LIST_MULTIPLIER_CAPACITY = 2;

const list_type SHIELD_VALUE = 0xF0F0F0F0;
const list_type POISON = 0xBADF00D;

#define CHECK(errcode)          \
if(errcode != LIST_CORRECT)     \
{                               \
    PrintError(errcode);        \
    return errcode;             \
}
#define VERIFY(list)                    \
{                                       \
    ListErr errr = ListVerify(list);    \
    if(errr != LIST_CORRECT)            \
    {                                   \
        PrintError(errr);               \
        ListDump(list);                 \
        return errr;                    \
    }                                   \
}

#endif // LIST_MANAGER_H_