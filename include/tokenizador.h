#include <iostream>
#include <list>
#include <vector>
using namespace std;

class Tokenizador{
    friend ostream& operator<<(ostream&, const Tokenizador&);

    public:
        // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha, en cuyo caso se eliminar�an los que hayan sido repetidos por la derecha); casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
        Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);
        
        Tokenizador (const Tokenizador&);

        Tokenizador ();
        
        ~Tokenizador ();
        
        Tokenizador& operator= (const Tokenizador&);

        // Tokeniza str devolviendo el resultado en tokens. La lista tokens se vaciar� antes de almacenar el resultado de la tokenizaci�n.
        void Tokenizar (const string& str, list<string>& tokens) const;
        
        // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada l�nea del fichero). Devolver� true si se realiza la tokenizaci�n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
        bool Tokenizar (const string& i, const string& f) const; 
        
        bool Tokenizar (const string & i) const;
            // Tokeniza el fichero i guardando la salida en un fichero de nombre ia�adi�ndole extensi�n .tk (sin eliminar previamente la extensi�n de i por ejemplo, del archivo pp.txt se generar�a el resultado en pp.txt.tk), y que contendr� una palabra en cada l�nea del fichero. Devolver� true si se realiza la tokenizaci�n de forma correcta; false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i)
        
        bool TokenizarListaFicheros (const string& i) const;
            // Tokeniza el fichero i que contiene un nombre de fichero por l�nea guardando la salida en ficheros (uno por cada l�nea de i) cuyo nombre ser� el le�do en i a�adi�ndole extensi�n .tk, y que contendr� una palabra en cada l�nea del fichero le�do en i. Devolver� true si se realiza la tokenizaci�n de forma correcta de todos los archivos que contiene i; devolver� false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el archivo i, o que se trate de un directorio, enviando a ?cerr? los archivos de i que no existan o que sean directorios; luego no se ha de interrumpir la ejecuci�n si hay alg�n archivo en i que no exista)
        
        bool TokenizarVector(const vector<string> &v) const;

        // Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios, guardando la salida en ficheros cuyo nombre ser� el de entrada a�adi�ndole extensi�n .tk, y que contendr� una palabra en cada l�nea del fichero. Devolver� true si se realiza la tokenizaci�n de forma correcta de todos los archivos; devolver� false en caso contrario enviando a cerr el mensaje correspondiente (p.ej. que no exista el directorio i, o los ficheros que no se hayan podido tokenizar)
        bool TokenizarDirectorio (const string& i) const;
        
        // Cambia ?delimiters? por ?nuevoDelimiters? comprobando que no hayan delimitadores repetidos (de izquierda a derecha), en cuyo caso se eliminar�an los que hayan sido repetidos (por la derecha)
        void DelimitadoresPalabra(const string& nuevoDelimiters);
        
        // A�ade al final de ?delimiters? los nuevos delimitadores que aparezcan en ?nuevoDelimiters? (no se almacenar�n caracteres repetidos)
        void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters); //
        
        // Devuelve ?delimiters?
        string DelimitadoresPalabra() const;
        
        // Cambia la variable privada ?casosEspeciales?
        void CasosEspeciales (const bool& nuevoCasosEspeciales);
        
        // Devuelve el contenido de la variable privada ?casosEspeciales?
        bool CasosEspeciales () const;

        // Cambia la variable privada ?pasarAminuscSinAcentos?. Atenci�n al formato de codificaci�n del corpus (comando ?file? de Linux). Para la correcci�n de la pr�ctica se utilizar� el formato actual (ISO-8859).
        void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos);
        
        // Devuelve el contenido de la variable privada ?pasarAminuscSinAcentos?
        bool PasarAminuscSinAcentos () const;

        bool toLowerCaseNoAccentsIfNeeded (string& str) const;

        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
    
    private:
        void TokenizarAux (string& str, list<string>& tokens) const;
        //Convierte str a minusculas sin acentos
        void toLowerCaseNoAccents (string& str) const;
        void toLowerCaseNoAccentsChar(char& c) const;
        bool isUrl(const string& str, const int begin) const;

        bool endOfToken(short int &tokenType,const string& localStr, int &i,bool &noReal,bool &noArroba ,int const beginToken) const;

        // Delimitadores de t�rminos. Aunque se modifique la forma de almacenamiento interna para mejorar la eficiencia, este campo debe permanecer para indicar el orden en que se introdujeron los delimitadores
        string delimiters;
        
        // Si true detectar� palabras compuestas y casos especiales. Sino, trabajar� al igual que el algoritmo propuesto en la secci�n ?Versi�n del tokenizador vista en clase?
        bool casosEspeciales;
        
        // Si true pasar� el token a min�sculas y quitar� acentos, antes de realizar la tokenizaci�n
        bool pasarAminuscSinAcentos;

        string urlNoDelimiters="_:/.?&-=#@";
        string mailNoDelimiters=".-_@";

        static const short int NO_TYPE=0;
        static const short int URL_TYPE=1;
        static const short int MULTI_TYPE=2;
        static const short int MAIL_TYPE=3;
        static const short int ACRON_TYPE=4;
        static const short int REAL_TYPE=5;
};