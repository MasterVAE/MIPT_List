#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "list_manager.h"
#include "list_logger.h"

#define WRITE(string) fwrite(string, strlen(string), 1, file);
#define WRITE_S() fwrite(string, strlen(string), 1, file);

static ListErr ConsoleDump(List_t* list);
static ListErr HTMLDump(List_t* list, FILE* file);
static ListErr WriteGraph(List_t* list, FILE* file);
static ListErr WriteGraphNodes(List_t* list, FILE* file);
static ListErr WriteGraphArrows(List_t* list, FILE* file);

const char* dot_file_name = "files/graph.dot";
const int STRING_BUFFER_SIZE = 200;

int current_dump = 1;

// ВЫВОД СПИСКА
ListErr ListDump(List_t* list)
{
    if(!list) return LIST_NULL;

    ListErr err = ConsoleDump(list);
    CHECK(err);

    err = HTMLDump(list, list->log_file);
    CHECK(err);

    current_dump++;

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В КОНСОЛЬ
static ListErr ConsoleDump(List_t* list)
{
    if(!list) return LIST_NULL;

    printf("========== LIST DUMP START ============\n\n");

    printf("HEAD:      %4d\n",   list->head);
    printf("TAIL:      %4d\n",   list->tail);
    printf("LIST SIZE: %4lu\n",  list->list_capacity);
    printf("NEXT EMPTY:%4d\n\n", list->first_empty);

    for(size_t element = 0; element < list->list_capacity; element++)
    {
        printf("[%2lu] VALUE:  %10.5f   NEXT: %3d   PREVIOUS:  %3d\n", 
                                                            element,
                                                            list->elements[element].value, 
                                                            list->elements[element].next,
                                                            list->elements[element].previous);
    }

    printf("PRINTING: \n");
    for(int i = list->head; i > 0; i = list->elements[i].next)
    {
        printf("%g|", list->elements[i].value);
    }

    printf("\n\n========== LIST DUMP END   ============\n\n");

    return LIST_CORRECT;
}

// ИНИЦИАЛИЗАЦИЯ HTML ФАЙЛА
ListErr StartBaseHTML(FILE* file)
{
    if(!file) return LIST_FILE_ERROR;
    
    WRITE(  "<!DOCTYPE html>\n"
            "<head>\n"
            "<title>List log</title>\n"
            "<link href=\"../html/main.css\" type=\"text/css\" rel=\"stylesheet\" />\n"
            "</head>\n"
            "<body class=\"main\">"
            "<hdr>LIST LOGGER</hdr>\n");

    return LIST_CORRECT;
}

// ОКОНЧАНИЕ HTML ФАЙЛА
ListErr EndBaseHTML(FILE* file)
{
    if(!file) return LIST_FILE_ERROR;
    
    WRITE(  "</body>");

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В HTML
static ListErr HTMLDump(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    char string[STRING_BUFFER_SIZE] = {};

    sprintf(string, "<h2>LOG №%d</h2>\n", current_dump);
    WRITE_S();

    sprintf(string, "<p>LIST CAPACITY: %4lu</p>\n",  list->list_capacity);
    WRITE_S();
    sprintf(string, "<p>NEXT EMPTY: %4d</p>\n\n", list->first_empty);
    WRITE_S();

    sprintf(string, "<ul>");
    WRITE_S();

    for(size_t element = 0; element < list->list_capacity; element++)
    {
        sprintf(string, "<li>[%2lu] VALUE: %10f      NEXT: %3d       PREVIOUS: %3d </li>\n", 
                                                            element,
                                                            list->elements[element].value, 
                                                            list->elements[element].next,
                                                            list->elements[element].previous);
        WRITE_S();
    }

    sprintf(string, "</ul>");
    WRITE_S();

    sprintf(string, "\n<p>PRINTING: </p>\n <p>");
    WRITE_S();
    for(int i = list->head; i > 0; i = list->elements[i].next)
    {
        sprintf(string, "%g -> ", list->elements[i].value);
        WRITE_S();
    }

    sprintf(string, "</p>\n\n");
    WRITE_S();

    FILE* dot_file = fopen(dot_file_name, "w+");
    if(!dot_file) return LIST_FILE_ERROR;

    WriteGraph(list, dot_file);
    fclose(dot_file);

    sprintf(string, "dot -Tpng %s -o files/log_%d.png", dot_file_name, current_dump);
    system(string);

    sprintf(string, "<h1> <img src=\"log_%d.png\" align=\"top\"/> </h1>", current_dump);
    WRITE_S();

    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ
static ListErr WriteGraph(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    char string[STRING_BUFFER_SIZE] = {};

    sprintf(string, "digraph\n"
                    "{\n"
                    "bgcolor=\"#111111\""
                    "rankdir=LR;\n"
                    "node[color=\"white\",fontsize=14, fillcolor=\"#111111\"" 
                                                     ",fontcolor=\"#F5DEB3\""
                                                     ",shape=\"rectangle\""
                                                     ",style=\"filled\"];\n"
                    "edge[color=\"darkgreen\",fontsize=15,penwidth=2];\n");
    WRITE_S();

    
    ListErr err = WriteGraphNodes(list, file);
    CHECK(err);

    err = WriteGraphArrows(list, file);
    CHECK(err);
    
    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ НОДОВ
static ListErr WriteGraphNodes(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    char string[STRING_BUFFER_SIZE] = {};

    if(list->elements[0].value == SHIELD_VALUE) 
    {
        sprintf(string, "ELEM_0["
                                "label=\" ELEMENT 0\n"
                                "value: SHIELD \n"
                                "head: %d \n"
                                "tail: %d\""
                                "]\n", 
                        list->head,
                        list->tail);
        WRITE_S();
    }
    else
    {
        sprintf(string, "ELEM_0["
                                "label=\" ELEMENT 0\n"
                                "value: %g \n"
                                "next: %d \n"
                                "previous: %d\""
                                "]\n", 
                        list->elements[0].value,
                        list->elements[0].next,
                        list->elements[0].previous);
        WRITE_S();
    }

    for(size_t element = 1; element < list->list_capacity; element++)
    {
        if(list->elements[element].value == POISON)
        {
            sprintf(string, "ELEM_%lu["
                                      "label=\" ELEMENT %lu\n"
                                      "value: POISON \n"
                                      "next: %d \n\""
                                      "]\n", 
                            element, 
                            element,
                            list->elements[element].next);
            WRITE_S();
        }
        else
        {
            sprintf(string, "ELEM_%lu["
                                      "label=\" ELEMENT %lu\n"
                                      "value: %g \n"
                                      "next: %d \n"
                                      "previous: %d\""
                                      "]\n", 
                            element, 
                            element, 
                            list->elements[element].value,
                            list->elements[element].next,
                            list->elements[element].previous);
            WRITE_S();
        }
    }
    
    sprintf(string, "HEAD[]\n"
                    "TAIL[]\n"
                    "FREE[]\n");
    WRITE_S();

    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ СТРЕЛОК
static ListErr WriteGraphArrows(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    char string[STRING_BUFFER_SIZE] = {};

    sprintf(string, "HEAD->ELEM_%d[headport=\"n\"]\n", list->head);
    WRITE_S();

    sprintf(string, "TAIL->ELEM_%d[headport=\"n\"]\n", list->tail);
    WRITE_S();

    sprintf(string, "FREE->ELEM_%d[headport=\"n\", color=\"pink\"]\n", list->first_empty);
    WRITE_S();

    for(size_t element = 0; element < list->list_capacity-1; element++)
    {
        sprintf(string, "ELEM_%lu->ELEM_%lu[color=\"white\"]\n", 
                                                        element, element+1);
        WRITE_S();
    }

    for(size_t element = 0; element < list->list_capacity; element++)
    {
        int next = list->elements[element].next;
        if(next >= 0)
        {
            sprintf(string, "ELEM_%lu->ELEM_%d[color=\"red\", tailport=\"n\", headport=\"n\"]\n",
                                                        element, next);
            WRITE_S();
        }
        else if(next < 0)
        {
            sprintf(string, "ELEM_%lu->ELEM_%d[color=\"pink\", tailport=\"n\", headport=\"n\"]\n",
                                                        element, -next);
            WRITE_S();
        }

        int prev = list->elements[element].previous;
        if(prev >= 0 && prev < (int)list->list_capacity)
        {

            sprintf(string, "ELEM_%lu->ELEM_%d[color=\"blue\", tailport=\"s\", headport=\"s\"]\n", 
                                                        element, abs(prev));
            WRITE_S();
        }
    }
    
    sprintf(string, "}");
    WRITE_S();

    return LIST_CORRECT;
}