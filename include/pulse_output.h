#pragma once

#include <stdint.h>

// Inicializa los pines de salida en estado inactivo.
void pulse_output_setup();

// Dispara la salida correspondiente al modo activo (Interface o Timer).
// Si ya hay un pulso en curso no hace nada.
// duracion_ms > 0 sobreescribe Misvalores.Salida_Duracion_ms para este disparo.
void pulse_output_trigger(uint32_t duracion_ms = 0);

// Gestiona el fin del pulso por timeout. Llamar desde el loop principal.
void pulse_output_poll();
