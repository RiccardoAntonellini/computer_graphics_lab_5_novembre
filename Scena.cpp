#pragma once
#include <glad/glad.h>
#include "ShaderMaker.h"
#include "strutture.h"
#include "init_geometrie.h"
#include "inizializzazioni.h"




extern int width, height;
void costruisci_Scena(vector<Figura>* Scena)
{
 
	
	Figura butterfly, heart, bkg;

	//Costruisce il piano del background

	
	//Costruisce i vertici del cuore: attributo posizione e color (in coordinate del sistema di riferimento dell'oggetto)
	butterfly.render = GL_TRIANGLE_FAN;
	butterfly.nTriangles = 60;
	INIT_BUTTERFLY(0.0,0.0,0.5,0.5,&butterfly, vec4(1.0, 1.0, 1.0, 0.5), vec4(1.0, 1.0, 0.0, 0.7));
	
	//Genera il VAO della struttura  butterrfly
	INIT_VAO(&butterfly);
	//Definisce la matrice di modellazione del butterfly: Model=T*S*R  per posizionare, scalare ed orientare l'oggetto nel mondo
	butterfly.Model = mat4(1.0);
	butterfly.Model = translate(butterfly.Model, vec3((float)(width)/2.0, (float)(height)/2.0, 0.0));
	butterfly.Model = scale(butterfly.Model, vec3((float)(width) / 9.0, (float)(width) / 9.0, 1.0));
	//Aggiunge al vector Scena la struttura di tipo Figura Butterfly
	Scena->push_back(butterfly);
	
	//Costruisce i vertici del cuore: attributo posizione e color attributo posizione e color (in coordinate del sistema di riferimento dell'oggetto)
	heart.nTriangles = 60;
	heart.render = GL_TRIANGLE_FAN;
	heart.nTriangles = 60;
	INIT_HEART(0.0, 0.0, 0.05, 0.05, &heart, vec4(1.0, 1.0, 1.0, .6), vec4(1.0, 0.0, 0.0, 0.6));
	INIT_VAO(&heart);
	
	//Definisce la matrice di Modellazione, Model=T*S*R  per posizionare, scalare ed orientare l'oggetto nel mondo
	heart.Model = mat4(1.0);
	heart.Model = translate(heart.Model, vec3((float)(width) / 3.0, (float)(height) / 3.0, 0.0));
	heart.Model = scale(heart.Model, vec3((float)(width) / 10.0, (float)(width) / 10.0, 1.0));
	//Aggiungere al vector scena la struttura cuore.
	Scena->push_back(heart);
}

