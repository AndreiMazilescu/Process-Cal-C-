/*
 * process_cal3.c
 * Andrei Mazilescu V00796396
 * Starter file provided to students for Assignment #3, SENG 265,
 * Fall 2021.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emalloc.h"
#include "ics.h"
#include "listy.h"
#include <time.h>

#define MAX_LINE_LEN 132

//taken from timeplay.c (copied from A1)
void dt_format(char *formatted_time, const char *dt_time, const int len)
{
    struct tm temp_time;
    time_t    full_time;
    //char      temp[5];

    /*  
     * Ignore for now everything other than the year, month and date.
     * For conversion to work, months must be numbered from 0, and the 
     * year from 1900.
     */  
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dt_time, "%4d%2d%2d",
        &temp_time.tm_year, &temp_time.tm_mon, &temp_time.tm_mday);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    full_time = mktime(&temp_time);
    strftime(formatted_time, len, "%B %d, %Y (%a)", 
        localtime(&full_time));
}
//taken from timeplay.c (copied from A1)
void dt_increment(char *after, const char *before, int const num_days)
{
    struct tm temp_time;
    time_t    full_time;
    //char      temp[5];

    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(before, "%4d%2d%2dT%2d%2d%2d", &temp_time.tm_year,
        &temp_time.tm_mon, &temp_time.tm_mday, &temp_time.tm_hour, 
        &temp_time.tm_min, &temp_time.tm_sec);
    temp_time.tm_year -= 1900;
    temp_time.tm_mon -= 1;
    temp_time.tm_mday += num_days;

    full_time = mktime(&temp_time);
    after[0] = '\0';
    strftime(after, 16, "%Y%m%dT%H%M%S", localtime(&full_time));
    strncpy(after + 16, before + 16, MAX_LINE_LEN - 16); 
    after[MAX_LINE_LEN - 1] = '\0';
}


//copied from A1, adapted to use dynamic memory. Takes variables from ics and creates an event.
Event *create_event(char start[], char end[], char loc[], char sum[], char u[]){
    Event *e = (Event *)malloc(sizeof(Event));
    strcpy(e->start, start);
    strcpy(e->end, end);
    strcpy(e->location, loc);
    strcpy(e->summary, sum);
    strcpy(e->until, u);
    int temp;
    int temp2;
    int sc;
    sscanf(start, "%8dT%3d", &temp, &temp2);
    sc = (temp*1000)+temp2;
    e->startcomp = temp;
    e->sc2 = sc;
    return e;
}

//Reads ics files, stores then to variables and calls creat_event. Returns a linked list of events.
node_t *readfile(char filename[], int *count){
    
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL){
        printf("Unable to open file\n");
        exit(1);
    }

    char *line = NULL;

    line = (char *)malloc(sizeof(char) * MAX_LINE_LEN);
    
    
    char start[MAX_LINE_LEN];
    char dtend[MAX_LINE_LEN];
    char location[MAX_LINE_LEN];
    char summary[MAX_LINE_LEN];
    char until[MAX_LINE_LEN];
    char end[MAX_LINE_LEN];
    Event *event;
    node_t *eventlist = NULL;
    
    
    while (fgets(line, MAX_LINE_LEN, input_file) != NULL){
        //printf("%s", line);
        sscanf(line, "DTSTART:%s", start);
        sscanf(line, "DTEND:%s", dtend);
        sscanf(line, "LOCATION:%[a-zA-Z0-9 ']", location);
        sscanf(line, "SUMMARY:%[-a-zA-Z0-9 ,]", summary);
        sscanf(line, "RRULE:%*[a-zA-TV-Z;=]UNTIL=%15s", until);
        if (sscanf(line, "END:VE%s", end) == 1) {
            
            event = create_event(start, dtend, location, summary, until);
            eventlist = add_inorder(eventlist, new_node(event));
            
            *count = *count +1;
            strcpy(location, "");
            strcpy(until, "");
            strcpy(summary, "");
            
            
        }
    }

    

    free(line);
    fclose(input_file);
    return eventlist;

}

//adapted from timeplay.c (Copied from A1)
void hour_format(char *formattedhour, const char *dthour, const int len){
    struct tm temp_time;
    time_t hourmin;
    //char temp[5];
    
    memset(&temp_time, 0, sizeof(struct tm));
    sscanf(dthour, "%2d%2d", &temp_time.tm_hour, &temp_time.tm_min);
    hourmin = mktime(&temp_time);
    strftime(formattedhour, len, "%I:%M %p", localtime(&hourmin));
    

}
//Copied from A1, takes an event struct and prints the DATE of event.
void print_eventdate(Event *e){
    char formattedStart[MAX_LINE_LEN];
    dt_format(formattedStart, e->start, MAX_LINE_LEN);
    printf("%s\n", formattedStart);
    int dashlength = strlen(formattedStart);
    for (int i = 0; i < dashlength; i++){
        printf("-");
    }
    printf("\n");
}
//Copied from A1, takes an event struct and prints the SUMMARY of the event per assignment specifications.    
void print_eventsum(Event *e){
    char unfstarttime[MAX_LINE_LEN];
    char unfendtime[MAX_LINE_LEN];
    char formatStime[MAX_LINE_LEN];
    char formatEtime[MAX_LINE_LEN];
    sscanf(e->start, "%*dT%s", unfstarttime);
    sscanf(e->end, "%*dT%s", unfendtime);
    hour_format(formatStime, unfstarttime, MAX_LINE_LEN);
    hour_format(formatEtime, unfendtime, MAX_LINE_LEN);
    if (formatStime[0] == '0'){
        formatStime[0] = ' ';
    }
    if (formatEtime[0] == '0'){
        formatEtime[0] = ' ';
    }
    printf("%s to %s: %s {{%s}}\n", formatStime, formatEtime, e->summary, e->location);



}
//Copied from A1, adapted to return a linked list of events, takes event and uses dt_increment to add repeated events to list.
node_t *rrule(node_t *e, Event *event, int *count, char until[]){
    
    int finish;
    int unti;
    sscanf(until, "%8dT%*d", &unti);
    sscanf(event->start, "%8dT%*d", &finish);
    char newstart[MAX_LINE_LEN];
    char newend[MAX_LINE_LEN];
    dt_increment(newstart, event->start, 7);
    dt_increment(newend, event->end, 7);
    while (finish < unti){
        e = add_inorder(e, new_node(create_event(newstart, newend, event->location, event->summary, "")));
        *count= *count +1;
        dt_increment(newstart, newstart, 7);
        dt_increment(newend, newend, 7);
        sscanf(newstart, "%8dT%*d", &finish);
    }
    return e;
}
//Copied from a1, adapted to print events from linked list instead of an array. 
//Variables to check when to print a newline. Compares eventdates for events on the same day.
void print_events(int from, int to, node_t *list, int count){
    
    node_t *currentnode = list;
    node_t *prev = NULL;
    int i = 0;
    int x = 1;
    while (currentnode != NULL){
        if (from <= currentnode->val->startcomp && currentnode->val->startcomp <= to){
            if (i>0 && currentnode->val->startcomp != prev->val->startcomp){
                print_eventdate(currentnode->val);
                x=0;
            }

            if (i==0) {
                print_eventdate(currentnode->val);
            }
            
            print_eventsum(currentnode->val);
            if (i+1 <count){
                if (currentnode->next->val->startcomp <= to){
                    if (currentnode->next->val->startcomp != currentnode->val->startcomp){
                        x=1;
                    }
                }
            }
            
            
        }
        else {
            x= 0;
        }
        if (i + 1 < count){
            if (x == 1 && currentnode->next->val->startcomp <= to){
                printf("\n");
                x = 0;
            }
        }
        i = i + 1;
        prev = currentnode;
        currentnode = currentnode->next;

    }

}


//lines 250-273 copied from A1 for getting arguments from stdin.
int main(int argc, char *argv[])
{
    int from_y = 0, from_m = 0, from_d = 0;
    int to_y = 0, to_m = 0, to_d = 0;
    char *filename = NULL;
    int i; 

    for (i = 0; i < argc; i++) {
        if (strncmp(argv[i], "--start=", 8) == 0) {
            sscanf(argv[i], "--start=%d/%d/%d", &from_y, &from_m, &from_d);
        } else if (strncmp(argv[i], "--end=", 6) == 0) {
            sscanf(argv[i], "--end=%d/%d/%d", &to_y, &to_m, &to_d);
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            filename = argv[i]+7;
        }
    }

    if (from_y == 0 || to_y == 0 || filename == NULL) {
        fprintf(stderr, 
            "usage: %s --start=yyyy/mm/dd --end=yyyy/mm/dd --file=icsfile\n",
            argv[0]);
        exit(1);
    }

    int fromfinal = (((from_y*100)+from_m)*100)+from_d;
    int tofinal = (((to_y*100)+to_m)*100)+to_d;

    
    int count = 0;

    
    node_t *list = readfile(filename, &count);

    
//goes through list and checks for RRULE, updates list accordingly
    int tempcount = count;
    int index = 0;
    node_t *currentnode = list;
    while (index != tempcount){
        if (strlen(currentnode->val->until) == 15){
            list = rrule(list, currentnode->val, &count, currentnode->val->until);
        }
        currentnode = currentnode->next;
        index = index + 1;
    }
    
    
    print_events(fromfinal, tofinal, list, count);

//freeing heap memory copied from lab 8
    node_t  *temp_n = NULL;
    for ( ; list != NULL; list = temp_n ) {
        temp_n = list->next;
        free(list->val);
        free(list);
    } 



    exit(0);
}
