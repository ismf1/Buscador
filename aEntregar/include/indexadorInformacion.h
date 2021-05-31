#ifndef IND_INF__H
#define IND_INF__H

#include <iostream>
#include <list>
#include <unordered_map>
#include <fstream>

using namespace std;

class InfColeccionDocs { 
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p);
    
    public:
        InfColeccionDocs (const InfColeccionDocs &);
        InfColeccionDocs ();
        ~InfColeccionDocs ();
        InfColeccionDocs & operator= (const InfColeccionDocs &);

        long int getNumDocs() const;
        long int getNumTotalPal() const;
        long int getNumTotalPalSinParada() const;
        long int getNumTotalPalDiferentes() const;
        long int getTamBytes() const;
        void incNumDocs() ;
        void incNumTotalPal() ;
        void incNumTotalPalSinParada() ;
        void incNumTotalPalDiferentes() ;
        void setNumDocs(int) ;
        void setNumTotalPal(int) ;
        void setNumTotalPalSinParada(int) ;
        void setNumTotalPalDiferentes(int) ;
        void setTamBytes(int) ;
        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
        
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        long int numDocs; // Nº total de documentos en la colección
        long int numTotalPal;   // Nº total de palabras en la colección
        long int numTotalPalSinParada; // Nº total de palabras sin stop-words en la colección 
        long int numTotalPalDiferentes; //  Nº  total  de  palabras  diferentes  en  la  colección  que  no  sean  stop-words (sin acumular la frecuencia de cada una de ellas)
        long int tamBytes; // Tamaño total en bytes de la colección
};

class InfDoc { 
    friend ostream& operator<<(ostream& s, const InfDoc& p);
    
    public:
        InfDoc (const InfDoc &);
        InfDoc ();
        InfDoc (long int id, long int fec);
        ~InfDoc ();
        InfDoc & operator= (const InfDoc &);
        bool vaciar();
        long int getIdDoc() const;
        int getNumPal() const;
        int getNumPalSinParada() const;
        int getNumPalDiferentes() const;
        void incNumPal();
        void incNumPalSinParada();
        void incNumPalDiferentes();
        void setFecha(long int newFecha);
        void setTamBytes(int file_size);
        int getTamBytes() const;
        long int getFechaModificacion() const;
        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
    
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        long int idDoc;   //  Identificador  del  documento.  El  primer  documento  indexado  en  la colección será el identificador 1
        int numPal; // Nº total de palabras del documento
        int numPalSinParada;   // Nº total de palabras sin stop-words del documento
        int numPalDiferentes;   // Nº total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
        int tamBytes;    // Tamaño en bytes del documento
        long int fechaModificacion; //  Atributo  correspondiente  a  la  fecha  y  hora  de  modificación  del documento. El tipo "Fecha/hora” lo elegirá/implementará el alumno
};

class InformacionPregunta {
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p);
    
    public:
        InformacionPregunta (const InformacionPregunta &);
        InformacionPregunta (); 
        ~InformacionPregunta ();
        InformacionPregunta & operator= (const InformacionPregunta &);
        long int getNumTotalPal() const;
        long int getNumTotalPalSinParada() const;
        long int getNumTotalPalDiferentes() const;

        void setNumTotalPal(int);   //FALTA comprobar ¿int o long int?
        void setNumTotalPalSinParada(int);
        void setNumTotalPalDiferentes(int);

        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
        
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        long int numTotalPal;   // Nº total de palabras en la pregunta
        long int numTotalPalSinParada; // Nº total de palabras sin stop-words en la pregunta
        long int numTotalPalDiferentes; // Nº total de palabras diferentes en la pregunta que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
};

class InfTermDoc {
    friend ostream& operator<<(ostream& s, const InfTermDoc& p);
    
    public:
        InfTermDoc (const InfTermDoc &);
        InfTermDoc ();    // Inicializa ft = 0
        ~InfTermDoc ();   // Pone ft = 0
        InfTermDoc & operator= (const InfTermDoc &);
        int getFt() const;
        void addPos(int pos,bool almacenarPosTerm);
        list<int> getPosTerm() const;
        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
        
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        int ft;   // Frecuencia del término en el documento
        list<int> posTerm; // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true//  Lista  de  números  de  palabra  en  los  que  aparece  el  término  en  el documento.  Los  números  de  palabra  comenzarán  desde  cero  (la  primera palabra  del  documento).  Se  numerarán  las  palabras  de  parada.  Estará ordenada de menor a mayor posición. 
    
};

class InformacionTermino {
    friend ostream& operator<<(ostream& s, const InformacionTermino& p);
    public:
        InformacionTermino (const InformacionTermino &);
        InformacionTermino ();   // Inicializa ftc = 0
        ~InformacionTermino ();  // Pone ftc = 0 y vacía l_docs
        InformacionTermino & operator= (const InformacionTermino &);
        int getFtc() const;
        void addPosTermDoc(InfDoc& doc,int pos,bool almacenarPosTerm);
        unordered_map<long int, InfTermDoc> getL_docs() const;
        int borrarDoc(long int idDoc);
        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        int ftc;  // Frecuencia total del término en la colección
        unordered_map<long int, InfTermDoc> l_docs;  // Tabla Hash que se accederá por el id del documento, devolviendo un objeto  de  la  clase  InfTermDoc  que  contiene  toda  la  información  de aparición del término en el documento
};

class InformacionTerminoPregunta {
    friend  ostream&  operator<<(ostream&  s,  const  InformacionTerminoPregunta& p);
    
    public:
        InformacionTerminoPregunta (const InformacionTerminoPregunta &);
        InformacionTerminoPregunta ();
        ~InformacionTerminoPregunta ();
        InformacionTerminoPregunta  &  operator=  (const  InformacionTerminoPregunta &);
        int getFt() const;
        list<int> getPosTerm() const;
        void addPosicion(const int pos,const bool almacenarPos);
        void guardarIndexacion(ofstream&) const;
        void recuperarIndexacion(ifstream&);
        
    //  Añadir  cuantos  métodos  se  consideren  necesarios  para  manejar  la  parte privada de la clase
    private:
        int ft;   // Frecuencia total del término en la pregunta
        list<int> posTerm; // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true //  Lista  de  números  de  palabra  en  los  que  aparece  el  término  en  la pregunta.  Los  números  de  palabra  comenzarán  desde  cero  (la  primera palabra  de  la  pregunta).  Se  numerarán  las  palabras  de  parada.  Estará ordenada de menor a mayor posición.
};

#endif