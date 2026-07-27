#pragma once
#include "plugin.hpp"

// un canal de relo
// relo tiene 2 de estos canales
// rerelo tiene 4 de estos canales
// incluye
// generador de pulsos
// contador de samples
// periodo

struct CanalRelo
{
    dsp::PulseGenerator generadorPulsos;
    float contador = 0.f;
    float periodo = 0.f;

    // reinicia el contador
    // se usa al resincronizar (boton o entrada externa)
    void reiniciar()
    {
        contador = 0.f;
    }

    // genera un pulso inmediato
    // se usa junto con reiniciar() para que el resincronizado
    // tambien se escuche en ese instante
    void generarPulso(float duracionPulso)
    {
        generadorPulsos.trigger(duracionPulso);
    }

    // avanza el canal un sample
    // calcula el periodo a partir del ppm, genera un pulso si corresponde,
    // y devuelve el voltaje de salida (0 a 10V) correspondiente a este sample
    float procesar(float sampleTiempo, float sampleTasa, float ppm, float duracionPulso)
    {
        periodo = 60.f * sampleTasa / ppm;

        if (contador > periodo)
        {
            generarPulso(duracionPulso);
            contador = contador - periodo;
        }

        contador = contador + 1;

        return 10.f * generadorPulsos.process(sampleTiempo);
    }
};
