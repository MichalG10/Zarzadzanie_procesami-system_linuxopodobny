#pragma once
//przygotowa³ Micha³ Gozdek 
#include <time.h>
#include <vector>
#include  <iostream>
#include  <string>
using namespace std;


class PCB {
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////BLOK PCB
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class stan_procesu { NEW, READY, ACTIVE, WAITING, ZOMBI, ENDED, INIT };
	PCB *ojciec, *s_brat, *m_brat, *dziecko;
	string nazwa;
	int PID, PPID;
	PCB *poprzednip, *nastepnyp;
	stan_procesu stan;
	int priorytet, rt_priorytet, kredyt;
	int IO[4];
	int A, B, C, licznik;

	struct exits {
		int kto;
		int rezultat;
	};

	struct waits {
		int kto, na_kogo;
	};

	void init();
	int fork(string nazwa);
	bool exec(string nazwa, string nowa_nazwa, string sciezka);
	int PCB::freePID();
	bool waitpid();
	void zczytajStan();
	bool exit(int status);
	bool kill(int pid);
	void pokaz_proces(string nazwa);
	void pokaz_liste();

	friend ostream& operator<<(ostream& strumien, PCB::stan_procesu & stan)
	{
		switch (stan)
		{
		case PCB::stan_procesu::NEW:
			strumien << "NEW";
			break;
		case PCB::stan_procesu::READY:
			strumien << "READY";
			break;
		case PCB::stan_procesu::INIT:
			strumien << "INIT";
			break;
		case PCB::stan_procesu::ACTIVE:
			strumien << "ACTIVE";
			break;
		case PCB::stan_procesu::ZOMBI:
			strumien << "ZOMBI";
			break;
		case PCB::stan_procesu::ENDED:
			strumien << "END";
			break;
		case PCB::stan_procesu::WAITING:
			strumien << "WAITING";
			break;
		default: break;
		}
		
		return strumien;
	}
	
	PCB() : ojciec(nullptr), dziecko(nullptr), m_brat(nullptr), s_brat(nullptr), PID(0), PPID(0), stan(stan_procesu::NEW), priorytet(0), kredyt(0), poprzednip(nullptr), nastepnyp(nullptr), A(0), B(0), C(0), licznik(0)
	{
	};
};