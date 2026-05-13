/*
 * CAPTURA DE PULSO DEL VALIDADOR
 * --------------------------------
 * Espera actividad en PIN_VALIDATOR_IN y registra todos los cambios de nivel
 * durante 2 segundos (con resolución de microsegundos) para poder replicar
 * la señal exacta cuando se recibe un pago online.
 *
 * Resultado por monitor serie (115200 baud):
 *   - Tabla de transiciones: tiempo desde inicio + nivel + duración
 *   - Resumen de los pulsos activos detectados
 *
 * PARA BORRAR ESTA PRUEBA:
 *   1. Eliminar la carpeta  test_captura/
 *   2. Eliminar el bloque   [env:test_captura]  de platformio.ini
 */

#include <Arduino.h>
#include "pins.h"

// ── Parámetros ────────────────────────────────────────────────────────────────
static const bool     ENTRADA_ACTIVO_ALTO  = true;   // false = activo bajo
static const uint32_t CAPTURA_DURACION_US  = 2000000; // 2 s de captura
static const uint16_t MAX_TRANSICIONES     = 500;     // techo del buffer

// ── Buffer de captura ─────────────────────────────────────────────────────────
struct Transicion {
  uint32_t tiempo_us; // µs desde el inicio de la captura
  bool     nivel;     // true = HIGH, false = LOW
};

static Transicion s_buf[MAX_TRANSICIONES];
static uint16_t   s_n = 0;

// ── Helpers ───────────────────────────────────────────────────────────────────
static inline int nivelActivo()   { return ENTRADA_ACTIVO_ALTO ? HIGH : LOW;  }
static inline int nivelReposo()   { return ENTRADA_ACTIVO_ALTO ? LOW  : HIGH; }
static inline bool esActivo(bool nivel) {
  return ENTRADA_ACTIVO_ALTO ? nivel : !nivel;
}

// ── Captura ───────────────────────────────────────────────────────────────────
static void capturarSenal() {
  s_n = 0;
  uint32_t t0   = micros();
  uint32_t fin  = t0 + CAPTURA_DURACION_US;
  bool     prev = (digitalRead(PIN_VALIDATOR_IN) == HIGH);

  // Primer estado (ya es el flanco activo que disparó la captura)
  s_buf[s_n++] = { 0, prev };

  while (micros() < fin) {
    bool actual = (digitalRead(PIN_VALIDATOR_IN) == HIGH);
    if (actual != prev) {
      uint32_t t = micros() - t0;
      s_buf[s_n++] = { t, actual };
      prev = actual;
      if (s_n >= MAX_TRANSICIONES) break;
    }
  }
}

// ── Impresión de resultados ───────────────────────────────────────────────────
static void imprimirResultados() {
  Serial.println(F("\n============================================"));
  Serial.println(F("        RESULTADO DE CAPTURA              "));
  Serial.println(F("============================================"));
  Serial.printf( "  Transiciones: %u   (máx buffer: %u)\n", s_n, MAX_TRANSICIONES);
  if (s_n >= MAX_TRANSICIONES)
    Serial.println(F("  *** BUFFER LLENO — puede haber perdido datos ***"));

  Serial.println(F("--------------------------------------------"));
  Serial.println(F("   t_inicio (µs)  | Nivel |  Duración (ms)"));
  Serial.println(F("--------------------------------------------"));

  for (uint16_t i = 0; i < s_n; i++) {
    uint32_t dur_us = (i + 1 < s_n)
                    ? s_buf[i + 1].tiempo_us - s_buf[i].tiempo_us
                    : CAPTURA_DURACION_US    - s_buf[i].tiempo_us;

    Serial.printf("  %14lu  |  %s  |  %10.3f\n",
                  (unsigned long)s_buf[i].tiempo_us,
                  s_buf[i].nivel ? "H" : "L",
                  dur_us / 1000.0f);
  }

  // ── Resumen de pulsos activos ──
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("  PULSOS ACTIVOS detectados:"));
  uint8_t cuenta = 0;
  for (uint16_t i = 0; i < s_n; i++) {
    if (!esActivo(s_buf[i].nivel)) continue;
    uint32_t fin_us = (i + 1 < s_n) ? s_buf[i + 1].tiempo_us : CAPTURA_DURACION_US;
    uint32_t dur_us = fin_us - s_buf[i].tiempo_us;
    Serial.printf("    #%u  inicio: %lu µs  |  duración: %.3f ms\n",
                  ++cuenta,
                  (unsigned long)s_buf[i].tiempo_us,
                  dur_us / 1000.0f);
  }
  if (!cuenta) Serial.println(F("    (ninguno — revisar polarity)"));

  Serial.println(F("============================================"));
  Serial.println(F("Esperando siguiente ficha...\n"));
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(600);

  pinMode(PIN_VALIDATOR_IN, INPUT);

  Serial.println(F("\n============================================"));
  Serial.println(F("   CAPTURA DE PULSO DEL VALIDADOR         "));
  Serial.println(F("============================================"));
  Serial.printf( "  Entrada: activo %s  (GPIO %d)\n",
                 ENTRADA_ACTIVO_ALTO ? "ALTO" : "BAJO", PIN_VALIDATOR_IN);
  Serial.printf( "  Ventana de captura: %.1f s\n",
                 CAPTURA_DURACION_US / 1000000.0f);
  Serial.printf( "  Buffer: %u transiciones\n", MAX_TRANSICIONES);
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("  Insertar ficha para capturar la señal."));
  Serial.println(F("============================================\n"));
  Serial.println(F("Esperando..."));
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
  if (digitalRead(PIN_VALIDATOR_IN) == nivelActivo()) {
    // Capturar inmediatamente para no perder el inicio del pulso
    capturarSenal();
    Serial.printf("[%lu ms] >> Captura completada (%u transiciones).\n",
                  millis(), s_n);
    imprimirResultados();

    // Esperar reposo antes de habilitar la próxima detección
    while (digitalRead(PIN_VALIDATOR_IN) == nivelActivo()) yield();
    delay(300);
  }
  yield();
}
