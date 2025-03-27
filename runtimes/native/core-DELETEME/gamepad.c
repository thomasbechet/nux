#include "platform.h"

nu_status_t
gamepad_init (vm_t *vm)
{
    nu_memset(vm->gamepad.buttons, 0, sizeof(vm->gamepad.buttons));
    return NU_SUCCESS;
}
void
gamepad_update (vm_t *vm)
{
    os_gamepad_update(vm);
}

nu_u32_t
sys_button (vm_t *vm, nu_u32_t player)
{
    if (player >= SYS_MAX_PLAYER_COUNT)
    {
        return 0;
    }
    return vm->gamepad.buttons[player];
}
nu_f32_t
sys_axis (vm_t *vm, nu_u32_t player, sys_axis_t axis)
{
    if (player >= SYS_MAX_PLAYER_COUNT)
    {
        return 0;
    }
    return vm->gamepad.axis[player][axis];
}
