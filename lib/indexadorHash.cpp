#include "indexadorHash.h"
#include "indexadorInformacion.h"
#include <fstream>
#include <sys/stat.h>

const string dirIndexTerms="/terms/";
const int N_PAL_POR_FICH=500;

/*  "fichStopWords"  será  el  nombre  del  archivo  que  contendrá  todas  las palabras 
de  parada  (una  palabra  por  cada  línea  del  fichero)  y  se almacenará en el campo
privado "ficheroStopWords". Asimismo, almacenará todas las palabras de parada que contenga
el archivo en el campo privado "stopWords", el índice de palabras de parada. */

/* "tStemmer" inicializará la variable privada "tipoStemmer": 
  0  =  no  se  aplica 
stemmer:  se  indexa  el  término  tal  y  como aparece tokenizado 
  1 = stemmer de Porter
para español
  2 = stemmer de Porter para inglés */

/*  Los  índices  (p.ej.  índice,  indiceDocs  e  informacionColeccionDocs) quedarán vacíos*/
IndexadorHash::IndexadorHash(const  string&  fichStopWords,  const  string&  delimitadores,
const  bool&  detectComp,  const  bool&  minuscSinAcentos,  const  string& dirIndice,
const  int&  tStemmer,  const  bool&  almEnDisco,  const  bool& almPosTerm){
    idActualDoc=1;

    Tokenizador token(delimitadores,detectComp,minuscSinAcentos);
    tok=token;

    tipoStemmer=tStemmer;
    almacenarEnDisco=almEnDisco;
    almacenarPosTerm=almPosTerm;

    ficheroStopWords=fichStopWords;

    almacenarStopWords(fichStopWords);

    directorioIndice=dirIndice;
    if(dirIndice=="") directorioIndice="./";

    string cmd = "mkdir -p " + directorioIndice;
    system(cmd.c_str());
    cmd = "mkdir -p " + directorioIndice + dirIndexTerms;
    system(cmd.c_str());
}

IndexadorHash::IndexadorHash(){
    //FALTA
}

bool
IndexadorHash::almacenarStopWords(const string& fichStopWords){
    stemmerPorter stemmer;

    ifstream i;
    string cadena;
    i.open(fichStopWords.c_str());

    if(!i) {
        cerr << "ERROR: No existe el archivo: " << fichStopWords << endl;
        return false;
    }
    else {
        while(!i.eof()) {
            cadena="";
            getline(i, cadena);
            if(cadena.length()!=0) {
                
                //stemmer.stemmer(cadena,tipoStemmer);
                stopWords.insert(cadena);

                tok.toLowerCaseNoAccentsIfNeeded(cadena);
                stemmer.stemmer(cadena,tipoStemmer);
                stopWordsWithStemming.insert(cadena);
            }
        }
    }
    i.close();
    return true;
}

/* Constructor para inicializarIndexadorHash a partir de una indexación previamente
realizada que habrá sido almacenada en "directorioIndexacion" mediante el método "bool
GuardarIndexacion()". Con ello toda la parte privada se inicializará convenientemente, igual
que si se acabase de indexar la colección de documentos. En caso que no exista el directorio
o que no contenga los datos de la indexación se tratará la excepción correspondiente*/
IndexadorHash::IndexadorHash(const string& directorioIndexacion){
    RecuperarIndexacion(directorioIndexacion);
    string cmd = "mkdir -p " + directorioIndice;
    system(cmd.c_str());
    cmd = "mkdir -p " + directorioIndice + dirIndexTerms;
    system(cmd.c_str());
}

IndexadorHash::IndexadorHash(const IndexadorHash& ih){
    indice=ih.indice;
    indiceDocs=ih.indiceDocs;
    informacionColeccionDocs=ih.informacionColeccionDocs;
    pregunta=ih.pregunta;
    indicePregunta=ih.indicePregunta;
    infPregunta=ih.infPregunta;
    stopWords=ih.stopWords;
    ficheroStopWords=ih.ficheroStopWords;
    tok=ih.tok;
    directorioIndice=ih.directorioIndice;
    tipoStemmer=ih.tipoStemmer;
    almacenarEnDisco=ih.almacenarEnDisco;
    almacenarPosTerm=ih.almacenarPosTerm;
    idActualDoc=ih.idActualDoc;
    string cmd = "mkdir -p " + directorioIndice;
    system(cmd.c_str());
    cmd = "mkdir -p " + directorioIndice + dirIndexTerms;
    system(cmd.c_str());
}
IndexadorHash::~IndexadorHash(){
    indice.clear();
    string cmd="rm -r -f "+ directorioIndice + dirIndexTerms;
    system(cmd.c_str());
    indiceDocs.clear();
    indicePregunta.clear();
    stopWords.clear();
}
IndexadorHash&
IndexadorHash::operator= (const IndexadorHash& ih){
    indice=ih.indice;
    indiceDocs=ih.indiceDocs;
    informacionColeccionDocs=ih.informacionColeccionDocs;
    pregunta=ih.pregunta;
    indicePregunta=ih.indicePregunta;
    infPregunta=ih.infPregunta;
    stopWords=ih.stopWords;
    ficheroStopWords=ih.ficheroStopWords;
    tok=ih.tok;
    directorioIndice=ih.directorioIndice;
    tipoStemmer=ih.tipoStemmer;
    almacenarEnDisco=ih.almacenarEnDisco;
    almacenarPosTerm=ih.almacenarPosTerm;
    idActualDoc=ih.idActualDoc;
}

/*Devuelve true si consigue crear el índice para la colección de documentos detallada en
ficheroDocumentos, el cual contendrá un nombre de documento por línea. Los añadirá a los
ya existentes anteriormente en el índice.*/

/*Devuelve falso si no finaliza la indexación (p.ej. por falta de memoria), mostrando
el mensaje de error correspondiente, indicando el documento y término en el que se ha
quedado, dejando en memoria lo que se haya indexado hasta ese momento.*/

/*En el caso que aparezcan documentos repetidos, documentos que no existen o que ya
estuviesen  previamente  indexados  (ha  de  coincidir  el nombre del documento y el
directorio en que se encuentre), se devolverá true,  mostrando  el  mensaje  de  excepción
correspondiente,  y  se  re-indexarán (borrar el documento previamente indexado e indexar el
nuevo) en caso que la fecha de modificación del documento sea más reciente que la
almacenada  previamente  (class  "InfDoc"  campo  "fechaModificacion"). Los casos de
reindexación mantendrán el mismo idDoc.*/
bool
IndexadorHash::Indexar(const string& ficheroDocumentos){
    bool todoOK=true;
    string cadena="";
    string cadAux;

    //Leemos el archivo
    ifstream iFile;
    iFile.open(ficheroDocumentos.c_str());
    if(!iFile) {
        cerr << "ERROR: No existe el archivo: " << ficheroDocumentos << endl;
        return false;
    }
    else {
        while(!iFile.eof()) {
            getline(iFile, cadAux);
            cadena+=cadAux + '\n';
            
        }
    }
    iFile.close();

    //Indexamos los documentos
    int pos;
    while ((pos = cadena.find('\n')) != std::string::npos) {
        cadAux = cadena.substr(0, pos);
        if(!cadAux.empty()) {
            if(!indexarDocumento(cadAux)) todoOK=false;
        }
        cadena.erase(0, pos + 1);
    }
    

    return todoOK;
} 

/*  Devuelve  true  si  consigue  crear  el  índice  para  la  colección  de documentos
que  se  encuentra  en  el  directorio  (y  subdirectorios  que contenga)  dirAIndexar
(independientemente  de  la  extensión  de  los mismos).  Se  considerará  que  todos
los  documentos  del  directorio  serán ficheros de texto. Los añadirá a los ya existentes
anteriormente en el índice. //  Devuelve  falso  si  no  finaliza  la  indexación
(p.ej.  por  falta  de memoria o porque no exista "dirAIndexar"), mostrando el mensaje de
error correspondiente,  indicando  el  documento  y  término  en  el  que  se  ha quedado,
dejando en memoria lo que se haya indexado hasta ese momento.*/

/*  En  el  caso  que  aparezcan  documentos  repetidos  o  que  ya  estuviesen previamente
indexados  (ha  de  coincidir  el  nombre  del  documento  y  el directorio  en  que  se
encuentre),  se  mostrará  el  mensaje  de  excepción correspondiente,  y  se  re-indexarán
(borrar  el  documento  previamente indexado e indexar el nuevo) en caso que la fecha de
modificación del documento sea más reciente que la almacenada previamente (class "InfDoc"
campo  "fechaModificacion").  Los  casos  de  reindexación  mantendrán  el mismo idDoc.*/
bool
IndexadorHash::IndexarDirectorio(const string& dirAIndexar){
    struct stat dir;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode))
        return false;
    else {
        // Hago una lista en un fichero con find>fich
        string cmd="find "+dirAIndexar+" -follow |sort > .lista_fich";
        system(cmd.c_str());
        return Indexar(".lista_fich");
    }
}

long int readDate(const string& doc){
    struct stat result;
    if(stat(doc.c_str(), &result)==0)
    {
        return result.st_mtime;
    }else{
        cerr << "Error al leer el archivo " << doc << "\n";
        return -1;
    }
}

//Mia
bool
IndexadorHash::indexarDocumento(const string& doc){
    //Comprobamos el documento
    struct stat dir;
    stat(doc.c_str(), &dir);
    if(S_ISDIR(dir.st_mode)){
        //ERROR
        return true;
    }

    //Indexamos los terminos del documento
    bool todoOK=true;
    string cadAux;

    //Leemos el archivo
    ifstream iFile;
    iFile.open(doc.c_str());
    if(!iFile) {
        cerr << "ERROR: No existe el archivo: " << doc << endl;
        return false;
    }
    else {
        //Leemos fecha de modificacion
        long int fechaAct=readDate(doc);
        iFile.seekg(0, ios::end);
        int file_size = iFile.tellg();
        iFile.seekg(0,ios::beg);

        //Comprobamos si ya estaba indexado
        std::unordered_map<string,InfDoc>::iterator docIt = indiceDocs.find(doc);
        if(docIt==indiceDocs.end()){
            //Si no existe
            InfDoc infDoc(idActualDoc,fechaAct);
            pair<string,InfDoc> p(doc,infDoc);
            indiceDocs.insert(p);
            
            idActualDoc++;

            docIt= indiceDocs.find(doc);
            informacionColeccionDocs.incNumDocs();
        }else{
            //Si ya existe
            //Comprobamos fechas
            if(fechaAct<=docIt->second.getFechaModificacion()){
                //Si la nueva fecha NO es mas moderna
                return false;
            }else{
                VaciaDoc(docIt);
                docIt->second.setFecha(fechaAct);
            }
        }
        docIt->second.setTamBytes(file_size);
        informacionColeccionDocs.setTamBytes(
            informacionColeccionDocs.getTamBytes() + file_size
        );



        int pos=0;
        while(!iFile.eof()) {
            getline(iFile, cadAux);
            
            //Indexar cadAux
            indexarLine(docIt,cadAux,pos);
        }
    }
    iFile.close();
    return todoOK;
}

int
IndexadorHash::ComprobarIndexTerm(const string& str) const{
    ifstream fOut(directorioIndice + dirIndexTerms + "terminos.txt");
    string line;
    int i=0;

    if(fOut.is_open()){
        while(getline(fOut,line)){
            if(line==str){
                fOut.close();
                return i;
            }
            i++;
        }
        fOut.close();
    }
    return -1;
}

bool
IndexadorHash::indexarLine(unordered_map<string,InfDoc>::iterator docIt,string& cadAux,int& pos){
    stemmerPorter stemmer;

    //Tokenizamos cadAux
    list<string> tokens;
    tok.Tokenizar(cadAux,tokens);

    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
        informacionColeccionDocs.incNumTotalPal();
        docIt->second.incNumPal();
        //Aplicamos stemming
        stemmer.stemmer(*it,tipoStemmer);

        //Comprobamos palabras de parada
        if(stopWordsWithStemming.find(*it)==stopWordsWithStemming.end()){
            informacionColeccionDocs.incNumTotalPalSinParada();
            docIt->second.incNumPalSinParada();
            //Comprobamos si el termino esta indexado
            if(!almacenarEnDisco){
                //RAM
                unordered_map<string,InformacionTermino>::iterator termIt = indice.find(*it);
                if(termIt==indice.end()){
                    //Si el termino no esta indexado
                    informacionColeccionDocs.incNumTotalPalDiferentes();
                    InformacionTermino infTerm;
                    infTerm.addPosTermDoc(docIt->second,pos,almacenarPosTerm);
                    pair<string,InformacionTermino> pairInfTerm (*it,infTerm);

                    indice.insert(pairInfTerm);

                }else{
                    //Si el termino esta indexado
                    termIt->second.addPosTermDoc(docIt->second,pos,almacenarPosTerm);
                }
            }else{
                //DISCO
                //cout << "Indexando " << *it << "...\n"; //borrar
                int lineTerm=ComprobarIndexTerm(*it);
                if(lineTerm==-1){
                    //Si el termino no esta indexado
                    int nFich = informacionColeccionDocs.getNumTotalPalDiferentes()/N_PAL_POR_FICH;
                    ofstream fIn(directorioIndice + dirIndexTerms + "infoTerms" + to_string(nFich) + ".txt",ios::out|ios::app);

                    informacionColeccionDocs.incNumTotalPalDiferentes();
                    InformacionTermino infTerm;
                    infTerm.addPosTermDoc(docIt->second,pos,almacenarPosTerm);
                    
                    fIn << "PAL::" << *it << "\n";
                    infTerm.guardarIndexacion(fIn);
                    fIn.close();

                    ofstream fOut(directorioIndice + dirIndexTerms + "terminos.txt",ios::out|ios::app);
                    fOut << *it + "\n";
                    fOut.close();

                }else{
                    //FALLA
                    //Si el termino esta indexado
                    string line="";

                    InformacionTermino infTerm;

                    //Buscamos el documento
                    //cout << *it << "\n";//borrar
                    //cout << "Recuyperando infoTerm...\n";//borrar
                    int nFich = lineTerm / N_PAL_POR_FICH;
                    int posTerm=recuperarInfoTerm(infTerm,*it,nFich);
                    //cout << "Llega, recupera infoTerm: \n";//borrar
                    //cout << infTerm << "\n";//borrar
                    infTerm.addPosTermDoc(docIt->second,pos,almacenarPosTerm);

                    ofstream fOut(directorioIndice + dirIndexTerms + "infoTerms" + to_string(nFich) + ".txt",ios::out|ios::app);
                    fOut.seekp(posTerm);
                    fOut << " ";    //descomentar
                    fOut.seekp(0,ios::end);
                    fOut << "PAL::" << *it << "\n";
                    infTerm.guardarIndexacion(fOut);
                    fOut.close();
                    //exit(-1);   //Borrar
                }
            }
        }
        pos++;


    }
}

/*  Se  guardará  en  disco  duro  (directorio  contenido  en  la  variable privada 
"directorioIndice")  la  indexación  actualmente  en  memoria (incluidos todos  los 
parámetros  dela  parte  privada).  La  forma  de almacenamiento  la  determinará  el 
alumno.  El  objetivo  es  que  esta indexación  se  pueda  recuperar  posteriormente 
mediante  el  constructor "IndexadorHash(const   string&   directorioIndexacion)".   Por 
ejemplo, supongamos  que  se  ejecuta  esta  secuencia  de  comandos: 
"IndexadorHash a("./fichStopWords.txt",   "[   ,.",   "./dirIndexPrueba",   0,   false);
a.Indexar("./fichConDocsAIndexar.txt"); a.GuardarIndexacion();
", entonces mediante el comando: "IndexadorHash b("./dirIndexPrueba");"
se recuperará la indexación realizada en la secuencia anterior, cargándola en "b"*/

/*  Devuelve  falso  si  no  finaliza  la  operación  (p.ej.  por  falta  de memoria,
o el nombre del directorio contenido en "directorioIndice" no es  correcto),  mostrando 
el  mensaje  de  error  correspondiente,  vaciando los ficheros generados.*/

/* En caso que no existiese el directorio directorioIndice, habría que crearlo previamente*/
bool
IndexadorHash::GuardarIndexacion() const{
    string cmd = "mkdir -p " + directorioIndice;
    system(cmd.c_str());

    ofstream f(directorioIndice + "/indexador.txt", std::ofstream::trunc);

    if(f.is_open()){
        //idActualDoc
        f << idActualDoc << "\n";
        
        //indice:
        f << "[\n";
        for(unordered_map<string, InformacionTermino>::const_iterator indIt = indice.begin() ; indIt!=indice.end() ; indIt++){
            f << "<\n";
            f << indIt->first << "\n";
            indIt->second.guardarIndexacion(f);
            f << ">\n";
        }
        f << "]\n";

        //indiceDocs:
        f << "[\n";
        for(unordered_map<string, InfDoc>::const_iterator indDocIt = indiceDocs.begin() ; indDocIt!=indiceDocs.end() ; indDocIt++){
            f << "<\n";
            f << indDocIt->first << "\n";
            indDocIt->second.guardarIndexacion(f);
            f << ">\n";
        }
        f << "]\n";

        //informacionColeccionDocs
        informacionColeccionDocs.guardarIndexacion(f);

        //pregunta
        f << pregunta << "\n";

        //indicePregunta
        f << "[\n";
        for(unordered_map<string,InformacionTerminoPregunta>::const_iterator it = indicePregunta.begin(); it!= indicePregunta.end() ; it++){
            f << "<\n";
            //termino
            f << it->first << "\n";

            //informacion
            it->second.guardarIndexacion(f);

            f << ">\n";
        }
        f << "]\n";

        //infPregunta
        infPregunta.guardarIndexacion(f);

        //stopWords
        f << "[\n";
        for(unordered_set<string>::const_iterator it = stopWords.begin() ; it!=stopWords.end() ; it++){
            f << *it << "\n";
        }
        f << "]\n";

        //ficheroStopWords
        f << ficheroStopWords << "\n";

        //tok
        tok.guardarIndexacion(f);

        //directorioIndice
        f << directorioIndice << "\n";

        //tipoStemmer
        f << tipoStemmer << "\n";

        //almacenarEnDisco
        f << almacenarEnDisco << "\n";

        //almacenarPosTerm
        f << almacenarPosTerm << "\n";
    }else{
        cerr << "Error al abrir el archivo " << directorioIndice << "/indexador.txt\n";
        return false;
    }

    f.close();
    return true;
} 

/*  Vacía  la  indexación  que  tuviese  en  ese  momento  e  inicializa IndexadorHash a
partir de una indexación previamente realizada que habrá sido  almacenada  en  
"directorioIndexacion"  mediante  el  método  "bool GuardarIndexacion()".  Con  ello toda  
la  parte  privada  se  inicializará convenientemente,  igual  que  si  se  acabase  de  
indexar  la  colección  de documentos. En caso que no exista el directorio o que no contenga 
los datos  de  la  indexación  se  tratará  la  excepción  correspondiente,  y  se devolverá 
false, dejando la indexación vacía.*/
bool
IndexadorHash::RecuperarIndexacion (const string& directorioIndexacion){
    string line;
    string nameInd;
    ifstream f(directorioIndexacion + "/indexador.txt");

    if(f.is_open()){
        Vaciar();

        //idActualDoc
        getline(f,line);
        idActualDoc = stoi(line);
        
        //indice:
        getline(f,line);    //f << "[\n";
        getline(f,line); //f << "<\n"; o "]\n"
        while(line!="]"){  
            
            //Nombre termino
            getline(f,line);
            nameInd=line;

            InformacionTermino it;
            it.recuperarIndexacion(f);

            pair<string,InformacionTermino> pairToInd(nameInd,it);

            indice.insert(pairToInd);

            getline(f,line);    //f << ">\n";

            getline(f,line); //f << "<\n"; o "]\n"
        }
        //indiceDocs:
        getline(f,line);    //f << "[\n";
        getline(f,line); //f << "<\n"; o "]\n"
        //for(unordered_map<string, InfDoc>::const_iterator indDocIt = indiceDocs.begin() ; indDocIt!=indiceDocs.end() ; indDocIt++){
        while(line!="]"){   
            //Nombre termino
            getline(f,line);
            nameInd=line;


            InfDoc it;
            it.recuperarIndexacion(f);

            pair<string,InfDoc> pairToInd(nameInd,it);
            indiceDocs.insert(pairToInd);

            getline(f,line);    //f << ">\n";
            getline(f,line); //f << "<\n"; o "]\n"
        }

        //informacionColeccionDocs
        informacionColeccionDocs.recuperarIndexacion(f);

        //pregunta
        getline(f,pregunta);

        //indicePregunta
        getline(f,line);    //[
        getline(f,line);    //< o ]
        while(line!="]"){
            //termino
            getline(f,nameInd);

            //informacion
            InformacionTerminoPregunta itp;
            itp.recuperarIndexacion(f);
            
            pair<string,InformacionTerminoPregunta> pairToInsert(nameInd,itp);
            indicePregunta.insert(pairToInsert);

            getline(f,line);    //>
            getline(f,line);    //< o ]
        }

        //infPregunta
        infPregunta.recuperarIndexacion(f);

        //stopWords
        getline(f,line);    //[

        getline(f,line);    //stopword o ]
        //for(unordered_set<string>::const_iterator it = stopWords.begin() ; it!=stopWords.end() ; it++){
        while(line!="]"){
            stopWords.insert(line);
            getline(f,line);    //stopword o ]
        }

        //ficheroStopWords
        getline(f,ficheroStopWords);

        //tok
        tok.recuperarIndexacion(f);

        //directorioIndice
        getline(f,directorioIndice);

        //tipoStemmer
        getline(f,line);
        tipoStemmer = stoi(line);

        //almacenarEnDisco
        getline(f,line);
        almacenarEnDisco = stoi(line);

        //almacenarPosTerm
        getline(f,line);
        almacenarPosTerm = stoi(line);
    }else{
        //FALTA comprobar mensaje de error
        cout << "ERROR. No se pudo abrir el archivo \"" << directorioIndexacion << "/indexador.txt" << " para recuperar la indexacion.\n";
    }

    f.close();
}

void
IndexadorHash::Vaciar(){
    VaciarIndiceDocs();
    VaciarIndicePreg();
}

void 
IndexadorHash::ImprimirIndexacion() const {
    cout << "Terminos indexados: " << endl;
    for(unordered_map<string,InformacionTermino>::const_iterator it = indice.begin() ; it!= indice.end() ;it++){
        cout << it->first << "\t" << it->second << endl;
    }

    cout << "Documentos indexados: " << endl;
    for(unordered_map<string,InfDoc>::const_iterator it = indiceDocs.begin() ; it!= indiceDocs.end() ;it++){
        cout << it->first << "\t" << it->second << endl;
    }

}

/* Devuelve  true si consigue crear  el índice para  la pregunta "preg". Antes   de   
realizar   la   indexación   vaciará   los   campos   privados indicePregunta e infPregunta*/

/*  Generará  la  misma  información  que  en  la  indexación  de  documentos, pero  
dejándola  toda  accesible  en  memoria  principal  (mediante  las variables privadas 
"pregunta, indicePregunta, infPregunta") //  Devuelve  falso  si  no  finaliza  la  operación  
(p.ej.  por  falta  de memoria o bien si la pregunta no contiene ningún término con
contenido), mostrando el mensaje de error correspondiente*/
bool 
IndexadorHash::IndexarPregunta(const string& preg){
    //FALTA REVISAR OPTIMIZACION
    list<string> tokens;
    stemmerPorter stemmer;

    //Vaciamos antigua pregunta
    VaciarIndicePreg();
    infPregunta.setNumTotalPal(0);
    infPregunta.setNumTotalPalSinParada(0);
    infPregunta.setNumTotalPalDiferentes(0);

    //Obtenemos los tokens
    tok.Tokenizar (preg, tokens);

    //Analizamos los tokens
    int pos=0;
    int nPalSinParada=0;
    int nPalDif=0;
    unordered_map<string,InformacionTerminoPregunta>::iterator itItp;

    for (list<string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
        //Aplicamos stemming
        stemmer.stemmer(*it,tipoStemmer);
        
        //Comprobamos palabras de parada
        if(stopWordsWithStemming.find(*it)==stopWordsWithStemming.end()){
            itItp = indicePregunta.find(*it);

            //Si no estaba indexado
            if(itItp==indicePregunta.end()){
                //Creamos el termino
                InformacionTerminoPregunta itp;
                //Anyadimos la nueva posicion al termino
                itp.addPosicion(pos,almacenarPosTerm);

                //Insertamos el termino en el indice
                pair<std::string,InformacionTerminoPregunta> pairToInsert (*it,itp);

                indicePregunta.insert(pairToInsert);
                nPalDif++;
            //Si ya estaba indexado
            }else{
                //Anyadimos la nueva posicion al termino
                itItp->second.addPosicion(pos,almacenarPosTerm);
            }
            nPalSinParada++;
        }

        pos++;
    }

    //Comprobamos si la pregunta tiene terminos validos
    if(nPalSinParada==0){
        return false;
    }

    infPregunta.setNumTotalPal(pos);
    infPregunta.setNumTotalPalSinParada(nPalSinParada);
    infPregunta.setNumTotalPalDiferentes(nPalDif);

    pregunta=preg;

    return true;
} 

/* Devuelve true si hay una pregunta indexada (con al menos un término que no sea palabra
de parada, o sea, que haya algún término indexado en indicePregunta), devolviéndola en
"preg"*/
bool 
IndexadorHash::DevuelvePregunta(string& preg) const{
    preg=pregunta;

    if(pregunta=="" || indicePregunta.size()==0){
        return false;
    }

    return true;
}

/*  Devuelve  true  si  word  (aplicándole  el  tratamiento  de  stemming  y mayúsculas
correspondiente) está indexado en la pregunta, devolviendo su información  almacenada
"inf".  En  caso  que  no  esté,  devolvería  "inf" vacío*/
bool 
IndexadorHash::DevuelvePregunta(const  string&  word,  InformacionTerminoPregunta&  inf) const{
    //FALTA REVISAR OPTIMIZACION
    stemmerPorter stemmer;

    string wordLocal=word;
    tok.toLowerCaseNoAccentsIfNeeded(wordLocal);

    stemmer.stemmer(wordLocal,tipoStemmer);

    std::unordered_map<std::string,InformacionTerminoPregunta>::const_iterator got = indicePregunta.find (wordLocal);
    if ( got == indicePregunta.end() ){
        return false;
    }else{
        inf = got->second;
        return true;
    }
    
} 

/*  Devuelve  true  si  hay  una  pregunta  indexada,  devolviendo  su información
almacenada (campo privado "infPregunta") en "inf". En caso que no esté, devolvería "inf"
vacío*/
bool 
IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const{
    if(pregunta=="" || indicePregunta.size()==0){
        //FALTA Revisar optimizaciones
        InformacionPregunta infAux;
        inf=infAux;

        return false;
    }

    inf=infPregunta;
    return true;
}
void 
IndexadorHash::ImprimirIndexacionPregunta() {
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Terminos indexados en la pregunta: " << endl;
    /*  A  continuación  aparecerá  un  listado  del  contenido  de "indicePregunta" 
    donde  para  cada  término  indexado  se  imprimirá:*/
    //cout << termino << ‘\t’ << InformacionTerminoPregunta << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

void 
IndexadorHash::ImprimirPregunta() {
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Informacion de la pregunta: " << infPregunta << endl;
}

int
IndexadorHash::recuperarInfoTerm(InformacionTermino& inf, string& wordLocal, int nFich) const{
    ifstream fIn(directorioIndice + dirIndexTerms + "infoTerms" + to_string(nFich) + ".txt");
    string line;
    int pos=-1;

    //Buscamos el documento
    do{
        pos=fIn.tellg();
        getline(fIn,line);
        //cout << line << "\n";   //borrar    //FALTA: Insertar tambien la palabra en infoTerm.txt
    }while(line!=("PAL::" + wordLocal));

    inf.recuperarIndexacion(fIn);

    fIn.close();
    return pos;
}

/*  Devuelve  true  si  word  (aplicándole  el  tratamiento  de  stemming  y mayúsculas  
correspondiente)  está  indexado,  devolviendo  su  información almacenada "inf". En caso 
que no esté, devolvería "inf" vacío*/
bool 
IndexadorHash::Devuelve(const string& word, InformacionTermino& inf) const{
    //FALTA REVISAR OPTIMIZACION
    stemmerPorter stemmer;

    string wordLocal=word;
    tok.toLowerCaseNoAccentsIfNeeded(wordLocal);

    stemmer.stemmer(wordLocal,tipoStemmer);

    if(!almacenarEnDisco){
        //RAM
        std::unordered_map<std::string,InformacionTermino>::const_iterator got = indice.find (wordLocal);
        if ( got == indice.end() ){
            return false;
        }else{
            inf = got->second;
            return true;
        }
    }else{
        //DISCO
        int lineTerm = ComprobarIndexTerm(wordLocal);
        if ( lineTerm == -1 ){
            return false;
        }else{       
            int nFich = lineTerm / N_PAL_POR_FICH;
            recuperarInfoTerm(inf,wordLocal,nFich);

            return true;
        }
    }
}  

/*  Devuelve  true  si  word  (aplicándole  el  tratamiento  de  stemming  y mayúsculas
correspondiente) está indexado y aparece en el documento de nombre nomDoc, en cuyo caso
devuelve la información almacenada para word en el documento. En caso que no esté, devolvería
"infDoc" vacío*/
bool 
IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& infDoc) const{
    //FALTA REVISAR OPTIMIZACION
    
    //Aplicamos stemming
    stemmerPorter stemmer;

    string wordLocal=word;
    tok.toLowerCaseNoAccentsIfNeeded(wordLocal);

    stemmer.stemmer(wordLocal,tipoStemmer);

    //Buscamos id del documento
    std::unordered_map<std::string,InfDoc>::const_iterator got = indiceDocs.find (nomDoc);
    long int idDoc=-1;
    if ( got == indiceDocs.end() ){
        return false;
    }else{
        idDoc=got->second.getIdDoc();
    }

    //Buscamos informacion del termino
    if(!almacenarEnDisco){
        //RAM
        std::unordered_map<std::string,InformacionTermino>::const_iterator gotTerm = indice.find (wordLocal);
        if ( gotTerm == indice.end() ){
            return false;
        }else{
            unordered_map<long int,InfTermDoc> l_docs = gotTerm->second.getL_docs();
            //Comprobamos si el termino esta indexado en el documento
            std::unordered_map<long int,InfTermDoc>::const_iterator gotTermDoc = l_docs.find (idDoc);

            if ( gotTermDoc == l_docs.end() ){
                return false;
            }else{
                infDoc = gotTermDoc->second;
                return true;
            }
        }
    }else{
        //DISCO
        int lineTerm = ComprobarIndexTerm(wordLocal);
        if ( lineTerm == -1 ){
            return false;
        }else{
            InformacionTermino infTerm;
            int nFich = lineTerm / N_PAL_POR_FICH;
            recuperarInfoTerm(infTerm,wordLocal,nFich);

            unordered_map<long int,InfTermDoc> l_docs = infTerm.getL_docs();
            //Comprobamos si el termino esta indexado en el documento
            std::unordered_map<long int,InfTermDoc>::const_iterator gotTermDoc = l_docs.find (idDoc);

            if ( gotTermDoc == l_docs.end() ){
                return false;
            }else{
                infDoc = gotTermDoc->second;
                return true;
            }
        }
    }
}

/*  Devuelve  true  si  word  (aplicándole  el  tratamiento  de  stemming  y mayúsculas
correspondiente) aparece como término indexado*/
bool 
IndexadorHash::Existe(const string& word) const{
    //Aplicamos stemming
    stemmerPorter stemmer;

    string wordRes;
    string wordLocal=word;

    stemmer.stemmer(wordLocal,tipoStemmer,wordRes);

    //Buscamos termino
    if(!almacenarEnDisco){
        //RAM
        std::unordered_map<std::string,InformacionTermino>::const_iterator gotTerm = indice.find (wordRes);
        if ( gotTerm == indice.end() ){
            return false;
        }else{
            return true;
        }
    }else{
        //DISCO
        return ComprobarIndexTerm(wordRes);
    }
}

/* Devuelve true si se realiza el borrado (p.ej. si word, aplicándole el tratamiento  
de  stemming  y  mayúsculas  correspondiente,  aparecía  como término indexado)*/
bool 
IndexadorHash::Borra(const string& word){
    //Aplicamos stemming
    stemmerPorter stemmer;

    string wordRes;
    string wordLocal=word;

    stemmer.stemmer(wordLocal,tipoStemmer,wordRes);

    //Borramos termino
    if(!almacenarEnDisco){
        //RAM
        if ( indice.erase(wordRes) > 0 ){
            return true;
        }else{
            return false;
        }
    }else{
        //DISCO
        int lineTerm = ComprobarIndexTerm(wordRes);
        if(lineTerm!=-1){
            InformacionTermino infoTerm;
            int nFich = lineTerm / N_PAL_POR_FICH;
            int pos = recuperarInfoTerm(infoTerm, wordRes,nFich);
            ofstream fOut(directorioIndice + dirIndexTerms + "infoTerms" + to_string(nFich) + ".txt",ios::out|ios::app); //CAMBIAR
            fOut.seekp(pos);
            fOut << " ";//descomentar
            fOut.close();

            fstream f("terminos.txt",ios::app);
            string line;
            do{
                getline(f,line);
                if(line==wordRes){
                    f << " ";//descomentar
                }
            }while(line!=wordRes);
            f.close();

            return true;
        }else{
            return false;
        }

        
    }
}

/* Devuelve  true  si  nomDoc  está  indexado  y  se  realiza  el  borrado  de todos los
términos del documento y del documento en los campos privados "indiceDocs" e
"informacionColeccionDocs"*/
bool 
IndexadorHash::BorraDoc(const string& nomDoc){
    //FALTA??
    unordered_map<string,InfDoc>::iterator it = indiceDocs.find(nomDoc);

    if(it!=indiceDocs.end() && VaciaDoc(it)){
        indiceDocs.erase(it);
        informacionColeccionDocs.setNumDocs(
            informacionColeccionDocs.getNumDocs() - 1
        );

        return true;
    }else{
        return false;
    }
}

bool 
IndexadorHash::VaciaDoc(unordered_map<string,InfDoc>::iterator docIt){
    //Eliminar los InfTermDoc
    //Editar InformacionTermino
    //Editar InfColeccionDocs

    long int idDoc=docIt->second.getIdDoc();
    list<unordered_map<string,InformacionTermino>::iterator> listInfTermItToErase;
    for (unordered_map<string,InformacionTermino>::iterator infTermIt = indice.begin(); infTermIt != indice.end(); ++infTermIt){
        infTermIt->second.borrarDoc(idDoc);

        if(infTermIt->second.getFtc()==0){
            informacionColeccionDocs.setNumTotalPalDiferentes(
                informacionColeccionDocs.getNumTotalPalDiferentes() - 1
            );
            listInfTermItToErase.push_back(infTermIt);
        }
    }

    for(list<unordered_map<string,InformacionTermino>::iterator>::const_iterator it = listInfTermItToErase.begin() ; it!=listInfTermItToErase.end(); it++){
        indice.erase(*it);
    }

    informacionColeccionDocs.setNumTotalPal(
        informacionColeccionDocs.getNumTotalPal() - docIt->second.getNumPal()
    );
    informacionColeccionDocs.setNumTotalPalSinParada(
        informacionColeccionDocs.getNumTotalPalSinParada() - docIt->second.getNumPalSinParada()
    );
    informacionColeccionDocs.setTamBytes(
        informacionColeccionDocs.getTamBytes() - docIt->second.getTamBytes()
    );

    docIt->second.vaciar();

    return true;
}

/* Borra todos los términos del índice de documentos: toda la indexación de documentos.*/
void 
IndexadorHash::VaciarIndiceDocs(){
    indice.clear();
    string cmd="rm -f "+directorioIndice + dirIndexTerms + "*";
    system(cmd.c_str());
    indiceDocs.clear();
    informacionColeccionDocs.setNumDocs(0);
    informacionColeccionDocs.setNumTotalPal(0);
    informacionColeccionDocs.setNumTotalPalDiferentes(0);
    informacionColeccionDocs.setTamBytes(0);
    informacionColeccionDocs.setNumTotalPalSinParada(0);
}  

/*  Borra  todos  los  términos  del  índice  de  la  pregunta:  toda  la indexación de la
pregunta actual.*/
void 
IndexadorHash::VaciarIndicePreg(){
    indicePregunta.clear();
    infPregunta.setNumTotalPal(0);
    infPregunta.setNumTotalPalSinParada(0);
    infPregunta.setNumTotalPalDiferentes(0);
}  

/*  Será  true  si  word  (aplicándole  el  tratamiento  de  stemming  y mayúsculas
correspondiente)  está  indexado,  sustituyendo  la  información almacenada por "inf"*/
bool 
IndexadorHash::Actualiza(const string& word, const InformacionTermino& inf){
    //FALTA: COMPROBAR OPTIMIZACION
    //Aplicamos stemming
    stemmerPorter stemmer;

    string wordLocal=word;
    tok.toLowerCaseNoAccentsIfNeeded(wordLocal);
    stemmer.stemmer(wordLocal,tipoStemmer);

    //Buscamos termino
    if(!almacenarEnDisco){
        //RAM
        std::unordered_map<std::string,InformacionTermino>::const_iterator gotTerm = indice.find (wordLocal);
        if ( gotTerm == indice.end() ){
            return false;
        }else{
            indice[wordLocal]=inf;
            return true;
        }
    }else{
        //DISCO
        int lineTerm = ComprobarIndexTerm(wordLocal);
        if ( lineTerm == -1 ){
            return false;
        }else{

            InformacionTermino infTerm;
            int nFich = lineTerm / N_PAL_POR_FICH;
            //Buscamos el documento
            int posTerm=recuperarInfoTerm(infTerm,wordLocal,nFich);


            ofstream fOut("infoTerm.txt");
            fOut.seekp(posTerm);
            fOut << " ";//descomentar
            fOut.seekp(0,ios::end);
            fOut << "PAL::" << wordLocal << "\n";
            infTerm.guardarIndexacion(fOut);
            fOut.close();

            return true;
        }
    }
}

/* Será true si se realiza la inserción (p.ej. si word, aplicándole el tratamiento  de 
stemming  y  mayúsculas  correspondiente,  no  estaba previamente indexado)*/
bool 
IndexadorHash::Inserta(const string& word, const InformacionTermino& inf){
    //FALTA: COMPROBAR OPTIMIZACION
    //Aplicamos stemming
    stemmerPorter stemmer;

    string wordLocal=word;
    tok.toLowerCaseNoAccentsIfNeeded(wordLocal);
    stemmer.stemmer(wordLocal,tipoStemmer);
    
    if(!almacenarEnDisco){
        //RAM
        pair<std::string,InformacionTermino> pairToInsert (wordLocal,inf);

        //Insertamos nuevo elemento
        if(indice.insert(pairToInsert).second) return true;
        else return false;
    }else{
        //DISCO
        int lineTerm = ComprobarIndexTerm(wordLocal);
        if(lineTerm != -1){
            return false;
        }else{
            int nFich = lineTerm / N_PAL_POR_FICH;
            ofstream fOut(directorioIndice + dirIndexTerms + "infoTerms" + to_string(nFich) + ".txt",ios::out|ios::app);
            fOut.seekp(0,ios::end);
            fOut << "PAL::" << wordLocal << "\n";
            inf.guardarIndexacion(fOut);
            fOut.close();

            ofstream fOut2("terminos.txt",ios::out|ios::app);
            fOut2.seekp(0,ios::end);
            fOut2 << wordLocal << "\n";
            fOut2.close();
        }
    }
}

/* Devolverá el número de términos diferentes indexados (cardinalidad de campo privado
"índice")*/
int 
IndexadorHash::NumPalIndexadas() const{
    return informacionColeccionDocs.getNumTotalPalDiferentes();
}  

/* Devuelve el contenido del campo privado "ficheroStopWords"*/
string 
IndexadorHash::DevolverFichPalParada () const{
    return ficheroStopWords;
}  

/* Mostrará por pantalla las palabras de parada almacenadas (originales, sin aplicar
stemming): una palabra por línea (salto de línea al final de cada palabra)*/
void 
IndexadorHash::ListarPalParada() const{
    for(const string pal : stopWords){
        cout << pal << '\n';
    }
}  

/* Devolverá el número de palabras de parada almacenadas*/
int 
IndexadorHash::NumPalParada() const{
    return stopWords.size();
}  

/* Devuelve los delimitadores utilizados por el tokenizador*/
string 
IndexadorHash::DevolverDelimitadores () const{
    return tok.DelimitadoresPalabra();
}  

/* Devuelve si el tokenizador analiza los casos especiales*/
bool 
IndexadorHash::DevolverCasosEspeciales () const{
    return tok.CasosEspeciales();
} 

/* Devuelve si el tokenizador pasa a minúsculas y sin acentos*/
bool 
IndexadorHash::DevolverPasarAminuscSinAcentos () const{
    return tok.PasarAminuscSinAcentos();
} 

/* Devuelve el valor de almacenarPosTerm*/
bool 
IndexadorHash::DevolverAlmacenarPosTerm () const{
    return almacenarPosTerm;
} 

/*  Devuelve  "directorioIndice"  (el  directorio  del  disco  duro  donde  se
almacenará el índice)*/
string 
IndexadorHash::DevolverDirIndice () const{
    return directorioIndice;
}  

/* Devolverá el tipo de stemming realizado en la indexación de acuerdo con el valor
indicado en la variable privada "tipoStemmer"*/
int 
IndexadorHash::DevolverTipoStemming () const{
    return tipoStemmer;
}  

/* Devolverá el valor indicado en la variable privada "almEnDisco"*/
bool 
IndexadorHash::DevolverAlmEnDisco () const{
    return almacenarEnDisco;
}  

/* Mostrar por pantalla: cout << informacionColeccionDocs << endl;*/
void 
IndexadorHash::ListarInfColeccDocs() const{
    cout << informacionColeccionDocs << '\n';
}  

/*  Mostrar  por  pantalla  el  contenido  el  contenido  del  campo  privado
"índice": cout << termino << ‘\t’ << InformacionTermino << endl;*/
void 
IndexadorHash::ListarTerminos() const{
    //FALTA: Revisar optimizacion
    for( const auto& n : indice ) {
        cout << n.first << '\t' << n.second << '\n';
    }
} 

/* Devuelve true si nomDoc existe en la colección y muestra por pantalla todos los
términos indexados del documento con nombre "nomDoc": cout << termino  <<  ‘\t’  << 
InformacionTermino  <<  endl;  .  Si  no  existe  no  se muestra nada*/
bool 
IndexadorHash::ListarTerminos(const string& nomDoc) const{
    std::unordered_map<std::string,InfDoc>::const_iterator got = indiceDocs.find (nomDoc);
    long int idDoc=-1;
    if ( got == indiceDocs.end() ){
        return false;
    }else{
        idDoc = got->second.getIdDoc();
    }

    //FALTA: Revisar optimizacion
    for( const auto& n : indice ) {
        std::unordered_map<long int,InfTermDoc>::const_iterator got = n.second.getL_docs().find(idDoc);
        if ( got != n.second.getL_docs().end() && got->second.getFt()>0){
            cout << n.first << '\t' << n.second << '\n';
        }
    }
    return true;

}  

/*  Mostrar  por  pantalla  el  contenido  el  contenido  del  campo  privado "indiceDocs":
cout << nomDoc << ‘\t’ << InfDoc << endl;*/
void 
IndexadorHash::ListarDocs() const{
    //FALTA: Revisar optimizacion
    for( const auto& n : indiceDocs ) {
        cout << n.first << '\t' << n.second << '\n';
    }
}  

/* Devuelve true si nomDoc existe en la colección y muestra por pantalla el contenido del
campo privado "indiceDocs" para el documento con nombre "nomDoc": cout << nomDoc << ‘\t’
<< InfDoc << endl; . Si no existe no se muestra nada*/
bool 
IndexadorHash::ListarDocs(const string& nomDoc) const{
    std::unordered_map<std::string,InfDoc>::const_iterator got = indiceDocs.find (nomDoc);
    
    if ( got == indiceDocs.end() ){
        return false;
    }else{
        cout << nomDoc << '\t' << got->second << '\n';
        return true;
    }
}