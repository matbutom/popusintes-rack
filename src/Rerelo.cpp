#include "Popusintes.hpp"

// definir tiempo de trigger
#define TIEMPO_TRIGGER 1e-3f

// modulo
struct RereloModule : Module
{
    enum ParamIds
    {
        // pulsos por minuto
        PARAM_PPM,
        NUM_PARAMS,
    };

    enum InputIds
    {
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_PULSO,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_PULSO,
        NUM_LIGHTS,
    };

    // clase auxiliar provista por rack
    dsp::PulseGenerator generadorPulsos;
    // variables de apoyo
    float contador, periodo;

    RereloModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        // los numeros son valor minimo, maximo e inicial
        // los strings son nombre del parametro y su unidad, ppm empieza con espacio
        configParam(PARAM_PPM, 30.0, 360.0, 120.0, "tempo", " ppm");
        contador = 0.f;
        periodo = 0.f;
    }

    void process(const ProcessArgs &args) override
    {
        // guardar en variable float el valor de la perilla
        float ppm = params[PARAM_PPM].getValue();
        // periodo calculado en samples
        periodo = 60.f * args.sampleRate / ppm;

        if (contador > periodo)
        {
            // TIEMPO_TRIGGER esta definida arriba
            // todavia no me gusta el nombre trigger
            // gate le puedo decir compuerta
            // pero trigger aun no se
            generadorPulsos.trigger(TIEMPO_TRIGGER);
            contador = contador - periodo;
        }

        contador = contador + 1;
        float salida = generadorPulsos.process(args.sampleTime);
        outputs[SALIDA_PULSO].setVoltage(10.f * salida);
        lights[LUZ_PULSO].setSmoothBrightness(salida, 5e-6f);
    }
};

// widget
struct RereloModuleWidget : ModuleWidget
{
    RereloModuleWidget(RereloModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/RereloModule.svg")));

        // tornillos
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // params
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.40)), module, RereloModule::PARAM_PPM));

        //  salidas
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.80)), module, RereloModule::SALIDA_PULSO));

        // luces
        addChild(createLightCentered<LargeLight<GreenLight>>(mm2px(Vec(25.4 * 0.5, 128.4 * 0.70)), module, RereloModule::LUZ_PULSO));
    }
};

Model *modelRerelo = createModel<RereloModule, RereloModuleWidget>("rerelo");