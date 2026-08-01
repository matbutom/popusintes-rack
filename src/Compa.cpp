#include "Popusintes.hpp"
#include "comun/Dimensiones.hpp"
#include "comun/Posicionador.hpp"
#include "comun/Tiempos.hpp"
#include "comun/Tornillos.hpp"

// modulo
struct CompaModule : Module
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

    CompaModule()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        // canal A
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

        // ---- canal B ----
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

// widget
struct CompaModuleWidget : ModuleWidget
{
    CompaModuleWidget(CompaModule *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CompaModule.svg")));

        // tornillos
        agregarTornillos(this);

        // entradas canal A
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.30)), module, CompaModule::ENTRADA_A_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.30)), module, CompaModule::ENTRADA_A_2));

        // entradas canal B
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.60)), module, CompaModule::ENTRADA_B_1));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.60)), module, CompaModule::ENTRADA_B_2));

        // salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.90)), module, CompaModule::SALIDA_COMPA_A));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.90)), module, CompaModule::SALIDA_COMPA_B));

        // luces
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.20, 128.4 * 0.80)), module, CompaModule::LUZ_SALIDA_A));
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.80, 128.4 * 0.80)), module, CompaModule::LUZ_SALIDA_B));
    }
};

Model *modelCompa = createModel<CompaModule, CompaModuleWidget>("compa");