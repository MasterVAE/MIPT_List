#ifndef LIST_LOGGER_H_
#define LIST_LOGGER_H_

#include "list_manager.h"

ListErr ListDump(List_t* list);
ListErr CreateBaseHTML(FILE* file);

#endif // LIST_LOGGER_H_