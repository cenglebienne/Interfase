#ifdef CON_DISPLAY  // solo en el entorno esp32dev_display (version "D")

#include "lcd_display.h"
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include "app_state.h"
#include "pins.h"
#include "pulse_output.h"

static constexpr uint8_t  LCD_COLS        = 16;
static constexpr uint8_t  LCD_ROWS        = 2;
static constexpr uint32_t LCD_REFRESCO_MS = 200;

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

enum class Pantalla : uint8_t { Ninguna, Reposo, Cuenta, PagoAceptado, ConfigAP };
static Pantalla s_pantalla   = Pantalla::Ninguna;
static bool     s_presente   = false;
static uint32_t s_ultimoSeg  = UINT32_MAX;
static uint32_t s_ultimoPoll = 0;

// Escribe el texto centrado y rellena con espacios toda la línea (evita restos).
static void linea_centrada(uint8_t row, const char *txt) {
  char buf[LCD_COLS + 1];
  int len = (int)strlen(txt);
  if (len > LCD_COLS) len = LCD_COLS;
  int pad = (LCD_COLS - len) / 2;
  memset(buf, ' ', LCD_COLS);
  memcpy(buf + pad, txt, len);
  buf[LCD_COLS] = '\0';
  lcd.setCursor(0, row);
  lcd.print(buf);
}

// Muestra una pantalla fija; solo escribe en el LCD si cambió.
static void mostrar(Pantalla p, const char *l1, const char *l2) {
  if (s_pantalla == p) return;
  s_pantalla = p;
  linea_centrada(0, l1);
  linea_centrada(1, l2);
}

void lcd_display_setup() {
  Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);
  Wire.beginTransmission(LCD_I2C_ADDR);
  s_presente = (Wire.endTransmission() == 0);
  if (!s_presente) {
    Serial.println("LCD no detectado: display deshabilitado.");
    return;
  }
  lcd.init();
  lcd.backlight();
  lcd.clear();
  Serial.println("LCD inicializado.");
}

void lcd_display_poll() {
  if (!s_presente) return;
  if ((millis() - s_ultimoPoll) < LCD_REFRESCO_MS) return;
  s_ultimoPoll = millis();

  if (g_levantarAP) {
    mostrar(Pantalla::ConfigAP, "Modo config.", "Access Point");
    return;
  }

  // Modo Timer: cuenta regresiva MM:SS de Tiempo_Programado (segundos).
  // Un nuevo pago reinicia s_inicioMs en pulse_output, y la cuenta lo sigue.
  if (g_enServicio && Misvalores.Modo_Timer) {
    uint32_t seg = (pulse_output_restante_ms() + 999UL) / 1000UL;  // redondeo hacia arriba
    if (s_pantalla != Pantalla::Cuenta) {
      s_pantalla  = Pantalla::Cuenta;
      s_ultimoSeg = UINT32_MAX;
      linea_centrada(0, "Tiempo restante");
    }
    if (seg != s_ultimoSeg) {
      s_ultimoSeg = seg;
      char buf[12];
      snprintf(buf, sizeof(buf), "%02lu:%02lu",
               (unsigned long)(seg / 60), (unsigned long)(seg % 60));
      linea_centrada(1, buf);
    }
    return;
  }

  if (g_enServicio) {
    mostrar(Pantalla::PagoAceptado, "Pago aceptado", "");
    return;
  }

  mostrar(Pantalla::Reposo, "Inserte ficha", "o escanee QR");
}

#endif // CON_DISPLAY
