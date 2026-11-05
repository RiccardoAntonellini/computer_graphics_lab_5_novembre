#pragma once
#include <glad/glad.h>
#include "ShaderMaker.h"
#include "strutture.h"

extern unsigned int programId, programId_CP, programId_Triangul, programId_TG;

void INIT_SHADER(void)
{
    GLenum ErrorCheckValue = glGetError();

    //Shader per la visualizzazione dei vertici di una struttuta Figura o Curva

    char* vertexShader = (char*)"vertexshaderM.glsl";
    char* fragmentShader = (char*)"fragmentshaderM.glsl";
    // La funzione successiva crea un programma shader completo a partire da due shader individuali: 
    // uno per la gestione dei vertici e uno per la gestione dei pixel. 
    // Il programma shader risultante viene identificato da un numero univoco (il programId) che verr  utilizzato in seguito per associarlo ad un oggetto grafico e per renderizzarlo.
     //All'interno della funzione ShaderMaker::createProgram
        //Compilazione degli shader : La funzione compila i due shader individuali, verificando che non ci siano errori di sintassi.
        //Linkaggio : Una volta compilati, i due shader vengono collegati insieme per formare un programma shader completo.
        // Creazione dell'identificativo: Viene generato un identificativo univoco per il programma shader e viene restituito alla funzione chiamante.

    programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
    
    //Shader per la visualizzazione dei vertici di controllo di una curva
    vertexShader = (char*)"vertexshaderM_CP.glsl";
    fragmentShader = (char*)"fragmentshaderM.glsl";
    // La funzione successiva crea un programma shader completo a partire da due shader individuali: 
    // uno per la gestione dei vertici e uno per la gestione dei pixel. 
    // Il programma shader risultante viene identificato da un numero univoco (il programId) che verr  utilizzato in seguito per associarlo ad un oggetto grafico e per renderizzarlo.
     //All'interno della funzione ShaderMaker::createProgram
        //Compilazione degli shader : La funzione compila i due shader individuali, verificando che non ci siano errori di sintassi.
        //Linkaggio : Una volta compilati, i due shader vengono collegati insieme per formare un programma shader completo.
        // Creazione dell'identificativo: Viene generato un identificativo univoco per il programma shader e viene restituito alla funzione chiamante.

    programId_CP = ShaderMaker::createProgram(vertexShader, fragmentShader);

    //Shader per la visualizzazione dei vertici di una curva triangolarizzata
    vertexShader = (char*)"vertexshaderM_Triangl.glsl";
    fragmentShader = (char*)"fragmentshaderM.glsl";
    // La funzione successiva crea un programma shader completo a partire da due shader individuali: 
    // uno per la gestione dei vertici e uno per la gestione dei pixel. 
    // Il programma shader risultante viene identificato da un numero univoco (il programId) che verr  utilizzato in seguito per associarlo ad un oggetto grafico e per renderizzarlo.
     //All'interno della funzione ShaderMaker::createProgram
        //Compilazione degli shader : La funzione compila i due shader individuali, verificando che non ci siano errori di sintassi.
        //Linkaggio : Una volta compilati, i due shader vengono collegati insieme per formare un programma shader completo.
        // Creazione dell'identificativo: Viene generato un identificativo univoco per il programma shader e viene restituito alla funzione chiamante.

    programId_Triangul = ShaderMaker::createProgram(vertexShader, fragmentShader);

    //Shader per la visualizzazione delle tangenti nei vertici di controllo
    vertexShader = (char*)"vertexshaderM_TG.glsl";
    fragmentShader = (char*)"fragmentshaderM.glsl";
    // La funzione successiva crea un programma shader completo a partire da due shader individuali: 
    // uno per la gestione dei vertici e uno per la gestione dei pixel. 
    // Il programma shader risultante viene identificato da un numero univoco (il programId) che verr  utilizzato in seguito per associarlo ad un oggetto grafico e per renderizzarlo.
     //All'interno della funzione ShaderMaker::createProgram
        //Compilazione degli shader : La funzione compila i due shader individuali, verificando che non ci siano errori di sintassi.
        //Linkaggio : Una volta compilati, i due shader vengono collegati insieme per formare un programma shader completo.
        // Creazione dell'identificativo: Viene generato un identificativo univoco per il programma shader e viene restituito alla funzione chiamante.

    programId_TG = ShaderMaker::createProgram(vertexShader, fragmentShader);

}


void INIT_VAO(Figura* fig)
{
    //La funzione INIT_VAO  ha per input Figura* fig, che è  un puntatore  che punta all'indirizzo ricevuto.
    //In questo modo è  possibile modificare direttamente i dati della struttura originale, evitando la creazione di copie inutili.

    //&fig->VAO indica la posizione di memoria dove scrivere il valore ID risultante dalla generazione del VAO
    glGenVertexArrays(1, &fig->VAO);
    glBindVertexArray(fig->VAO);
    //Genero , rendo attivo, riempio il VBO della geometria dei vertices
    glGenBuffers(1, &fig->VBO_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_vertices);

    //fig->vertices.size() : Ottiene il numero di elementi nel vettore vertices della struttura Figura puntata da fig.
    // sizeof(vec3) : Ottiene la dimensione in byte di un singolo elemento di tipo vec3.
    //La moltiplicazione fig->vertices.size() * sizeof(vec3): calcola la dimensione totale in byte dei dati dei vertici. Calcola la dimensione totale in byte di tutti i dati dei vertici.

    //fig->vertices.data() : fornisce un puntatore all'inizio dei dati del vettore vertices. Questo puntatore indica la posizione esatta in memoria dove sono memorizzati i dati

    glBufferData(GL_ARRAY_BUFFER, fig->vertices.size() * sizeof(vec3), fig->vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //Genero , rendo attivo, riempio il VBO dei colori
    glGenBuffers(1, &fig->VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
    //Adesso carico il VBO dei coloris nel layer 2
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    //EBO di tipo indici
    glGenBuffers(1, &fig->EBO_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fig->EBO_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fig->indices.size() * sizeof(GLuint), fig->indices.data(), GL_STATIC_DRAW);

}

void INIT_VAO_DYNAMIC(Figura* fig, bool isCurve= false)
{
    const int max_indices = 2000;
    //La funzione INIT_VAO  ha per input Figura* fig, che è  un puntatore  che punta all'indirizzo ricevuto.
    //In questo modo è  possibile modificare direttamente i dati della struttura originale, evitando la creazione di copie inutili.

    glGenVertexArrays(1, &fig->VAO);
    glBindVertexArray(fig->VAO);
    //Genero , rendo attivo, riempio il VBO della geometria dei vertices
    glGenBuffers(1, &fig->VBO_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_vertices);

    //fig->vertices.size() : Ottiene il numero di elementi nel vettore vertices della struttura Figura puntata da fig.
    // sizeof(vec3) : Ottiene la dimensione in byte di un singolo elemento di tipo vec3.
    //La moltiplicazione fig->vertices.size() * sizeof(vec3): calcola la dimensione totale in byte dei dati dei vertici. Calcola la dimensione totale in byte di tutti i dati dei vertici.

    //fig->vertices.data() : fornisce un puntatore all'inizio dei dati del vettore vertices. Questo puntatore indica la posizione esatta in memoria dove sono memorizzati i dati

    glBufferData(GL_ARRAY_BUFFER, fig->nv * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //Genero , rendo attivo, riempio il VBO dei colori
    glGenBuffers(1, &fig->VBO_colors);
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, fig->nv * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    //Adesso carico il VBO dei colori nel layer 2
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // Nuovo VBO per distanza normalizzata (float)
    glGenBuffers(1, &fig->VBO_dist_norm);
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_dist_norm);
    glBufferData(GL_ARRAY_BUFFER, fig->nv * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(6);


    glGenBuffers(1, &fig->EBO_indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fig->EBO_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

    if (isCurve == true)
    {
        Curva* curva = static_cast<Curva*>(fig);
        //Genero , rendo attivo, riempio il VBO della geometria dei vertici di COntrollo
        glGenBuffers(1, &curva->VBO_CP);
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_CP);
        glBufferData(GL_ARRAY_BUFFER, curva->ncp * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);

        //Genero , rendo attivo, riempio il VBO dei colori nei vertici di controllo
        glGenBuffers(1, &curva->VBO_CP_colors);
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_CP_colors);
        glBufferData(GL_ARRAY_BUFFER, curva->ncp * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
        //Adesso carico il VBO dei colori nel layer 2
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(3);

        //Genero , rendo attivo, riempio il VBO della geometria dei vertici di COntrollo
        glGenBuffers(1, &curva->VBO_TG);
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_TG);
        glBufferData(GL_ARRAY_BUFFER, curva->ntg * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(4);

        //Genero , rendo attivo, riempio il VBO dei colori nei vertici di controllo
        glGenBuffers(1, &curva->VBO_TG_colors);
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_TG_colors);
        glBufferData(GL_ARRAY_BUFFER, curva->ntg * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
        //Adesso carico il VBO dei colori nel layer 2
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(5);
    }
    
}


void UPDATE_VAO_DYNAMIC(Figura* fig, bool isCurve = false)
{
    //La funzione INIT_VAO  ha per input Figura* fig, che è  un puntatore  che punta all'indirizzo ricevuto.
    //In questo modo è  possibile modificare direttamente i dati della struttura originale, evitando la creazione di copie inutili.

    glBindVertexArray(fig->VAO);
    //Genero , rendo attivo, riempio il VBO della geometria dei vertices
     glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_vertices);
      
    glBufferData(GL_ARRAY_BUFFER, fig->vertices.size() * sizeof(vec3), fig->vertices.empty() ? nullptr : fig->vertices.data(), GL_DYNAMIC_DRAW);

   

    //rendo attivo, riempio il VBO dei colori
    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_colors);
    glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.empty() ? nullptr : fig->colors.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fig->EBO_indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, fig->indices.size() * sizeof(GLuint), fig->indices.empty() ? nullptr : fig->indices.data(), GL_DYNAMIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_dist_norm);
    glBufferData(GL_ARRAY_BUFFER, fig->distNormalized.size() * sizeof(float), fig->distNormalized.empty() ? nullptr : fig->distNormalized.data(), GL_DYNAMIC_DRAW);
        
    if (isCurve == true)
    {

        Curva* curva = static_cast<Curva*>(fig);
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_CP);
        glBufferData(GL_ARRAY_BUFFER, curva->CP.size() * sizeof(vec3), curva->CP.empty() ? nullptr : curva->CP.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_CP_colors);
        glBufferData(GL_ARRAY_BUFFER, curva->colCP.size() * sizeof(vec4), curva->colCP.empty() ? nullptr : curva->colCP.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_TG);
        glBufferData(GL_ARRAY_BUFFER, curva->tg.size() * sizeof(vec3), curva->tg.empty() ? nullptr : curva->tg.data(), GL_DYNAMIC_DRAW);

         
        glBindBuffer(GL_ARRAY_BUFFER, curva->VBO_TG_colors);
        glBufferData(GL_ARRAY_BUFFER, curva->col_tg.size() * sizeof(vec4), curva->col_tg.empty() ? nullptr : curva->col_tg.data(), GL_DYNAMIC_DRAW);
        //Adesso carico il VBO dei colori nel layer 2
        


    }


}