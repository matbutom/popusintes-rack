#include "Popusintes.hpp"

// modulo
struct SumaModulo : Module
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

    SumaModulo()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct SumaModuloWidget : ModuleWidget
{
    SumaModuloWidget(SumaModulo *modulo)
    {

        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SumaModulo.svg")));

        // tornillos
        agregarTornillos(this);
    }
};

Model *modeloSuma = createModel<SumaModulo, SumaModuloWidget>("suma");