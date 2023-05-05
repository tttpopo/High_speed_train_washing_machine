#ifndef _CONSOLE_H_
#define _CONSOLE_H_

typedef void (*CS_FUN)(unsigned char* cmd);

typedef enum
{
    CONSOLE_OK = 0x00U,
    CONSOLE_ERROR = 0x01U,
} Console_StatusTypeDef;

typedef struct fun_list_t
{
    unsigned char *cmd;
    CS_FUN fun;
    struct fun_list_t *next;
} fun_list_t;

void cs_recv_callback(unsigned short Size);
Console_StatusTypeDef cs_init(void);
Console_StatusTypeDef cs_reg_fun(unsigned char *cmd, CS_FUN fun);
Console_StatusTypeDef cs_dereg_fun(unsigned char *cmd);
void cs_display_list(unsigned char* cmd);
void console_task(void);

#endif
