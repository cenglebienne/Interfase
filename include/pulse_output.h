#pragma once

// Inicializa los pines de salida en estado inactivo.
void pulse_output_setup();

// Dispara la salida correspondiente al modo activo (Interface o Timer).
// Si ya hay un pulso en curso no hace nada.
void pulse_output_trigger();

// Gestiona el fin del pulso por timeout. Llamar desde el loop principal.
void pulse_output_poll();
