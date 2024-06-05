
#include "esp_zigbee_core.h"

#define MAX_CHILDREN                    10          
#define INSTALLCODE_POLICY_ENABLE       false     
#define HA_THERMOSTAT_ENDPOINT          1           
#define ESP_ZB_PRIMARY_CHANNEL_MASK     (1l << 13)  


#define MANUFACTURER_NAME               "\x0B""ESPRESSIF"
#define MODEL_IDENTIFIER                "\x09"CONFIG_IDF_TARGET

#define ESP_ZB_ZC_CONFIG()                                                              \
    {                                                                                   \
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_COORDINATOR,                                  \
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,                               \
        .nwk_cfg.zczr_cfg = {                                                           \
            .max_children = MAX_CHILDREN,                                               \
        },                                                                              \
    }

#define ESP_ZB_DEFAULT_RADIO_CONFIG()                           \
    {                                                           \
        .radio_mode = RADIO_MODE_NATIVE,                        \
    }

#define ESP_ZB_DEFAULT_HOST_CONFIG()                            \
    {                                                           \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,      \
    }
