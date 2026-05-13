#pragma once
#include <stdint.h>

// ── Identificación del producto ───────────────────────────────────────────────
#ifndef PRODUCTO
  #define PRODUCTO "INTERFACE VENDING"
#endif

#ifndef FW_VERSION
  #define FW_VERSION 1
#endif
// FW_VERSION se define en platformio.ini como entero (ej: -DFW_VERSION=100).
// constexpr permite tomar su dirección con & para enviarlo por MQTT (comando D).
constexpr uint16_t VERSION_FIRMWARE = static_cast<uint16_t>(FW_VERSION);

// ── Filesystem ────────────────────────────────────────────────────────────────
#define FS_LOG_FILE        "/log_actual.txt"
#define FS_VERSION_FILE    "/versionfilesystem.txt"
#define LOG_ACTUAL         FS_LOG_FILE
#define MAX_LOG_SIZE       (100UL * 1024UL)
#define VERSION_FS_PATH    FS_VERSION_FILE
#define HTTP_PORT          80

#ifndef PIN_FTP_ENABLE
  #define PIN_FTP_ENABLE  -1
#endif

// ── Longitudes de campos ──────────────────────────────────────────────────────
#define CAMPO_URL_LENGTH      120
#define CAMPOS_CONFIG_LENGTH   41

// =============================================================================
// Structs de configuración persistente (mismo esquema que los otros proyectos)
// =============================================================================

struct Entorno {
  char     CLIENTE_WIFI_ssid[CAMPOS_CONFIG_LENGTH];
  char     CLIENTE_WIFI_clave[CAMPOS_CONFIG_LENGTH];
  bool     CLIENTE_WIFI_Habilitado;
  uint32_t CLIENTE_WIFI_segundosEsperarConexion;
  char     MQTT_servidor[64];
  char     MQTT_topic[64];
  char     MQTT_usuario[32];
  char     MQTT_clave[32];
  char     EQUIPO_serie[32];
  bool     EQUIPO_inicializado;
  uint16_t MQTT_GPRS_segundosEntreRafagas;
  uint8_t  MQTT_GPRS_intentosPorRafagas;
};

struct Contadores {
  uint32_t Cont_Parcial_Cortesia;
  uint32_t Cont_Parcial_Dinero;     // sin uso (siempre 0)
  uint32_t Cont_Total_Dinero;       // sin uso (siempre 0)
  uint32_t Cont_Parcial_Cospeles;   // fichas físicas parciales
  uint32_t Cont_Total_Cospeles;     // fichas físicas totales
  uint32_t Cont_Parcial_OnLine;
  uint32_t Cont_Total_Online;
  uint32_t Cont_Total_Cortesia;
};

struct Valores {
  bool     Modo_Timer;     // false = Modo Interface, true = Modo Timer
  bool     Modo_Espejo;           // salida sigue a la entrada del validador en tiempo real
  bool     Opcion_Extra_2;        // reservado para uso futuro
  int32_t  Tiempo_Programado;     // Modo Timer: duración de salida en segundos
  char     Empresa[17];
  char     Telefono[17];
  // ── Configuración de pulsos (específico de este proyecto) ────────────────────
  bool     Entrada_Activo_Alto;   // true = pulso del validador es nivel HIGH
  uint16_t Entrada_Duracion_ms;   // ancho mínimo de pulso para considerarlo válido (ms)
  uint16_t Entrada_Max_Duracion_ms; // ancho máximo — pulsos más largos se descartan
  bool     Salida_Activo_Alto;    // true = pulso de salida es nivel HIGH
  uint16_t Salida_Duracion_ms;    // duración del pulso de salida en Modo Interface (ms)
};

struct UpdateCfg {
  bool updateFirmwarePendiente;
  bool updateFirmwareAlIniciar;
  char urlFirmware[CAMPO_URL_LENGTH];
  bool updateFilesystemPendiente;
  bool updateFilesystemAlIniciar;
  char urlFilesystem[CAMPO_URL_LENGTH];
};

// ── Struct mínima para boot rápido (antes de cargar NVS completo) ─────────────
struct Config {
  bool wifi_mode_ap = false;
};

bool boot_load_config(Config &out);
