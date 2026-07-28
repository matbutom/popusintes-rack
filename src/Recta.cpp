#include "Popusintes.hpp"
#include "Dimensiones.hpp"
#include "Posicionador.hpp"
#include "Tiempos.hpp"
#include "Tornillos.hpp"

// modulo
struct RectaModule : Module
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

    RectaModule()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct RectaModuleWidget : ModuleWidget
{
    RectaModuleWidget(RectaModule *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RectaModule.svg")));

        // tornillos
        agregarTornillos(this);

        // entradas canal A
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.30)), module, RectaModule::ENTRADA_A_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.30)), module, RectaModule::ENTRADA_A_2));

        // entradas canal B
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.60)), module, RectaModule::ENTRADA_B_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.60)), module, RectaModule::ENTRADA_B_2));

        // salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.90)), module, RectaModule::SALIDA_RECTA_A));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.90)), module, RectaModule::SALIDA_RECTA_B));

        // luces
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.80)), module, RectaModule::LUZ_SALIDA_A));
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.80)), module, RectaModule::LUZ_SALIDA_B));
    }
};

Model *modelRecta = createModel<RectaModule, RectaModuleWidget>("recta");