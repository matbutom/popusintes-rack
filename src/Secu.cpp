#include "Popusintes.hpp"

// secuenciador con entrada de reloj
// sensible a la subida de la señal
// dos canales independientes a y b, cada uno de 4 pasos
// con sus 4 perillas en la misma columna

// para celebrar la materia computacional
// se contaran los pasos de 0 a 3

// decidir que hacer si se desconecta el reloj

struct SecuModulo : Module
{
    enum ParamIds
    {
        PARAM_PASO_A_0,
        PARAM_PASO_A_1,
        PARAM_PASO_A_2,
        PARAM_PASO_A_3,
        PARAM_PASO_B_0,
        PARAM_PASO_B_1,
        PARAM_PASO_B_2,
        PARAM_PASO_B_3,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_TRIGGER_A,
        ENTRADA_TRIGGER_B,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_A,
        SALIDA_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_SALIDA_A,
        LUZ_SALIDA_B,
        NUM_LIGHTS,
    };

    // un CanalSecu por canal: agrupa el detector de flancos y el paso actual
    CanalSecu canalA;
    CanalSecu canalB;

    SecuModulo()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // configurar pasos canal a
        for (int i = 0; i < CanalSecu::NUM_PASOS; i++)
        {
            configParam(PARAM_PASO_A_0 + i, 0.0, 5.0, 1.0, "paso " + std::to_string(i + 1) + " a");
        }

        // configurar pasos canal b
        for (int i = 0; i < CanalSecu::NUM_PASOS; i++)
        {
            configParam(PARAM_PASO_B_0 + i, 0.0, 5.0, 1.0, "paso " + std::to_string(i + 1) + " b");
        }

        configInput(ENTRADA_TRIGGER_A, "reloj a");
        configInput(ENTRADA_TRIGGER_B, "reloj b");

        configOutput(SALIDA_A, "secuencia a");
        configOutput(SALIDA_B, "secuencia b");
    }

    void process(const ProcessArgs &args) override
    {
        // canal a
        float pasosA[CanalSecu::NUM_PASOS] = {
            params[PARAM_PASO_A_0].getValue(),
            params[PARAM_PASO_A_1].getValue(),
            params[PARAM_PASO_A_2].getValue(),
            params[PARAM_PASO_A_3].getValue(),
        };

        float salidaA = canalA.procesar(inputs[ENTRADA_TRIGGER_A].getVoltage(), pasosA);
        outputs[SALIDA_A].setVoltage(salidaA);
        lights[LUZ_SALIDA_A].setSmoothBrightness(salidaA / 5.f, args.sampleTime);

        // canal b
        float pasosB[CanalSecu::NUM_PASOS] = {
            params[PARAM_PASO_B_0].getValue(),
            params[PARAM_PASO_B_1].getValue(),
            params[PARAM_PASO_B_2].getValue(),
            params[PARAM_PASO_B_3].getValue(),
        };

        float salidaB = canalB.procesar(inputs[ENTRADA_TRIGGER_B].getVoltage(), pasosB);
        outputs[SALIDA_B].setVoltage(salidaB);
        lights[LUZ_SALIDA_B].setSmoothBrightness(salidaB / 5.f, args.sampleTime);
    }
};

namespace layout
{
    constexpr float PORCENTAJE_COLUMNA_IZQ = columnas::DOS_1;
    constexpr float PORCENTAJE_COLUMNA_DER = columnas::DOS_2;

    // entrada de reloj, arriba de todo
    constexpr float PORCENTAJE_ENTRADA_TRIGGER_Y = 0.12f;

    // los 4 pasos, apilados en la misma columna, perillas chicas
    constexpr float PORCENTAJE_PASO_0_Y = 0.24f;
    constexpr float PORCENTAJE_PASO_1_Y = PORCENTAJE_PASO_0_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR;
    constexpr float PORCENTAJE_PASO_2_Y = PORCENTAJE_PASO_1_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR;
    constexpr float PORCENTAJE_PASO_3_Y = PORCENTAJE_PASO_2_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR;

    // salida y luz, misma altura para los dos canales
    constexpr float PORCENTAJE_SALIDA_Y = 0.90f;
    constexpr float PORCENTAJE_LUCES_Y = PORCENTAJE_SALIDA_Y - espaciado::DELTA_Y_SALIDA_LUZ;
}

// widget
struct SecuModuloWidget : ModuleWidget
{
    SecuModuloWidget(SecuModulo *modulo)
    {

        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SecuModulo.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::SECU_ANCHO, dimensiones::SECU_ALTURA);

        // canal a: entrada de reloj y sus 4 pasos
        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_ENTRADA_TRIGGER_Y),
            modulo, SecuModulo::ENTRADA_TRIGGER_A));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_PASO_0_Y),
            modulo, SecuModulo::PARAM_PASO_A_0));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_PASO_1_Y),
            modulo, SecuModulo::PARAM_PASO_A_1));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_PASO_2_Y),
            modulo, SecuModulo::PARAM_PASO_A_2));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_PASO_3_Y),
            modulo, SecuModulo::PARAM_PASO_A_3));

        // canal b: entrada de reloj y sus 4 pasos
        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_ENTRADA_TRIGGER_Y),
            modulo, SecuModulo::ENTRADA_TRIGGER_B));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_PASO_0_Y),
            modulo, SecuModulo::PARAM_PASO_B_0));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_PASO_1_Y),
            modulo, SecuModulo::PARAM_PASO_B_1));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_PASO_2_Y),
            modulo, SecuModulo::PARAM_PASO_B_2));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_PASO_3_Y),
            modulo, SecuModulo::PARAM_PASO_B_3));

        // salidas y luces, una columna por canal
        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_SALIDA_Y),
            modulo, SecuModulo::SALIDA_A));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_LUCES_Y),
            modulo, SecuModulo::LUZ_SALIDA_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_SALIDA_Y),
            modulo, SecuModulo::SALIDA_B));
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_LUCES_Y),
            modulo, SecuModulo::LUZ_SALIDA_B));
    }
};

Model *modeloSecu = createModel<SecuModulo, SecuModuloWidget>("secu");
