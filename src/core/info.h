#pragma once
#include <WiFi.h>

namespace reflow_esp {

struct SystemInfo {
    IPAddress addr;
    String wifi_ssid;
    String profile_title;
};

} // namespace reflow_esp
