#pragma once
#include "rack.hpp"

using namespace rack;

// deben incluirse despues de "using namespace rack"
// porque usan Vec, dsp, ModuleWidget, etc. sin calificar
#include "comun/CanalRelo.hpp"
#include "comun/Dimensiones.hpp"
#include "comun/Espaciado.hpp"
#include "comun/Posicionador.hpp"
#include "comun/Tiempos.hpp"
#include "comun/Tornillos.hpp"

extern Plugin *pluginInstance;

extern Model *modelCompa;
extern Model *modelEnvo;
extern Model *modelPane;
extern Model *modelRecta;
extern Model *modelRelo;
extern Model *modelRerelo;
extern Model *modelSecu;
extern Model *modelSuma;