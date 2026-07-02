#include "button_led.h"
#include "pins.h"
#include "constants.h"
#include "app_state.h"
#include "connections.h"
#include "memory_api.h"
#include <Arduino.h>

// ── LED ──────────────────────────────────────────────────────────────────────
// Aprendizaje → parpadea rápido 100 ms
// AP          → parpadea lento  500 ms
// MQTT        → encendido fijo
// Ninguno     → apagado

static uint32_t s_ledMs = 0;
static bool     s_ledOn = false;

// Cuando es true, btn_poll controla el LED y led_poll no interfiere.
static bool s_btnOverrideLed = false;

// ── Parpadeo de arranque (no bloqueante) ──────────────────────────────────────
// 5 parpadeos de 100 ms en PIN_LED_INTERNO; s_bootStep >= 10 = terminado.
static uint8_t  s_bootStep = 10;
static uint32_t s_bootMs   = 0;

static void boot_blink_poll() {
  if (s_bootStep >= 10) return;
  if ((millis() - s_bootMs) < 50UL) return;
  s_bootMs = millis();
  s_bootStep++;
  if (s_bootStep >= 10) {
    digitalWrite(PIN_LED_INTERNO, LOW);
  } else {
    digitalWrite(PIN_LED_INTERNO, (s_bootStep & 1) ? LOW : HIGH);
  }
}

static void led_poll() {
  if (s_btnOverrideLed) return;

  if (g_modoAprendizaje) {
    if ((millis() - s_ledMs) >= 100UL) {
      s_ledMs = millis();
      s_ledOn = !s_ledOn;
      digitalWrite(PIN_LED, s_ledOn ? HIGH : LOW);
    }
  } else if (g_levantarAP) {
    if ((millis() - s_ledMs) >= 500UL) {
      s_ledMs = millis();
      s_ledOn = !s_ledOn;
      digitalWrite(PIN_LED, s_ledOn ? HIGH : LOW);
    }
  } else if (g_conectadoMQTT) {
    if (!s_ledOn) { s_ledOn = true; digitalWrite(PIN_LED, HIGH); }
  } else {
    if (s_ledOn)  { s_ledOn = false; digitalWrite(PIN_LED, LOW); }
  }
}

// ── BOTÓN ────────────────────────────────────────────────────────────────────
// Comportamiento según tiempo de pulsación (se evalúa al soltar):
//   < 3 s              → reset normal
//   3 s … 10 s         → modo AP  (LED parpadea a 500 ms durante la espera)
//   ≥ 10 s             → modo aprendizaje (LED parpadea a 100 ms durante la espera)

static bool     s_btnPrevLow = false;
static uint32_t s_btnPressMs = 0;

static void btn_poll() {
  bool     presionado = (digitalRead(PIN_BUTTON) == LOW);
  uint32_t ahora      = millis();

  // ── Flanco de bajada ──────────────────────────────────────────────────────
  if (presionado && !s_btnPrevLow) {
    s_btnPressMs     = ahora;
    s_btnPrevLow     = true;
    s_btnOverrideLed = false;
    return;
  }

  // ── Botón mantenido: feedback visual progresivo ───────────────────────────
  if (presionado && s_btnPrevLow) {
    uint32_t dur = ahora - s_btnPressMs;
    if (dur >= 3000UL) {
      s_btnOverrideLed     = true;
      uint32_t intervalo   = (dur >= 10000UL) ? 100UL : 500UL;
      if ((ahora - s_ledMs) >= intervalo) {
        s_ledMs = ahora;
        s_ledOn = !s_ledOn;
        digitalWrite(PIN_LED, s_ledOn ? HIGH : LOW);
      }
    }
    return;
  }

  // ── Flanco de subida: acción según duración ───────────────────────────────
  if (!presionado && s_btnPrevLow) {
    uint32_t dur     = ahora - s_btnPressMs;
    s_btnPrevLow     = false;
    s_btnOverrideLed = false;

    if (dur < DEBOUNCE_BTN_MS) return;  // rebote, ignorar

    if (dur >= 10000UL) {
      g_modoAprendizaje = true;
      Serial.println("Modo aprendizaje activado. Inserte una ficha.");
    } else if (dur >= 3000UL) {
      Set_Flash_Bool("Levantar_AP", true);
      ESP.restart();
    } else {
      ESP.restart();
    }
  }
}

// ── API pública ───────────────────────────────────────────────────────────────

void button_led_setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  pinMode(PIN_LED_INTERNO, OUTPUT);
  digitalWrite(PIN_LED_INTERNO, HIGH);  // primer mitad del primer parpadeo
  s_bootMs   = millis();
  s_bootStep = 0;
}

void button_led_poll() {
  boot_blink_poll();
  btn_poll();
  led_poll();
}
