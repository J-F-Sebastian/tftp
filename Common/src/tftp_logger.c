#include <stdio.h>
#include <time.h>

#include "tftp_logger.h"

static FILE *logfile = NULL;
static unsigned myflags = 0;

void tftp_log_init(const unsigned flags)
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (flags & TFTP_LOG_TIMESTAMP) {
        myflags |= TFTP_LOG_TIMESTAMP;
    }

    if (flags & TFTP_LOG_FILE) {
        logfile = fopen("tftp_server.log","a");
        if (!logfile) {
            printf("Cannot open tftp_server.log for logging.\n");
        } else {
            fprintf(logfile, "\n============ LOGFILE START %s", asctime(timeinfo));
            myflags |= TFTP_LOG_FILE;
        }
    }

    if (flags & TFTP_LOG_USR) {
        printf("\n============ LOG START %s", asctime(timeinfo));
        myflags |= TFTP_LOG_USR;
    }
}

void tftp_log_done()
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    if (myflags & TFTP_LOG_FILE) {
        fprintf(logfile, "\n============ LOGFILE END %s", asctime(timeinfo));
    }
    if (logfile) {
        fclose(logfile);
    }
    if (myflags & TFTP_LOG_USR) {
        printf("\n============ LOG END %s", asctime(timeinfo));
    }
    myflags = 0;
}

void tftp_log_message(const char *message)
{
    char mymessage[160];
    time_t rawtime;
    struct tm * timeinfo;

    if (myflags & TFTP_LOG_TIMESTAMP) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        snprintf(mymessage,sizeof(mymessage),"%s - %s",asctime(timeinfo),message);
        message = mymessage;
    }
    if (myflags & TFTP_LOG_USR) {
        printf("%s\n", message);
    }
    if (myflags & TFTP_LOG_FILE) {
        fprintf(logfile, "%s\n", message);
    }
}
