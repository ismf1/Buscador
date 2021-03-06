#include "buscador.h"
#include <math.h>

using namespace std;

//##################### RESULTADO RI ########################################
ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np) {
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
}

double
ResultadoRI::VSimilitud() const {
    return vSimilitud;
}

int
ResultadoRI::NumPregunta() const{
    return numPregunta;
}

long int ResultadoRI::IdDoc() const {
    return idDoc;
}

bool ResultadoRI::operator< (const ResultadoRI& lhs) const {
    return (vSimilitud < lhs.vSimilitud);
}

ostream& operator<<(ostream &os, const ResultadoRI &res){
    os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta << "\n";
    return os;
}

//######################### BUSCADOR ###########################################
/* Este constructor se pone en la parte privada porque no se permitirá crear  
un  buscador  sin  inicializarlo  convenientemente  a  partir  de  una indexación. 
*/
/*  Se  inicializará  con  todos  los  campos  vacíos  y  la  variable  
privada "formSimilitud" con valor 0 y las constantes de cada modelo: 
"c = 2;  k1 = 1.2; b = 0.75"
*/
Buscador::Buscador(){
    k1 = 1.2;
    b = 0.75;
    c = 2;
    formSimilitud = 0;
}

/* 
Constructor  para  inicializar  Buscador  a  partir  de  la  indexación generada
previamente y almacenada en "directorioIndexacion". En caso que no exista el
directorio o que no contenga los datos de la indexación se enviará a cerr la
excepción correspondiente y se continuará la ejecución del programa manteniendo los
índices vacíos
*/
/*
Inicializará  la  variable  privada  "formSimilitud"  a  "f"  y  las constantes de
cada modelo: "c = 2;  k1 = 1.2;  b = 0.75;"
*/
Buscador::Buscador(const string& directorioIndexacion, const int& f):IndexadorHash(directorioIndexacion){
    formSimilitud = f;
    c = 2;
    k1 = 1.2;
    b = 0.75;
}

Buscador::Buscador(const Buscador& buscador):IndexadorHash(buscador){
    docsOrdenados = buscador.docsOrdenados;
    formSimilitud = buscador.formSimilitud; 
    c = buscador.c; 
    k1 = buscador.k1;
    b = buscador.b;
}

Buscador::~Buscador(){
    
}

/* En los métodos de "Buscar" solo se evaluarán TODOS los documentos que contengan
alguno  de  los  términos  de  la  pregunta  (tras  eliminar  las palabras de parada).
*/
Buscador&
Buscador::operator= (const Buscador& buscador){
    IndexadorHash::operator=(buscador); //FALTA REVISAR
    docsOrdenados = buscador.docsOrdenados;
    formSimilitud = buscador.formSimilitud; 
    c = buscador.c; 
    k1 = buscador.k1;
    b = buscador.b;
}

//FALTA RETURN FALSE
bool
Buscador::Buscar(const int& numDocumentos,const int& nPregunta){
    int maxId=-1;
    for(unordered_map<string, InfDoc>::const_iterator it=indiceDocs.begin();it!=indiceDocs.end();it++){
        if(it->second.getIdDoc()>maxId){
            maxId=it->second.getIdDoc();
        }
    }
    
    namesDocs.resize(maxId+1);
    
    for(unordered_map<string, InfDoc>::const_iterator it=indiceDocs.begin();it!=indiceDocs.end();it++){
        namesDocs[it->second.getIdDoc()] = it->first;
    }
    
    //Vector de documentos ordenados por id
    vector<double> resultados(maxId+1,0);
    //vector<int> valoresComunes();   //avgdl , 

    //Calcula los valores comunes en funcion de la funcion de similitud, el resto devuelve -1
    //calcularValoresComunes(...);
    //Idea: Poner valores en un vector para luego pasar unicamente el puntero del vector a la funcion

    double idf=-1;
    double avgdl = (double)informacionColeccionDocs.getNumTotalPalSinParada()/informacionColeccionDocs.getNumDocs();
    double lambda_t=-1;
    int ft=-1;
    double wiq=-1;
    double logwid0=-1;
    double logwid1=-1;
    double DFR_values[4];

    //Para cada palabra
    for(unordered_map<string, InformacionTerminoPregunta>::const_iterator itPal = indicePregunta.begin() ; itPal!=indicePregunta.end() ; itPal++){
        InformacionTermino infTerm = indice[itPal->first];

        unordered_map<long int, InfTermDoc> l_docs = infTerm.getL_docs();
        int n = l_docs.size();
        //vector<int> valoresPalabra();
        //calcularValoresPalabra(...);
        //Para cada termino con la palabra indexada
        
        //FALTA LIMPIAR:
        if(formSimilitud == 0){
            //DFR
            DFR_values[0] = (double)ft/informacionColeccionDocs.getNumDocs();    //lambda_t
            DFR_values[1] = (double)itPal->second.getFt()/infPregunta.getNumTotalPalSinParada(); //wiq
            DFR_values[2] = log2(1+lambda_t); //logwid0
            DFR_values[3] = log2((1+lambda_t)/lambda_t); //logwid1
        }else{
            //BM25
            idf = calcIdf(n);
        }
        
        

        for(unordered_map<long int, InfTermDoc>::const_iterator itDoc = l_docs.begin() ; itDoc != l_docs.end() ; itDoc++){

            resultados[itDoc->first] += similitudPalabraDoc(avgdl,idf,infTerm,itDoc,namesDocs,
                                                            &DFR_values[0]);
        }
    }

    //Actualizamos docsOrdenados
    priority_queue<ResultadoRI> docsOrdenadosTot;
    for(int i=0;i<resultados.size();i++){
        if(resultados[i]!=0){
            ResultadoRI ri(resultados[i], i, nPregunta);

            docsOrdenadosTot.push(ri);
        }
    }


    //FALTA COMPROBAR SI SE ESTAN FILTRANDO LOS MAS IMPORTANTES O LOS MENOS
    //Filtramos docsOrdenados
    for(int i=0;i<numDocumentos && !docsOrdenadosTot.empty() ; i++){
        docsOrdenados.push_back(docsOrdenadosTot.top());
        docsOrdenadosTot.pop();
    }

    return true;
}

/* Devuelve true si en IndexadorHash.pregunta hay indexada una pregunta no  vacía
con  algún  término  con  contenido,  y  si  sobre  esa  pregunta  se finaliza la
búsqueda correctamente con la fórmula de similitud indicada en la variable privada
"formSimilitud". 
*/
/* Por ejemplo, devuelve falso si no finaliza la búsqueda por falta de memoria,
mostrando el mensaje de error correspondiente, e indicando el documento y término
en el que se ha quedado.
*/
/* Se guardarán los primeros "numDocumentos" documentos más relevantes en  la
variable  privada  "docsOrdenados"  en  orden  decreciente  según  la relevancia
sobre  la  pregunta  (se  vaciará  previamente  el  contenido  de esta  variable
antes  de  realizar  la  búsqueda).  Se  almacenarán  solo  los documentos  que
compartan  algún  término  (no  de  parada)  con  la  query (aunque ese número de
documentos sea inferior a "numDocumentos"). Como número de pregunta en
"ResultadoRI.numPregunta" se almacenará el valor 0.  En  caso  de  que  no  se
introduzca  el  parámetro  "numDocumentos", entonces dicho parámetro se inicializará
a 99999)
*/
bool
Buscador::Buscar(const int& numDocumentos){
    //Vaciamos docsOrdenados
    docsOrdenados.clear();
    
    return Buscar(numDocumentos,0);
}

/* Realizará la búsqueda entre el número de pregunta "numPregInicio" y "numPregFin", 
ambas preguntas incluidas. El corpus de preguntas estará en el directorio 
"dirPreguntas", y tendrá la estructura de cada pregunta en  un  fichero  
independiente,  de  nombre  el  número  de  pregunta,  y extensión  ".txt"  
(p.ej.  1.txt  2.txt  3.txt  ...  83.txt).  Esto  significa que habrá que indexar 
cada pregunta por separado y ejecutar una búsqueda por cada pregunta añadiendo los 
resultados de cada pregunta (junto con su número de pregunta) en la variable privada 
"docsOrdenados". Asimismo, se supone que previamente se mantendrá la indexación del 
corpus.
*/
/* Se guardarán los primeros "numDocumentos" documentos más relevantes para  cada  
pregunta  en  la  variable  privada  "docsOrdenados".  Se almacenarán  solo  los  
documentos  que  compartan  algún  término  (no  de parada)  con  la  query  
(aunque  ese  número  de  documentos  sea  inferior  a "numDocumentos").
*/
/*  La  búsqueda  se  realiza  con  la  fórmula  de  similitud  indicada  en  
la variable privada "formSimilitud".
*/
/*  Devuelve  falso  si  no  finaliza  la  búsqueda  (p.ej.  por  falta  de memoria), 
mostrando  el  mensaje  de  error  correspondiente,  indicando  el documento, 
pregunta y término en el que se ha quedado.
*/
bool  
Buscador::Buscar(const  string&  dirPreguntas,  const  int&  numDocumentos,  const int& numPregInicio, const int& numPregFin){
    ifstream iFile;
    string nameFile;
    string preguntaLocal;

    //Vaciamos docsOrdenados
    docsOrdenados.clear();

    for(int i=numPregInicio;i<numPregFin;i++){
        nameFile = dirPreguntas + "/" + to_string(i) + ".txt";

        iFile.open(nameFile.c_str());

        if(!iFile) {
            //FALTA COMPROBAR CONTROL ERROR
            cerr << "ERROR: No existe el archivo: " << nameFile << "\n";
            return false;
        }else{
            preguntaLocal = "";

            getline(iFile, preguntaLocal);

            iFile.close();

            IndexarPregunta(preguntaLocal);
            
            Buscar(numDocumentos,i);    //FALTA CONTROL ERRORES (return false)
        }
        
    }
    return true;
}

/*  Imprimirá  por  pantalla  los  resultados  de  la  última  búsqueda  
(un número  MÁXIMO  de  "numDocumentos"  (en  caso  de  que  no  se  introduzca  
el parámetro  "numDocumentos",  entonces  dicho  parámetro  se  inicializará  a 99999)
por  cada  pregunta,  entre  los  que  tienen  algún  término  de  la pregunta),  
los  cuales  estarán  almacenados  en  la  variable  privada "docsOrdenados"  en  
orden  decreciente  según  la  relevancia  sobre  la pregunta, en el siguiente 
formato (una línea por cada documento):  

NumPregunta  FormulaSimilitud  NomDocumento  Posicion  PuntuacionDoc PreguntaIndexada

*/
/* Donde:     
NumPregunta  sería  el  número  de  pregunta  almacenado  en "ResultadoRI.numPregunta" 
FormulaSimilitud  sería:  
    "DFR"  si  la  variable  privada "formSimilitud == 0"; 
    "BM25" si es 1. 
NomDocumento sería el nombre     del  documento  almacenado en la indexación 
(habrá que extraer el nombre del documento a  partir  de  "ResultadoRI.idDoc",  
pero  sin  el  directorio donde esté almacenado ni la extensión del archivo) 
Posicion  empezaría  desde  0  (indicando  el  documento  más relevante  para  la  
pregunta)  incrementándose  por  cada documento (ordenado por relevancia). 
Se imprimirá un máximo de líneas de "numDocumentos" (es decir, el máximo valor de este 
campo será "numDocumentos - 1") 
PuntuacionDoc  sería  el  valor  numérico  de  la  fórmula  de similitud empleada 
almacenado en "ResultadoRI.vSimilitud". Se mostrarán los decimales con el punto en 
lugar de con la coma. PreguntaIndexada se corresponde con IndexadorHash.pregunta si  
"ResultadoRI.numPregunta  ==  0".  En  caso  contrario  se imprimirá "ConjuntoDePreguntas"
*/
/* Por ejemplo: (COMPROBAR SALTOS DE LINEA)
1 BM25 EFE19950609-05926 0 64.7059 ConjuntoDePreguntas 
1 BM25 EFE19950614-08956 1 63.9759 ConjuntoDePreguntas 
1 BM25 EFE19950610-06424 2 62.6695 ConjuntoDePreguntas 
2 BM25 EFE19950610-00234 0 0.11656233535972 ConjuntoDePreguntas 
2 BM25 EFE19950610-06000 1 0.10667871616613 ConjuntoDePreguntas
*/
/*  Este  archivo  debería  usarse  con  la  utilidad 
"trec_eval  -q  -o frelevancia_trec_eval_TIME.txt 
fich_salida_buscador.txt > fich_salida_trec_eval.res",  
para  obtener  los  datos  de  precisión  y cobertura
*/
void 
Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const{
    ImprimirResultadoBusqueda(numDocumentos,cout);
}

/* Lo mismo que "ImprimirResultadoBusqueda()" pero guardando la salida en el fichero 
de nombre "nombreFichero"
*/
/* Devolverá false si no consigue crear correctamente el archivo
*/
bool  
Buscador::ImprimirResultadoBusqueda(const  int&  numDocumentos,  const  string& nombreFichero) const{
    ofstream oFile;
    oFile.open(nombreFichero);

    if(!oFile){
        //FALTA ERROR
        return false;
    }else{
        ImprimirResultadoBusqueda(numDocumentos,oFile);
        oFile.close();
    }

    return true;
}

void
Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, ostream& os) const{
    int pregAnt=-1;
    int i=0;
    string preguntaIndexada="";
    string formSimilitudStr="";
    if(formSimilitud==0){
        formSimilitudStr="DFR";
    }else{
        formSimilitudStr="BM25";
    }

    for(list<ResultadoRI>::const_iterator it=docsOrdenados.begin();it!=docsOrdenados.end();it++){

        if(i<numDocumentos){
            if(it->NumPregunta()==0){
                preguntaIndexada = pregunta;
            }else{
                preguntaIndexada = "ConjuntoDePreguntas";
            }

            //NumPregunta  FormulaSimilitud  NomDocumento  Posicion  PuntuacionDoc PreguntaIndexada
            os << it->NumPregunta() << " " << formSimilitudStr << " " << namesDocs[it->IdDoc()] << " "
            << i << " " << it->VSimilitud() << " " << preguntaIndexada << "\n";
        }

        if(pregAnt == it->NumPregunta()){
            i++;
        }else{
            pregAnt = it->NumPregunta();
            i=0;
        }

    }
}

// Devuelve el valor del campo privado "formSimilitud"

int 
Buscador::DevolverFormulaSimilitud() const{
    return formSimilitud;
}

/*  Cambia  el  valor  de  "formSimilitud"  a  "f"  si  contiene  un  valor 
correcto (f == 0 || f == 1);
*/
/* Devolverá false si "f" no contiene un valor correcto, en cuyo caso no cambiaría 
el valor anterior de "formSimilitud"
*/
bool 
Buscador::CambiarFormulaSimilitud(const int& f){
    if (f==0 || f==1){
        formSimilitud = f;
        return true;
    }else{
        return false;
    }
}

// Cambia el valor de "c = kc"
void 
Buscador::CambiarParametrosDFR(const double& kc){
    c = kc;
}

// Devuelve el valor de "c"
double 
Buscador::DevolverParametrosDFR() const{
    return c;
}

// Cambia el valor de "k1 = kk1; b = kb;"
void 
Buscador::CambiarParametrosBM25(const double& kk1, const double& kb){
    k1 = kk1;
    b = kb;
}

// Devuelve el valor de "k1" y "b"
void 
Buscador::DevolverParametrosBM25(double& kk1, double& kb) const{
    kk1 = k1;
    kb = b;
}

double
Buscador::calcIdf(const int n) const{
    int N = informacionColeccionDocs.getNumDocs();

    double num = N - n + 0.5;
    double den = n + 0.5;
    return log2(num/den);   //Falta comprobar base del logaritmo
}

double
Buscador::similitudPalabraDoc(double avgdl,double idf,const InformacionTermino &infTerm,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs,
                                double *DFR_values){
    if(formSimilitud==0){
        return DFR(avgdl,itDoc,namesDocs,&DFR_values[0],infTerm);
    }else{
        return BM25(avgdl,idf,infTerm,itDoc,namesDocs);
    }
}

double
Buscador::DFR(double avgdl,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs,
                double* DFR_values,const InformacionTermino &infTerm){
    int ftd = itDoc->second.getFt();
    int ld = indiceDocs[namesDocs[itDoc->first]].getNumPalSinParada();

    double ftd2 = ftd * log2(1+c*(double)avgdl/ld);

    int ft = infTerm.getFtc();
    int nt = infTerm.getL_docs().size();
    
    double wid0 = DFR_values[2] + ftd2 * DFR_values[3];
    double wid1 = (double)(ft+1)/(nt*(ftd2+1));
    double wid = wid0 * wid1;

    return wid * DFR_values[1];
}

double
Buscador::BM25(double avgdl,double idf,const InformacionTermino &infTerm,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs){
    int f = itDoc->second.getFt();
    int D = indiceDocs[namesDocs[itDoc->first]].getNumPalSinParada();
    int N = informacionColeccionDocs.getNumDocs();

    //Ecuacion
    double num = f*(k1+1);
    double den = f+k1*(1-b+b*((double)D/avgdl));

    return idf*num/den;
}
