#include "mqtt_handlers.h"
#include "memory_api.h"
#include "app_state.h"
#include "config.h"
#include "validator.h"
#include "connections.h"
#include <LittleFS.h>
#include <WiFi.h>

extern AsyncMqttClient mqttClient;

bool onMqttComandoProyecto(char cmd, char *payload, size_t len,
                            char *buf, size_t cap, size_t *inOutUsed,
                            const char *topicPTS) {
  auto ensure = [&](size_t n) -> bool {
    if (*inOutUsed + n > cap) {
      Serial.printf("MQTT proyecto: buffer lleno (need %u, free %u)\n",
                    (unsigned)n, (unsigned)(cap - *inOutUsed));
      return false;
    }
    return true;
  };
  auto ptr     = [&]() -> char * { return buf + *inOutUsed; };
  auto advance = [&](size_t n)   { *inOutUsed += n; };

  switch (cmd) {

    // ── C: Cambio de configuración (formato legacy) ───────────────────────────
    case 'C':
      Serial.println("Comando C: Cambio de configuración...");
      if (len >= 11) {
        Misvalores.Tiempo_Programado = (int32_t)convertirLittleEndianAEntero(payload + 6);
        Misvalores.Modo_Timer = (payload[10] == 'Y');
        Save_Flash_ValorServicio();
      }
      if (!ensure(1)) return true;
      *ptr() = 'S'; advance(1);
      return true;

    // ── D: Pedido de datos generales ─────────────────────────────────────────
    case 'D': {
      Serial.println("Comando D: Datos generales...");
      unsigned int verFS = 0;
      File f = LittleFS.open(FS_VERSION_FILE, "r");
      if (f) { verFS = (unsigned int)f.readString().toInt(); f.close(); }

      if (!ensure(2)) return true;
      memcpy(ptr(), &VERSION_FIRMWARE, 2); advance(2);

      if (!ensure(2)) return true;
      memcpy(ptr(), &verFS, 2); advance(2);

      if (!ensure(sizeof(uint32_t))) return true;
      uint32_t tp = (uint32_t)Misvalores.Tiempo_Programado;
      memcpy(ptr(), &tp, sizeof(tp)); advance(sizeof(tp));

      if (!ensure(1)) return true;
      *ptr() = Misvalores.Modo_Timer ? 1 : 0; advance(1);

      String mac = WiFi.macAddress();
      mac.replace(":", "");
      mac.toLowerCase();
      size_t macLen = mac.length();
      if (!ensure(macLen + 2)) return true;
      memcpy(ptr(), mac.c_str(), macLen); advance(macLen);
      buf[*inOutUsed + 1] = '\0';
      advance(2);
      return true;
    }

    // ── V: Cambio de opciones (incluye configuración de pulsos) ──────────────
    case 'V':
      Serial.println("Comando V: Cambio de opciones...");
      if (len >= 8) {
        // [5..6] flags uint16 little-endian:
        // bit0=Modo_Timer (0=Interface, 1=Timer)
        // bit1=Modo_Espejo, bit2=Opcion_Extra_2
        // [7] = Tiempo_Programado en segundos (uint8)
        uint16_t flags;
        memcpy(&flags, payload + 5, 2);
        Misvalores.Modo_Timer = (flags >> 0) & 1;
        Misvalores.Modo_Espejo    = (flags >> 1) & 1;
        Misvalores.Opcion_Extra_2    = (flags >> 2) & 1;
        Misvalores.Tiempo_Programado = (int32_t)(uint8_t)payload[7];
        Save_Flash_ValorServicio();
        if (!ensure(1)) return true;
        *ptr() = 'S'; advance(1);
        if (g_enServicio) {
          Serial.println("Comando V diferido: esperando fin de servicio.");
          g_reinicio_pendiente = true;
        } else {
          mqttClient.publish(topicPTS, 2, false, buf, *inOutUsed);
          delay(300);
          ESP.restart();
        }
      } else {
        if (!ensure(1)) return true;
        *ptr() = 'N'; advance(1);
      }
      return true;

    default:
      return false;
  }
}
