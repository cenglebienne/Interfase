#pragma once
#include <stdint.h>

// ── Entradas ─────────────────────────────────────────────────────────────────
constexpr int PIN_VALIDATOR_IN  = 23;  // Pulso desde el validador
constexpr int PIN_VMC_ENABLE_IN = 18;  // Habilitación desde la VMC (LOW = habilitado)
constexpr int PIN_BUTTON        =  21;  // Pulsador Reset/AP (pull-up interno, activo LOW)

// ── Salidas ───────────────────────────────────────────────────────────────────
constexpr int PIN_VMC_OUT       = 22;  // Pulso hacia la VMC (Modo 1 – Interface)
constexpr int PIN_TIMER_OUT     = 22;  // Salida temporizada (Modo 2 – Timer)
constexpr int PIN_HABILITACION  =  19;  // Habilitación del validador (HIGH = deshabilitar)
constexpr int PIN_LED           =  33;  // LED de indicación (HIGH = encendido)
constexpr int PIN_LED_INTERNO   =  2;  // LED interno de la placa ()
constexpr int PIN_BUZZER        = 32;  // Buzzer de pago aceptado (siempre activo-ALTO, reposo LOW)

// ── Display LCD 1602 I2C ──────────────────────────────────────────────────────
// GPIO21/22 (I2C por defecto) están ocupados por el pulsador y la salida VMC.
constexpr int     PIN_LCD_SDA  = 25;
constexpr int     PIN_LCD_SCL  = 26;
constexpr uint8_t LCD_I2C_ADDR = 0x27;  // 0x3F en algunos módulos PCF8574A


// ── Lógica de habilitación VMC ────────────────────────────────────────────────
// La VMC habilita la máquina con nivel LOW en PIN_VMC_ENABLE_IN.
// Modificar esta constante si la placa usa lógica inversa.
constexpr bool VMC_ENABLE_ACTIVE_LOW = true;
