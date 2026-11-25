#pragma once

#include <stdint.h>

// Inicializa los pines de salida en estado inactivo.
void pulse_output_setup();

// Dispara la salida correspondiente al modo activo (Interface o Timer).
// Si ya hay un pulso (o tren de pulsos) en curso no hace nada.
// duracion_ms > 0 sobreescribe Misvalores.Salida_Duracion_ms para este disparo.
// En Modo Interface se emiten Misvalores.Salida_Cantidad_Pulsos pulsos.
void pulse_output_trigger(uint32_t duracion_ms = 0);

// Gestiona el fin del pulso (y el intervalo entre pulsos del tren) por timeout.
// Llamar desde el loop principal.
void pulse_output_poll();

// Emite BUZZER_PULSOS beeps de BUZZER_PULSO_MS en PIN_BUZZER, sin bloquear.
// Siempre activo-ALTO, independiente de Salida_Activo_Alto. Lo apaga pulse_output_poll().
void buzzer_beep();

// Modo Timer: milisegundos que faltan para terminar el servicio (0 si no hay servicio).
uint32_t pulse_output_restante_ms();
