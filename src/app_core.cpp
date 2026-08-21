#include "app_core.h"
#include "app_state.h"
#include "pins.h"
#include "constants.h"
#include "pulse_output.h"
#include "memory_api.h"
#include "logger.h"
#include "connections.h"
#include "http_server_app.h"
#include "validator.h"

// Logica habilitacion
// En la entrada hay negacion, esto quiere decir que cuando la maquina me quiere deshabililtar el validador con un valor en alto, yo recibo un cero.
// en la salida  hay negacion, esto quiere decir que para habilitar el validador tengo que mandar un valor alto para que me de un cero.

// logica pulso
// En la entrada no hay negacion , esto quiere decir que leo un activo bajo cada vez que entra una ficha (en casi todos los casos)
//  la salida es negada, por lo que tengo que mandar un pulso en alto para simular lo que recibi del validador

// ── Actualiza g_vmcEnabled y habilita/deshabilita el validador ─────────────────
static void check_vmc_enable()
{

  bool habilitado = Misvalores.Usar_Habilitacion_Externa
                       ? (digitalRead(PIN_VMC_ENABLE_IN) == HIGH)
                       : true;  // pin ignorado: el validador queda siempre habilitado
  static bool cambiomensaje = false;

  if (!g_modoAprendizaje)
  {
    if (habilitado != g_vmcEnabled)
    {
      g_vmcEnabled = habilitado;
      digitalWrite(PIN_HABILITACION, habilitado ? HIGH : LOW); // HIGH = habilitar
      // logEvent(habilitado ? "VMC habilitó la máquina" : "VMC deshabilitó la máquina");
    }
  }
  else
  {
    digitalWrite(PIN_HABILITACION, HIGH);  // modo aprendizaje: habilitar validador
  }
  // El pago QR está disponible cuando la VMC habilita y no hay servicio en curso.
  // En Modo Timer, un servicio en curso no bloquea: un nuevo evento reinicia la cuenta regresiva.
  g_pagoQrDisponible = g_vmcEnabled && (!g_enServicio || Misvalores.Modo_Timer);
}

// ── Procesa una ficha física del validador ────────────────────────────────────
static void procesar_ficha()
{
  // En Modo Timer, un nuevo evento durante la cuenta regresiva la reinicia en vez de ignorarse.
  if (g_enServicio && !Misvalores.Modo_Timer) return;
  if (!Misvalores.Modo_Timer && !g_vmcEnabled) return;

  Miscontadores.Cont_Parcial_Cospeles++;
  Miscontadores.Cont_Total_Cospeles++;
  Save_Flash_Contadores();
  logEvent("Ficha detectada");
  pulse_output_trigger();
}

// ── Procesa un pago electrónico online ────────────────────────────────────────
static void procesar_pago_online()
{
  if (!g_vmcEnabled || (g_enServicio && !Misvalores.Modo_Timer))
  {
    g_pagoAceptado = false;
    g_pagoelectronico = false;
    return;
  }
  Miscontadores.Cont_Parcial_OnLine++;
  Miscontadores.Cont_Total_Online++;
  Save_Flash_Contadores();
  logEvent("Pago online procesado");
  pulse_output_trigger();
  g_pagoAceptado = true;
  g_pagoelectronico = false;
}

// ── Procesa un servicio de cortesía ──────────────────────────────────────────
static void procesar_cortesia()
{
/*   if (!g_vmcEnabled || g_enServicio)
  {
    g_pagoAceptado = false;
    g_servicioCortesia = false;
    return;
  } */
  Miscontadores.Cont_Parcial_Cortesia++;
  Miscontadores.Cont_Total_Cortesia++;
  Save_Flash_Contadores();
  logEvent("Servicio de cortesía procesado");
  pulse_output_trigger();
  g_pagoAceptado = true;
  g_servicioCortesia = false;
}

// ── Loop principal ────────────────────────────────────────────────────────────
void app_core_loop()
{
  check_vmc_enable();

  if (g_cospelIn)
  {
    g_cospelIn = false;
    if (g_modoAprendizaje)
    {
      Misvalores.Entrada_Duracion_ms = (uint16_t)pulseDuration;  // referencia para filtro de entrada
      Misvalores.Salida_Duracion_ms  = (uint16_t)pulseDuration;  // duración del pulso de salida
      Save_Flash_ValorServicio();
      validator_update_config();  // actualiza s_refDurMs en el ISR
      g_modoAprendizaje = false;
      Serial.println("Aprendizaje completado. Duracion: " + String(Misvalores.Salida_Duracion_ms) + " ms");
    }
    else if (!Misvalores.Modo_Timer && Misvalores.Modo_Espejo) {
      // Modo espejo: la salida ya fue controlada por el ISR; solo actualizar contadores.
      Miscontadores.Cont_Parcial_Cospeles++;
      Miscontadores.Cont_Total_Cospeles++;
      Save_Flash_Contadores();
      logEvent("Ficha detectada (espejo)");
    }
    else
    {
      Serial.println("Pulso de validador detectado.");
      procesar_ficha();
    }
  }

  if (g_pagoelectronico)
  {
    Serial.println("Pago online detectado.");
    procesar_pago_online();
  }

  if (g_servicioCortesia)
  {
    Serial.println("Servicio de cortesía detectado.");
    procesar_cortesia();
  }

  pulse_output_poll();

  if (g_enviar_log_Por_FTP)
  {
    g_enviar_log_Por_FTP = false;
    enviarArchivoPorFTP(LOG_ACTUAL, generarNombreRemotoLog(), false);
  }

  net_poll();
  http_server_poll();
}
