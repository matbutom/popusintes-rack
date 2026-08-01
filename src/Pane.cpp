#include "Popusintes.hpp"
#include "comun/Dimensiones.hpp"
#include "comun/Tornillos.hpp"

// modulo
struct PaneModule : Module
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

    PaneModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

    void process(const ProcessArgs &args) override
    {
        ;
    }
};

// widget
struct PaneModuleWidget : ModuleWidget
{
    PaneModuleWidget(PaneModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PaneModule.svg")));

        // tornillos
        agregarTornillos(this);
    }
};

Model *modelPane = createModel<PaneModule, PaneModuleWidget>("pane");