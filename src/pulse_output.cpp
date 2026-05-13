#include "pulse_output.h"
#include "app_state.h"
#include "pins.h"
#include "constants.h"

static uint32_t s_inicioMs  = 0;
static bool     s_activo    = false;
static int      s_pinActivo = PIN_VMC_OUT;

// Lleva el pin al nivel activo o inactivo según la polaridad configurada.
static void set_nivel(int pin, bool activar) {
  digitalWrite(pin, activar == Misvalores.Salida_Activo_Alto ? HIGH : LOW);
}

void pulse_output_setup() {
  pinMode(PIN_VMC_OUT,   OUTPUT);
  pinMode(PIN_TIMER_OUT, OUTPUT);
  digitalWrite(PIN_VMC_OUT, LOW);
  digitalWrite(PIN_HABILITACION, LOW);

  set_nivel(PIN_VMC_OUT,   false);
  set_nivel(PIN_TIMER_OUT, false);
}

void pulse_output_trigger() {

  bool modoTimer = (Misvalores.Modo_Timer);
  if ((s_activo) && (!modoTimer)) return;  // si estoy mandando un pulso , salgo
  
  s_pinActivo = modoTimer ? PIN_TIMER_OUT : PIN_VMC_OUT;

  set_nivel(s_pinActivo, true);
  s_inicioMs  = millis();
  s_activo    = true;
  g_enServicio = true;
} 

void pulse_output_poll() {
if (!s_activo) return;

  bool modoTimer = (Misvalores.Modo_Timer);

  if (!modoTimer) {
    uint32_t duracion = (uint32_t)Misvalores.Salida_Duracion_ms;
    if ((millis() - s_inicioMs) >= duracion) {
      set_nivel(PIN_VMC_OUT, false);
      s_activo     = false;
      g_enServicio = false;

      if (g_reinicio_pendiente) {
        g_reinicio_pendiente = false;
        ESP.restart();
      }
    }
    return;
  }

  // Modo Timer: sin VMC. Habilita directamente el mecanismo (aspiradora, acceso a baños, TV, etc.)
  // mediante un pulso de duración variable (Tiempo_Programado segundos) en PIN_TIMER_OUT.
  uint32_t duracion = (uint32_t)Misvalores.Tiempo_Programado * 1000UL;
  if ((millis() - s_inicioMs) >= duracion) {
    set_nivel(PIN_TIMER_OUT, false);
    s_activo     = false;
    g_enServicio = false;

    if (g_reinicio_pendiente) {
      g_reinicio_pendiente = false;
      ESP.restart();
    }
  }
}
