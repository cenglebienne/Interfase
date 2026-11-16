#pragma once

// Actualización remota (OTA por HTTP) ordenada desde el servidor con los
// comandos MQTT 'U' (firmware) y 'W' (filesystem), que guardan la URL y el
// flag pendiente en Miupdate (ver lib_shared/mqtt_handlers).
//
// Llamar desde el loop principal. Solo actúa si hay una actualización
// pendiente, WiFi conectado y ningún servicio en curso. Es bloqueante mientras
// descarga; si termina bien reinicia el equipo. No compara versiones: instala
// lo que indique la URL (permite subir o bajar de versión).
void ota_remoto_poll();
