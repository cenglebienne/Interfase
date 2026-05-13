#pragma once
#include "config.h"
#include "app_state_common.h"   // externs compartidos en lib_shared

// ── Instancias de structs de configuración ────────────────────────────────────
extern Config    g_cfg;
extern Entorno   Mientorno;
extern Contadores Miscontadores;
extern Valores    Misvalores;
extern UpdateCfg  Miupdate;

// ── Estado de la habilitación de la VMC ───────────────────────────────────────
extern bool g_vmcEnabled;        // true = VMC habilitó la máquina

// ── Modo aprendizaje de duración de pulso ─────────────────────────────────────
extern bool g_modoAprendizaje;   // true = esperando ficha para aprender duración

// ── Temperatura (requerida por mqtt_handlers, sin sensor → siempre 0) ─────────
extern float g_temperatura;

// ── Versión del firmware (requerida por mqtt_handlers) ────────────────────────
extern uint16_t VERSION_FIRMWARE2;
