#include "http_server_app.h"
#include "app_state.h"
#include "memory_api.h"
#include "validator.h"
#include "config.h"
#include <WebServer.h>
#include <LittleFS.h>
#include <WiFi.h>

static WebServer server(HTTP_PORT);

// ── Helpers ───────────────────────────────────────────────────────────────────
static void sendJson(const String &json) {
  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.send(200, "application/json", json);
}

static bool argBool(const char *name) {
  String v = server.arg(name);
  return (v == "true" || v == "1");
}

// Sirve un archivo de LittleFS con el Content-Type indicado.
static void serveFile(const char *path, const char *mime) {
  File f = LittleFS.open(path, "r");
  if (!f) { server.send(404, "text/plain", "Not found"); return; }
  server.streamFile(f, mime);
  f.close();
}

// ── GET: estadoCliente.json ───────────────────────────────────────────────────
static void handleEstadoCliente() {
  String json = "{";
  json += "\"SSID destino\":\"" + String(Mientorno.CLIENTE_WIFI_ssid) + "\",";
  json += "\"Clave\":\"****\",";
  json += "\"SSID secundario\":\"" + String(Mientorno.CLIENTE_WIFI2_ssid) + "\",";
  json += "\"Clave secundaria\":\"****\",";
  json += "\"MAC Address\":\"" + WiFi.macAddress() + "\",";
  json += "\"IP asignada\":\"" + WiFi.localIP().toString() + "\"";
  json += "}";
  sendJson(json);
}

// ── GET: estadoMQTT.json ──────────────────────────────────────────────────────
static void handleEstadoMQTT() {
  String json = "{";
  json += "\"Servidor\":\"" + String(Mientorno.MQTT_servidor) + "\",";
  json += "\"Topic\":\"" + String(Mientorno.MQTT_topic) + "\",";
  json += "\"Usuario\":\"" + String(Mientorno.MQTT_usuario) + "\",";
  json += "\"Clave\":\"****\"";
  json += "}";
  sendJson(json);
}

// ── GET: estadoEquipo.json ────────────────────────────────────────────────────
static void handleEstadoEquipo() {
  String json = "{\"Serie\":\"" + String(Mientorno.EQUIPO_serie) + "\"}";
  sendJson(json);
}

// ── GET: estadoContadores.json ────────────────────────────────────────────────
static void handleEstadoContadores() {
  String json = "{";
  json += "\"Cont_Parcial_Cospeles\":" + String(Miscontadores.Cont_Parcial_Cospeles) + ",";
  json += "\"Cont_Total_Cospeles\":"   + String(Miscontadores.Cont_Total_Cospeles)   + ",";
  json += "\"Cont_Parcial_OnLine\":"   + String(Miscontadores.Cont_Parcial_OnLine)   + ",";
  json += "\"Cont_Total_Online\":"     + String(Miscontadores.Cont_Total_Online);
  json += "}";
  sendJson(json);
}

// ── GET: estadoValores.json ───────────────────────────────────────────────────
static void handleEstadoValores() {
  String json = "{";
  json += "\"modoTimer\":"           + String(Misvalores.Modo_Timer ? "true" : "false") + ",";
  json += "\"entradaMaxDurMs\":"     + String(Misvalores.Entrada_Max_Duracion_ms) + ",";
  json += "\"entradaActivoAlto\":"   + String(Misvalores.Entrada_Activo_Alto ? "true" : "false") + ",";
  json += "\"salidaDurMs\":"         + String(Misvalores.Salida_Duracion_ms) + ",";
  json += "\"salidaActivoAlto\":"    + String(Misvalores.Salida_Activo_Alto ? "true" : "false") + ",";
  json += "\"modoEspejo\":"          + String(Misvalores.Modo_Espejo ? "true" : "false") + ",";
  json += "\"tiempoProgramado\":"    + String(Misvalores.Tiempo_Programado);
  json += "}";
  sendJson(json);
}

// ── GET: estadoAcerca.json ────────────────────────────────────────────────────
static void handleEstadoAcerca() {
  unsigned int verFS = 0;
  File f = LittleFS.open(FS_VERSION_FILE, "r");
  if (f) { verFS = f.readString().toInt(); f.close(); }

  String json = "{";
  json += "\"Producto\":\"" PRODUCTO "\",";
  json += "\"Version Firmware\":" + String(VERSION_FIRMWARE) + ",";
  json += "\"Version Filesystem\":" + String(verFS);
  json += "}";
  sendJson(json);
}

// ── GET: APdisponibles.json ───────────────────────────────────────────────────
static void handleAPDisponibles() {
  int n = WiFi.scanNetworks();
  String json = "[";
  for (int i = 0; i < n; i++) {
    if (i) json += ",";
    json += "{\"SSID\":\"" + WiFi.SSID(i) + "\",";
    json += "\"potencia\":" + String(WiFi.RSSI(i)) + ",";
    json += "\"encriptacion\":" + String((int)WiFi.encryptionType(i)) + "}";
  }
  json += "]";
  sendJson(json);
}

// ── POST: grabarConfigCliente.php ─────────────────────────────────────────────
static void handleGrabarConfigCliente() {
  strlcpy(Mientorno.CLIENTE_WIFI_ssid,   server.arg("ssidDestino").c_str(),   CAMPOS_CONFIG_LENGTH);
  strlcpy(Mientorno.CLIENTE_WIFI_clave,  server.arg("clave_cliente").c_str(),  CAMPOS_CONFIG_LENGTH);
  strlcpy(Mientorno.CLIENTE_WIFI2_ssid,  server.arg("ssidDestino2").c_str(),  CAMPOS_CONFIG_LENGTH);
  strlcpy(Mientorno.CLIENTE_WIFI2_clave, server.arg("clave_cliente2").c_str(), CAMPOS_CONFIG_LENGTH);
  Mientorno.CLIENTE_WIFI_Habilitado = argBool("habilitadoWiFi");
  Save_Flash_Entorno();
  server.send(200, "text/plain", "ok");
}

// ── POST: grabarConfigMQTT.php ────────────────────────────────────────────────
static void handleGrabarConfigMQTT() {
  strlcpy(Mientorno.MQTT_servidor, server.arg("servidorMQTT").c_str(), 64);
  strlcpy(Mientorno.MQTT_topic,    server.arg("pathMQTT").c_str(),     64);
  strlcpy(Mientorno.MQTT_usuario,  server.arg("usuarioMQTT").c_str(),  32);
  strlcpy(Mientorno.MQTT_clave,    server.arg("claveMQTT").c_str(),    32);
  Save_Flash_Entorno();
  server.send(200, "text/plain", "ok");
}

// ── POST: grabarConfigEquipo.php ──────────────────────────────────────────────
static void handleGrabarConfigEquipo() {
  strlcpy(Mientorno.EQUIPO_serie, server.arg("serieEquipo").c_str(), 32);
  Save_Flash_Entorno();
  server.send(200, "text/plain", "ok");
}

// ── POST: grabarConfigContadores.php ─────────────────────────────────────────
static void handleGrabarConfigContadores() {
  Miscontadores.Cont_Parcial_Cospeles = server.arg("Cont_Parcial_Cospeles").toInt();
  Miscontadores.Cont_Total_Cospeles   = server.arg("Cont_Total_Cospeles").toInt();
  Miscontadores.Cont_Parcial_OnLine   = server.arg("Cont_Parcial_OnLine").toInt();
  Miscontadores.Cont_Total_Online     = server.arg("Cont_Total_Online").toInt();
  Save_Flash_Contadores();
  server.send(200, "text/plain", "ok");
}

// ── POST: grabarConfigValores.php ─────────────────────────────────────────────
static void handleGrabarConfigValores() {
  Misvalores.Modo_Timer              = argBool("modoTimer");
  Misvalores.Entrada_Max_Duracion_ms = (uint16_t)server.arg("entradaMaxDurMs").toInt();
  Misvalores.Entrada_Activo_Alto     = argBool("entradaActivoAlto");
  Misvalores.Salida_Duracion_ms      = (uint16_t)server.arg("salidaDurMs").toInt();
  Misvalores.Salida_Activo_Alto      = argBool("salidaActivoAlto");
  Misvalores.Modo_Espejo             = argBool("modoEspejo");
  Misvalores.Tiempo_Programado       = server.arg("tiempoProgramado").toInt();
  Save_Flash_ValorServicio();
  validator_update_config();  // sincroniza ISR con los nuevos valores
  server.send(200, "text/plain", "ok");
}

// ── POST: reiniciar.php ───────────────────────────────────────────────────────
static void handleReiniciar() {
  server.send(200, "text/plain", "ok");
  delay(300);
  ESP.restart();
}

// ── Setup y poll ──────────────────────────────────────────────────────────────
void http_server_setup() {
  // Archivos estáticos
  server.on("/",           HTTP_GET,  []{ serveFile("/index.html", "text/html"); });
  server.on("/index.html", HTTP_GET,  []{ serveFile("/index.html", "text/html"); });
  server.on("/js/script.js", HTTP_GET, []{ serveFile("/js/script.js", "application/javascript"); });
  server.on("/css/w3.css",   HTTP_GET, []{ serveFile("/css/w3.css",   "text/css"); });

  // Endpoints JSON
  server.on("/estadoCliente.json",    HTTP_GET,  handleEstadoCliente);
  server.on("/estadoMQTT.json",       HTTP_GET,  handleEstadoMQTT);
  server.on("/estadoEquipo.json",     HTTP_GET,  handleEstadoEquipo);
  server.on("/estadoContadores.json", HTTP_GET,  handleEstadoContadores);
  server.on("/estadoValores.json",    HTTP_GET,  handleEstadoValores);
  server.on("/estadoAcerca.json",     HTTP_GET,  handleEstadoAcerca);
  server.on("/APdisponibles.json",    HTTP_GET,  handleAPDisponibles);

  // Endpoints POST
  server.on("/grabarConfigCliente.php",    HTTP_POST, handleGrabarConfigCliente);
  server.on("/grabarConfigMQTT.php",       HTTP_POST, handleGrabarConfigMQTT);
  server.on("/grabarConfigEquipo.php",     HTTP_POST, handleGrabarConfigEquipo);
  server.on("/grabarConfigContadores.php", HTTP_POST, handleGrabarConfigContadores);
  server.on("/grabarConfigValores.php",    HTTP_POST, handleGrabarConfigValores);
  server.on("/reiniciar.php",              HTTP_POST, handleReiniciar);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

void http_server_poll() {
  server.handleClient();
}
