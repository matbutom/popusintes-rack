#include "Popusintes.hpp"

// definir tiempo de trigger
#define TIEMPO_TRIGGER 1e-3f

// tiempos minimo y maximo (segundos)
#define TIEMPO_MIN 0.001f
#define TIEMPO_MAX 5.0f

// definir ancho modulo en mm (5 hp)
#define MODULO_ANCHO 25.4f

// definir altura modulo en mm (3 u)
#define MODULO_ALTURA 128.4f

// modulo
struct EnvoModule : Module
{
    enum ParamIds
    {
        PARAM_SUBIDA,
        PARAM_BAJADA,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_PULSO_A,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_ENVO_A,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_ENVO_A,
        NUM_LIGHTS,
    };

    enum EstadoEnvo {
        ESTADO_REPOSO,
        ESTADO_SUBIDA,
        ESTADO_BAJADA,
    };

    EstadoEnvo estado = ESTADO_REPOSO;
    float salida = 0.0f;
    dsp::SchmittTrigger detectorPulso;

    EnvoModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(
            PARAM_SUBIDA, TIEMPO_MIN, TIEMPO_MAX,
            0.1f, "subida", " s");
        configParam(
            PARAM_BAJADA , TIEMPO_MIN, TIEMPO_MAX,
            0.1f, "bajada" " s");
    }

    void process(const ProcessArgs &args) override
    {
        // detectar subida de pulso (umbral de 1V)
        bool detectado = detectorPulso.process(inputs[ENTRADA_PULSO_A].getVoltage(), 0.1f, 1.0f);

        if (detectado) {
            estado = ESTADO_SUBIDA;
        }

        float tiempoSubida = params[PARAM_SUBIDA].getValue();
        float tiempoBajada = params[PARAM_BAJADA].getValue();

        switch(estado) {
            case ESTADO_SUBIDA: {
                // incremento para llegar a 10V en tiempoSubida
                float incremento = (10.0f / tiempoSubida) * args.sampleTime;
                salida = salida + incremento;

                // detectar si ya subimos, para empezar la bajada
                if (salida >= 10.0f) {
                    salida = 10.0f;
                    estado = ESTADO_BAJADA;
                }
                break;
            }

            case ESTADO_BAJADA: {
                //dencremento para llegar a 0V en tiempoBajada
                float decremento = (10.0f / tiempoBajada) * args.sampleTime;
                salida = salida - decremento;

                if (salida <= 0.0f) {
                    salida = 0.0f;
                    estado = ESTADO_REPOSO;
                }
                break;
            }

            case ESTADO_REPOSO:
            default:
                break;
        }

        // emitir voltaje de salida
        outputs[SALIDA_ENVO_A].setVoltage(salida);
        lights[LUZ_ENVO_A].setBrightness(salida * 0.1f);
    }
};

// widget
struct EnvoModuleWidget : ModuleWidget
{
    EnvoModuleWidget(EnvoModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/EnvoModule.svg")));

        // tornillos
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // parametros
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.35, 30)), module, EnvoModule::PARAM_SUBIDA));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.05, 30)), module, EnvoModule::PARAM_BAJADA));


        // entrada
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.7, 60)), module, EnvoModule::ENTRADA_PULSO_A));


        // salida
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.7, 100)), module, EnvoModule::SALIDA_ENVO_A));


        // luz
        addChild(createLightCentered<SmallLight<GreenLight>>(mm2px(Vec(12.7, 90)), module, EnvoModule::LIGHT_ENVO));

    }
};

Model *modelEnvo = createModel<EnvoModule, EnvoModuleWidget>("envo");