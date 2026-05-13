#include "memory_api.h"
#include "app_state.h"
#include "constants.h"
#include <Preferences.h>

// Namespace NVS — igual al de los otros proyectos para homogeneidad.
static const char *NVS_NS = "My_Flash";

// ── Lectura completa desde NVS ────────────────────────────────────────────────
void Get_Flash_All() {
  Preferences p;
  p.begin(NVS_NS, true);

  // Entorno
  strlcpy(Mientorno.CLIENTE_WIFI_ssid,  p.getString("SSID",      "Farina").c_str(),      CAMPOS_CONFIG_LENGTH);
  strlcpy(Mientorno.CLIENTE_WIFI_clave, p.getString("CLAVE",     "Lacar2025").c_str(),   CAMPOS_CONFIG_LENGTH);
  Mientorno.CLIENTE_WIFI_Habilitado            = p.getBool("WIFI_EN", true);
  Mientorno.CLIENTE_WIFI_segundosEsperarConexion = p.getUInt("WIFI_TO", 20);
  strlcpy(Mientorno.MQTT_servidor, p.getString("MQTT_SRV",   "66.97.47.118").c_str(), 64);
  strlcpy(Mientorno.MQTT_topic,    p.getString("MQTT_TOPIC", "FarinaVending").c_str(),   64);
  strlcpy(Mientorno.MQTT_usuario,  p.getString("MQTT_USER",  "farinamqtt").c_str(),      32);
  strlcpy(Mientorno.MQTT_clave,    p.getString("MQTT_CLAVE", "fv#297esp8266").c_str(),   32);
  strlcpy(Mientorno.EQUIPO_serie,  p.getString("SERIE",      "IFACE-001").c_str(),    32);
  Mientorno.EQUIPO_inicializado            = p.getBool("INICIALIZADO", false);
  Mientorno.MQTT_GPRS_segundosEntreRafagas = p.getUShort("MQTT_INT",  60);
  Mientorno.MQTT_GPRS_intentosPorRafagas   = p.getUChar("MQTT_INT2",   3);

  // Contadores
  Miscontadores.Cont_Parcial_Cortesia  = p.getUInt("Cont_PCor", 0);
  Miscontadores.Cont_Parcial_Dinero    = p.getUInt("Cont_PD",   0);
  Miscontadores.Cont_Total_Dinero      = p.getUInt("Cont_TD",   0);
  Miscontadores.Cont_Parcial_Cospeles  = p.getUInt("Cont_PCos", 0);
  Miscontadores.Cont_Total_Cospeles    = p.getUInt("Cont_TCos", 0);
  Miscontadores.Cont_Parcial_OnLine    = p.getUInt("Cont_POL",  0);
  Miscontadores.Cont_Total_Online      = p.getUInt("Cont_TOL",  0);
  Miscontadores.Cont_Total_Cortesia    = p.getUInt("Cont_TCor", 0);

  // Valores
  Misvalores.Modo_Timer   = p.getBool(  "Carrousel",   false);
  Misvalores.Modo_Espejo      = p.getBool(  "Opt1",        false);
  Misvalores.Opcion_Extra_2      = p.getBool(  "Opt2",        false);
  Misvalores.Tiempo_Programado   = p.getInt(   "TiempoProg",  DEFAULT_TIEMPO_PROGRAMADO);
  strlcpy(Misvalores.Empresa,  p.getString("Empresa",  "").c_str(), 17);
  strlcpy(Misvalores.Telefono, p.getString("Telefono", "").c_str(), 17);
  Misvalores.Entrada_Activo_Alto    = p.getBool(  "EntActAlto",  false);
  Misvalores.Entrada_Duracion_ms    = p.getUShort("EntDurMs",    DEFAULT_INPUT_DURATION_MS);
  Misvalores.Entrada_Max_Duracion_ms= p.getUShort("EntMaxDurMs", DEFAULT_INPUT_MAX_DURATION_MS);
  Misvalores.Salida_Activo_Alto     = p.getBool(  "SalActAlto",  false);
  Misvalores.Salida_Duracion_ms     = p.getUShort("SalDurMs",    DEFAULT_OUTPUT_DURATION_MS);

  // UpdateCfg
  Miupdate.updateFirmwarePendiente   = p.getBool("updFW",     false);
  Miupdate.updateFirmwareAlIniciar   = p.getBool("updFW_ini", false);
  Miupdate.updateFilesystemPendiente = p.getBool("updFS",     false);
  Miupdate.updateFilesystemAlIniciar = p.getBool("updFS_ini", false);
  strlcpy(Miupdate.urlFirmware,   p.getString("urlFW", "").c_str(), CAMPO_URL_LENGTH);
  strlcpy(Miupdate.urlFilesystem, p.getString("urlFS", "").c_str(), CAMPO_URL_LENGTH);

  p.end();
}

// ── Persistencia por sección ──────────────────────────────────────────────────
void Save_Flash_Entorno() {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putString("SSID",       Mientorno.CLIENTE_WIFI_ssid);
  p.putString("CLAVE",      Mientorno.CLIENTE_WIFI_clave);
  p.putBool(  "WIFI_EN",    Mientorno.CLIENTE_WIFI_Habilitado);
  p.putUInt(  "WIFI_TO",    Mientorno.CLIENTE_WIFI_segundosEsperarConexion);
  p.putString("MQTT_SRV",   Mientorno.MQTT_servidor);
  p.putString("MQTT_TOPIC", Mientorno.MQTT_topic);
  p.putString("MQTT_USER",  Mientorno.MQTT_usuario);
  p.putString("MQTT_CLAVE", Mientorno.MQTT_clave);
  p.putString("SERIE",      Mientorno.EQUIPO_serie);
  p.putBool(  "INICIALIZADO", Mientorno.EQUIPO_inicializado);
  p.putUShort("MQTT_INT",   Mientorno.MQTT_GPRS_segundosEntreRafagas);
  p.putUChar( "MQTT_INT2",  Mientorno.MQTT_GPRS_intentosPorRafagas);
  p.end();
}

void Save_Flash_Contadores() {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putUInt("Cont_PCor", Miscontadores.Cont_Parcial_Cortesia);
  p.putUInt("Cont_PD",   Miscontadores.Cont_Parcial_Dinero);
  p.putUInt("Cont_TD",   Miscontadores.Cont_Total_Dinero);
  p.putUInt("Cont_PCos", Miscontadores.Cont_Parcial_Cospeles);
  p.putUInt("Cont_TCos", Miscontadores.Cont_Total_Cospeles);
  p.putUInt("Cont_POL",  Miscontadores.Cont_Parcial_OnLine);
  p.putUInt("Cont_TOL",  Miscontadores.Cont_Total_Online);
  p.putUInt("Cont_TCor", Miscontadores.Cont_Total_Cortesia);
  p.end();
}

void Save_Flash_ValorServicio() {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putBool(  "Carrousel",   Misvalores.Modo_Timer);
  p.putBool(  "Opt1",        Misvalores.Modo_Espejo);
  p.putBool(  "Opt2",        Misvalores.Opcion_Extra_2);
  p.putInt(   "TiempoProg",  Misvalores.Tiempo_Programado);
  p.putString("Empresa",     Misvalores.Empresa);
  p.putString("Telefono",    Misvalores.Telefono);
  p.putBool(  "EntActAlto",   Misvalores.Entrada_Activo_Alto);
  p.putUShort("EntDurMs",     Misvalores.Entrada_Duracion_ms);
  p.putUShort("EntMaxDurMs",  Misvalores.Entrada_Max_Duracion_ms);
  p.putBool(  "SalActAlto",   Misvalores.Salida_Activo_Alto);
  p.putUShort("SalDurMs",     Misvalores.Salida_Duracion_ms);
  p.end();
}

void Save_Flash_Actualizar() {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putBool(  "updFW",     Miupdate.updateFirmwarePendiente);
  p.putBool(  "updFW_ini", Miupdate.updateFirmwareAlIniciar);
  p.putBool(  "updFS",     Miupdate.updateFilesystemPendiente);
  p.putBool(  "updFS_ini", Miupdate.updateFilesystemAlIniciar);
  p.putString("urlFW",     Miupdate.urlFirmware);
  p.putString("urlFS",     Miupdate.urlFilesystem);
  p.end();
}

void Set_Flash_Bool(const char *key, bool value) {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putBool(key, value);
  p.end();
}

void Set_Flash_UInt(const char *key, uint32_t value) {
  Preferences p;
  p.begin(NVS_NS, false);
  p.putUInt(key, value);
  p.end();
}

// ── Defaults de primera ejecución ─────────────────────────────────────────────
void Inicializar_Flash() {
  Serial.println("Flash: primera inicialización con valores por defecto.");
  strlcpy(Mientorno.CLIENTE_WIFI_ssid,  "Farina",       CAMPOS_CONFIG_LENGTH);
  strlcpy(Mientorno.CLIENTE_WIFI_clave, "Lacar2025",    CAMPOS_CONFIG_LENGTH);
  Mientorno.CLIENTE_WIFI_Habilitado             = true;
  Mientorno.CLIENTE_WIFI_segundosEsperarConexion = 20;
  strlcpy(Mientorno.MQTT_servidor, "66.97.47.118", 64);
  strlcpy(Mientorno.MQTT_topic,    "FarinaVending",   64);
  strlcpy(Mientorno.MQTT_usuario,  "farinamqtt",      32);
  strlcpy(Mientorno.MQTT_clave,    "fv#297esp8266",   32);
  strlcpy(Mientorno.EQUIPO_serie,  "6666",    32);
  Mientorno.EQUIPO_inicializado             = true;
  Mientorno.MQTT_GPRS_segundosEntreRafagas  = 60;
  Mientorno.MQTT_GPRS_intentosPorRafagas    = 3;

  memset(&Miscontadores, 0, sizeof(Miscontadores));

  Misvalores.Modo_Timer    = false;
  Misvalores.Modo_Espejo       = false;
  Misvalores.Opcion_Extra_2       = false;
  Misvalores.Tiempo_Programado    = DEFAULT_TIEMPO_PROGRAMADO;
  Misvalores.Empresa[0]           = '\0';
  Misvalores.Telefono[0]          = '\0';
  Misvalores.Entrada_Activo_Alto      = false;
  Misvalores.Salida_Activo_Alto       = false;
  Misvalores.Salida_Duracion_ms   = DEFAULT_OUTPUT_DURATION_MS;

  memset(&Miupdate, 0, sizeof(Miupdate));

  Save_Flash_Entorno();
  Save_Flash_Contadores();
  Save_Flash_ValorServicio();
  Save_Flash_Actualizar();
}

// ── Bootstrap: carga todo al iniciar, inicializa si es primera vez ─────────────
void Storage_Bootstrap() {
  Preferences p;
  p.begin(NVS_NS, false);  // crea el namespace si no existe
  p.end();

  Get_Flash_All();

  if (!Mientorno.EQUIPO_inicializado) {
    Inicializar_Flash();
  }
    Serial.println("Lectura de memoria Flash.....");
    Serial.println("Serie: " + String(Mientorno.EQUIPO_serie));
    Serial.println("Levantar AP: " + String(g_levantarAP));
    Serial.println ("Red Wifi"+ String(Mientorno.CLIENTE_WIFI_ssid) + " - " + String(Mientorno.CLIENTE_WIFI_clave));
    String texto = "Firmware:" + String(VERSION_FIRMWARE);
    Serial.println(texto);
    Serial.println("Duracion salida: " + String(Misvalores.Salida_Duracion_ms));
    Serial.println("Modo de uso: " + String(Misvalores.Modo_Timer ? "Temporizador" : "Interface"));
    Misvalores.Entrada_Activo_Alto ? Serial.println("Pulso de entrada: Activo Alto") : Serial.println("Pulso de entrada: Activo Bajo");
    
}

// ── Implementación de boot_load_config (corre antes de Storage_Bootstrap) ─────
bool boot_load_config(Config &out) {
  Preferences p;
  p.begin(NVS_NS, false);
  out.wifi_mode_ap = p.getBool("Levantar_AP", false);
  if (out.wifi_mode_ap) {
    p.end();
    Preferences pw;
    pw.begin(NVS_NS, false);
    pw.putBool("Levantar_AP", false);
    pw.end();
  }
  p.end();
  return true;
}
