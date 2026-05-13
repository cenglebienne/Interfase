/*
 * REPLAY DE TREN DE PULSOS HACIA VMC
 * ------------------------------------
 * Reproduce en PIN_VMC_OUT el tren de pulsos capturado del validador para
 * comprobar si la VMC lo reconoce como ficha válida.
 *
 * El tren tiene 22 transiciones en ~5.2 ms (11 pulsos activos, señal ALTO).
 * La tabla PASOS_US almacena el tiempo en µs de cada estado antes de cambiar.
 * El primer estado es ACTIVO (igual que la captura del validador).
 *
 * Controles por monitor serie (115200 baud, NL al final):
 *   ENTER        → enviar el tren y esperar resultado
 *   <número>     → repetir N veces seguidas (ej: "5")
 *   q + ENTER    → reiniciar conteo
 *
 * PARA BORRAR ESTA PRUEBA:
 *   1. Eliminar la carpeta  test_replay/
 *   2. Eliminar el bloque   [env:test_replay]  de platformio.ini
 */

#include <Arduino.h>
#include "pins.h"

// ── Parámetros ────────────────────────────────────────────────────────────────
static const bool SALIDA_ACTIVO_ALTO = true ; // false = activo bajo en VMC_OUT

// Pausa entre envíos consecutivos cuando se piden múltiples repeticiones (ms)
static const uint32_t PAUSA_ENTRE_ENVIOS_MS = 500;

// ── Tren capturado ────────────────────────────────────────────────────────────
// Duración (µs) de cada estado, comenzando por ACTIVO y alternando.
// 21 duraciones → 22 transiciones → el pin termina en REPOSO.
//
//  Estado:   ACT  REP  ACT  REP  ACT  REP  ACT  REP  ACT   REP   ACT  REP   ACT    REP    ACT  REP  ACT  REP  ACT  REP  ACT
static const uint32_t PASOS_US[] = {
  159, 150,   // pulso 1 (159µs) + hueco
   62, 151,   // pulso 2 ( 62µs) + hueco
   63, 151,   // pulso 3 ( 63µs) + hueco
   63, 150,   // pulso 4 ( 63µs) + hueco
   62, 1156,  // pulso 5 ( 62µs) + hueco largo
   63, 150,   // pulso 6 ( 63µs) + hueco
  705, 1261,  // pulso 7 (705µs) + hueco largo
   63, 257,   // pulso 8 ( 63µs) + hueco
   63, 150,   // pulso 9 ( 63µs) + hueco
   63, 151,   // pulso 10( 63µs) + hueco
   63          // pulso 11( 63µs) → fin, el pin queda en REPOSO
};
static const uint16_t N_PASOS = sizeof(PASOS_US) / sizeof(PASOS_US[0]); // 21

// Duración total estimada del tren (µs)
static const uint32_t DURACION_TOTAL_US = 5156;

// ── Contadores ────────────────────────────────────────────────────────────────
static uint32_t s_envios = 0;

// ── Helpers ───────────────────────────────────────────────────────────────────
static inline int nivelActivo() { return SALIDA_ACTIVO_ALTO ? HIGH : LOW; }
static inline int nivelReposo() { return SALIDA_ACTIVO_ALTO ? LOW  : HIGH; }

// Bloquea hasta recibir una línea completa por Serial.
static String leerLinea() {
  String s = "";
  while (true) {
    if (Serial.available()) {
      char c = (char)Serial.read();
      if (c == '\n') break;
      if (c != '\r') s += c;
    }
    yield();
  }
  s.trim();
  return s;
}

// ── Emisión del tren ──────────────────────────────────────────────────────────
static void emitirTren() {
  bool esActivo = true;
  for (uint16_t i = 0; i < N_PASOS; i++) {
    digitalWrite(PIN_VMC_OUT, esActivo ? nivelActivo() : nivelReposo());
    delayMicroseconds(PASOS_US[i]);
    esActivo = !esActivo;
  }
  // Garantizar que el pin queda en reposo al terminar
  digitalWrite(PIN_VMC_OUT, nivelReposo());
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(600);

  pinMode(PIN_VMC_OUT, OUTPUT);
  digitalWrite(PIN_VMC_OUT, nivelReposo());

  Serial.println(F("\n============================================"));
  Serial.println(F("     REPLAY DE TREN DE PULSOS → VMC       "));
  Serial.println(F("============================================"));
  Serial.printf( "  Salida: activo %s  (GPIO %d)\n",
                 SALIDA_ACTIVO_ALTO ? "ALTO" : "BAJO", PIN_VMC_OUT);
  Serial.printf( "  Tren: %u transiciones  |  ~%.2f ms total\n",
                 N_PASOS + 1, DURACION_TOTAL_US / 1000.0f);
  Serial.printf( "  Pausa entre envíos: %u ms\n", PAUSA_ENTRE_ENVIOS_MS);
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("  ENTER       enviar un tren"));
  Serial.println(F("  <n>         enviar n veces seguidas"));
  Serial.println(F("  q + ENTER   reiniciar conteo"));
  Serial.println(F("============================================\n"));
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
  Serial.printf("[envíos: %lu]  > ", s_envios);
  String cmd = leerLinea();

  if (cmd.equalsIgnoreCase("q")) {
    s_envios = 0;
    Serial.println(F("  → Conteo reiniciado.\n"));
    return;
  }

  uint32_t repeticiones = 1;
  if (cmd.length() > 0) {
    uint32_t val = (uint32_t)cmd.toInt();
    if (val > 0) repeticiones = val;
  }

  for (uint32_t i = 0; i < repeticiones; i++) {
    s_envios++;
    Serial.printf("  #%lu  Enviando tren ... ", s_envios);
    emitirTren();
    Serial.println(F("listo."));

    if (i + 1 < repeticiones) {
      delay(PAUSA_ENTRE_ENVIOS_MS);
    }
  }
  Serial.println();
}
