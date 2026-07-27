#include "Popusintes.hpp"
#include "Tornillos.hpp"

// modulo
struct SumaModule : Module
{
    enum ParamIds
    {
        NUM_PARAMS,
    };

    enum InputIds
    {
        NUM_INPUTS,
    };

    enum OutputIds
    {
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        NUM_LIGHTS,
    };

    SumaModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct SumaModuleWidget : ModuleWidget
{
    SumaModuleWidget(SumaModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SumaModule.svg")));

        // tornillos
        agregarTornillos(this);
    }
};

Model *modelSuma = createModel<SumaModule, SumaModuleWidget>("suma");