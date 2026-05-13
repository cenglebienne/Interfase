#include <Arduino.h>
#include "config.h"
#include "app_state.h"
#include "pins.h"
#include "constants.h"
#include "memory_api.h"
#include "system_init.h"
#include "app_core.h"
#include "button_led.h"
#include "connections.h"
#include "http_server_app.h"

static uint32_t s_apStartMs = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Boot Interface Vending ===");

  // 1. Carga mínima de config (solo flag AP) antes de cualquier init de hardware
  boot_load_config(g_cfg);

  // 2. Carga completa desde NVS
  Storage_Bootstrap();

  // 3. GPIOs, filesystem y conectividad
  system_init_pins();
  system_init_filesystem();

  if (g_cfg.wifi_mode_ap) {
    Serial.println("Modo Access Point solicitado.");
    g_levantarAP = true;
  }

  system_init_connectivity();
  s_apStartMs = millis();
}

void loop() {
  button_led_poll();

  if (g_levantarAP) {
    // Modo AP: solo atender peticiones web
    if ((millis() - s_apStartMs) > AP_TIMEOUT_MS) {
      Serial.println("Timeout AP. Reiniciando...");
      ESP.restart();
    }
    http_server_poll();
    return;
  }

  app_core_loop();
}
