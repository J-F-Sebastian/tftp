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

#ifndef TFTP_LOGGER_H_INCLUDED
#define TFTP_LOGGER_H_INCLUDED

typedef enum tftp_log_config
{
        TFTP_LOG_FILE = (1 << 0),
        TFTP_LOG_USR = (1 << 1),
        TFTP_LOG_TIMESTAMP = (1 << 2)
} tftp_log_config_e;

void tftp_log_init(const unsigned flags);
void tftp_log_done(void);

void tftp_log_message(const char *format, ...);

#endif // TFTP_LOGGER_H_INCLUDED
