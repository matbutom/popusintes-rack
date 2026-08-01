#include "Popusintes.hpp"

// modulo
struct PaneModulo : Module
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

    PaneModulo()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct PaneModuloWidget : ModuleWidget
{
    PaneModuloWidget(PaneModulo *modulo)
    {

        setModule(modulo);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PaneModulo.svg")));

        // tornillos
        agregarTornillos(this);
    }
};

Model *modeloPane = createModel<PaneModulo, PaneModuloWidget>("pane");