#ifndef LIST_LOGGER_H_
#define LIST_LOGGER_H_

#include "list_manager.h"

ListErr ListDump(List_t* list);

ListErr StartBaseHTML(FILE* file);
ListErr EndBaseHTML(FILE* file);

#endif // LIST_LOGGER_H_