#include "Popusintes.hpp"
#include "CanalRelo.hpp"
#include "Dimensiones.hpp"
#include "Espaciado.hpp"
#include "Posicionador.hpp"
#include "Tiempos.hpp"
#include "Tornillos.hpp"

// define porcentajes columnas
#define PORCENTAJE_COLUMNA_IZQ 0.30f
#define PORCENTAJE_COLUMNA_DER 0.70f

// define coordenadas ppm
#define PORCENTAJE_PPM_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_PPM_Y 0.15f

// define coordenadas resinc
#define PORCENTAJE_BOTON_RESINC_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_BOTON_RESINC_Y 0.35f
#define PORCENTAJE_ENTRADA_RESINC_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_ENTRADA_RESINC_Y (PORCENTAJE_BOTON_RESINC_Y + espaciado::DELTA_Y_BOTON_ENTRADA)

// definir coordenadas desfase
#define PORCENTAJE_DESFASE_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_DESFASE_B_Y 0.60f
#define PORCENTAJE_DESFASE_CV_ATEN_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_DESFASE_CV_ATEN_Y (PORCENTAJE_DESFASE_B_Y + espaciado::DELTA_Y_PERILLA_ATENUVERSOR)
#define PORCENTAJE_ENTRADA_DESFASE_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_ENTRADA_DESFASE_B_Y (PORCENTAJE_DESFASE_B_Y + 2.f * espaciado::DELTA_Y_PERILLA_ATENUVERSOR)

// definir coordenadas salida y luz a
#define PORCENTAJE_LUCES_A_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_LUCES_A_Y 0.85f
#define PORCENTAJE_SALIDA_A_X (PORCENTAJE_COLUMNA_IZQ)
#define PORCENTAJE_SALIDA_A_Y (PORCENTAJE_LUCES_A_Y + espaciado::DELTA_Y_SALIDA_LUZ)

// definir coordenadas salida y luz b
#define PORCENTAJE_LUCES_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_LUCES_B_Y 0.85f
#define PORCENTAJE_SALIDA_B_X (PORCENTAJE_COLUMNA_DER)
#define PORCENTAJE_SALIDA_B_Y (PORCENTAJE_LUCES_B_Y + espaciado::DELTA_Y_SALIDA_LUZ)

// modulo
struct ReloModule : Module
{
    enum ParamIds
    {
        // pulsos por minuto
        PARAM_PPM,
        // DESFASE de B en % de PARAM_PPM
        PARAM_DESFASE_B,
        // atenuversor para la entrada CV de desfase
        PARAM_DESFASE_CV_ATEN,
        // boton que resincroniza A y B
        PARAM_BOTON_RESINC,
        NUM_PARAMS,
    };

    enum InputIds
    {
        ENTRADA_RESINC,
        // CV que se suma a PARAM_DESFASE_B (bipolar +-5V)
        ENTRADA_DESFASE_B,
        NUM_INPUTS,
    };

    enum OutputIds
    {
        SALIDA_PULSO_A,
        SALIDA_PULSO_B,
        NUM_OUTPUTS,
    };

    enum LightsIds
    {
        LUZ_PULSO_A,
        LUZ_PULSO_B,
        NUM_LIGHTS,
    };

    // un CanalRelo por canal: agrupa generador de pulsos, contador y periodo
    CanalRelo canalA;
    CanalRelo canalB;
    dsp::SchmittTrigger botonResinc;
    dsp::SchmittTrigger entradaResinc;

    ReloModule()
    {

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        // los numeros son valor minimo, maximo e inicial
        // los strings son nombre del parametro y su unidad, ppm empieza con espacio
        configParam(PARAM_PPM, 30.0, 360.0, 120.0, "tempo", " ppm");

        configParam(PARAM_DESFASE_B, -10.0, 10.0, 0.0, "desfase canal b", " %");
        configParam(PARAM_DESFASE_CV_ATEN, -1.0, 1.0, 0.0, "cv desfase atenuversor");
        configParam(PARAM_BOTON_RESINC, 0.0, 1.0, 0.0, "resincronizar");

        configInput(ENTRADA_RESINC, "resincronizar");
        configInput(ENTRADA_DESFASE_B, "cv desfase b");

        configOutput(SALIDA_PULSO_A, "pulso a");
        configOutput(SALIDA_PULSO_B, "pulso b");
    }

    void process(const ProcessArgs &args) override
    {
        // guardar en variables float los valor de la perillas
        float ppmA = params[PARAM_PPM].getValue();

        // desfase = perilla + cv atenuado
        // cv bipolar entre -5V y +5V
        // con atenuversor en 1.0 cubre el rango completo

        float desfase = params[PARAM_DESFASE_B].getValue();
        desfase = desfase + inputs[ENTRADA_DESFASE_B].getVoltage() * params[PARAM_DESFASE_CV_ATEN].getValue() * 2.f;

        // atajar desfase en rango de -10.0f a +10.f

        if (desfase > 10.f)
        {
            desfase = 10.f;
        }
        else if (desfase < -10.f)
        {
            desfase = -10.f;
        }

        float ppmB = ppmA * (1.f + desfase / 100.f);

        // resincronizar boton del panel o externo
        bool resetBoton = botonResinc.process(params[PARAM_BOTON_RESINC].getValue());
        bool resetExterno = entradaResinc.process(inputs[ENTRADA_RESINC].getVoltage());
        bool reset = resetBoton || resetExterno;

        if (reset)
        {
            canalA.reiniciar();
            canalB.reiniciar();

            // ademas de reiniciar los contadores
            // emitimuos un puslo en A y B
            // el resincronizado se escucha en ese instante
            canalA.generarPulso(tiempos::PULSO_MS);
            canalB.generarPulso(tiempos::PULSO_MS);
        }

        float salidaA = canalA.procesar(args.sampleTime, args.sampleRate, ppmA, tiempos::PULSO_MS);
        float salidaB = canalB.procesar(args.sampleTime, args.sampleRate, ppmB, tiempos::PULSO_MS);

        outputs[SALIDA_PULSO_A].setVoltage(10.f * salidaA);
        lights[LUZ_PULSO_A].setSmoothBrightness(salidaA, 5e-6f);

        outputs[SALIDA_PULSO_B].setVoltage(10.f * salidaB);
        lights[LUZ_PULSO_B].setSmoothBrightness(salidaB, 5e-6f);
    }
};

// widget
struct ReloModuleWidget : ModuleWidget
{
    ReloModuleWidget(ReloModule *module)
    {

        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ReloModule.svg")));

        // tornillos
        agregarTornillos(this);

        Posicionador posicionador(dimensiones::RELO_ANCHO, dimensiones::RELO_ALTURA);

        // params
        addParam(createParamCentered<RoundBlackKnob>(
            posicionador.posicion(PORCENTAJE_PPM_X, PORCENTAJE_PPM_Y),
            module, ReloModule::PARAM_PPM));

        addParam(createParamCentered<RoundSmallBlackKnob>(
            posicionador.posicion(PORCENTAJE_DESFASE_B_X, PORCENTAJE_DESFASE_B_Y),
            module, ReloModule::PARAM_DESFASE_B));

        // atenuverter chico para la entrada CV de desfase
        addParam(createParamCentered<Trimpot>(
            posicionador.posicion(PORCENTAJE_DESFASE_CV_ATEN_X, PORCENTAJE_DESFASE_CV_ATEN_Y),
            module, ReloModule::PARAM_DESFASE_CV_ATEN));

        // boton resinc
        addParam(createParamCentered<TL1105>(
            posicionador.posicion(PORCENTAJE_BOTON_RESINC_X, PORCENTAJE_BOTON_RESINC_Y),
            module, ReloModule::PARAM_BOTON_RESINC));

        // entradas
        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_RESINC_X, PORCENTAJE_ENTRADA_RESINC_Y),
            module, ReloModule::ENTRADA_RESINC));

        addInput(createInputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_ENTRADA_DESFASE_B_X, PORCENTAJE_ENTRADA_DESFASE_B_Y),
            module, ReloModule::ENTRADA_DESFASE_B));

        //  salidas
        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_A_X, PORCENTAJE_SALIDA_A_Y),
            module, ReloModule::SALIDA_PULSO_A));

        addOutput(createOutputCentered<PJ301MPort>(
            posicionador.posicion(PORCENTAJE_SALIDA_B_X, PORCENTAJE_SALIDA_B_Y),
            module, ReloModule::SALIDA_PULSO_B));

        // luces
        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_A_X, PORCENTAJE_LUCES_A_Y),
            module, ReloModule::LUZ_PULSO_A));

        addChild(createLightCentered<MediumLight<GreenLight>>(
            posicionador.posicion(PORCENTAJE_LUCES_B_X, PORCENTAJE_LUCES_B_Y),
            module, ReloModule::LUZ_PULSO_B));
    }
};

Model *modelRelo = createModel<ReloModule, ReloModuleWidget>("relo");