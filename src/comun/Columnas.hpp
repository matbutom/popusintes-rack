#pragma once

// porcentajes de columnas en x, compartidos entre modulos
// evita repetir los mismos numeros de layout en cada modulo

namespace columnas
{
    // layout de dos columnas, cada una a 0.30 de su borde mas cercano
    constexpr float DOS_1 = 0.30f;
    constexpr float DOS_2 = 0.70f;

    // layout de cuatro columnas, simetrico: cada una centrada en su cuarto del ancho
    constexpr float CUATRO_1 = 0.125f;
    constexpr float CUATRO_2 = 0.375f;
    constexpr float CUATRO_3 = 0.625f;
    constexpr float CUATRO_4 = 0.875f;
}
