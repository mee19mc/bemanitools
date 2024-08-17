#include "hooklib/config-adapter.h"

void hooklib_config_adapter_get(
    const bt_core_config_t *config, struct hooklib_config_adapter *config_adapter)
{
    bt_core_config_str_get(config, "adapter/override_ip", config_adapter->override_ip, sizeof(config_adapter->override_ip));
}
