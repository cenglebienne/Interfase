#pragma once
#include <Arduino.h>

// ── Valores por defecto de pulsos ─────────────────────────────────────────────
constexpr uint16_t DEFAULT_INPUT_DURATION_MS      =  450;  // Ancho mínimo de pulso válido (ms)
constexpr uint16_t DEFAULT_OUTPUT_DURATION_MS = 500;  // Duración del pulso de salida (ms)
constexpr uint8_t  DEFAULT_OUTPUT_PULSE_COUNT =   1;  // Cantidad de pulsos de salida por ficha
constexpr int32_t  DEFAULT_TIEMPO_PROGRAMADO  =   5;  // Modo Timer: duración de salida (s)

// ── Timeouts generales ────────────────────────────────────────────────────────
constexpr uint32_t AP_TIMEOUT_MS      = 300000UL;  // 5 minutos en modo AP
constexpr uint32_t ONLINE_TIMEOUT_MS  =  60000UL;  // 60 s para servicio online
constexpr uint32_t DEBOUNCE_BTN_MS    =    100UL;  // Antirrebote pulsador

// ── Modos de operación (Modo_Timer) ──────────────────────────────────────────
constexpr uint8_t MODO_INTERFACE = 0;  // Replica pulsos del validador
constexpr uint8_t MODO_TIMER     = 1;  // Salida temporizada independiente
