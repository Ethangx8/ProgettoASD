/**
 * Autori: Denis Eduard Tapu 147018, Ethan Carena 147017, Ilaria Fenos 142494
 **/

#include <iostream>
#include <math.h> 
#include <stdlib.h>
#include <chrono>
#include <time.h>
#include <string>
#include <vector>
#include<fstream>

using namespace std; 
using namespace std::chrono; 


//generatore stringhe
// 2<= numeroDiLettere <= 26
string generaStringa(int lunghezza,string tipoGeneratore, int numeroDiLettere){
	
	string stringa(lunghezza, 'x');

	int intervallo;

	intervallo = (tipoGeneratore == "b" || tipoGeneratore == "c") ? (( ((int)rand()) % lunghezza ) + 1) : lunghezza;
	
	for(int i=0; i < intervallo-1; i++){
		stringa[i] = (((int)rand()) % numeroDiLettere) + 'a';
	}

	stringa[intervallo-1] = (tipoGeneratore == "c") ? '*' : (( ((int)rand()) % numeroDiLettere ) + 'a');

	for(int i=intervallo; i<lunghezza; i++){
		stringa[i] = stringa[i % intervallo];
	}

	return stringa;
}

//calcolatori del periodo
int periodoSmart(string stringa){
	int n = stringa.length();
	
	vector<int> bordoMax(n);
	bordoMax[0] = 0;

	int bordoNow;
	for (int i=0; i<n-1; i++){

		bordoNow = bordoMax[i];

		while (bordoNow > 0 && stringa[i+1] != stringa[bordoNow]){
			bordoNow = bordoMax[bordoNow-1];
		}

		if (stringa[i+1] == stringa[bordoNow]){
			bordoMax[i+1] = bordoNow+1;
		}	
		else{
			bordoMax[i+1] = 0;
		}

	} 
	return n - bordoMax[n-1];
}

int periodoNaive(string stringa) {
    int n = stringa.length();

    for (int p = 1; p <n; p++) {

        string s1 = stringa.substr(0, n - p);
        string s2 = stringa.substr(p, n);

        if (s1 == s2)
            return p;
    }
    return n;
}

/*
*distribuzione frequenze assolute periodo delle stringhe generate dai generatori
*l'indice dell'array indica il periodo, quindi l'indice 0 non va considerato perchè sarà sempre vuoto
*/
vector<int> distribuzionePeriodo(int lunghezzaStringa,int dimensioneCampione, string tipoGeneratore, int numeroDiLettere){
	vector<int> distribuzione(lunghezzaStringa+1, 0);
	string stringa;

	for (int i = 0; i < dimensioneCampione; i++)
	{
		stringa=generaStringa(lunghezzaStringa,tipoGeneratore,numeroDiLettere);
		distribuzione[periodoSmart(stringa)]+=1;
	}

	return distribuzione;
	
}

//misura durata degli algoritmi
double durata(steady_clock::time_point start, steady_clock::time_point end){
	typedef duration<double, seconds::period> secs;
	return duration_cast<secs>(end-start).count();
}


double risoluzione(){
	steady_clock::time_point start = steady_clock::now(), end;

	do{
		end = steady_clock::now();
	}while(start == end);

	return durata(start,end);
}


double misuraDurata(int(*func)(string), string stringa, double erroreMassimo){

	double tempoMinimo = risoluzione() * (1/erroreMassimo + 1), tempoDurata;

	int misurazioni = 0;

	steady_clock::time_point start = steady_clock::now();
	do{
		(*func)(stringa);
		tempoDurata = durata(start, steady_clock::now());
		misurazioni++;
	} while(tempoDurata < tempoMinimo);

	return tempoDurata/misurazioni;
}


template<typename T>
void sommaVettori(vector<T> &a, vector<T> &b){
	for (int i = 0; i < a.size(); i++)
	{
		a[i]+=b[i];
	}
}

void testDistribuzione(){
	ofstream fout;
	srand(time(0));

	int s=1, numTest=20, lunghezzaStringa=100, dimCampione=1000000, numLettere=3;
	string tipoGeneratore="b";
	vector<int> freqAssoluta, aux;

	cout<<"\nUsare i parametri standard per il test? ("<<numTest<<
	" test con un campione di "<<dimCampione<<
	" stringhe di lunghezza "<<lunghezzaStringa<<
	" generato con il metodo '"<<tipoGeneratore<<
	"' con "<<numLettere<<" lettere)";

	cout<<"\nScelta ( 0 = NO, 1 = SI): ";
	cin>>s;

	if(s==0){
		cout<<"\nNumero di test da eseguire, il cui risultato sara la media dei singoli test (>=1): ";
		cin>>numTest;
		cout<<"\nDimensione del campione del singolo test (>=1): ";
		cin>>dimCampione;
		cout<<"\nTipo di generatore di stringhe (a, b, c): ";
		cin>>tipoGeneratore;
		cout<<"\nLunghezza delle stringhe su cui viene effettuato il test (>=1): ";
		cin>>lunghezzaStringa;
		cout<<"\nNumero di lettere con cui generare le stringhe (>=2 e <=26): ";
		cin>>numLettere;
	}
	
	vector<double> media(lunghezzaStringa+1, 0),varianza(lunghezzaStringa+1, 0);
	cout<<"\nTest in esecuzione...\n";
	/*freqAssoluta = distribuzionePeriodo(lunghezzaStringa, dimCampione,tipoGeneratore, numLettere);
	for (int i = 1; i < freqAssoluta.size(); i++)
	{
		media[i]+=freqAssoluta[i];
		varianza[i]+=pow(freqAssoluta[i],2);
	}*/
	for (int j = 0; j < numTest; j++)
	{
		freqAssoluta=distribuzionePeriodo(lunghezzaStringa, dimCampione,tipoGeneratore, numLettere);
		for (int i = 1; i < freqAssoluta.size(); i++)
		{
			media[i]+=freqAssoluta[i];
			varianza[i]+=pow(freqAssoluta[i],2);
		}
	}
	
	
	//cout<<"\nRisultati:\n"; //risultati in frequenze assolute
	fout.open("results.csv");
	for (int i = 1; i < freqAssoluta.size() ; i++)
	{	
		media[i]=(double)media[i]/numTest;
		varianza[i]=((double)varianza[i]/numTest)-pow(media[i],2);
		fout<<i<<" "<<(double)media[i]<<" "<<varianza[i]<<endl;
	}

}

vector<double> confrontoSingoloTempi(vector<int> lunghezze, int dimensioneCampione, double erroreMax, int numLettere, string tipoGeneratore){
	srand(time(0));
	vector<double>  durate;
	string stringa;

	for(int i=0; i<dimensioneCampione; i++){
		stringa = generaStringa(lunghezze[i], tipoGeneratore, numLettere);

		durate.push_back(misuraDurata(periodoSmart,stringa, erroreMax));
		durate.push_back(misuraDurata(periodoNaive, stringa, erroreMax));
	}

	return durate;
}

void testTempi(){
	srand(time(0));
	ofstream fout;
	int s=1, numTest=20, lunghezzaMIN = 1000, lunghezzaMAX = 500000, dimCampione = 100, numLettere=3;
	double errMassimo = 0.001;
	string stringa, tipoGeneratore="b";
	vector<double>  durate;//posizioni indici: pari = periodoSmart, dispari = periodoNaive
	vector<int> lunghezze;
	

	cout<<"\nUsare i parametri standard per il test? ("<<numTest<<
	" test con un campione di "<<dimCampione<<
	" stringhe di lunghezza da "<<lunghezzaMIN<<
	" a "<<lunghezzaMAX<<
	" generato con il metodo '"<<tipoGeneratore<<
	"' con "<<numLettere<<
	" lettere e con un errore massimo di "<<errMassimo<<")";
	
	cout<<"\nScelta ( 0 = NO, 1 = SI): ";
	cin>>s;

	if(s==0){
		cout<<"\nNumero di test da eseguire, il cui risultato sara la media dei singoli test (>=1): ";
		cin>>numTest;
		cout<<"\nDimensione del campione del singolo test (>=1): ";
		cin>>dimCampione;
		cout<<"\nTipo di generatore di stringhe (a, b, c): ";
		cin>>tipoGeneratore;
		cout<<"\nLunghezza MINIMA delle stringhe su cui viene effettuato il test (>=1): ";
		cin>>lunghezzaMIN;
		cout<<"\nLunghezza MASSIMA delle stringhe su cui viene effettuato il test (>"<<lunghezzaMIN<<"): ";
		cin>>lunghezzaMAX;
		cout<<"\nNumero di lettere con cui generare le stringhe (>=2 e <=26): ";
		cin>>numLettere;
		cout<<"\nErrore massimo ammissibile (>0 e <=1, es. 0.001): ";
		cin>>errMassimo;
	}

	float tassoIncremento = exp((log(lunghezzaMAX) - log(lunghezzaMIN))/(dimCampione - 1));

	for (int i = 0; i < dimCampione; i++)
	{
		lunghezze.push_back(round(lunghezzaMIN*pow(tassoIncremento, i)));
	}
	
	vector<double> mediaSmart(dimCampione, 0), mediaNaive(dimCampione, 0), varianzaSmart(dimCampione, 0), varianzaNaive(dimCampione, 0); 
	cout<<"\nTest in esecuzione...\n";

	for (int j = 0; j < numTest; j++)
	{	
		durate=confrontoSingoloTempi(lunghezze, dimCampione, errMassimo, numLettere, tipoGeneratore);
		for (int i = 0; i < dimCampione ; i++)
		{
			mediaSmart[i]+=durate[i*2];
			mediaNaive[i]+=durate[(i*2)+1];
			varianzaSmart[i]+=pow(durate[i*2],2);
			varianzaNaive[i]+=pow(durate[(i*2)+1],2);
		}
	}
	
	//cout<<"\nRisultati:\n";
	fout.open("results.csv");
	for (int i = 0; i < dimCampione ; i++)
	{	
		mediaSmart[i]=(double)mediaSmart[i]/numTest;
		mediaNaive[i]=(double)mediaNaive[i]/numTest;
		varianzaSmart[i]=((double)varianzaSmart[i]/numTest)-pow(mediaSmart[i],2);
		varianzaNaive[i]=((double)varianzaNaive[i]/numTest)-pow(mediaNaive[i],2);
		fout<<lunghezze[i]<<" "<<mediaSmart[i]<<" "<<mediaNaive[i]<<" "<<varianzaSmart[i]<<" "<<varianzaNaive[i]<<endl;
	}
}

int menu(){
	int s;

	do{
		cout<<"\nChe operazione eseguire:\n";
		cout<<"1. Test confronto delle durate degli algoritmi per il calcolo del pariodo di una stringa.\n";
		cout<<"2. Test distribuzione di frequenza degli algoritmi per la generazione delle stringhe.\n";
		cout<<"0. Esci.\n";
		cout<<"Scelta:";
		cin>>s;
	}while(s<0 || s>2);
	
	return s;
}

//main
int main(){
	
	int s=0;

	do{
		s=menu();

		switch (s){
			case 1:
				testTempi();
				break;
			
			case 2:
				testDistribuzione();
				break;
			
			default:
				break;
		}
	}while(s!=0);

	return 1;
}
