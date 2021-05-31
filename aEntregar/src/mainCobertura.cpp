#include <iostream> 
#include <string>
#include <list>
#include <sys/resource.h>
#include "buscador.h"
using namespace std;

double getcputime(void) {  
    struct timeval tim;         
    struct rusage ru;         
    getrusage(RUSAGE_SELF, &ru);         
    tim=ru.ru_utime;         
    double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
    tim=ru.ru_stime;         
    t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;         
    return t;  
} 

main() { 
    //IndexadorHash b("./StopWordsIngles.txt",   ".   ,:",   false,   true, "./CorpusTime/Documentos", 0, false, false); 
    IndexadorHash b("./StopWordsIngles.txt",   ".   ,:",   false,   true, "./indicePruebaEspanyol", 2, false, false); 
    b.Indexar("ficherosTimes.txt"); 
    b.GuardarIndexacion(); 

    double aa=getcputime();
    //Buscador a("./indicePruebaEspanyol", 1); 
    Buscador a("./indicePruebaEspanyol", 0); 
    a.IndexarPregunta("KENNEDY  ADMINISTRATION  PRESSURE  ON  NGO  DINH  DIEM  TO STOP SUPPRESSING THE BUDDHISTS . "); 
    a.Buscar(423); 
    a.ImprimirResultadoBusqueda(423); 
    double bb=getcputime()-aa; 
    
    time_t inicioB, finB; 
    time(&inicioB); 
    double aaB=getcputime(); 
    a.Buscar("./CorpusTime/Preguntas/",   423,   1, 83); 
    a.ImprimirResultadoBusqueda(423,"fich_salida_buscador_alumno-DFR-conStem.txt"); 
    double bbB=getcputime()-aaB; 
    cout << "\nHa tardado primero " << bb << " segundos\n\n"; 
    cout << "\nHa tardado segundo " << bbB << " segundos\n\n"; 
} 