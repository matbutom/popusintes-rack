#include "Popusintes.hpp"

// modulo
struct EnvoModulo : Module
{
    enum ParamIds
    {
        PARAM_SUBIDA_A,
        PARAM_BAJADA_A,
        PARAM_SUBIDA_B,
        PARAM_BAJADA_B,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_PULSO_A,
        ENTRADA_PULSO_A_FORZAR,
        ENTRADA_PULSO_B,
        ENTRADA_PULSO_B_FORZAR,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_ENVO_A,
        SALIDA_ENVO_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_ENVO_A,
        LUZ_ENVO_B,
        NUM_LIGHTS,
    };

    // un CanalEnvo por canal: agrupa la maquina de estados de la envolvente
    CanalEnvo canalA;
    CanalEnvo canalB;

    EnvoModulo()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(
            PARAM_SUBIDA_A, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "subida a", " s");
        configParam(
            PARAM_BAJADA_A, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "bajada a", " s");

        configParam(
            PARAM_SUBIDA_B, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "subida b", " s");
        configParam(
            PARAM_BAJADA_B, tiempos::TIEMPO_MIN, tiempos::TIEMPO_MAX,
            0.1f, "bajada b", " s");

        configInput(ENTRADA_PULSO_A, "pulso a");
        configInput(ENTRADA_PULSO_A_FORZAR, "forzar a");
        configInput(ENTRADA_PULSO_B, "pulso b");
        configInput(ENTRADA_PULSO_B_FORZAR, "forzar b");

        configOutput(SALIDA_ENVO_A, "envolvente a");
        configOutput(SALIDA_ENVO_B, "envolvente b");
    }

    void process(const ProcessArgs &args) override
    {
        float tiempoSubidaA = params[PARAM_SUBIDA_A].getValue();
        float tiempoBajadaA = params[PARAM_BAJADA_A].getValue();

        float salidaA = canalA.procesar(
            args.sampleTime, tiempoSubidaA, tiempoBajadaA,
            inputs[ENTRADA_PULSO_A].getVoltage(), inputs[ENTRADA_PULSO_A_FORZAR].getVoltage());

        outputs[SALIDA_ENVO_A].setVoltage(salidaA);
        lights[LUZ_ENVO_A].setBrightness(salidaA * 0.1f);

        float tiempoSubidaB = params[PARAM_SUBIDA_B].getValue();
        float tiempoBajadaB = params[PARAM_BAJADA_B].getValue();

        float salidaB = canalB.procesar(
            args.sampleTime, tiempoSubidaB, tiempoBajadaB,
            inputs[ENTRADA_PULSO_B].getVoltage(), inputs[ENTRADA_PULSO_B_FORZAR].getVoltage());

        outputs[SALIDA_ENVO_B].setVoltage(salidaB);
        lights[LUZ_ENVO_B].setBrightness(salidaB * 0.1f);
    }
};

namespace layout
{
    constexpr float PORCENTAJE_COLUMNA_IZQ = columnas::DOS_1;
    constexpr float PORCENTAJE_COLUMNA_DER = columnas::DOS_2;

    // canal a: columna izquierda, arranca mas arriba
    constexpr float PORCENTAJE_SUBIDA_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_SUBIDA_A_Y = 0.35f;
    constexpr float PORCENTAJE_BAJADA_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_BAJADA_A_Y = PORCENTAJE_SUBIDA_A_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR;
    constexpr float PORCENTAJE_ENTRADA_PULSO_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_PULSO_A_Y = PORCENTAJE_BAJADA_A_Y + espaciado::DELTA_Y_BOTON_ENTRADA;
    constexpr float PORCENTAJE_ENTRADA_FORZAR_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_FORZAR_A_Y = PORCENTAJE_ENTRADA_PULSO_A_Y + espaciado::DELTA_Y_ENTRADA_ENTRADA;

    // canal b: columna derecha, arranca mas abajo, para el efecto escalera
    constexpr float PORCENTAJE_SUBIDA_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_SUBIDA_B_Y = 0.55f;
    constexpr float PORCENTAJE_BAJADA_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_BAJADA_B_Y = PORCENTAJE_SUBIDA_B_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR;
    constexpr float PORCENTAJE_ENTRADA_PULSO_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_PULSO_B_Y = PORCENTAJE_BAJADA_B_Y + espaciado::DELTA_Y_BOTON_ENTRADA;
    constexpr float PORCENTAJE_ENTRADA_FORZAR_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_FORZAR_B_Y = PORCENTAJE_ENTRADA_PULSO_B_Y + espaciado::DELTA_Y_ENTRADA_ENTRADA;

    // salidas y luces, misma altura para los dos canales
    constexpr float PORCENTAJE_SALIDA_Y = 0.90f;
    constexpr float PORCENTAJE_LUCES_Y = PORCENTAJE_SALIDA_Y - espaciado::DELTA_Y_SALIDA_LUZ;

    constexpr float PORCENTAJE_LUCES_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_SALIDA_A_X = PORCENTAJE_COLUMNA_IZQ;

    constexpr float PORCENTAJE_LUCES_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_SALIDA_B_X = PORCENTAJE_COLUMNA_DER;
}

// widget
struct EnvoModuloWidget : ModuleWidget
{
    EnvoModuloWidget(EnvoModulo *modulo)
    {

        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EnvoModulo.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::ENVO_ANCHO, dimensiones::ENVO_ALTURA);

        // canal a: perillas, entradas
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_SUBIDA_A_X, layout::PORCENTAJE_SUBIDA_A_Y),
            modulo, EnvoModulo::PARAM_SUBIDA_A));

        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_BAJADA_A_X, layout::PORCENTAJE_BAJADA_A_Y),
            modulo, EnvoModulo::PARAM_BAJADA_A));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_PULSO_A_X, layout::PORCENTAJE_ENTRADA_PULSO_A_Y),
            modulo, EnvoModulo::ENTRADA_PULSO_A));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_FORZAR_A_X, layout::PORCENTAJE_ENTRADA_FORZAR_A_Y),
            modulo, EnvoModulo::ENTRADA_PULSO_A_FORZAR));

        // canal b: perillas, entradas
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_SUBIDA_B_X, layout::PORCENTAJE_SUBIDA_B_Y),
            modulo, EnvoModulo::PARAM_SUBIDA_B));

        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_BAJADA_B_X, layout::PORCENTAJE_BAJADA_B_Y),
            modulo, EnvoModulo::PARAM_BAJADA_B));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_PULSO_B_X, layout::PORCENTAJE_ENTRADA_PULSO_B_Y),
            modulo, EnvoModulo::ENTRADA_PULSO_B));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_FORZAR_B_X, layout::PORCENTAJE_ENTRADA_FORZAR_B_Y),
            modulo, EnvoModulo::ENTRADA_PULSO_B_FORZAR));

        // salidas y luces, una columna por canal
        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_SALIDA_A_X, layout::PORCENTAJE_SALIDA_Y),
            modulo, EnvoModulo::SALIDA_ENVO_A));
        addChild(createLightCentered<SmallLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_LUCES_A_X, layout::PORCENTAJE_LUCES_Y),
            modulo, EnvoModulo::LUZ_ENVO_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_SALIDA_B_X, layout::PORCENTAJE_SALIDA_Y),
            modulo, EnvoModulo::SALIDA_ENVO_B));
        addChild(createLightCentered<SmallLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_LUCES_B_X, layout::PORCENTAJE_LUCES_Y),
            modulo, EnvoModulo::LUZ_ENVO_B));
    }
};

Model *modeloEnvo = createModel<EnvoModulo, EnvoModuloWidget>("envo");
