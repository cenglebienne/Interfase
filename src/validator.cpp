#include "validator.h"
#include "app_state.h"
#include "pins.h"
#include "constants.h"
#include "driver/gpio.h"

// Variables sombra para el ISR (no se puede acceder a Misvalores desde IRAM).
static volatile bool     s_activoAlto       = false;
static volatile uint32_t s_refDurMs         = DEFAULT_INPUT_DURATION_MS;
static volatile bool     s_enPulso          = false;
static volatile uint32_t s_inicioMs         = 0;
static volatile bool     s_modoEspejo       = false;
static volatile bool     s_salidaActivoAlto = false;

// Calcula el rango de aceptación: ±10% para ref > 100 ms, ±20% para ref ≤ 100 ms.
static inline void IRAM_ATTR dur_range(uint32_t ref, uint32_t &lo, uint32_t &hi) {
  uint32_t tol = (ref > 100) ? ref / 10 : ref / 5;
  lo = (ref > tol) ? ref - tol : 0;
  hi = ref + tol;
}

void IRAM_ATTR isr_validador() {
  bool     estado = (bool)digitalRead(PIN_VALIDATOR_IN);
  uint32_t ahora  = (uint32_t)millis();

  // true cuando el pin está en el nivel que se considera "activo".
  bool inputActivo = s_activoAlto ? estado : !estado;

  if (s_modoEspejo) {
    // Modo espejo: la salida sigue a la entrada en tiempo real.
    // Mismo pin/etapa de salida que set_nivel() en pulse_output.cpp: el opto
    // 4N25 + pull-up invierte GPIO↔terminal, por eso se escribe en el GPIO el
    // nivel contrario al que s_salidaActivoAlto describe para el terminal.
    gpio_set_level((gpio_num_t)PIN_VMC_OUT,
                   (inputActivo == s_salidaActivoAlto) ? 0 : 1);

    // Medir duración para conteo (en ms; pulsos sub-ms quedan en 0 ms).
    if (inputActivo) {
      s_inicioMs = ahora;
      s_enPulso  = true;
    } else if (s_enPulso) {
      uint32_t dur = ahora - s_inicioMs;
      uint32_t lo, hi;
      dur_range(s_refDurMs, lo, hi);
      if (dur >= lo && dur <= hi) {
        pulseDuration = dur;
        g_cospelIn    = true;
      }
      s_enPulso = false;
    }
    return;
  }

  // Modo normal: detectar pulso válido y señalizar al loop principal.
  if (inputActivo) {
    s_inicioMs = ahora;
    s_enPulso  = true;
  } else if (s_enPulso) {
    uint32_t dur = ahora - s_inicioMs;
    uint32_t lo, hi;
    if (g_modoAprendizaje) {
      g_cospelIn    = true;
      pulseDuration = dur;
    } 

    dur_range(s_refDurMs, lo, hi);
    if (dur >= lo && dur <= hi) {
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
  s_refDurMs         = Misvalores.Entrada_Duracion_ms;
  s_modoEspejo       = Misvalores.Modo_Espejo && !Misvalores.Modo_Timer;
  s_salidaActivoAlto = Misvalores.Salida_Activo_Alto;
  interrupts();
}
