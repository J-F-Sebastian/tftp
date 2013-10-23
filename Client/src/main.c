#include <windows.h>
#include <stdio.h>

#include "tftp_client.h"
#include "tftp_protocol.h"
#include "sock_utils.h"
#include "tftp_client_state.h"
#include "tftp_logger.h"

static BOOL rrq = TRUE;
static struct sockaddr_in server_address;
static char filename[TFTP_CLNT_FILENAME_MAX + 1] = {0};
static tftp_client_state_t client;
static unsigned blocksize = TFTP_DEFAULT_DATA;

static void
print_help()
{
    printf("\n Syntax\n\n TFTPClient.exe <Command> <ip address> [options] <filename>\n");
    printf("\n Commands :\n GET\t\t Read a file from the server\n PUT\t\t Write a file to the server\n");
    printf("\n Options :\n blksize=n\t Specify a block size (default=512 bytes)\n\n");
}

static BOOL
process_cmdline(char *argv[])
{
    uint32_t ipaddress;
    unsigned pos = 1;

    if (!strncasecmp(argv[pos],"GET",3)) {
        rrq = TRUE;
    } else if (!strncasecmp(argv[pos],"PUT",3)) {
        rrq = FALSE;
    } else {
        return FALSE;
    }

    ++pos;

    ipaddress = inet_addr(argv[pos]);

    if (INADDR_NONE == ipaddress) {
        return FALSE;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(TFTP_PORT_NUM);
    server_address.sin_addr.s_addr = ipaddress;

    ++pos;

    if (!strncasecmp(argv[pos],"blksize=",8)) {
        blocksize = atoi(argv[pos] + 8);
        printf("%s is %d\n", argv[pos], blocksize);
        ++pos;
    }

    snprintf(filename, sizeof(filename),"%s",argv[pos]);

    return TRUE;
}

static void on_client_exit(void)
{
    sock_done();
    printf("TFTPClient terminated.\n");
}

int main(int argc, char *argv[])
{
    sock_errno_e retcode;
    uint32_t tstart, tend;

    SetConsoleTitle("TFTP Client");
    atexit(on_client_exit);

    tftp_log_init(0);
    tftp_log_message(" TFTPClient version 1.0\n");
    printf(" TFTPClient version 1.0\n");

    retcode = sock_init();
    if (SOCK_ERR_OK != retcode) {
        printf("sock_init failed: %s\n", sock_errno_to_string(retcode));
        return -1;
    }

    if (argc < 4) {
        print_help();
        return 0;
    }

    if (!process_cmdline(argv)) {
        print_help();
        return -1;
    }

    printf("\n %s %s %s %s [octet].\n", (rrq) ? "Retrieving" : "Storing",
           filename,
           (rrq) ? "from" : "to",
           inet_ntoa(server_address.sin_addr));

    init_client_state(&client);

    if (rrq) {
        client.state_flags = TFTP_SRV_RCVING;
    } else {
        client.state_flags = TFTP_SRV_SENDING;
    }

    client.destination = server_address;
    client.opt_blocksize = blocksize;

    retcode = sock_create(&client.client);
    if (SOCK_ERR_OK != retcode) {
        printf("sock_setup failed: %s\n", sock_errno_to_string(retcode));
        sock_done();
        return -2;
    }

    tstart = GetTickCount();

    retcode = tftp_client(&client, filename);

    tend = GetTickCount() - tstart;

    if (!tend) {
        tend = 1;
    }

    printf("Transferred %u bytes in %u.%u seconds [%u KByte/s]\n",
           client.octets, tend/1000, tend%1000, ((client.octets/tend)*1000)/1024);

    reset_client_state(&client);

    return 0;
}
