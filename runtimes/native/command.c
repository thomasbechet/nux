#include "internal.h"

static command_t cmds[MAX_COMMAND];
static int       cmds_count = 0;

void
command_push (command_t cmd)
{
    if (cmds_count < MAX_COMMAND)
    {
        cmds[cmds_count++] = cmd;
    }
}
bool
command_poll (command_t *cmd)
{
    if (!cmds_count)
    {
        return false;
    }
    *cmd = cmds[--cmds_count];
    return true;
}
