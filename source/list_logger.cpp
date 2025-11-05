#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../include/list_manager.h"
#include "../include/list_logger.h"

static ListErr ConsoleDump      (const List_t* list);
static ListErr HTMLDump         (const List_t* list, FILE* file);
static ListErr HTMLWriteInfo    (const List_t* list, FILE* file);
static ListErr WriteGraph       (const List_t* list, FILE* file);
static ListErr WriteGraphNodes  (const List_t* list, FILE* file);
static ListErr WriteGraphArrows (const List_t* list, FILE* file);

static const char* const DOT_FILENAME = "files/graph.dot";
static const int STRING_BUFFER_SIZE = 200;

// ВЫВОД СПИСКА
ListErr ListDump(const List_t* list)
{
    if(!list) return LIST_NULL;

    ListErr err = ConsoleDump(list);
    CHECK(err);

    err = HTMLDump(list, list->log_file);
    CHECK(err);

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В КОНСОЛЬ
static ListErr ConsoleDump(const List_t* list)
{
    if(!list) return LIST_NULL;

    printf("========== LIST DUMP START ============\n\n");
 
    printf("HEAD:      %4lu\n",   list->elements[0].next);
    printf("TAIL:      %4ld\n",   list->elements[0].previous);
    printf("LIST SIZE: %4lu\n",   list->list_capacity);
    printf("NEXT EMPTY:%4lu\n\n", list->first_empty);

    for(size_t element = 0; element < list->list_capacity; element++)
    {
        printf("[%2lu] VALUE:  %10.5f   NEXT: %3lu   PREVIOUS:  %3ld\n", 
                                                            element,
                                                            list->elements[element].value, 
                                                            list->elements[element].next,
                                                            list->elements[element].previous);
    }

    printf("PRINTING: \n");
    for(size_t elem_index = list->elements[0].next; elem_index > 0;
                                                elem_index = list->elements[elem_index].next)
    {
        printf("%g|", list->elements[elem_index].value);
    }

    printf("\n\n========== LIST DUMP END   ============\n\n");

    return LIST_CORRECT;
}

// ИНИЦИАЛИЗАЦИЯ HTML ФАЙЛА
ListErr StartBaseHTML(FILE* file)
{
    if(!file) return LIST_FILE_ERROR;
    
    fprintf(file, "<!DOCTYPE html>\n"
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
    
    fprintf(file, "</body>\n"
                  "</html>\n");

    return LIST_CORRECT;
}

static ListErr HTMLWriteInfo(const List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    static size_t current_dump = 1;

    fprintf(file, "<h2>LOG №%lu</h2>\n", current_dump++);
    fprintf(file, "<p>LIST CAPACITY: %4lu</p>\n",  list->list_capacity);
    fprintf(file, "<p>NEXT EMPTY: %4lu</p>\n\n", list->first_empty);
    fprintf(file, "<ul>");

    for(size_t elem_index = 0; elem_index < list->list_capacity; elem_index++)
    {
        fprintf(file, "<li>[%2lu] VALUE: %10f      NEXT: %3lu       PREVIOUS: %3ld </li>\n", 
                                                            elem_index,
                                                            list->elements[elem_index].value, 
                                                            list->elements[elem_index].next,
                                                            list->elements[elem_index].previous);
    }

    fprintf(file,  "</ul>");

    fprintf(file, "\n<p>PRINTING: </p>\n <p>");
    for(size_t elem_index = list->elements[0].next; elem_index > 0; 
                                                elem_index = list->elements[elem_index].next)
    {
        if(list->elements[elem_index].next == 0)
        {
            fprintf(file, "%g", list->elements[elem_index].value);
        }
        else
        {
            fprintf(file, "%g -> ", list->elements[elem_index].value);
        }
    }

    fprintf(file, "</p>\n\n");

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В HTML
static ListErr HTMLDump(const List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    static int current_dump = 0;

    HTMLWriteInfo(list, file);

    FILE* dot_file = fopen(DOT_FILENAME, "w+");
    if(!dot_file) return LIST_FILE_ERROR;

    WriteGraph(list, dot_file);
    fclose(dot_file);

    char string[STRING_BUFFER_SIZE] = {};
    sprintf(string, "dot -Tpng %s -o files/log_%d.png", DOT_FILENAME, current_dump);
    system(string);

    fprintf(file, "<h1> <img src=\"log_%d.png\" align=\"top\"/> </h1>", current_dump);
    current_dump++;

    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ
static ListErr WriteGraph(const List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    fprintf(file,   "digraph\n"
                    "{\n"
                    "bgcolor=\"#111111\""
                    "rankdir=LR;\n"
                    "node[color=\"white\",fontsize=14, fillcolor=\"#111111\"" 
                                                     ",fontcolor=\"#F5DEB3\""
                                                     ",shape=\"rectangle\""
                                                     ",style=\"filled\"];\n"
                    "edge[color=%s,fontsize=15, penwidth=2, dir=forward];\n", DEFAULT_COLOR);

    
    ListErr err = WriteGraphNodes(list, file);
    CHECK(err);

    err = WriteGraphArrows(list, file);
    CHECK(err);
    
    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ НОД
static ListErr WriteGraphNodes(const List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    fprintf(file, "ELEM_0[label=\" ELEMENT 0\n");
    
    if(ValueEquality(list->elements[0].value, SHIELD_VALUE)) 
    {
        fprintf(file,"value: SHIELD \n");
    }
    else
    {
        fprintf(file, "value: %g \n", list->elements[0].value);
    }

    fprintf(file, "head: %lu \n ",  list->elements[0].next);
    fprintf(file, "tail: %ld\"]\n", list->elements[0].previous);

    for(size_t elem_index = 1; elem_index < list->list_capacity; elem_index++)
    {
        fprintf(file, "ELEM_%lu[label=\" ELEMENT %lu\n", elem_index, elem_index);

        if(ValueEquality(list->elements[elem_index].value, POISON))
        {
            fprintf(file,   "value: POISON \n"
                            "next: %lu \n\""
                            "]\n", 
                                list->elements[elem_index].next);
        }
        else
        {
            fprintf(file, "value: %g \n",            list->elements[elem_index].value);
            fprintf(file, "next: %lu \n",            list->elements[elem_index].next);
            fprintf(file, "previous: %ld\"\n]\n",    list->elements[elem_index].previous);                        
        }
    }
    
    fprintf(file, "HEAD[]\n"
                  "TAIL[]\n"
                  "FREE[]\n");

    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ СТРЕЛОК
static ListErr WriteGraphArrows(const List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    fprintf(file, "HEAD->ELEM_%lu[headport=\"n\"]\n", list->elements[0].next);
    fprintf(file, "TAIL->ELEM_%ld[headport=\"n\"]\n", list->elements[0].previous);
    fprintf(file, "FREE->ELEM_%lu[headport=\"n\", color=%s]\n", list->first_empty, FREE_COLOR);

    for(size_t elem_index = 0; elem_index < list->list_capacity - 1; elem_index++)
    {
        fprintf(file, "ELEM_%lu->ELEM_%lu[style=\"invis\"]\n", elem_index, elem_index+1);
    }

    for(size_t elem_index = 0; elem_index < list->list_capacity; elem_index++)
    {
        size_t next = list->elements[elem_index].next;
        fprintf(file, "ELEM_%lu->ELEM_%lu[color=%s,tailport=\"n\", headport=\"n\"]\n"
                                                                 , elem_index, next, NEXT_COLOR);


        ssize_t prev = list->elements[elem_index].previous;
        if(prev >= 0 && prev < (ssize_t)list->list_capacity)
        {
            fprintf(file, "ELEM_%lu->ELEM_%ld[color=%s,tailport=\"s\",headport=\"s\"]\n"
                                                                 , elem_index, prev, PREV_COLOR);
        }
    }

    fprintf(file, "}");

    return LIST_CORRECT;
}