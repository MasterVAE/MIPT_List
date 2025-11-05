#ifndef LIST_LOGGER_H_
#define LIST_LOGGER_H_

#include "list_manager.h"

ListErr ListDump(const List_t* list);

ListErr StartBaseHTML(FILE* file);
ListErr EndBaseHTML(FILE* file);

static const char* const DEFAULT_COLOR = "darkgreen";
static const char* const NEXT_COLOR = "red";
static const char* const PREV_COLOR = "blue";
static const char* const FREE_COLOR = "pink";

#endif // LIST_LOGGER_H_