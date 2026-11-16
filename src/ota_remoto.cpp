#include "ota_remoto.h"
#include "app_state.h"
#include "memory_api.h"
#include "logger.h"
#include "pins.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <Update.h>
#include <LittleFS.h>
#include <AsyncMqttClient.h>
#include <esp_ota_ops.h>

extern AsyncMqttClient mqttClient;

static void limpiar_pendiente(bool isFirmware) {
  if (isFirmware) Miupdate.updateFirmwarePendiente   = false;
  else            Miupdate.updateFilesystemPendiente = false;
  Save_Flash_Actualizar();
}

// Descarga la imagen de filesystem y la escribe en la partición de datos.
static bool actualizar_filesystem(const String &url) {
  WiFiClient client;
  HTTPClient http;
  bool ok = false;
  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();
    if (Update.begin(contentLength < 0 ? UPDATE_SIZE_UNKNOWN : (size_t)contentLength, U_SPIFFS)) {
      size_t written = Update.writeStream(*http.getStreamPtr());
      if (Update.end(true)) {
        Serial.printf("[OTA-FS] OK: %u bytes escritos.\n", (unsigned)written);
        ok = true;
      } else {
        Serial.printf("[OTA-FS] Update.end() error: %s\n", Update.errorString());
      }
    } else {
      Serial.printf("[OTA-FS] Update.begin() error: %s\n", Update.errorString());
    }
  } else {
    Serial.printf("[OTA-FS] HTTP GET fallo: codigo %d\n", httpCode);
  }
  http.end();
  return ok;
}

void ota_remoto_poll() {
  bool isFirmware   = Miupdate.updateFirmwarePendiente;
  bool isFilesystem = !isFirmware && Miupdate.updateFilesystemPendiente;
  if (!isFirmware && !isFilesystem) return;

  // Esperar a que termine el servicio en curso (pulso o cuenta regresiva)
  // y a tener WiFi; el flag queda guardado en flash mientras tanto.
  if (g_enServicio || WiFi.status() != WL_CONNECTED) return;

  String url = isFirmware ? String(Miupdate.urlFirmware) : String(Miupdate.urlFilesystem);
  if (url.length() == 0) {
    limpiar_pendiente(isFirmware);
    return;
  }

  Serial.printf("\n[OTA REMOTO] Descargando %s desde:\n%s\n",
                isFirmware ? "FIRMWARE" : "FILESYSTEM", url.c_str());
  logEvent("Iniciando OTA " + String(isFirmware ? "FW" : "FS") + " desde URL: " + url);

  // Deshabilitar el validador durante la descarga (bloqueante) para no
  // perder fichas que no se podrían procesar.
  digitalWrite(PIN_HABILITACION, LOW);

  delay(500);                // dejar salir la respuesta 'S' al comando U/W
  mqttClient.disconnect();   // el reconector de connections lo restablece si falla

  bool ok;
  if (isFirmware) {
    WiFiClient client;
    httpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ret = httpUpdate.update(client, url);
    ok = (ret == HTTP_UPDATE_OK);
    if (!ok) {
      String err = "OTA FW error (" + String(httpUpdate.getLastError()) + "): " +
                   httpUpdate.getLastErrorString();
      Serial.println("[OTA REMOTO] " + err);
      logEvent(err);
    } else {
      const esp_partition_t *boot = esp_ota_get_boot_partition();
      String info = "OTA FW OK, proximo arranque en " + String(boot ? boot->label : "?");
      Serial.println("[OTA REMOTO] " + info);
      logEvent(info);
    }
  } else {
    LittleFS.end();          // desmontar antes de escribir sobre la partición
    ok = actualizar_filesystem(url);
  }

  // El flag se limpia siempre, para no reintentar en bucle una URL inválida.
  limpiar_pendiente(isFirmware);

  if (ok) {
    Serial.println("[OTA REMOTO] EXITO. Reiniciando equipo...");
    if (isFirmware) logEvent("OTA Exitoso. Reiniciando.");
    delay(1000);
    ESP.restart();
  }

  Serial.println("[OTA REMOTO] Fallo en la actualizacion.");
  if (isFilesystem) {
    // LittleFS quedó desmontado: reiniciar es la salida limpia.
    delay(1000);
    ESP.restart();
  }

  logEvent("OTA FW fallido");
  // Restaurar la habilitación del validador según el estado actual de la VMC.
  digitalWrite(PIN_HABILITACION, g_vmcEnabled ? HIGH : LOW);
}
