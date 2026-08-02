#pragma once
#include "plugin.hpp"
#include "Tiempos.hpp"

// un canal de envo
// envo tiene 2 de estos canales
// incluye la maquina de estados de la envolvente: subida, bajada y forzado

struct CanalEnvo
{
    enum Estado
    {
        ESTADO_REPOSO,
        ESTADO_SUBIDA,
        ESTADO_BAJADA,
        ESTADO_FORZAR,
    };

    Estado estado = ESTADO_REPOSO;
    float salida = 0.f;
    dsp::SchmittTrigger detectorPulso;
    dsp::SchmittTrigger detectorForzar;

    // avanza el canal un sample
    // tiempoSubida y tiempoBajada en segundos, controlan la rampa de la envolvente
    // entradaPulso dispara la subida, entradaForzar corta a 0V y reinicia
    // devuelve el voltaje de salida (0 a 10V) correspondiente a este sample
    float procesar(float sampleTime, float tiempoSubida, float tiempoBajada, float entradaPulso, float entradaForzar)
    {
        // detectar subida de pulso (umbral de 1V)
        bool detectado = detectorPulso.process(entradaPulso, 0.1f, 1.0f);

        // pulso normal solamente ocurre si estado actual es reposo
        if (detectado && estado == ESTADO_REPOSO)
        {
            estado = ESTADO_SUBIDA;
        }

        // detectar forzado
        bool forzadoDetectado = detectorForzar.process(entradaForzar, 0.1f, 1.0f);

        if (forzadoDetectado)
        {
            estado = ESTADO_FORZAR;
        }

        switch (estado)
        {
        case ESTADO_SUBIDA:
        {
            // incremento para llegar a 10V en tiempoSubida
            float incremento = (10.0f / tiempoSubida) * sampleTime;
            salida = salida + incremento;

            // detectar si ya subimos, para empezar la bajada
            if (salida >= 10.0f)
            {
                salida = 10.0f;
                estado = ESTADO_BAJADA;
            }
            break;
        }

        case ESTADO_BAJADA:
        {
            // decremento para llegar a 0V en tiempoBajada
            float decremento = (10.0f / tiempoBajada) * sampleTime;
            salida = salida - decremento;

            if (salida <= 0.0f)
            {
                salida = 0.0f;
                estado = ESTADO_REPOSO;
            }
            break;
        }

        case ESTADO_FORZAR:
        {
            // rampa rapida a 0V, luego subida
            float decrementoRapido = (10.0f - tiempos::PULSO_MS) * sampleTime;
            salida = salida - decrementoRapido;

            if (salida <= 0.0f)
            {
                salida = 0.0f;
                estado = ESTADO_SUBIDA;
            }
            break;
        }

        case ESTADO_REPOSO:
        default:
            break;
        }

        return salida;
    }
};
