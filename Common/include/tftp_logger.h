#ifndef TFTP_LOGGER_H_INCLUDED
#define TFTP_LOGGER_H_INCLUDED

typedef enum tftp_log_config {
    TFTP_LOG_FILE       = (1 << 0),
    TFTP_LOG_USR        = (1 << 1),
    TFTP_LOG_TIMESTAMP  = (1 << 2)
} tftp_log_config_e;

void tftp_log_init(const unsigned flags);
void tftp_log_done(void);

void tftp_log_message(const char *message);


#endif // TFTP_LOGGER_H_INCLUDED
