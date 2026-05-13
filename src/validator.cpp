#include "validator.h"
#include "app_state.h"
#include "pins.h"
#include "driver/gpio.h"

// Variables sombra para el ISR (no se puede acceder a Misvalores desde IRAM).
static volatile bool     s_activoAlto       = false;
static volatile uint32_t s_minDurMs         = 50;
static volatile uint32_t s_maxDurMs         = 500;
static volatile bool     s_enPulso          = false;
static volatile uint32_t s_inicioMs         = 0;
static volatile bool     s_modoEspejo       = false;
static volatile bool     s_salidaActivoAlto = false;

void IRAM_ATTR isr_validador() {
  bool     estado = (bool)digitalRead(PIN_VALIDATOR_IN);
  uint32_t ahora  = (uint32_t)millis();

  if (s_modoEspejo) {
    // Modo espejo: la salida sigue a la entrada en tiempo real.
    // inputActivo=true → output activo; false → output inactivo.
    // outLevel = 1 cuando (inputActivo == s_salidaActivoAlto).
    bool inputActivo = !estado;  // entrada activa en bajo
    gpio_set_level((gpio_num_t)PIN_VMC_OUT,
                   (inputActivo == s_salidaActivoAlto) ? 1 : 0);

    // Medir duración para aprendizaje y conteo.
    if (inputActivo) {
      s_inicioMs = ahora;
      s_enPulso  = true;
    } else if (s_enPulso) {
      uint32_t dur = ahora - s_inicioMs;
      if (dur >= 100 && dur <= 550) {
        pulseDuration = dur;
        g_cospelIn    = true;
      }
      s_enPulso = false;
    }
    return;
  }

  // Modo normal: detectar pulso válido y señalizar al loop principal.
  if (!estado) {
    s_inicioMs = ahora;
    s_enPulso  = true;
  } else if (s_enPulso) {
    uint32_t dur = ahora - s_inicioMs;
    if (dur >= 100 && dur <= 550) {
      pulseDuration = dur;
      g_cospelIn    = true;
    }
    s_enPulso = false;
  }
}

void validator_setup() {
  validator_update_config();
  pinMode(PIN_VALIDATOR_IN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_VALIDATOR_IN), isr_validador, CHANGE);
}

void validator_update_config() {
  noInterrupts();
  s_activoAlto       = Misvalores.Entrada_Activo_Alto;
  s_minDurMs         = Misvalores.Entrada_Duracion_ms;
  s_maxDurMs         = Misvalores.Entrada_Max_Duracion_ms;
  s_modoEspejo       = Misvalores.Modo_Espejo;
  s_salidaActivoAlto = Misvalores.Salida_Activo_Alto;
  interrupts();
}
