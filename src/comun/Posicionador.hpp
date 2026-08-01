#pragma once
#include "plugin.hpp"

// agrupa el ancho y alto de un panel
// para convertir porcentajes en posiciones px

struct Posicionador
{
    // dimensiones en mm
    float ancho;
    float altura;

    Posicionador(float anchoMm, float alturaMm)
        : ancho(anchoMm), altura(alturaMm) {}

    // porcentajeX y porcentajeY
    // rango de 0.f a 1.f
    Vec posicion(float porcentajeX, float porcentajeY) const
    {
        return mm2px(Vec(ancho * porcentajeX, altura * porcentajeY));
    }
};