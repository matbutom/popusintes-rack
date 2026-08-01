#include "Popusintes.hpp"

Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;

	p->addModel(modeloCompa);
	p->addModel(modeloEnvo);
	p->addModel(modeloPane);
	p->addModel(modeloRecta);
	p->addModel(modeloRelo);
	p->addModel(modeloRerelo);
	p->addModel(modeloSecu);
	p->addModel(modeloSuma);
}
