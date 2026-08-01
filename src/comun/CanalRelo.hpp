#pragma once
#include "plugin.hpp"

// calcula el ppm de un canal desfasado a partir del ppm base y sus controles de desfase
// desfase = perilla + cv atenuado
// cv bipolar +-5V, con atenuversor en 1.0 cubre el rango completo
// se ataja el desfase resultante en el rango de -10.0f a +10.f
// usada en relo y rerelo
inline float calcularPpmDesfasado(float ppmBase, float desfasePerilla, float desfaseCvAten, float desfaseCvVoltaje)
{
    float desfase = desfasePerilla + desfaseCvVoltaje * desfaseCvAten * 2.f;

    if (desfase > 10.f)
    {
        desfase = 10.f;
    }
    else if (desfase < -10.f)
    {
        desfase = -10.f;
    }

    return ppmBase * (1.f + desfase / 100.f);
}

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
    // y devuelve el valor del pulso (0 a 1) correspondiente a este sample
    // el llamador decide como usarlo (voltaje de salida, brillo de luz)
    float procesar(float sampleTiempo, float sampleTasa, float ppm, float duracionPulso)
    {
        periodo = 60.f * sampleTasa / ppm;

        if (contador > periodo)
        {
            generarPulso(duracionPulso);
            contador = contador - periodo;
        }

        contador = contador + 1;

        return generadorPulsos.process(sampleTiempo);
    }
};
