#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../include/list_manager.h"
#include "../include/list_logger.h"

static ListErr ConsoleDump      (List_t* list);
static ListErr HTMLDump         (List_t* list, FILE* file);
static ListErr HTMLWriteInfo    (List_t* list, FILE* file);
static ListErr WriteGraph       (List_t* list, FILE* file);
static ListErr WriteGraphNodes  (List_t* list, FILE* file);
static ListErr WriteGraphArrows (List_t* list, FILE* file);

static const char* const DOT_FILENAME = "files/graph.dot";
static const int STRING_BUFFER_SIZE = 200;

// ВЫВОД СПИСКА
ListErr ListDump(List_t* list)
{
    if(!list) return LIST_NULL;

    ListErr err = ConsoleDump(list);
    CHECK(err);

    err = HTMLDump(list, list->log_file);
    CHECK(err);

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В КОНСОЛЬ
static ListErr ConsoleDump(List_t* list)
{
    if(!list) return LIST_NULL;

    printf("========== LIST DUMP START ============\n\n");
 
    printf("HEAD:      %4d\n",   list->elements[0].next);
    printf("TAIL:      %4d\n",   list->elements[0].previous);
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
    for(int i = list->elements[0].next; i > 0; i = list->elements[i].next)
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

static ListErr HTMLWriteInfo(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    static int current_dump = 0;

    fprintf(file, "<h2>LOG №%d</h2>\n", current_dump++);
    fprintf(file, "<p>LIST CAPACITY: %4lu</p>\n",  list->list_capacity);
    fprintf(file, "<p>NEXT EMPTY: %4d</p>\n\n", list->first_empty);
    fprintf(file, "<ul>");

    for(size_t elem_index = 0; elem_index < list->list_capacity; elem_index++)
    {
        fprintf(file, "<li>[%2lu] VALUE: %10f      NEXT: %3d       PREVIOUS: %3d </li>\n", 
                                                            elem_index,
                                                            list->elements[elem_index].value, 
                                                            list->elements[elem_index].next,
                                                            list->elements[elem_index].previous);
    }

    fprintf(file,  "</ul>");

    fprintf(file, "\n<p>PRINTING: </p>\n <p>");
    for(int elem_index = list->elements[0].next; elem_index > 0; 
                                                elem_index = list->elements[elem_index].next)
    {
        fprintf(file, "%g -> ", list->elements[elem_index].value);
    }

    fprintf(file, "</p>\n\n");

    return LIST_CORRECT;
}

// ВЫВОД СПИСКА В HTML
static ListErr HTMLDump(List_t* list, FILE* file)
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
static ListErr WriteGraph(List_t* list, FILE* file)
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
                    "edge[color=" DEFAULT_COLOR ",fontsize=15, penwidth=2, dir=forward];\n");

    
    ListErr err = WriteGraphNodes(list, file);
    CHECK(err);

    err = WriteGraphArrows(list, file);
    CHECK(err);
    
    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ НОД
static ListErr WriteGraphNodes(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    fprintf(file, "ELEM_0[pos=\"0,0!\"label=\" ELEMENT 0\n");
    
    if(ValueEquality(list->elements[0].value, SHIELD_VALUE)) 
    {
        fprintf(file,"value: SHIELD \n");
    }
    else
    {
        fprintf(file, "value: %g \n", list->elements[0].value);
    }

    fprintf(file, "head: %d \n ", list->elements[0].next);
    fprintf(file, "tail: %d\"]\n", list->elements[0].previous);

    for(size_t elem_index = 1; elem_index < list->list_capacity; elem_index++)
    {
        fprintf(file,   "ELEM_%lu[pos=\"%d,0!\", label=\" ELEMENT %lu\n", 
                            elem_index, (int)elem_index*3, elem_index);

        if(ValueEquality(list->elements[elem_index].value, POISON))
        {
            fprintf(file,   "value: POISON \n"
                            "next: %d \n\""
                            "]\n", 
                                list->elements[elem_index].next);
        }
        else
        {
            fprintf(file, "value: %g \n",           list->elements[elem_index].value);
            fprintf(file, "next: %d \n",            list->elements[elem_index].next);
            fprintf(file, "previous: %d\"\n]\n",    list->elements[elem_index].previous);                        
        }
    }
    
    fprintf(file, "HEAD[]\n"
                  "TAIL[]\n"
                  "FREE[]\n");

    return LIST_CORRECT;
}

// ЗАПИСЬ В ГРАФ СТРЕЛОК
static ListErr WriteGraphArrows(List_t* list, FILE* file)
{
    if(!list) return LIST_NULL;
    if(!file) return LIST_FILE_ERROR;

    fprintf(file, "HEAD->ELEM_%d[headport=\"n\"]\n", list->elements[0].next);
    fprintf(file, "TAIL->ELEM_%d[headport=\"n\"]\n", list->elements[0].previous);
    fprintf(file, "FREE->ELEM_%d[headport=\"n\", color=" FREE_COLOR "]\n", list->first_empty);

    for(size_t elem_index = 0; elem_index < list->list_capacity - 1; elem_index++)
    {
        fprintf(file, "ELEM_%lu->ELEM_%lu[style=\"invis\"]\n", elem_index, elem_index+1);
    }

    for(size_t elem_index = 0; elem_index < list->list_capacity; elem_index++)
    {
        int next = list->elements[elem_index].next;
        if(next >= 0)
        {
            fprintf(file, "ELEM_%lu->ELEM_%d[color=" NEXT_COLOR ",tailport=\"n\", headport=\"n\"]\n"
                                                                 , elem_index, next);
        }
        else if(next < 0)
        {
            fprintf(file, "ELEM_%lu->ELEM_%d[color=" FREE_COLOR ",tailport=\"n\", headport=\"n\"]\n"
                                                                 , elem_index, -next);
        }

        int prev = list->elements[elem_index].previous;
        if(prev >= 0 && prev < (int)list->list_capacity)
        {

            fprintf(file, "ELEM_%lu->ELEM_%d[color=" PREV_COLOR ",tailport=\"s\", headport=\"s\"]\n"
                                                                 , elem_index, abs(prev));
        }
    }
    
    fprintf(file, "}");

    return LIST_CORRECT;
}