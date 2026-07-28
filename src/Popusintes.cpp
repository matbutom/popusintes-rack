#include "Popusintes.hpp"

Plugin *pluginInstance;

void init(rack::Plugin *p)
{
	pluginInstance = p;

	p->addModel(modelCompa);
	p->addModel(modelEnvo);
	p->addModel(modelPane);
	p->addModel(modelRecta);
	p->addModel(modelRelo);
	p->addModel(modelRerelo);
	p->addModel(modelSecu);
	p->addModel(modelSuma);
}
