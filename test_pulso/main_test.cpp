/*
 * PRUEBA DE CALIBRACIÓN DE PULSO HACIA VMC
 * -----------------------------------------
 * Envía pulsos de ancho creciente en PIN_VMC_OUT para determinar
 * qué duración reconoce la VMC como una ficha válida.
 *
 * Controles por monitor serie (115200 baud, NL al final):
 *   ENTER        → enviar el pulso mostrado y avanzar al siguiente ancho
 *   r + ENTER    → repetir el ancho actual sin avanzar
 *   <número>     → saltar a ese ancho exacto (ej: "250" → 250 ms)
 *   q + ENTER    → volver al primer ancho
 *
 * PARA BORRAR ESTA PRUEBA:
 *   1. Eliminar la carpeta  test_pulso/
 *   2. Eliminar el bloque   [env:test_pulso]  de platformio.ini
 */

#include <Arduino.h>
#include "pins.h"

// ── Parámetros ────────────────────────────────────────────────────────────────
static const uint32_t PULSO_INICIO_MS    =  1;   // primer ancho a probar (ms)
static const uint32_t PULSO_FIN_MS       = 800;   // último ancho a probar (ms)
static const uint32_t PASO_MS            =  5;   // incremento entre intentos (ms)
static const bool     SALIDA_ACTIVO_ALTO = false; // false = activo bajo (default del hardware)

// ── Estado ────────────────────────────────────────────────────────────────────
static uint32_t s_ancho = PULSO_INICIO_MS;

// ── Helpers ───────────────────────────────────────────────────────────────────
static int nivelActivo()   { return SALIDA_ACTIVO_ALTO ? HIGH : LOW;  }
static int nivelInactivo() { return SALIDA_ACTIVO_ALTO ? LOW  : HIGH; }

static void enviarPulso(uint32_t ms) {
  digitalWrite(PIN_VMC_OUT, nivelActivo());
  delay(ms);
  digitalWrite(PIN_VMC_OUT, nivelInactivo());
}

// Bloquea hasta recibir una línea completa por Serial.
static String leerLinea() {
  String s = "";
  while (true) {
    if (Serial.available()) {
      char c = (char)Serial.read();
      if (c == '\n') break;
      if (c != '\r') s += c;
    }
    yield();  // permite background tasks del ESP32
  }
  s.trim();
  return s;
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(600);

  pinMode(PIN_VMC_OUT, OUTPUT);
  digitalWrite(PIN_VMC_OUT, nivelInactivo());

  Serial.println(F("\n============================================"));
  Serial.println(F("      CALIBRACION DE PULSO HACIA VMC      "));
  Serial.println(F("============================================"));
  Serial.printf( "  Rango : %u – %u ms   |   Paso : %u ms\n",
                 PULSO_INICIO_MS, PULSO_FIN_MS, PASO_MS);
  Serial.printf( "  Salida: activo %s  (GPIO %d)\n",
                 SALIDA_ACTIVO_ALTO ? "ALTO" : "BAJO", PIN_VMC_OUT);
  Serial.println(F("--------------------------------------------"));
  Serial.println(F("  ENTER       enviar y avanzar al siguiente"));
  Serial.println(F("  r + ENTER   repetir el ancho actual"));
  Serial.println(F("  <ms>        saltar a ese ancho (ej: 350)"));
  Serial.println(F("  q + ENTER   reiniciar desde el inicio"));
  Serial.println(F("============================================\n"));
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
  if (s_ancho > PULSO_FIN_MS) {
    Serial.println(F("\n>>> Fin del rango. Reiniciando desde el inicio.\n"));
    s_ancho = PULSO_INICIO_MS;
  }

  Serial.printf("[%4u ms]  > ", s_ancho);
  String cmd = leerLinea();

  if (cmd.equalsIgnoreCase("q")) {
    s_ancho = PULSO_INICIO_MS;
    Serial.println(F("  → Reiniciando.\n"));
    return;
  }

  bool repetir = cmd.equalsIgnoreCase("r");
  if (!repetir && cmd.length() > 0) {
    uint32_t val = (uint32_t)cmd.toInt();
    if (val > 0) {
      s_ancho = val;
      Serial.printf("  → Saltando a %u ms\n", s_ancho);
    }
  }

  Serial.printf("  Enviando %u ms ... ", s_ancho);
  enviarPulso(s_ancho);
  Serial.println(F("listo."));

  if (!repetir) s_ancho += PASO_MS;
}
