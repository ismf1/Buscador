#include "indexadorInformacion.h"

//-------------------INF COLECCION DOCS--------------------
ostream& operator<<(ostream& s, const InfColeccionDocs& p){
    s  <<  "numDocs:  "  <<  p.numDocs  <<  "\tnumTotalPal:  "  <<  p.numTotalPal  << "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << "\tnumTotalPalDiferentes:  "  <<  p.numTotalPalDiferentes  <<  "\ttamBytes:  "  << p.tamBytes;
    return s;
}

InfColeccionDocs::InfColeccionDocs (const InfColeccionDocs &icd){
    numTotalPal=icd.getNumTotalPal();
    numTotalPalDiferentes=icd.getNumTotalPalDiferentes();
    numTotalPalSinParada=icd.getNumTotalPalSinParada();
    numDocs=icd.getNumDocs();
    tamBytes=icd.getTamBytes();
}
InfColeccionDocs::InfColeccionDocs (){
    numDocs=0;
    numTotalPal=0;
    numTotalPalSinParada=0;
    numTotalPalDiferentes=0;
    tamBytes=0;
}

InfColeccionDocs::~InfColeccionDocs (){

}

InfColeccionDocs &
InfColeccionDocs:: operator= (const InfColeccionDocs &icd){
    numTotalPal=icd.getNumTotalPal();
    numTotalPalDiferentes=icd.getNumTotalPalDiferentes();
    numTotalPalSinParada=icd.getNumTotalPalSinParada();
    numDocs=icd.getNumDocs();
    tamBytes=icd.getTamBytes();

    return (*this);
}

long int
InfColeccionDocs::getNumDocs() const{
    return numDocs;
}

long int
InfColeccionDocs::getNumTotalPal() const{
    return numTotalPal;
}

long int
InfColeccionDocs::getNumTotalPalSinParada() const{
    return numTotalPalSinParada;
}

long int
InfColeccionDocs::getNumTotalPalDiferentes() const{
    return numTotalPalDiferentes;
}

long int
InfColeccionDocs::getTamBytes() const{
    return tamBytes;
}

void
InfColeccionDocs::incNumDocs(){
    numDocs++;
}

void
InfColeccionDocs::incNumTotalPal(){
    numTotalPal++;
}

void
InfColeccionDocs::incNumTotalPalSinParada(){
    numTotalPalSinParada++;
}

void
InfColeccionDocs::incNumTotalPalDiferentes(){
    numTotalPalDiferentes++;
}


void
InfColeccionDocs::setNumDocs(int n){
    numDocs=n;
}

void
InfColeccionDocs::setNumTotalPal(int n){
    numTotalPal=n;
}

void
InfColeccionDocs::setNumTotalPalSinParada(int n){
    numTotalPalSinParada=n;
}

void
InfColeccionDocs::setNumTotalPalDiferentes(int n){
    numTotalPalDiferentes=n;
}

void
InfColeccionDocs::setTamBytes(int n){
    tamBytes=n;
}

void
InfColeccionDocs::guardarIndexacion(ofstream& f) const{
    f << numDocs << "\n";
    f << numTotalPal << "\n";
    f << numTotalPalSinParada << "\n";
    f << numTotalPalDiferentes << "\n";
    f << tamBytes << "\n";
}

void
InfColeccionDocs::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,line);
    numDocs = stoi(line);

    getline(f,line);
    numTotalPal = stoi(line);

    getline(f,line);
    numTotalPalSinParada = stoi(line);

    getline(f,line);
    numTotalPalDiferentes = stoi(line);

    getline(f,line);
    tamBytes = stoi(line);
}

//--------------------INF DOC--------------------
ostream& operator<<(ostream& s, const InfDoc& p) {
    s   <<   "idDoc:   "   <<   p.idDoc   <<   "\tnumPal:   "   <<   p.numPal   << "\tnumPalSinParada:  "  <<  p.numPalSinParada  <<  "\tnumPalDiferentes:  "  << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

InfDoc::InfDoc (const InfDoc &icd){
    numPal=icd.getNumPal();
    numPalDiferentes=icd.getNumPalDiferentes();
    numPalSinParada=icd.getNumPalSinParada();
    idDoc=icd.getIdDoc();
    tamBytes=icd.getTamBytes();
    fechaModificacion=icd.getFechaModificacion();
}

InfDoc::InfDoc (){

}

InfDoc::InfDoc (long int id, long int fec){
    numPal=0;
    numPalDiferentes=0;
    numPalSinParada=0;
    idDoc=id;
    tamBytes=0;
    fechaModificacion=fec;
}

InfDoc::~InfDoc (){

}

InfDoc &
InfDoc::operator= (const InfDoc &icd){
    numPal=icd.getNumPal();
    numPalDiferentes=icd.getNumPalDiferentes();
    numPalSinParada=icd.getNumPalSinParada();
    idDoc=icd.getIdDoc();
    tamBytes=icd.getTamBytes();
    fechaModificacion=icd.getFechaModificacion();

    return (*this);
}

bool
InfDoc::vaciar(){
    numPal=0;
    numPalSinParada=0;
    numPalDiferentes=0;
    tamBytes=0;
}

void
InfDoc::setFecha(long int newFecha){
    fechaModificacion=newFecha;
}

void
InfDoc::setTamBytes(int file_size){
    tamBytes=file_size;
}

void
InfDoc::incNumPal(){
    numPal++;
}

void
InfDoc::incNumPalSinParada(){
    numPalSinParada++;
}

void
InfDoc::incNumPalDiferentes(){
    numPalDiferentes++;
}


long int 
InfDoc::getFechaModificacion() const{
    return fechaModificacion;
}

long int
InfDoc::getIdDoc() const{
    return idDoc;
}

int
InfDoc::getNumPal() const{
    return numPal;
}

int
InfDoc::getNumPalSinParada() const{
    return numPalSinParada;
}

int
InfDoc::getNumPalDiferentes() const{
    return numPalDiferentes;
}

int
InfDoc::getTamBytes() const{
    return tamBytes;
}

void
InfDoc::guardarIndexacion(ofstream& f) const{
    f << idDoc << "\n";
    f << numPal << "\n";
    f << numPalSinParada << "\n";
    f << numPalDiferentes << "\n";
    f << tamBytes << "\n";
    f << fechaModificacion << "\n";
}

void
InfDoc::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,line);
    idDoc = stoi(line);

    getline(f,line);
    numPal = stoi(line);

    getline(f,line);
    numPalSinParada = stoi(line);

    getline(f,line);
    numPalDiferentes = stoi(line);

    getline(f,line);
    tamBytes = stoi(line);

    getline(f,line);
    fechaModificacion = stoi(line);
}

//-------------------------INFORMACION PREGUNTA----------------------------
ostream& operator<<(ostream& s, const InformacionPregunta& p){
    s  <<  "numTotalPal:  "  <<  p.numTotalPal  <<  "\tnumTotalPalSinParada:  "<< p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
    return s;
}

InformacionPregunta::InformacionPregunta (const InformacionPregunta &ip){
    numTotalPalDiferentes=ip.getNumTotalPalDiferentes();
    numTotalPalSinParada=ip.getNumTotalPalSinParada();
    numTotalPal=ip.getNumTotalPal();
}

InformacionPregunta::InformacionPregunta (){
    numTotalPalDiferentes=0;
    numTotalPalSinParada=0;
    numTotalPal=0;
}

InformacionPregunta::~InformacionPregunta (){
    
}

InformacionPregunta &
InformacionPregunta::operator= (const InformacionPregunta &ip){
    numTotalPalDiferentes=ip.getNumTotalPalDiferentes();
    numTotalPalSinParada=ip.getNumTotalPalSinParada();
    numTotalPal=ip.getNumTotalPal();

    return (*this);
}

long int
InformacionPregunta::getNumTotalPal() const{
    return numTotalPal;
}

long int
InformacionPregunta::getNumTotalPalSinParada() const{
    return numTotalPalSinParada;
}

long int
InformacionPregunta::getNumTotalPalDiferentes() const{
    return numTotalPalDiferentes;
}

void
InformacionPregunta::setNumTotalPal(int n){
    numTotalPal=n;
}

void
InformacionPregunta::setNumTotalPalSinParada(int n){
    numTotalPalSinParada=n;
}

void
InformacionPregunta::setNumTotalPalDiferentes(int n){
    numTotalPalDiferentes=n;
}

void
InformacionPregunta::guardarIndexacion(ofstream& f) const{
    f << numTotalPal << "\n";
    f << numTotalPalSinParada << "\n";
    f << numTotalPalDiferentes << "\n";
}

void
InformacionPregunta::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,line);
    numTotalPal = stoi(line);

    getline(f,line);
    numTotalPalSinParada = stoi(line);

    getline(f,line);
    numTotalPalDiferentes = stoi(line);
}


//----------------INFORMACION TERMINO--------------------------
ostream& operator<<(ostream& s, const InformacionTermino& p) {
    s << "Frecuencia total: " << p.ftc << "\tfd: " << p.l_docs.size();

    //  A  continuación  se  mostrarían  todos  los  elementos  de  p.l_docs: s  << "\tId.Doc: " << idDoc << "\t" << InfTermDoc;
    for(unordered_map<long int,InfTermDoc>::const_iterator infTermIt = p.l_docs.begin();infTermIt!=p.l_docs.end();infTermIt++){
        s  << "\tId.Doc: " << infTermIt->first << "\t" << infTermIt->second;
    }
    return s;
}

InformacionTermino::InformacionTermino (const InformacionTermino &it){
    ftc=it.getFtc();
    l_docs=it.l_docs;
}

InformacionTermino::InformacionTermino (){
    ftc=0;
}   // Inicializa ftc = 0

InformacionTermino::~InformacionTermino (){
    ftc=0;
    l_docs.clear();
}  // Pone ftc = 0 y vacía l_docs

InformacionTermino &
InformacionTermino::operator= (const InformacionTermino &it){
    ftc=it.getFtc();
    l_docs=it.l_docs;
}

int
InformacionTermino::getFtc() const{
    return ftc;
}

int
InformacionTermino::borrarDoc(long int idDoc){
    unordered_map<long int,InfTermDoc>::iterator infTermDocIt = l_docs.find(idDoc);
    int palabrasBorradas=0;
    if(infTermDocIt!=l_docs.end()){
        //Si existe el termino en el documento a vaciar

        palabrasBorradas=infTermDocIt->second.getFt();
        //Restamos en ftc
        ftc = ftc - palabrasBorradas;

        //Eliminamos InfTermDoc
        l_docs.erase(idDoc);
    }
    return palabrasBorradas;
}

void
InformacionTermino::addPosTermDoc(InfDoc& doc,int pos,bool almacenarPosTerm){
    long int idDoc=doc.getIdDoc();
    ftc++;
    unordered_map<long int,InfTermDoc>::iterator termDocIt = l_docs.find(idDoc);
    if(termDocIt == l_docs.end()){
        //Si no estaba indexado
        doc.incNumPalDiferentes();
        InfTermDoc infTermDoc;
        infTermDoc.addPos(pos,almacenarPosTerm);
        pair<long int,InfTermDoc> pairInfTermDoc(idDoc,infTermDoc);
        l_docs.insert(pairInfTermDoc);
    }else{
        //Si ya estaba indexado
        termDocIt->second.addPos(pos,almacenarPosTerm);
    }
}

unordered_map<long int, InfTermDoc>
InformacionTermino::getL_docs() const{
    return l_docs;
}

void
InformacionTermino::guardarIndexacion(ofstream& f) const{
    f << ftc << "\n";
    
    f << "[\n";
    for(unordered_map<long int, InfTermDoc>::const_iterator it = l_docs.begin() ; it != l_docs.end() ; it++){
        f << "<\n";
        f << it->first << "\n";
        it->second.guardarIndexacion(f);
        f << ">\n";
    }
    f << "]\n";
}

void
InformacionTermino::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,line);
    ftc = stoi(line);
    
    getline(f,line);    //[
    getline(f,line);    //< o ]
    while(line!="]"){
        //id
        getline(f,line);

        //informacion
        InfTermDoc itd;
        itd.recuperarIndexacion(f);

        pair<long int,InfTermDoc> pairToInd(stoi(line),itd);
        l_docs.insert(pairToInd);

        getline(f,line);    //>
        getline(f,line);    //< o ]
    }
}

//----------------INF TERM DOC--------------------------------
ostream& operator<<(ostream& s, const InfTermDoc& p) {
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”): s << "\t" << posicion;
    for(list<int>::const_iterator it = p.posTerm.begin() ; it!=p.posTerm.end() ; it++){
        s << "\t" << *it;
    }
    return s;
}

InfTermDoc::InfTermDoc (const InfTermDoc &itd){
    ft=itd.getFt();
    posTerm=itd.getPosTerm();
}

InfTermDoc::InfTermDoc (){
    ft=0;
}    // Inicializa ft = 0

InfTermDoc::~InfTermDoc (){
    ft=0;
    posTerm.clear();
}   // Pone ft = 0

InfTermDoc &
InfTermDoc::operator= (const InfTermDoc &itd) {
    ft=itd.getFt();
    posTerm=itd.getPosTerm();

    return (*this);
}

void
InfTermDoc::addPos(int pos,bool almacenarPosTerm){
    ft++;
    if(almacenarPosTerm) posTerm.push_back(pos);
}

int
InfTermDoc::getFt() const{
    return ft;
}

list<int>
InfTermDoc::getPosTerm() const{
    return posTerm;
}

void
InfTermDoc::guardarIndexacion(ofstream& f) const{
    f << ft << "\n";

    f << "[\n";
    for(list<int>::const_iterator it = posTerm.begin() ; it != posTerm.end() ; it++){
        f << *it << "\n";
    }
    f << "]\n";
}

void
InfTermDoc::recuperarIndexacion(ifstream& f){
    string line;
    int pos;

    getline(f,line);
    ft = stoi(line);

    getline(f,line);    //[
    getline(f,line);    //] o pos
    //for(list<int>::const_iterator it = posTerm.begin() ; it != posTerm.end() ; it++){
    while(line!="]"){
        pos = stoi(line);
        posTerm.push_back(pos);
        getline(f,line);    //] o pos
    }
}

//Informacion termino pregunta
ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p) {
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm ("posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”): s << "\t" << posicion;
    for(list<int>::const_iterator it = p.posTerm.begin() ; it!=p.posTerm.end() ; it++){
        s << "\t" << *it; 
    }
    return s;
} 

InformacionTerminoPregunta::InformacionTerminoPregunta(const InformacionTerminoPregunta &itp){
    ft=itp.getFt();
    posTerm=itp.getPosTerm();
}

InformacionTerminoPregunta::InformacionTerminoPregunta (){
    ft=0;
}

InformacionTerminoPregunta::~InformacionTerminoPregunta (){
    ft=0;
    posTerm.clear();
}

InformacionTerminoPregunta  &
InformacionTerminoPregunta::operator=  (const  InformacionTerminoPregunta &itp){
    ft=itp.getFt();
    posTerm=itp.getPosTerm();

    return *this;
}

int
InformacionTerminoPregunta::getFt() const{
    return ft;
}

list<int>
InformacionTerminoPregunta::getPosTerm() const{
    return posTerm;
}

void
InformacionTerminoPregunta::addPosicion(const int pos,const bool almacenarPos){
    ft++;
    if(almacenarPos) posTerm.push_back(pos);
}

void
InformacionTerminoPregunta::guardarIndexacion(ofstream& f) const{
    f << ft << "\n";

    f << "[\n";
    for(list<int>::const_iterator it = posTerm.begin() ; it != posTerm.end() ; it++){
        f << *it << "\n";
    }
    f << "]\n";
}

void
InformacionTerminoPregunta::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,line);
    ft = stoi(line);

    getline(f,line);    //[
    getline(f,line);    //pos o ]
    
    //for(list<int>::const_iterator it = posTerm.begin() ; it != posTerm.end() ; it++){
    while(line!="]"){
        posTerm.push_back(stoi(line));

        getline(f,line);    //pos o ]
    }
}