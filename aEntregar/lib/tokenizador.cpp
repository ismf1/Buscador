#include "../include/tokenizador.h"
#include <iostream>
#include <list>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib> 
/*#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>*/

// Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia, el campo delimiters se imprimir? con el string con el que se inicializ? el tokenizador
ostream& operator<<(ostream& os, const Tokenizador& obj){
    os << "DELIMITADORES: " << obj.delimiters << " TRATA CASOS ESPECIALES: " << obj.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << obj.pasarAminuscSinAcentos;
    return os;
}

// Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar?an los que hayan sido repetidos por la derecha)
Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos){
    string delimitadoresPalabraAux=delimitadoresPalabra;
    
    DelimitadoresPalabra(delimitadoresPalabraAux);
    casosEspeciales=kcasosEspeciales;
    pasarAminuscSinAcentos=minuscSinAcentos;
    

}

Tokenizador::Tokenizador (const Tokenizador& t){
    delimiters=t.delimiters;
    casosEspeciales=t.casosEspeciales;
    pasarAminuscSinAcentos=t.pasarAminuscSinAcentos;
}

Tokenizador::Tokenizador (){
    delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
    
    casosEspeciales=true;
    pasarAminuscSinAcentos=false;
}

Tokenizador::~Tokenizador (){
    delimiters="";
}

Tokenizador&
Tokenizador::operator= (const Tokenizador& t){
    delimiters = t.DelimitadoresPalabra();
    casosEspeciales = t.CasosEspeciales();
    pasarAminuscSinAcentos=t.PasarAminuscSinAcentos();

    return *this;
}

bool
Tokenizador::toLowerCaseNoAccentsIfNeeded (string& str) const{
    if(PasarAminuscSinAcentos()){
        toLowerCaseNoAccents(str);
        return true;
    }else{
        return false;
    }
}

void
Tokenizador::toLowerCaseNoAccents(string& str) const{
    size_t tam=str.length();
    for(size_t i=0;i<tam;i++){
        toLowerCaseNoAccentsChar(str[i]);
    }
}

void
Tokenizador::toLowerCaseNoAccentsChar(char& c) const{
    //char aux=c;

    if(c>='A' && c<='Z'){
        c=c-('A'-'a');
    }else if(c>='À' && c<='Ú'){
        c=c-('Á'-'á');
    }
    switch(c){
        case 'à':
        case 'á': c='a';
        break;
        case 'è':
        case 'é': c='e';
        break;
        case 'ì':
        case 'í': c='i';
        break;
        case 'ò':
        case 'ó': c='o';
        break;
        case 'ù':
        case 'ú': c='u';
        break;
    }

    
}

/*  Tokeniza 1 cadena
    Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar? antes de
    almacenar el resultado de la tokenizaci?n.
*/

/*
//VERSION ORIGINAL
void
Tokenizador:: Tokenizar (const string& str, list<string>& tokens) const{
    string localStr;
    if(pasarAminuscSinAcentos){
        localStr=toLowerCaseNoAccents(str);
    }
    //Comprobar vaciado de tokens
    //tokens.clear();
    string::size_type lastPos = localStr.find_first_not_of(delimiters,0);
    string::size_type pos = localStr.find_first_of(delimiters,lastPos);
    while(string::npos != pos || string::npos != lastPos) {
        tokens.push_back(localStr.substr(lastPos, pos - lastPos));
        lastPos = localStr.find_first_not_of(delimiters, pos);
        pos = localStr.find_first_of(delimiters, lastPos);
    }
}*/

bool
Tokenizador::isUrl(const string& str, const int begin) const{
    if(!casosEspeciales || str[begin+1]!='t') return false; //ATENCION: Solo vale si URL solo si comienzan por ftp: http: o https:
    bool aux=false;
    int tam=-1;
    string auxStr=str.substr(begin,4);
    
    if(auxStr=="ftp:"){
        aux=true;
        tam=4;
        
    }else if(auxStr=="http"){
        if(str[4]==':'){
            aux=true;
            tam=5;
        }else if(str[4]=='s' && str[5]==':'){
            aux=true;
            tam=6;
        }
    }

    if(tam+begin>=str.length()) return false;

    /*if(delimiters.find(str[tam+begin])!=string::npos &&
    urlNoDelimiters.find(str[tam+begin]==string::npos)){
        return false;
    }*/
    
    return aux;
}

bool
Tokenizador::endOfToken(short int &tokenType,const string& localStr, int &i,bool &noReal,bool &noArroba,int const beginToken) const{
    string delimitersLocal=delimiters;
    if(casosEspeciales) delimitersLocal+=" \t\n";

    //SI EL CARACTER ES DELIMITADOR
    if(delimitersLocal.find(localStr[i])!=std::string::npos){
        switch(tokenType){
            case NO_TYPE:
                //Deteccion de real
                if(casosEspeciales && (localStr[i]=='.' || localStr[i]==',') &&
                    !noReal &&
                    localStr[i+1]>='0' && localStr[i+1]<='9' &&
                    i+1<localStr.length()){
                        tokenType=REAL_TYPE;
                        return false;
                }
                //Deteccion de multi-palabra
                if(casosEspeciales && localStr[i]=='-' &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length()){
                    tokenType=MULTI_TYPE;
                    return false;
                }
                //Deteccion de acronimo
                if(casosEspeciales && localStr[i]=='.' &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length()){
                    tokenType=ACRON_TYPE;
                    return false;
                }
                //Deteccion de mail
                if(casosEspeciales && localStr[i]=='@' && !noArroba &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length()){
                    tokenType=MAIL_TYPE;
                    return false;
                }
                
                return true;
            case URL_TYPE: 
                if(urlNoDelimiters.find(localStr[i])!=std::string::npos){
                    return false;
                }
                return true;
            case MULTI_TYPE: 
                if(localStr[i]=='-' &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length()){
                    return false;
                }
                return true;
            case MAIL_TYPE:
                if(mailNoDelimiters.find(localStr[i])!=std::string::npos &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length() ){
                    
                    if(localStr[i]=='@'){
                        noArroba=true;
                        tokenType=NO_TYPE;
                        i=beginToken;
                    }
                    return false;
                }
                return true;
            case ACRON_TYPE:
                if(localStr[i]=='.' &&
                    delimitersLocal.find(localStr[i+1])==std::string::npos && i+1<localStr.length()){
                    return false;
                }
                return true;
            case REAL_TYPE:  
                if((localStr[i]=='.' || localStr[i]==',') &&
                    delimiters.find(localStr[i+1])==std::string::npos &&
                    i+1<localStr.length()){
                    return false;
                }
                
            
            return true;
            default: return true;
        }
                
        return true;
    }

    //SI EL CARACTER NO ES DELIMITADOR
    //Si el token es de tipo REAL
    if(tokenType==REAL_TYPE){
        //Comprobacion fin de token
        if(localStr[i]=='$' || localStr[i]=='%'){
            //i--;
            return true;
        }
        //Comprobacion posibilidad seguir siendo tipo REAL
        if(localStr[i]<'0' || localStr[i]>'9'){
            noReal=true;
            tokenType=NO_TYPE;
            i=beginToken;
        }
    }

    //Si el token es de tipo ENTERO
    if(!noReal && i!=beginToken){
        //Comprobacion fin de token
        if(localStr[i]=='$' || localStr[i]=='%'){
            return true;
        }
    }

    return false;
    
}

void
Tokenizador:: TokenizarAux (string& localStr, list<string>& tokens) const{
    bool noReal=false;
    bool noArroba=false;
    string delimitersLocal=delimiters;
    if(casosEspeciales) delimitersLocal+=" \t\n";
    
    if(pasarAminuscSinAcentos){
        toLowerCaseNoAccents(localStr);
    }

    tokens.clear();

    int i=0;
    int strlength=localStr.length();
    int beginToken=-1;
    int endToken=-1;
    short int tokenType=0; //0=ninguno , 1=url , 2=multi , 3=mail , 4=acronimo , 5=real

    while(i<strlength){
        noReal=false;

        //Ignoramos delimitadores (exceptuando el '.' y ',')
        while(delimitersLocal.find(localStr[i])!=std::string::npos && localStr[i]!='.' && localStr[i]!=',' && i<strlength) i++;
        
        tokenType=NO_TYPE;

        //Comprobamos tipo URL
        if(isUrl(localStr,i))tokenType=URL_TYPE;
        
        //Extraemos el indice de principio y fin de token
        beginToken=i;
        do{
            if(endOfToken(tokenType,localStr,i,noReal,noArroba,beginToken)){
                break;
            }
            if(localStr[i]<'0' || localStr[i]>'9'){
                noReal=true;
            }

            i++;
        }while(i<strlength);
        endToken=i;

        //Ignoramos los '.' y ',' del principio del token en caso de no ser un tipo REAL
        if(tokenType!=REAL_TYPE && delimitersLocal.find('.')!=std::string::npos){
            while((localStr[beginToken]=='.' || localStr[beginToken]==',') && beginToken!=endToken){
                beginToken++;
            }
        }

        //Extraemos el token a partir de los indices
        string aux=localStr.substr(beginToken, endToken - beginToken);
        if(tokenType==REAL_TYPE && (localStr[beginToken]=='.' || localStr[beginToken]==',')){
            aux='0' + aux;
        }

        //Anyadimos el token
        if(!aux.empty()) {
            tokens.push_back(aux);
        }


        if(delimitersLocal.find(localStr[i])!=string::npos)i++;
    }
}

void
Tokenizador:: Tokenizar (const string& str, list<string>& tokens) const{
    string localStr=str;
    
    TokenizarAux(localStr,tokens);
}

/*  Tokeniza 1 archivo
    - Tokenizar (const string & i) pero eligiendo el nombre del fichero de salida
*/
bool
Tokenizador::Tokenizar (const string& NomFichEntr, const string& NomFichSal) const{
    struct stat dir;
    int err=stat(NomFichEntr.c_str(), &dir);

    ifstream i;
    ofstream f;
    string cadena;
    list<string> tokens;
    i.open(NomFichEntr.c_str());

    if(!i || S_ISDIR(dir.st_mode)) {
        //COMPROBAR MENSAJE SI ES UN DIRECTORIO
        cerr << "ERROR: No existe el archivo: " << NomFichEntr << endl;
        return false;
    }
    else {
        while(!i.eof()) {
            cadena="";
            getline(i, cadena);
            if(cadena.length()!=0) {
                TokenizarAux(cadena, tokens);
            }
        }
    }
    i.close();
    f.open(NomFichSal.c_str());
    list<string>::iterator itS;
    for(itS= tokens.begin();itS!= tokens.end();itS++) {
        f << (*itS) << '\n';
    }
    f.close();
    return true;
}

/*  Tokeniza 1 archivo
    - Guarda salida en fichero con nombre del fichero + .tk  ||  ejemplo.txt -> ejemplo.txt.tk
    - 1 palabra por linea de fichero
    - True: tokenizacion correcta
    - False en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista
    el archivo i)
*/
bool
Tokenizador::Tokenizar (const string & i) const{
    return Tokenizar(i,i+".tk");
}

/*  Tokeniza varios archivos
    Pasos 2, 3 y medio 4 en funcion Tokenizar (const string & i)

    1. Tokenizar fichero linea
    2. Guardar salida en fichero.tk (Nombre descrito fichero i)
    3. 1 Palabra por linea
    4. Devolver bool:
      - True: Tokenizacion correcta de todos los archivos
      - False: Caso contrario. Enviar a cerr mensaje correspondiente
        (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a ?cerr? los
         archivos de i que no existan o que sean directorios; no se ha de interrumpir la
         ejecuci?n si hay alg?n archivo en i que no exista)
*/
bool
Tokenizador::TokenizarListaFicheros (const string& i) const{
    bool todoOK=true;
    string cadena="";
    string cadAux;
    ifstream iFile;
    iFile.open(i.c_str());
    if(!iFile) {
        cerr << "ERROR: No existe el archivo: " << i << endl;
        return false;
    }
    else {
        while(!iFile.eof()) {
            getline(iFile, cadAux);
            cadena+=cadAux + '\n';
            
        }
    }
    iFile.close();

    int pos;
    while ((pos = cadena.find('\n')) != std::string::npos) {
        cadAux = cadena.substr(0, pos);
        //cout << cadAux << endl;
        if(!cadAux.empty()) {
            if(!Tokenizar(cadAux)) todoOK=false;
        }
        cadena.erase(0, pos + 1);
    }
    

    return todoOK;
}


/* Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los
subdirectorios, guardando la salida en ficheros cuyo nombre ser? el de entrada a?adi?ndole
extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero. Devolver? true si
se realiza la tokenizaci?n de forma correcta de todos los archivos; devolver? false en
caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio
i, o los ficheros que no se hayan podido tokenizar)
*/
//VERSION 1
/*bool
Tokenizador::TokenizarDirectorio (const string& dirAIndexar) const{
    struct stat dir;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode))
        return false;
    else {
        // Hago una lista en un fichero con find>fich
        string cmd="find "+dirAIndexar+" -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}*/

//VERSION 3
bool
Tokenizador::TokenizarDirectorio (const string& dirAIndexar) const{
    struct stat dir;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode))
        return false;
    else {
        // Hago una lista en un fichero con find>fich
        string cmd="find "+dirAIndexar+" -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}

//-----------------------VERSION 2-----------------
// Necesario compilar con "-lboost_system -lboost_filesystem"
/*void read_directory(const std::string& name, vector<string>& v)
{
    boost::filesystem::path p(name);

    if(is_directory(p)) {
        std::cout << p << " is a directory containing:\n";

        for(auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
            std::cout << entry << "\n";
    }
}

bool
Tokenizador::TokenizarVector(const vector<string> &v) const{
    for(size_t i=0;i<v.size();i++){
        cout << v[i] << endl;
    }
    return false;
}

bool
Tokenizador::TokenizarDirectorio (const string& dirAIndexar) const{
    struct stat dir;
    vector<string> v;
    // Compruebo la existencia del directorio
    int err=stat(dirAIndexar.c_str(), &dir);
    if(err==-1 || !S_ISDIR(dir.st_mode))
        return false;
    else {
        // Hago una lista en un fichero con find>fic
        read_directory(dirAIndexar,v);
        return TokenizarVector(v);
    }
}*/
//------------------------------------------------------

// Cambia ?delimiters? por ?nuevoDelimiters? comprobando que no hayan delimitadores repetidos (de izquierda a derecha), en cuyo caso se eliminar?an los que hayan sido repetidos (por la derecha)
void
Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters){
    /*if(casosEspeciales)delimiters=" \n\t";
    else */delimiters="";
    AnyadirDelimitadoresPalabra(nuevoDelimiters);
}

// A?ade al final de ?delimiters? los nuevos delimitadores que aparezcan en ?nuevoDelimiters? (no se almacenar?n caracteres repetidos)
void
Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters){
    for(size_t i=0;i<nuevoDelimiters.size();i++){
        if(delimiters.find(nuevoDelimiters[i])){
            delimiters+=nuevoDelimiters[i];
        }
    }
}

string
Tokenizador::DelimitadoresPalabra() const{
    return delimiters;
}

void
Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales){
    casosEspeciales=nuevoCasosEspeciales;
}

bool
Tokenizador::CasosEspeciales () const{
    return casosEspeciales;
}

// Cambia la variable privada ?pasarAminuscSinAcentos?. Atenci?n al formato de codificaci?n del corpus (comando ?file? de Linux). Para la correcci?n de la pr?ctica se utilizar? el formato actual (ISO-8859).
void
Tokenizador::PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos){
    pasarAminuscSinAcentos=nuevoPasarAminuscSinAcentos;
}

bool
Tokenizador::PasarAminuscSinAcentos () const{
    return pasarAminuscSinAcentos;
}

void
Tokenizador::guardarIndexacion(ofstream& f) const{
    f << delimiters << "\n";
    f << casosEspeciales << "\n";
    f << pasarAminuscSinAcentos << "\n";
}

void
Tokenizador::recuperarIndexacion(ifstream& f){
    string line;

    getline(f,delimiters);

    getline(f,line);
    casosEspeciales = stoi(line);

    getline(f,line);
    pasarAminuscSinAcentos = stoi(line);
}