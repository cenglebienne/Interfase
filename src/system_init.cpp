#include "system_init.h"
#include "app_state.h"
#include "pins.h"
#include "validator.h"
#include "pulse_output.h"
#include "button_led.h"
#include "connections.h"
#include "http_server_app.h"
#include "logger.h"
#include <LittleFS.h>

void system_init_pins() {
  // Entradas
  pinMode(PIN_VMC_ENABLE_IN, INPUT);
  pinMode(PIN_BUTTON,        INPUT_PULLUP);

  // Salida de habilitación del validador: arranca deshabilitado hasta confirmar VMC
  pinMode(PIN_HABILITACION, OUTPUT);
  digitalWrite(PIN_HABILITACION, HIGH);  // HIGH = deshabilitar validador

  // Salidas de pulso (las inicializa pulse_output_setup)
  pulse_output_setup();

  // Interrupt del validador
  validator_setup();

  // LED de estado y manejo del botón
  button_led_setup();

  Serial.println("Pines inicializados.");
}

void system_init_filesystem() {
  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: no se pudo montar LittleFS.");
    return;
  }
  Serial.println("LittleFS montado OK.");
  inicializarLogs();
}

void system_init_connectivity() {
  net_setup();
  http_server_setup();
  Serial.println("Conectividad iniciada.");
}
