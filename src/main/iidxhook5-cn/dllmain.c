#include <windows.h>

#include "hook/table.h"

#include "iidxhook5-cn/iidxhook5-cn.h"

#include "main/sdk-hook/dllentry.h"

#include "util/defs.h"

static ATOM WINAPI _iidxhook5_cn_dllmain_my_RegisterClassA(const WNDCLASSA *lpWndClass);
static ATOM(WINAPI *_iidxhook5_cn_dllmain_real_RegisterClassA)(const WNDCLASSA *lpWndClass);

static bool _iidxhook5_cn_dllmain_registerclassa_call_check;

static const struct hook_symbol _iidxhook5_cn_dllmain_hook_syms[] = {
    {
        .name = "RegisterClassA",
        .patch = _iidxhook5_cn_dllmain_my_RegisterClassA,
        .link = (void **) &_iidxhook5_cn_dllmain_real_RegisterClassA
    },
};

static ATOM WINAPI _iidxhook5_cn_dllmain_my_RegisterClassA(const WNDCLASSA *lpWndClass)
{
    if (_iidxhook5_cn_dllmain_registerclassa_call_check) {
        return _iidxhook5_cn_dllmain_real_RegisterClassA(lpWndClass);
    }

    _iidxhook5_cn_dllmain_registerclassa_call_check = true;

    bt_hook_dllentry_main_init();

    return _iidxhook5_cn_dllmain_real_RegisterClassA(lpWndClass);
}

// TODO find another call right before main exits to hook cleanup and stuff with
// bt_hook_dllentry_main_fini()

BOOL WINAPI DllMain(HMODULE mod, DWORD reason, void *ctx)
{
    if (reason == DLL_PROCESS_ATTACH) {
        bt_hook_dllentry_init(
            mod,
            "iidxhook5-cn",
            bt_module_core_config_api_set,
            bt_module_core_log_api_set,
            bt_module_core_thread_api_set,
            bt_module_hook_api_get);

        _iidxhook5_cn_dllmain_registerclassa_call_check = false;

        hook_table_apply(
            NULL, "kernel32.dll", _iidxhook5_cn_dllmain_hook_syms, lengthof(_iidxhook5_cn_dllmain_hook_syms));

    } else if (reason == DLL_PROCESS_DETACH) {
        // https://learn.microsoft.com/en-us/windows/win32/dlls/dllmain#remarks
        if (ctx == NULL) {
            hook_table_revert(NULL, "kernel32.dll", _iidxhook5_cn_dllmain_hook_syms, lengthof(_iidxhook5_cn_dllmain_hook_syms));

            // Hacky to have this here, should be close/right after application main exits, see TODO above
            bt_hook_dllentry_main_fini();

            bt_hook_dllentry_fini();
        }
    }

    return TRUE;
}