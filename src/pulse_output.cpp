#include "pulse_output.h"
#include "app_state.h"
#include "pins.h"
#include "constants.h"

static uint32_t s_inicioMs   = 0;
static uint32_t s_duracionMs = 0;
static bool     s_activo     = false;
static int      s_pinActivo  = PIN_VMC_OUT;
static uint8_t  s_pulsosRestantes = 0;  // pulsos que faltan emitir tras el actual (Modo Interface)
static bool     s_enGap           = false;  // true = en el intervalo entre pulsos del tren
static uint8_t  s_buzzerFases     = 0;  // fases restantes (ON/OFF alternadas); 0 = inactivo
static uint32_t s_buzzerInicioMs  = 0;

// ── Buzzer de pago aceptado ───────────────────────────────────────────────────
// BUZZER_PULSOS beeps de BUZZER_PULSO_MS separados por BUZZER_PAUSA_MS, en todos
// los modos. Siempre activo-ALTO: no depende de Salida_Activo_Alto, así queda
// apagado en reposo.
void buzzer_beep() {
  digitalWrite(PIN_BUZZER, HIGH);
  s_buzzerFases    = BUZZER_PULSOS * 2 - 1;  // ON, OFF, ON, ...
  s_buzzerInicioMs = millis();
}

static void buzzer_poll() {
  if (s_buzzerFases == 0) return;
  // Fases impares restantes = buzzer encendido; pares = pausa.
  bool     encendido = (s_buzzerFases & 1);
  uint32_t duracion  = encendido ? BUZZER_PULSO_MS : BUZZER_PAUSA_MS;
  if ((millis() - s_buzzerInicioMs) < duracion) return;

  s_buzzerFases--;
  s_buzzerInicioMs = millis();
  digitalWrite(PIN_BUZZER, (s_buzzerFases & 1) ? HIGH : LOW);
}

// ── Tren de pulsos (cliente específico) ───────────────────────────────────────
// Activar con  -DPULSO_TREN_VALIDADOR  en build_flags de platformio.ini.
// Señal activo-ALTO capturada del validador: 22 transiciones en ~5.2 ms.
#ifdef PULSO_TREN_VALIDADOR
static const uint32_t TREN_PASOS_US[] = {
  159, 150,
   62, 151,
   63, 151,
   63, 150,
   62, 1156,
   63, 150,
  705, 1261,
   63, 257,
   63, 150,
   63, 151,
   63
};
static const uint16_t TREN_N = sizeof(TREN_PASOS_US) / sizeof(TREN_PASOS_US[0]);

static void emitir_tren() {
  // Polaridad hardcodeada: activo-ALTO confirmado para este cliente.
  // No depende de Misvalores.Salida_Activo_Alto para evitar errores de configuración.
  bool act = true;
  for (uint16_t i = 0; i < TREN_N; i++) {
    digitalWrite(PIN_VMC_OUT, act ? HIGH : LOW);
    delayMicroseconds(TREN_PASOS_US[i]);
    act = !act;
  }
  digitalWrite(PIN_VMC_OUT, LOW); // reposo = LOW (activo-ALTO)
}
#endif

// Lleva el pin al nivel activo o inactivo según la polaridad configurada.
// La etapa de salida (opto 4N25 + pull-up) invierte la señal entre el GPIO
// y el terminal físico, por lo que aquí se escribe el nivel contrario al
// que Salida_Activo_Alto describe: así "Activo Alto" refleja lo que se mide
// en el conector (reposo 0V / pulso +12V), no el nivel interno del ESP32.
static void set_nivel(int pin, bool activar) {
  digitalWrite(pin, activar == Misvalores.Salida_Activo_Alto ? LOW : HIGH);
}

void pulse_output_setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);  // buzzer apagado en reposo
  pinMode(PIN_VMC_OUT,   OUTPUT);
  pinMode(PIN_TIMER_OUT, OUTPUT);
  digitalWrite(PIN_VMC_OUT, LOW);
  digitalWrite(PIN_HABILITACION, LOW);

  set_nivel(PIN_VMC_OUT,   false);
  set_nivel(PIN_TIMER_OUT, false);
}

void pulse_output_trigger(uint32_t duracion_ms) {
  bool modoTimer = Misvalores.Modo_Timer;

  if (modoTimer) {
    // Un nuevo evento durante la cuenta regresiva reinicia el tiempo desde cero
    // en vez de ser ignorado.
    buzzer_beep();  // también en cada reinicio de la cuenta regresiva
    if (!s_activo) {
      s_pinActivo = PIN_TIMER_OUT;
      set_nivel(PIN_TIMER_OUT, true);
      s_activo     = true;
      g_enServicio = true;
    }
    s_inicioMs = millis();
    return;
  }

#ifdef PULSO_TREN_VALIDADOR
  // Modo interfase con tren de pulsos (cliente específico).
  // Se emite de forma bloqueante (~5 ms); el flag g_enServicio protege
  // contra un segundo disparo mientras dura.
  if (s_activo) return;
  g_enServicio = true;
  buzzer_beep();
  emitir_tren();
  g_enServicio = false;
  if (g_reinicio_pendiente) {
    g_reinicio_pendiente = false;
    ESP.restart();
  }
  return;
#endif

  // Modo interfase normal: tren de 1 o más pulsos, no bloqueante.
  if (s_activo) return;
  s_pinActivo  = PIN_VMC_OUT;
  s_duracionMs = duracion_ms ? duracion_ms : (uint32_t)Misvalores.Salida_Duracion_ms;
  uint8_t cantidad  = Misvalores.Salida_Cantidad_Pulsos ? Misvalores.Salida_Cantidad_Pulsos : 1;
  s_pulsosRestantes = cantidad - 1;
  s_enGap           = false;
  set_nivel(PIN_VMC_OUT, true);
  buzzer_beep();
  s_inicioMs   = millis();
  s_activo     = true;
  g_enServicio = true;
}

void pulse_output_poll() {
  buzzer_poll();  // independiente del pulso: el beep puede durar más o menos que la salida
  if (!s_activo) return;

  bool modoTimer = (Misvalores.Modo_Timer);

  if (!modoTimer) {
    if ((millis() - s_inicioMs) < s_duracionMs) return;

    if (!s_enGap) {
      // Termina el pulso activo.
      set_nivel(PIN_VMC_OUT, false);
      if (s_pulsosRestantes > 0) {
        s_pulsosRestantes--;
        s_enGap    = true;
        s_inicioMs = millis();
        return;
      }
    } else {
      // Termina el intervalo entre pulsos: arranca el siguiente pulso.
      set_nivel(PIN_VMC_OUT, true);
      s_enGap    = false;
      s_inicioMs = millis();
      return;
    }

    // Tren de pulsos completo.
    s_activo     = false;
    g_enServicio = false;

    if (g_reinicio_pendiente) {
      g_reinicio_pendiente = false;
      ESP.restart();
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
