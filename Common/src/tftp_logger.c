/*
 * TFTP Client and Server
 *
 * Copyright (C) 2013 - 2025 Diego Gallizioli
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "tftp_logger.h"

#define STRFTIME_FMT_LEN (24)

static const char strftime_frmt[] = "%a %x %X%% ";

static FILE *logfile = NULL;
static unsigned myflags = 0;

static size_t tftp_log_get_tstamp(char *buffer, int bsize)
{
        time_t rawtime;
        struct tm timeinfo;
        size_t retval;

        time(&rawtime);
        if (localtime_s(&timeinfo, &rawtime))
                return 0;

        retval = strftime(buffer, bsize, strftime_frmt, &timeinfo);
        if (!retval)
        {
                retval = snprintf(buffer, bsize, "N/A %% ");
        }

        return retval;
}

void tftp_log_init(const unsigned flags)
{
        /* 23 Is the maximum required, but 24 is multiple of 4 :-) */
        char buffer[STRFTIME_FMT_LEN];

        tftp_log_get_tstamp(buffer, sizeof(buffer));

        if (flags & TFTP_LOG_TIMESTAMP)
        {
                myflags |= TFTP_LOG_TIMESTAMP;
        }

        if (flags & TFTP_LOG_FILE)
        {
                if (fopen_s(&logfile, "tftp_server.log", "a"))
                {
                        printf("Cannot open tftp_server.log for logging.\n");
                }
                else
                {
                        fprintf(logfile, "\n============ LOGFILE START %s\n", buffer);
                        myflags |= TFTP_LOG_FILE;
                }
        }

        if (flags & TFTP_LOG_USR)
        {
                printf("\n============ LOG START %s\n", buffer);
                myflags |= TFTP_LOG_USR;
        }
}

void tftp_log_done()
{
        /* 23 Is the maximum required, but 24 is multiple of 4 :-) */
        char buffer[STRFTIME_FMT_LEN];

        tftp_log_get_tstamp(buffer, sizeof(buffer));

        if (myflags & TFTP_LOG_FILE)
        {
                fprintf(logfile, "\n============ LOGFILE END %s\n", buffer);
        }
        if (logfile)
        {
                fclose(logfile);
        }
        if (myflags & TFTP_LOG_USR)
        {
                printf("\n============ LOG END %s\n", buffer);
        }
        myflags = 0;
}

void tftp_log_message(const char *format, ...)
{
        char mylogmessage[160];
        va_list args;
        size_t pos = 0;

        va_start(args, format);

        if (myflags & TFTP_LOG_TIMESTAMP)
        {
                /* 23 Is the maximum required, but 24 is multiple of 4 :-) */
                pos = tftp_log_get_tstamp(mylogmessage, STRFTIME_FMT_LEN);
        }

        vsnprintf(mylogmessage + pos, sizeof(mylogmessage) - pos, format, args);

        if (myflags & TFTP_LOG_USR)
        {
                printf("%s\n", mylogmessage);
        }
        if (myflags & TFTP_LOG_FILE)
        {
                fprintf(logfile, "%s\n", mylogmessage);
        }
        va_end(args);
}
