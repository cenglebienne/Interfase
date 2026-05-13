#pragma once

// Configura el interrupt y las variables sombra de polaridad/duración.
void validator_setup();

// Llamar después de modificar Misvalores para sincronizar las variables del ISR.
void validator_update_config();
