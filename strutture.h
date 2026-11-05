/*
#pragma once //: È una direttiva che indica al compilatore di includere il file una sola volta per unità di compilazione.
//  Il meccanismo esatto varia a seconda del compilatore, ma in generale si basa su un meccanismo interno per tenere traccia dei file già inclusi.
//Non è parte dello standard C++, quindi la sua disponibilità e il comportamento preciso possono variare tra diversi compilatori. Tuttavia, è supportata dalla maggior parte dei compilatori moderni.
*/


#pragma once

#include "lib.h"

enum Forma {
    FARFALLA = 0,
    CUORE = 1
};


typedef struct {
    GLuint VAO;  //identificatore del  Vertex Array Object (VAO)
    GLuint  VBO_vertices; //identificatore del  Vertex Buffer Object (VBO) che contiene le coordinate dei vertici dell'oggetto.
    GLuint  VBO_colors;   // l'identificatore del VBO che contiene i colori dei vertici dell'oggetto.
    GLuint EBO_indices;  // Element Buffer Object per gli indici dei vertici
    GLuint VBO_dist_norm;
    int nTriangles;  // numero di triangoli che costituiscono l'oggetto.
    // vertices
    vector<vec3> vertices;  //vector che contiene le coordinate dei vertici dell'oggetto. Ogni elemento del vettore è un vec3, che rappresenta una posizione 3D (x, y, z).
    vector<vec4> colors; //vector che contiene i colori dei vertici dell'oggetto. Ogni elemento del vettore è un vec4, che rappresenta un colore RGBA (red, green, blue, alpha).
    vector<uint32_t>indices;
    vector<float> distNormalized;
    // Numero vertices  
    int nv;  //Numero di vertici della figura geometrica
    int render; //specifica come devono essere interpretati i vertici  nella fase di rendering
    mat4 Model; //Matrice di Modellazione associata alla figura che posizione l'oggetto nel mondo
    vec3 position;  //Posizione in coordinate del mondo
    vec3 scale;  //scala
    unsigned int shader_programId; // Identificatore del programma shader associato all'oggetto per il rendering.
    vec4 min_BB_obj;// Coordinate del corner inferiore sinistro del Bounding Box dell'oggetto, definite nel sistema di riferimento locale.
    vec4 max_BB_obj;// Coordinate del corner superiore destro del Bounding Box dell'oggetto, definite nel sistema di riferimento locale.
    vec4 min_BB_w; // Coordinate del corner inferiore sinistro del Bounding Box dell'oggetto, trasformate nel sistema di riferimento del mondo.
    vec4 max_BB_w;// Coordinate del corner superiore destro del Bounding Box dell'oggetto, trasformate nel sistema di riferimento del mondo.
    float timerFig;
    bool isalive;
} Figura;

// La struttura Curva estende la struttura Figura per rappresentare specificamente una curva.  Aggiunge i seguenti membri :
//Identificatori OpenGL aggiuntivi : VBO_CP, VBO_CP_colors, VBO_TG, VBO_TG_colors : Questi identificatori sono utilizzati per gestire i dati dei control points(punti di controllo della curva), dei loro colori e dei vettori tangenti.
//Dati geometrici della curva : CP, colCP, tg, col_tg : Contengono le coordinate e i colori dei control points e dei vettori tangenti, che definiscono la forma della curva.
//Parametri di interpolazione : Derivata, Tens, Bias, Cont : Questi parametri sono  utilizzati per controllare il comportamento dell'interpolazione tra i control points, ma la loro esatta funzione dipende dall'algoritmo di interpolazione utilizzato(ad esempio, spline di Bézier, NURBS).
//Informazioni sulla curva : ncp, ntg : Indicano il numero di control points e di vettori tangenti.
//In questo modo è possibile utilizzare le funzioni che prendono in input variabili di tipo Figura anche per il tipo curva.


struct Curva :Figura {
    bool chiusaC1 = false;
    vec4 coloreCurva = vec4(0.90f, 0.35f, 0.35f, 1.0f);
    GLuint VBO_CP;             // Vertex Buffer Object per le coordinate dei control points
    GLuint VBO_CP_colors;      // Vertex Buffer Object per i colori dei control points
    GLuint VBO_TG;             // Vertex Buffer Object per le coordinate delle tangenti
    GLuint VBO_TG_colors;      // Vertex Buffer Object per i colori delle tangenti
    // Dati geometrici della curva
    vector<vec3> CP;        // Coordinate dei control points (punti di controllo)
    vector<vec4> colCP;     // Colori dei control points
    vector<vec3> tg;        // Coordinate dei vettori tangenti ai control points
    vector<vec4> col_tg;     // Colori dei vettori tangenti

    // Parametri di interpolazione
    vector<vec3> Derivata;  // Vettore derivata: se è uguale al vettore nullo significa che non sono state modificate 
    //T,B e C dellla derivata nel vertice di controllo
    vector<float> Tens;       // Tensore di tensione
    vector<float> Bias;       // Bias
    vector<float> Cont;       // Continuity

    // Informazioni sulla curva

    int ncp;                    // Numero di control points
    int ntg;                    // Numero di vettori tangenti



};