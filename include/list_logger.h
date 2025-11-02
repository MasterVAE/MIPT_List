#ifndef LIST_LOGGER_H_
#define LIST_LOGGER_H_

#include "list_manager.h"

ListErr ListDump(List_t* list);

ListErr StartBaseHTML(FILE* file);
ListErr EndBaseHTML(FILE* file);

#define DEFAULT_COLOR "darkgreen"
#define NEXT_COLOR "red"
#define PREV_COLOR "blue"
#define FREE_COLOR "pink"

#endif // LIST_LOGGER_H_