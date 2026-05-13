#include "app_state.h"
#include "constants.h"
#include "connections.h"

// ── Instancias de structs de configuración ────────────────────────────────────
Config     g_cfg;
Entorno    Mientorno;
Contadores Miscontadores;
Valores    Misvalores;
UpdateCfg  Miupdate;

// ── Estado de habilitación de la VMC ──────────────────────────────────────────
bool g_vmcEnabled = false;

// ── Temperatura (sin sensor, siempre 0) ───────────────────────────────────────
float g_temperatura = 0.0f;

// ── Variables declaradas en app_state_common.h (lib_shared) ──────────────────
// g_servicioCortesia y pingPromedio son definidos en mqtt_handlers.cpp.
bool              g_levantarAP            = false;
bool              g_inTest                = false;
volatile bool     g_entroPulso            = false;
uint32_t          minimoPulso             = 0;
uint32_t          maximoPulso             = 0;
volatile uint32_t pulseDuration           = 0;
bool              pingPromedioCalculado   = false;
bool              g_pagoQrDisponible      = false;
bool              g_pagoAceptado          = false;
bool              g_enServicio            = false;
uint16_t          VERSION_FIRMWARE2       = VERSION_FIRMWARE;
volatile bool     g_cospelIn             = false;
bool              g_pagoelectronico       = false;
bool              g_enviar_log_Por_FTP    = false;
bool              g_reinicio_pendiente    = false;

// ── Estado de modo aprendizaje de duración de pulso ──────────────────────────
bool g_modoAprendizaje = false;

// ── Variables declaradas en connections.h (lib_shared/connectivity) ───────────
bool g_conectado_wifi = false;
bool g_conectadoMQTT  = false;
