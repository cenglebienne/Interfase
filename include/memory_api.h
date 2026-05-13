#pragma once
#include <Arduino.h>

void Storage_Bootstrap();
void Get_Flash_All();

void Save_Flash_Entorno();
void Save_Flash_Contadores();
void Save_Flash_ValorServicio();
void Save_Flash_Actualizar();

void Set_Flash_Bool(const char *key, bool value);
void Set_Flash_UInt(const char *key, uint32_t value);
void Inicializar_Flash();
