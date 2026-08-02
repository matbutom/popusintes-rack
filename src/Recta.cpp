#include "Popusintes.hpp"

// vco de onda rectangular
// dos canales independientes a y b
// cada canal:
// perilla de frecuencia
// entrada para modular frecuencia
// perilla de ancho de pulso pwm, en porcentaje
// entrada cv para modular pwm
// salida de audio y luz

// modulo
struct RectaModulo : Module
{
    enum ParamIds
    {
        PARAM_FRECUENCIA_A,
        PARAM_PWM_A,
        PARAM_FRECUENCIA_B,
        PARAM_PWM_B,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_VOCT_A,
        ENTRADA_PWM_A,
        ENTRADA_VOCT_B,
        ENTRADA_PWM_B,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_RECTA_A,
        SALIDA_RECTA_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_SALIDA_A,
        LUZ_SALIDA_B,
        NUM_LIGHTS,
    };

    // rango de las perillas de frecuencia
    static constexpr float FRECUENCIA_OCTAVAS_MIN = -4.0f;
    static constexpr float FRECUENCIA_OCTAVAS_MAX = 4.0f;

    static constexpr float PWM_MIN = 0.01f;
    static constexpr float PWM_MAX = 0.99f;

    // fase acumulada
    float faseA = 0.0f;
    float faseB = 0.0f;

    RectaModulo()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // canal A
        configParam(PARAM_FRECUENCIA_A, FRECUENCIA_OCTAVAS_MIN, FRECUENCIA_OCTAVAS_MAX, 0.0f, "frecuencia A", " oct");
        configParam(PARAM_PWM_A, PWM_MIN * 100.0f, PWM_MAX * 100.f, 50.f, "ancho pulso A", " %");

        // canal B
        configParam(PARAM_FRECUENCIA_B, FRECUENCIA_OCTAVAS_MIN, FRECUENCIA_OCTAVAS_MAX, 0.0f, "frecuencia B", " oct");
        configParam(PARAM_PWM_B, PWM_MIN * 100.0f, PWM_MAX * 100.f, 50.f, "ancho pulso B", " %");

        configInput(ENTRADA_VOCT_A, "v/oct A");
        configInput(ENTRADA_PWM_A, "cv pwm A");

        configInput(ENTRADA_VOCT_B, "v/oct B");
        configInput(ENTRADA_PWM_B, "cv pwm B");

        configOutput(SALIDA_RECTA_A, "rectangular A");
        configOutput(SALIDA_RECTA_B, "rectangular B");
    }

    // fase acumulada del canal

    float procesarCanal(float &fase, float args_sampleTime, float paramFrecuencia, float entradaVoct, float paramPwm, float entradaPwm)
    {
        float altura = paramFrecuencia;
        altura = altura + entradaVoct;

        float frecuencia = dsp::FREQ_C4 * std::pow(2.0f, altura);

        fase = fase + frecuencia * args_sampleTime;

        // limites de fase
        if (fase >= 1.0f)
        {
            fase = fase - std::floor(fase);
        }
        else if (fase < 0.0f)
        {
            fase = fase - std::floor(fase);
        }

        float anchoPulso = paramPwm * 0.01f;
        anchoPulso = anchoPulso + entradaPwm * 0.1f;

        // limites de ancho de puslo
        if (anchoPulso > PWM_MAX)
        {
            anchoPulso = PWM_MAX;
        }
        else if (anchoPulso < PWM_MIN)
        {
            anchoPulso = PWM_MIN;
        }

        // onda rectangular, bipolar, entre -5v y +5v
        if (fase < anchoPulso)
        {
            return 5.0f;
        }
        else
        {
            return -5.0f;
        }
    }

    void process(const ProcessArgs &args) override
    {
        // canal A
        float salidaA = procesarCanal(
            faseA,
            args.sampleTime,
            params[PARAM_FRECUENCIA_A].getValue(),
            inputs[ENTRADA_VOCT_A].getVoltage(),
            params[PARAM_PWM_A].getValue(),
            inputs[ENTRADA_PWM_A].getVoltage());

        outputs[SALIDA_RECTA_A].setVoltage(salidaA);
        lights[LUZ_SALIDA_A].setSmoothBrightness(salidaA / 5.0f, args.sampleTime);

        // canal B
        float salidaB = procesarCanal(
            faseB,
            args.sampleTime,
            params[PARAM_FRECUENCIA_B].getValue(),
            inputs[ENTRADA_VOCT_B].getVoltage(),
            params[PARAM_PWM_B].getValue(),
            inputs[ENTRADA_PWM_B].getVoltage());

        outputs[SALIDA_RECTA_B].setVoltage(salidaB);
        lights[LUZ_SALIDA_B].setSmoothBrightness(salidaB / 5.0f, args.sampleTime);
    }
};

namespace layout
{
    constexpr float PORCENTAJE_COLUMNA_IZQ = columnas::DOS_1;
    constexpr float PORCENTAJE_COLUMNA_DER = columnas::DOS_2;

    // canal a: columna izquierda, arranca mas arriba
    constexpr float PORCENTAJE_FRECUENCIA_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_FRECUENCIA_A_Y = 0.15f;
    constexpr float PORCENTAJE_ENTRADA_VOCT_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_VOCT_A_Y = PORCENTAJE_FRECUENCIA_A_Y + espaciado::DELTA_Y_BOTON_ENTRADA;
    constexpr float PORCENTAJE_PWM_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_PWM_A_Y = 0.40f;
    constexpr float PORCENTAJE_ENTRADA_PWM_A_X = PORCENTAJE_COLUMNA_IZQ;
    constexpr float PORCENTAJE_ENTRADA_PWM_A_Y = PORCENTAJE_PWM_A_Y + espaciado::DELTA_Y_BOTON_ENTRADA;

    // canal b: columna derecha, arranca mas abajo, para el efecto escalera
    constexpr float PORCENTAJE_FRECUENCIA_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_FRECUENCIA_B_Y = 0.35f;
    constexpr float PORCENTAJE_ENTRADA_VOCT_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_VOCT_B_Y = PORCENTAJE_FRECUENCIA_B_Y + espaciado::DELTA_Y_BOTON_ENTRADA;
    constexpr float PORCENTAJE_PWM_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_PWM_B_Y = 0.60f;
    constexpr float PORCENTAJE_ENTRADA_PWM_B_X = PORCENTAJE_COLUMNA_DER;
    constexpr float PORCENTAJE_ENTRADA_PWM_B_Y = PORCENTAJE_PWM_B_Y + espaciado::DELTA_Y_BOTON_ENTRADA;

    // salida y luz, misma altura para los dos canales
    constexpr float PORCENTAJE_SALIDA_Y = 0.90f;
    constexpr float PORCENTAJE_LUZ_Y = PORCENTAJE_SALIDA_Y - espaciado::DELTA_Y_SALIDA_LUZ;
}

// widget
struct RectaModuloWidget : ModuleWidget
{
    RectaModuloWidget(RectaModulo *modulo)
    {
        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectaModulo.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::RECTA_ANCHO, dimensiones::RECTA_ALTURA);

        // canal a: perillas, entradas
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_FRECUENCIA_A_X, layout::PORCENTAJE_FRECUENCIA_A_Y),
            modulo, RectaModulo::PARAM_FRECUENCIA_A));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_VOCT_A_X, layout::PORCENTAJE_ENTRADA_VOCT_A_Y),
            modulo, RectaModulo::ENTRADA_VOCT_A));

        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_PWM_A_X, layout::PORCENTAJE_PWM_A_Y),
            modulo, RectaModulo::PARAM_PWM_A));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_PWM_A_X, layout::PORCENTAJE_ENTRADA_PWM_A_Y),
            modulo, RectaModulo::ENTRADA_PWM_A));

        // canal b: perillas, entradas
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_FRECUENCIA_B_X, layout::PORCENTAJE_FRECUENCIA_B_Y),
            modulo, RectaModulo::PARAM_FRECUENCIA_B));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_VOCT_B_X, layout::PORCENTAJE_ENTRADA_VOCT_B_Y),
            modulo, RectaModulo::ENTRADA_VOCT_B));

        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(layout::PORCENTAJE_PWM_B_X, layout::PORCENTAJE_PWM_B_Y),
            modulo, RectaModulo::PARAM_PWM_B));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_ENTRADA_PWM_B_X, layout::PORCENTAJE_ENTRADA_PWM_B_Y),
            modulo, RectaModulo::ENTRADA_PWM_B));

        // salidas y luces, misma altura para los dos canales
        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_SALIDA_Y),
            modulo, RectaModulo::SALIDA_RECTA_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_SALIDA_Y),
            modulo, RectaModulo::SALIDA_RECTA_B));

        addChild(createLightCentered<LargeLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_IZQ, layout::PORCENTAJE_LUZ_Y),
            modulo, RectaModulo::LUZ_SALIDA_A));

        addChild(createLightCentered<LargeLight<GreenLight>>(
            posicionador.posicion(layout::PORCENTAJE_COLUMNA_DER, layout::PORCENTAJE_LUZ_Y),
            modulo, RectaModulo::LUZ_SALIDA_B));
    }
};

Model *modeloRecta = createModel<RectaModulo, RectaModuloWidget>("recta");