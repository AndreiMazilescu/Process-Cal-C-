#ifndef _ICS_H_
#define _ICS_H_

#define DT_LEN       17
#define SUMMARY_LEN  80
#define LOCATION_LEN 80
#define RRULE_LEN    80

/*typedef struct event_t
{
    char dtstart[DT_LEN];
    char dtend[DT_LEN];
    char summary[SUMMARY_LEN];
    char location[LOCATION_LEN];
    char rrule[RRULE_LEN];
} event_t;*/
//copied from A1 my original struct for events with defined lengths added.
typedef struct Event {
    char start[DT_LEN];
    char end[DT_LEN];
    char location[LOCATION_LEN];
    char summary[SUMMARY_LEN];
    char until[RRULE_LEN];
    int startcomp;
    int sc2;
} Event;

#endif
