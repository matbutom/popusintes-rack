#pragma once

// deltas de espaciado compartidos entre modulos
// porcentajes de separacion en el eje y entre controles relacionados

namespace espaciado
{
    // entre boton y entrada
    constexpr float DELTA_Y_BOTON_ENTRADA = 0.08f;

    // entre perilla/atenuversor y entrada
    constexpr float DELTA_Y_PERILLA_ATENUVERSOR = 0.08f;

    // entre salida y su luz
    constexpr float DELTA_Y_SALIDA_LUZ = 0.05f;

    // entre dos entradas relacionadas de un mismo canal
    constexpr float DELTA_Y_ENTRADA_ENTRADA = 0.15f;
}
