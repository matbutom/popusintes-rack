#pragma once
#include "plugin.hpp"

// un canal de secu
// secu tiene 2 de estos canales, cada uno de 4 pasos
// agrupa el detector de flancos y el paso actual

struct CanalSecu
{
    static constexpr int NUM_PASOS = 4;

    dsp::SchmittTrigger detectorBordes;
    int pasoActual = 0;

    // avanza el canal un sample
    // entradaTrigger dispara el avance al siguiente paso (sensible a la subida)
    // pasos son los NUM_PASOS voltajes configurados en las perillas del canal
    // devuelve el voltaje del paso actual
    float procesar(float entradaTrigger, const float *pasos)
    {
        if (detectorBordes.process(entradaTrigger))
        {
            pasoActual = (pasoActual + 1) % NUM_PASOS;
        }

        return pasos[pasoActual];
    }
};
