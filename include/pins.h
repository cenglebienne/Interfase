#pragma once

// ── Entradas ─────────────────────────────────────────────────────────────────
constexpr int PIN_VALIDATOR_IN  = 23;  // Pulso desde el validador
constexpr int PIN_VMC_ENABLE_IN = 18;  // Habilitación desde la VMC (LOW = habilitado)
constexpr int PIN_BUTTON        =  21;  // Pulsador Reset/AP (pull-up interno, activo LOW)

// ── Salidas ───────────────────────────────────────────────────────────────────
constexpr int PIN_VMC_OUT       = 22;  // Pulso hacia la VMC (Modo 1 – Interface)
constexpr int PIN_TIMER_OUT     = 22;  // Salida temporizada (Modo 2 – Timer)
constexpr int PIN_HABILITACION  =  19;  // Habilitación del validador (HIGH = deshabilitar)
constexpr int PIN_LED           =  33;  // LED de indicación (HIGH = encendido)



// ── Lógica de habilitación VMC ────────────────────────────────────────────────
// La VMC habilita la máquina con nivel LOW en PIN_VMC_ENABLE_IN.
// Modificar esta constante si la placa usa lógica inversa.
constexpr bool VMC_ENABLE_ACTIVE_LOW = true;
