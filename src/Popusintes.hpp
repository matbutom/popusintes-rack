#pragma once
#include "rack.hpp"

// primero se incluye namespace rack
// para poder usarlo en las includes
using namespace rack;

#include "comun/CanalEnvo.hpp"
#include "comun/CanalRelo.hpp"
#include "comun/CanalSecu.hpp"
#include "comun/Columnas.hpp"
#include "comun/Dimensiones.hpp"
#include "comun/Espaciado.hpp"
#include "comun/Posicionador.hpp"
#include "comun/Tiempos.hpp"
#include "comun/Tornillos.hpp"

// plugin
extern Plugin *pluginInstance;

// modulos
extern Model *modeloCompa;
extern Model *modeloEnvo;
extern Model *modeloPane;
extern Model *modeloRecta;
extern Model *modeloRelo;
extern Model *modeloRerelo;
extern Model *modeloSecu;
extern Model *modeloSuma;