#include <iostream>
#include "indexadorHash.h"
#include <queue>
using namespace std;

class ResultadoRI {
    friend ostream& operator<<(ostream&, const ResultadoRI&);
public:
    ResultadoRI(const  double&  kvSimilitud,  const  long  int&  kidDoc,  const int& np);
    double VSimilitud() const;
    long int IdDoc() const;
    bool operator< (const ResultadoRI& lhs) const; 
private:
    double vSimilitud;
    long int idDoc; 
    int numPregunta;
};

class Buscador: public IndexadorHash {
    friend ostream& operator<<(ostream& s, const Buscador& p) {
        string preg;
        s << "Buscador: " << endl;
        if(p.DevuelvePregunta(preg))
            s << "\tPregunta indexada: " << preg << endl;
        else 
            s << "\tNo hay ninguna pregunta indexada" << endl;
        s << "\tDatos del indexador: " << endl << (IndexadorHash) p; // Invoca a la sobrecarga de la salida del Indexador
        return s; 
    }
    public:
        Buscador(const string& directorioIndexacion, const int& f);

        Buscador(const Buscador&);
        ~Buscador();
        Buscador& operator= (const Buscador&);
        
        bool Buscar(const int& numDocumentos = 99999);

        bool  Buscar(const  string&  dirPreguntas,  const  int&  numDocumentos,  const int& numPregInicio, const int& numPregFin);

        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999) const;

        bool  ImprimirResultadoBusqueda(const  int&  numDocumentos,  const  string& nombreFichero) const;

        int DevolverFormulaSimilitud() const;
        // Devuelve el valor del campo privado "formSimilitud"
        
        bool CambiarFormulaSimilitud(const int& f);

        void CambiarParametrosDFR(const double& kc);
        // Cambia el valor de "c = kc"
        
        double DevolverParametrosDFR() const;
        // Devuelve el valor de "c"
        
        void CambiarParametrosBM25(const double& kk1, const double& kb);
        // Cambia el valor de "k1 = kk1; b = kb;"
        
        void DevolverParametrosBM25(double& kk1, double& kb) const;
        // Devuelve el valor de "k1" y "b"
        
    private:
        Buscador();
        int calcIdf(const int n) const;
        int calcAvgdl() const;
        int similitudPalabraDoc(int avgdl,int idf,const InformacionTermino &infTerm,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs,
                                int ft,int lambda_t,int wiq,int logwid0,int logwid1);
        int BM25(int avgdl,int idf,const InformacionTermino &infTerm,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs);
        int DFR(int avgdl,int ft,int lambda_t,int wiq,unordered_map<long int, InfTermDoc>::const_iterator &itDoc,vector<string> &namesDocs,
                int logwid0,int logwid1,const InformacionTermino &infTerm);

        priority_queue< ResultadoRI > docsOrdenados;
        int formSimilitud; // 0: DFR, 1: BM25
        double c; // Constante del modelo DFR
        double k1; // Constante del modelo BM25
        double b; // Constante del modelo BM25
};