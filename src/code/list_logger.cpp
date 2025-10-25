#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "list_manager.h"
#include "list_logger.h"

#define WRITE(string) fwrite(string, strlen(string), 1, file);
#define WRITE_S() fwrite(string, strlen(string), 1, file);

int current_dump = 1;

static void ConsoleDump(List_t* list);
static void HTMLDump(List_t* list, FILE* file);
static void WriteGraph(List_t* list, FILE* file);

void ListDump(List_t* list)
{
    assert(list);

    //ConsoleDump(list);
    HTMLDump(list, list->log_file);

    current_dump++;
}

static void ConsoleDump(List_t* list)
{
    assert(list);

    printf("========== LIST DUMP START ============\n\n");

    printf("HEAD:      %4d\n",   list->head);
    printf("TAIL:      %4d\n",   list->tail);
    printf("LIST SIZE: %4lu\n",  list->list_size);
    printf("NEXT EMPTY:%4d\n\n", list->first_empty);

    for(size_t element = 0; element < list->list_size; element++)
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
}

void CreateBaseHTML(FILE* file)
{
    assert(file);
    
    WRITE("<pre>\n");
    WRITE("<h2>LIST LOGGER</h2>\n");

}

static void HTMLDump(List_t* list, FILE* file)
{
    assert(file);
    //FIXME 100;
    char string[100] = {};

    sprintf(string, "<h3>LOG №%d</h3>", current_dump);
    WRITE_S();

    sprintf(string, "HEAD:      %4d\n",   list->head);
    WRITE_S();
    sprintf(string, "TAIL:      %4d\n",   list->tail);
    WRITE_S();
    sprintf(string, "LIST SIZE: %4lu\n",  list->list_size);
    WRITE_S();
    sprintf(string, "NEXT EMPTY:%4d\n\n", list->first_empty);
    WRITE_S();

    for(size_t element = 0; element < list->list_size; element++)
    {
        sprintf(string, "[%2lu] VALUE:  %10.5f   NEXT: %3d   PREVIOUS:  %3d\n", 
                                                            element,
                                                            list->elements[element].value, 
                                                            list->elements[element].next,
                                                            list->elements[element].previous);
        WRITE_S();
    }

    sprintf(string, "\nPRINTING: \n");
    WRITE_S();
    for(int i = list->head; i > 0; i = list->elements[i].next)
    {
        sprintf(string, "%g|", list->elements[i].value);
        WRITE_S();
    }

    sprintf(string, "\n\n");
    WRITE_S();

    //FIXME name to const
    FILE* dot_file = fopen("graph.dot", "w+");
    WriteGraph(list, dot_file);
    fclose(dot_file);

    sprintf(string, "dot -Tpng graph.dot -o files/log_%d.png", current_dump);
    system(string);

    sprintf(string, "<h1> <img src=\"files/log_%d.png\" align=\"top\"/> </h1>", current_dump);
    WRITE_S();
}

static void WriteGraph(List_t* list, FILE* file)
{
    assert(list);
    assert(file);

    char string[200] = {};

    sprintf(string, "digraph\n"
                    "{\n"
                    "rankdir=LR;\n"
                    "node[color=\"red\",fontsize=14];\n"
                    "edge[color=\"darkgreen\",fontcolor=\"blue\",fontsize=12];\n");
    WRITE_S();

//OPEN[shape="rectangle",style="filled",fillcolor="lightgrey"];
//CLOSED[shape="octagon",label="Финиш"];
//VERIFIED[shape="rectangle",style="rounded"];

    for(size_t element = 0; element < list->list_size; element++)
    {
        sprintf(string, "ELEM_%lu[shape=\"rectangle\",style=\"filled\",fillcolor=\"lightgrey\""
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
    
    for(size_t element = 0; element < list->list_size-1; element++)
    {
        sprintf(string, "ELEM_%lu->ELEM_%lu[color=\"white\"]\n", 
                                                        element, element+1);
        WRITE_S();
    }

    for(size_t element = 0; element < list->list_size; element++)
    {
        int next = list->elements[element].next;
        if(abs(next) < list->list_size)
        {

            sprintf(string, "ELEM_%lu->ELEM_%d[color=\"red\", tailport=\"n\","
                                                             "headport=\"n\"]\n",
                                                        element, abs(next));
            WRITE_S();
        }

        int prev = list->elements[element].previous;
        if(prev >= 0 && prev < list->list_size)
        {

            sprintf(string, "ELEM_%lu->ELEM_%d[color=\"blue\", tailport=\"s\", headport=\"s\"]\n", 
                                                        element, abs(prev));
            WRITE_S();
        }
    }
    
    sprintf(string, "}");
    WRITE_S();
}