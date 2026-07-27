#include "Popusintes.hpp"
#include "Dimensiones.hpp"
#include "Posicionador.hpp"
#include "Tiempos.hpp"
#include "Tornillos.hpp"

// secuenciador con entrada de reloj
// sensible a la subida de la señal
// de 8 pasos y 8 perillas
// con boton para resetear

// para celebrar la materia computacional
// se contaran los pasos de 0 a 7

// decidir que hacer si se desconecta el reloj

struct SecuModule : Module
{
    enum ParamIds
    {
        PARAM_PASO_0,
        PARAM_PASO_1,
        PARAM_PASO_2,
        PARAM_PASO_3,
        PARAM_PASO_4,
        PARAM_PASO_5,
        PARAM_PASO_6,
        PARAM_PASO_7,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_A,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_SECUENCIADOR,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_PASO_0,
        LUZ_PASO_1,
        LUZ_PASO_2,
        LUZ_PASO_3,
        LUZ_PASO_4,
        LUZ_PASO_5,
        LUZ_PASO_6,
        LUZ_PASO_7,
        NUM_LIGHTS,
    };

    // schmitt trigger para detectar subidas de entrada
    dsp::SchmittTrigger detectorBordes;

    // variable para almacenar paso actual
    int pasoActual = 0;

    SecuModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // configurar pasos
        for (int i = 0; i < SecuModule::NUM_PARAMS; i++)
        {
            configParam(PARAM_PASO_0 + i, 0.0, 5.0, 1.0);
        }
    }

    void process(const ProcessArgs &args) override
    {

        // si detector de bordes detecta nuevo voltaje
        if (detectorBordes.process(inputs[ENTRADA_A].getVoltage()))
        {
            pasoActual = pasoActual + 1;
            pasoActual = pasoActual % 8;
        }

        for (int i = 0; i < NUM_LIGHTS; i++)
        {
            // apagar todas las luces
            lights[i].setSmoothBrightness(0.f, 5e-6f);
            // prender solamente la de interes
            lights[pasoActual].setSmoothBrightness(1.f, 5e-6f);
        }

        // emitir salida
        outputs[SALIDA_SECUENCIADOR].setVoltage(params[pasoActual].getValue());
    }
};

// widget
struct SecuModuleWidget : ModuleWidget
{
    SecuModuleWidget(SecuModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SecuModule.svg")));

        // tornillos
        agregarTornillos(this);

        // entradas
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(50.8 * 0.50, 128.4 * 0.40)), module, SecuModule::ENTRADA_A));

        // params
        for (int i = 0; i < SecuModule::NUM_PARAMS; i++)
        {
            // las primeras 4 van a una altura
            if (i < 4)
            {
                addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.8 * (0.20 + 0.2 * (i % 4)), 128.4 * 0.50)), module, SecuModule::PARAM_PASO_0 + i));
            }
            // las siguientes 4 van a otra altura
            else
            {
                addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.8 * (0.20 + 0.2 * (i % 4)), 128.4 * 0.60)), module, SecuModule::PARAM_PASO_0 + i));
            }
        }

        //  salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.8 * 0.50, 128.4 * 0.80)), module, SecuModule::SALIDA_SECUENCIADOR));

        // luces
        for (int i = 0; i < SecuModule::NUM_LIGHTS; i++)
        {
            if (i < 4)
            {
                addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(50.8 * (0.20 + 0.2 * (i % 4)), 128.4 * 0.70)), module, SecuModule::LUZ_PASO_0 + i));
            }
            else
            {
                addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(50.8 * (0.20 + 0.2 * (i % 4)), 128.4 * 0.75)), module, SecuModule::LUZ_PASO_0 + i));
            }
        }
    }
};

Model *modelSecu = createModel<SecuModule, SecuModuleWidget>("secu");