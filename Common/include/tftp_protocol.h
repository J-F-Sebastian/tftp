#ifndef TFTP_PROTOCOL_H_INCLUDED
#define TFTP_PROTOCOL_H_INCLUDED

#define TFTP_PORT_NAME "69"
#define TFTP_PORT_NUM (69)

/* RFC 1350 */
#define TFTP_HDR_SIZE (4)
#define TFTP_DEFAULT_DATA (512)

/* RFC 2347 */
#define TFTP_MIN_DATA (8)
#define TFTP_MAX_DATA (65464)

enum TFTP_PAK_TYPE
{
    /* RFC 1350 */
    TFTP_RRQ    = 1,
    TFTP_WRQ    = 2,
    TFTP_DATA   = 3,
    TFTP_ACK    = 4,
    TFTP_ERROR  = 5,
    /* RFC 2347 */
    TFTP_OACK   = 6
};

enum TFTP_ERROR
{
    /* RFC 1350 */
    /* Not defined, see error message (if any). */
    TFTP_ERR_UNDEF = 0,
    /* File not found. */
    TFTP_ERR_NOTFOUND,
    /* Access violation. */
    TFTP_ERR_ACCESS,
    /* Disk full or allocation exceeded. */
    TFTP_ERR_DISKFULL,
    /* Illegal TFTP operation. */
    TFTP_ERR_ILLEGAL,
    /* Unknown transfer ID. */
    TFTP_ERR_UNKNOWNID,
    /* File already exists. */
    TFTP_ERR_FILEEXIST,
    /* No such user. */
    TFTP_ERR_NOUSER,
    /* RFC 2347 */
    /* Unsupported option */
    TFTP_ERR_OPTION,
    /* Number of known errors */
    TFTP_ERR_MAX
};

/* RFC 1350 */
#define TFTP_MODE_ASCII "netascii"
#define TFTP_MODE_BIN "octet"
#define TFTP_MODE_MAIL "mail"

/* RFC 2348 */
#define TFTP_OPT_BLKSIZE "blksize"

#endif // TFTP_PROTOCOL_H_INCLUDED
