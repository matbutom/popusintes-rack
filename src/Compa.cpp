#include "Popusintes.hpp"

// modulo
struct CompaModulo : Module
{
    enum ParamIds
    {
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_A_1,
        ENTRADA_A_2,
        ENTRADA_B_1,
        ENTRADA_B_2,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_COMPA_A,
        SALIDA_COMPA_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_SALIDA_A,
        LUZ_SALIDA_B,
        NUM_LIGHTS,
    };

    CompaModulo()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configInput(ENTRADA_A_1, "a1");
        configInput(ENTRADA_A_2, "a2");
        configInput(ENTRADA_B_1, "b1");
        configInput(ENTRADA_B_2, "b2");

        configOutput(SALIDA_COMPA_A, "compa a1>a2");
        configOutput(SALIDA_COMPA_B, "compa b1>b2");
    }

    void process(const ProcessArgs &args) override
    {
        // canal a
        if (inputs[ENTRADA_A_1].isConnected() && inputs[ENTRADA_A_2].isConnected())
        {
            float salidaA = inputs[ENTRADA_A_1].getVoltage() > inputs[ENTRADA_A_2].getVoltage();

            outputs[SALIDA_COMPA_A].setVoltage(10.f * salidaA);
            lights[LUZ_SALIDA_A].setBrightness(salidaA);
        }
        else
        {
            outputs[SALIDA_COMPA_A].setVoltage(0.f);
            lights[LUZ_SALIDA_A].setBrightness(0.f);
        }

        // canal b
        if (inputs[ENTRADA_B_1].isConnected() && inputs[ENTRADA_B_2].isConnected())
        {
            float salidaB = inputs[ENTRADA_B_1].getVoltage() > inputs[ENTRADA_B_2].getVoltage();

            outputs[SALIDA_COMPA_B].setVoltage(10.f * salidaB);
            lights[LUZ_SALIDA_B].setBrightness(salidaB);
        }
        else
        {
            outputs[SALIDA_COMPA_B].setVoltage(0.f);
            lights[LUZ_SALIDA_B].setBrightness(0.f);
        }
    }
};

namespace layout
{
    constexpr float PORCENTAJE_COLUMNA_IZQ = columnas::DOS_1;
    constexpr float PORCENTAJE_COLUMNA_DER = columnas::DOS_2;

    // coordenadas entradas canal a
    constexpr float PORCENTAJE_ENTRADA_A_1_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_A_1_Y = 0.40f;
    constexpr float PORCENTAJE_ENTRADA_A_2_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_A_2_Y = PORCENTAJE_ENTRADA_A_1_Y + espaciado::DELTA_Y_ENTRADA_ENTRADA;

    // coordenadas entradas canal b
    constexpr float PORCENTAJE_ENTRADA_B_1_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_B_1_Y = 0.60f;
    constexpr float PORCENTAJE_ENTRADA_B_2_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_B_2_Y = PORCENTAJE_ENTRADA_B_1_Y + espaciado::DELTA_Y_ENTRADA_ENTRADA;

    // coordenadas salida y luz a
    constexpr float PORCENTAJE_LUCES_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_LUCES_A_Y = 0.85f;
    constexpr float PORCENTAJE_SALIDA_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_SALIDA_A_Y = PORCENTAJE_LUCES_A_Y + espaciado::DELTA_Y_SALIDA_LUZ;

    // coordenadas salida y luz b
    constexpr float PORCENTAJE_LUCES_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_LUCES_B_Y = 0.85f;
    constexpr float PORCENTAJE_SALIDA_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_SALIDA_B_Y = PORCENTAJE_LUCES_B_Y + espaciado::DELTA_Y_SALIDA_LUZ;
}

// widget
struct CompaModuloWidget : ModuleWidget
{
    CompaModuloWidget(CompaModulo *modulo)
    {
        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CompaModulo.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::COMPA_ANCHO, dimensiones::COMPA_ALTURA);

        // entradas canal a
        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_A_1_X, layout::PORCENTAJE_ENTRADA_A_1_Y),
            modulo, CompaModulo::ENTRADA_A_1));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_A_2_X, layout::PORCENTAJE_ENTRADA_A_2_Y),
            modulo, CompaModulo::ENTRADA_A_2));

        // entradas canal b
        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_B_1_X, layout::PORCENTAJE_ENTRADA_B_1_Y),
            modulo, CompaModulo::ENTRADA_B_1));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_B_2_X, layout::PORCENTAJE_ENTRADA_B_2_Y),
            modulo, CompaModulo::ENTRADA_B_2));

        // salidas
        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_SALIDA_A_X, layout::PORCENTAJE_SALIDA_A_Y),
            modulo, CompaModulo::SALIDA_COMPA_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_SALIDA_B_X, layout::PORCENTAJE_SALIDA_B_Y),
            modulo, CompaModulo::SALIDA_COMPA_B));

        // luces
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_LUCES_A_X, layout::PORCENTAJE_LUCES_A_Y),
            modulo, CompaModulo::LUZ_SALIDA_A));

        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_LUCES_B_X, layout::PORCENTAJE_LUCES_B_Y),
            modulo, CompaModulo::LUZ_SALIDA_B));
    }
};

Model *modeloCompa = createModel<CompaModulo, CompaModuloWidget>("compa");
