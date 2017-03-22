#include "Zarzadca_procesami.h"

PCB INIT;
PCB z;
vector<PCB::exits> exitP;
vector<PCB::waits> waitP;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTEZY
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int A = 0, B = 0, C = 0, LICZNIK = 0;
string Rozkazy;

bool zwolnijPamiec(string nazwa) {
	bool Powodzenie = true;
	return Powodzenie;
}

bool zarezerwujPamiec(string plik, string nazwa, string rozkazy)
{
	bool Powodzenie = true;
	if (rozkazy == "")
		Rozkazy = "STARY KOD";
	else
		Rozkazy = rozkazy;

	return Powodzenie;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTEZY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int last_PID = 1;


void PCB::init() {
	INIT.nazwa = "INIT";
	INIT.stan = PCB::stan_procesu::INIT;
	INIT.PID = 1;
	INIT.PPID = 0;
	INIT.ojciec = nullptr; INIT.dziecko = nullptr; INIT.m_brat = nullptr; INIT.s_brat = nullptr; INIT.poprzednip = nullptr; INIT.nastepnyp = nullptr;

}


int PCB::fork(string nazwaP) {
	auto nowy = new PCB();
	nowy->nazwa = nazwaP;
	if (zarezerwujPamiec(this->nazwa, nazwaP, "") == true) {
		nowy->stan = PCB::stan_procesu::READY;
		int prio;
		prio = rand() % 15 + 0;
		nowy->A = this->A;
		nowy->B = this->B;
		nowy->C = this->C;
		nowy->licznik = this->licznik;
		nowy->kredyt = prio;
		nowy->priorytet = prio;
		nowy->PPID = this->PID;
		nowy->ojciec = this;
		nowy->PID = freePID();
		if (!this->dziecko) {
			this->dziecko = nowy;
		}
		else {
			PCB *wsk = this->dziecko;

			while (wsk->m_brat) {
				wsk = wsk->m_brat;
			}
			wsk->m_brat = nowy;
			nowy->s_brat = wsk;
		}
		auto *wsk = this;
		while (wsk->nastepnyp) {
			wsk = wsk->nastepnyp;
		}
		wsk->nastepnyp = nowy;
		nowy->nastepnyp = nullptr;
		nowy->poprzednip = wsk;
		return nowy->PID;
	}
	else return -1;
};


bool PCB::exec(string nazwa, string nowa_nazwa, string sciezka) {
	if (zarezerwujPamiec(this->nazwa, this->nazwa, sciezka)) {
		this->A = 0;
		this->B = 0;
		this->C = 0;
		this->licznik = 0;
		this->stan = PCB::stan_procesu::READY;
		return true;
	}
	return false;
}


int PCB::freePID() {
	if (!(last_PID >= 1 && last_PID <= 0xffff8000))last_PID = 1;
	auto wsk = &INIT;
	while (wsk->nastepnyp) {
		wsk = wsk->nastepnyp;
	}
	last_PID = wsk->PID;
	return last_PID = last_PID + 1;
}


bool PCB::waitpid() {
	int stat = -1;
	exits z;
	if (this->stan==PCB::stan_procesu::ZOMBI) { cout << "Proces o nazwie " << this->nazwa << " nie istnieje, wiec nie mozna wywolac waiptida na jego dziecku" << endl; }
	else {
		for (int i = 0; i < exitP.size(); i++) {
			z = exitP.at(i);
			if (z.kto == this->dziecko->PID) {
				stat = i;
			}
		}
		if (stat >= 0) {
			z.rezultat = 1;
			auto wsk = &INIT;
			while (wsk->nastepnyp) {
				wsk = wsk->nastepnyp;
				if (wsk->PID == this->dziecko->PID) {
					cout << wsk->PID << endl;
					PCB *wsk2 = wsk->poprzednip;
					if (wsk->nastepnyp != nullptr) {
						PCB *wsk1 = wsk->nastepnyp;
						cout << wsk2->nazwa;
						cout << wsk1->nazwa;
						wsk2->nastepnyp = wsk1;
						wsk1->poprzednip = wsk2;
					}
					else {
						wsk2->nastepnyp = nullptr;
					}
					wsk->poprzednip = nullptr;
					wsk->nastepnyp = nullptr;
					vector<PCB::exits>::iterator it;
					for (it = exitP.begin(); it != exitP.end(); it++) {
						if (it->kto == this->dziecko->PID) {
							exitP.erase(it);
							return true;
						}
					}
				}
			}

		}
		else {
			if (this->stan == stan_procesu::ACTIVE) {
				zczytajStan();
			}
			if (this->nazwa != "INIT") {
				this->stan = stan_procesu::WAITING;
			}
			waits wa;
			wa.kto = this->PID;
			wa.na_kogo = this->PID;
			waitP.push_back(wa);
			return false;

		}
	}
}

bool PCB::exit(int status) {
	bool czyUs = false;
	int stat = -1;
	waits z;
	for (int i = 0; i < waitP.size(); i++) {
		z = waitP.at(i);
		if (z.na_kogo == this->PID) {
			stat = i;
		}
	}
	if (stat >= 0) {
		bool czyTylko = true;
		for (int i = 0; i < waitP.size(); i++) {
			waitP.at(i) = z;
			if (z.kto == this->PPID&&z.na_kogo != this->PID) {
				czyTylko = false;
			}
		}
		if (czyTylko == false) {
			if (zwolnijPamiec(this->nazwa)) {
				vector <waits>::iterator it;
				for (it = waitP.begin(); it != waitP.end(); it++) {
					if (it->na_kogo == this->PID) {
						waitP.erase(it);
					}
				}
				this->stan = stan_procesu::ENDED;
				auto wsk1 = this->nastepnyp;
				auto wsk2 = this->poprzednip;
				wsk1->poprzednip = wsk2;
				wsk2->nastepnyp = wsk1;
				this->poprzednip = nullptr;
				this->nastepnyp = nullptr;
				wsk1 = wsk2 = nullptr;
				if (this->dziecko) {
					auto wsk = this->dziecko;
					wsk->PPID = 1;
					if (INIT.dziecko) {
						auto wsk2 = INIT.dziecko;
						while (wsk2->m_brat) {
							wsk2 = wsk2->m_brat;
						}
						wsk2->m_brat = wsk;
						wsk->s_brat = wsk2;
						if (this->s_brat) {
							auto wsk3 = this->s_brat;
							wsk3->m_brat = this->m_brat;
						}
						if (this->m_brat) {
							auto wsk3 = this->m_brat;
							wsk3->s_brat = this->s_brat;
						}
					}
					while (wsk->m_brat) {
						wsk = wsk->m_brat;
						wsk->PPID = 1;
					}
				}
				cout << "usniêto proces " << this->nazwa << " pomyslnie" << endl;
				delete this;
				czyUs = true;
				return czyUs;
			}
			else cout << "Blad zwalniania pamieci" << endl;
		}
		else {
			if (zwolnijPamiec(this->nazwa)) {
				vector <waits>::iterator it;
				for (it = waitP.begin(); it != waitP.end(); it++) {
					if (it->na_kogo == this->PID) {
						waitP.erase(it);
					}
				}
				this->stan = stan_procesu::ENDED;
				this->ojciec->stan = stan_procesu::READY;
				auto wsk1 = this->nastepnyp;
				auto wsk2 = this->poprzednip;
				wsk1->poprzednip = wsk2;
				wsk2->nastepnyp = wsk1;
				this->poprzednip = nullptr;
				this->nastepnyp = nullptr;
				wsk1 = wsk2 = nullptr;
				if (this->dziecko) {
					auto wsk = this->dziecko;
					wsk->PPID = 1;
					if (INIT.dziecko) {
						auto wsk2 = INIT.dziecko;
						while (wsk2->m_brat) {
							wsk2 = wsk2->m_brat;
						}
						wsk2->m_brat = wsk;
						wsk->s_brat = wsk2;
						if (this->s_brat) {
							auto wsk3 = this->s_brat;
							wsk3->m_brat = this->m_brat;
						}
						if (this->m_brat) {
							auto wsk3 = this->m_brat;
							wsk3->s_brat = this->s_brat;
						}
					}
					while (wsk->m_brat) {
						wsk = wsk->m_brat;
						wsk->PPID = 1;
					}
				}
				cout << "usniêto proces " << this->nazwa << " pomyslnie" << endl;
				delete this;
				czyUs = true;
				return czyUs;
			}
			else cout << "Blad zwalniania pamieci" << endl;
		}
	}
	else {
		if (zwolnijPamiec(this->nazwa)) {
			this->stan = stan_procesu::ZOMBI;
			if (this->dziecko) {
				auto wsk = this->dziecko;
				wsk->PPID = 1; 
				while (wsk->m_brat) {
					wsk = wsk->m_brat;
					wsk->PPID = 1;
				}
				wsk = this->dziecko;
				if (INIT.dziecko) {
					auto wsk2 = INIT.dziecko;
					while (wsk2->m_brat) {
						wsk2 = wsk2->m_brat;
					}
					wsk2->m_brat = wsk;
					wsk->s_brat = wsk2;
					if (this->s_brat) {
						auto wsk3 = this->s_brat;
						wsk3->m_brat = this->m_brat;
					}
					if(this->m_brat) {
						auto wsk3 = this->m_brat;
						wsk3->s_brat = this->s_brat;						
					}
				}
				
			}
			exits l;
			l.kto = this->PID;
			l.rezultat = status;
			exitP.push_back(l);
			cout << "Nie wykonano jeszcze na nim waitpid, wiec zosta³ dodany do vectora zamykanych" << endl;
			czyUs = true;
			return czyUs;
		}
		else cout << "Nie wykonano jeszcze na nim waitpid lecz wystapil blad usuwania pamieci" << endl;
	}
}


void PCB::zczytajStan() {
	this->A = ::A;
	this->B = ::B;
	this->C = ::C;

	this->licznik = ::LICZNIK;
}


bool PCB::kill(int pid) {
	if (pid == 1) { cout << "Wykonanie tego zamknie caly sytem. Czy jestes pewny ze chcesz to zrobic? T\N" << endl; char a; cin >> a; if (a == 'T')exit(0); else if (a == 'N')cout << "Nie usuwam procesu, dzialam dalej" << endl; else cout << "Zla komenda" << endl; }
	auto wsk = &INIT;
	bool czyJest = false;
	while (wsk->nastepnyp) {
		wsk = wsk->nastepnyp;
		if (wsk->PID == pid) { 
			if (wsk->stan == PCB::stan_procesu::ZOMBI) {
				cout << "Ten proces jest juz w stanie ZOOMBI wiec nie mozna na nim wywolac metody kill" << endl; return false;
			}
			czyJest = true;
			break; }
	}
	
	
	
	if (czyJest == true) {
		bool czyUs = false;
		int stat = -1;
		waits z;
		for (int i = 0; i < waitP.size(); i++) {
			z = waitP.at(i);
			if (z.na_kogo == wsk->PID) {
				stat = i;
			}
		}
		if (stat >= 0) {
			bool czyTylko = true;

			for (int i = 0; i < waitP.size(); i++) {
				waitP.at(i) = z;
				if (z.kto == wsk->PPID&&z.na_kogo != wsk->PID) {
					czyTylko = false;
				}
			}
			if (czyTylko == false) {
				if (zwolnijPamiec(wsk->nazwa)) {
					vector <waits>::iterator it;
					for (it = waitP.begin(); it != waitP.end(); it++) {
						if (it->na_kogo == wsk->PID) {
							waitP.erase(it);
						}
					}
					wsk->stan = PCB::stan_procesu::ENDED;
					auto wsk2 = wsk->poprzednip;
					if (wsk->nastepnyp != nullptr) {
					auto wsk1 = wsk->nastepnyp;
					wsk1->poprzednip = wsk2;
					wsk2->nastepnyp = wsk1;
					}
					else {
					wsk2->nastepnyp = nullptr;
					}
					wsk->poprzednip = nullptr;
					wsk->nastepnyp = nullptr;
				
					if (wsk->dziecko) {
						auto pom = wsk->dziecko;
						pom->PPID = 1;
				
						if (INIT.dziecko) {
				
							auto wsk2 = INIT.dziecko;
							while (wsk2->m_brat) {
								wsk2 = wsk2->m_brat;
							}
							wsk2->m_brat = pom;
							pom->s_brat = wsk2;
							
							if (wsk->s_brat) {
								auto wsk3 = wsk->s_brat;
								wsk3->m_brat = wsk->m_brat;
							}
							if (wsk->m_brat) {
								auto wsk3 = wsk->m_brat;
								wsk3->s_brat = wsk->s_brat;
							}
						}
						
						while (pom->m_brat) {
							pom = pom->m_brat;
							pom->PPID = 1;
						}
					}
					cout << "usniêto proces " << wsk->nazwa << " pomyslnie" << endl;
					delete wsk;
					czyUs = true;
					return czyUs;
				}
				else cout << "Blad zwalniania pamieci" << endl;
			}
			else {
				if (zwolnijPamiec(wsk->nazwa)) {
					
					vector <waits>::iterator it;
					for (it = waitP.begin(); it != waitP.end(); it++) {
						if (it->na_kogo == wsk->PID) {
							waitP.erase(it);
							break;
						}
					}

				
					wsk->stan = PCB::stan_procesu::ENDED;
					
					wsk->ojciec->stan = PCB::stan_procesu::READY;
			
					auto wsk2 = wsk->poprzednip;
					if (wsk->nastepnyp != nullptr) {
						auto wsk1 = wsk->nastepnyp;
						wsk1->poprzednip = wsk2;
						wsk2->nastepnyp = wsk1;
					}
					else {
						wsk2->nastepnyp = nullptr;
					}
					wsk->poprzednip = nullptr;
					wsk->nastepnyp = nullptr;
			
					if (wsk->dziecko) {
						auto pom = wsk->dziecko;
						pom->PPID = 1;
					
						if (INIT.dziecko) {
							
							auto wsk2 = INIT.dziecko;
							while (wsk2->m_brat) {
								wsk2 = wsk2->m_brat;
							}
							wsk2->m_brat = pom;
							pom->s_brat = wsk2;
						
							if (wsk->s_brat) {
								auto wsk3 = wsk->s_brat;
								wsk3->m_brat = wsk->m_brat;
							}
							if (wsk->m_brat) {
								auto wsk3 = wsk->m_brat;
								wsk3->s_brat = wsk->s_brat;
							}
						}
						
						while (pom->m_brat) {
							pom = pom->m_brat;
							pom->PPID = 1;
						}
					}
					cout << "usniêto proces " << wsk->nazwa << " pomyslnie" << endl;
					delete wsk;
					czyUs = true;
					return czyUs;
				}
				else cout << "Blad zwalniania pamieci" << endl;
			}
		}
	
		else {
			if (zwolnijPamiec(wsk->nazwa)) {
			
				wsk->stan = PCB::stan_procesu::ENDED;
				auto z = wsk->nastepnyp;
				auto v = wsk->poprzednip;
				z->poprzednip = v;
				v->nastepnyp = z;
				wsk->nastepnyp = nullptr;
				wsk->poprzednip = nullptr;
				if (wsk->dziecko) {
					auto pom = wsk->dziecko;
					pom->PPID = 1;
				
					if (INIT.dziecko) {
					
						auto wsk2 = INIT.dziecko;
						while (wsk2->m_brat) {
							wsk2 = wsk2->m_brat;
						}
						wsk2->m_brat = pom;
						pom->s_brat = wsk2;
						
						if (wsk->s_brat) {
							auto wsk3 = wsk->s_brat;
							wsk3->m_brat = wsk->m_brat;
						}
						if (wsk->m_brat) {
							auto wsk3 = wsk->m_brat;
							wsk3->s_brat = wsk->s_brat;
						}
					}
					 
					while (pom->m_brat) {
						pom = pom->m_brat;
						pom->PPID = 1;
					}
				}

				
				exits l;
				l.kto = wsk->PID;
				l.rezultat = 0;
				exitP.push_back(l);
				cout << "Nie wykonano jeszcze na nim waitpid, wiec zosta³ dodany do vectora zamykanych" << endl;
				czyUs = true;
				return czyUs;
			}
			else cout << "Nie wykonano jeszcze na nim waitpid lecz wystapil blad usuwania pamieci" << endl;
		}
	}
	if (czyJest == false)cout << "Nie ma takiego procesu" << endl;
}




PCB *znajdzproces(string nazw) {
	if (nazw == "INIT")return &INIT;
	auto wsk = &INIT;
	while (wsk->nastepnyp)
	{
		wsk = wsk->nastepnyp;
		if (wsk->nazwa == nazw) { return wsk; }
	}
	cout << "nie ma procesu o pidzie";
	return nullptr;
}

void PCB::pokaz_liste() {
	auto wsk = &INIT;
	cout << "PID	" << "PPID	" << "Stan	" << endl;
	cout << wsk->PID << "	" << wsk->PPID << "	" << wsk->stan << endl;
	while (wsk->nastepnyp) {
		wsk = wsk->nastepnyp;
		cout << wsk->PID << "	" << wsk->PPID << "	" << wsk->stan << endl;
	}
	cout << endl;
}


void PCB::pokaz_proces(string nazwa) {
	if (auto *po = znajdzproces(nazwa)) {
		auto *wsk = znajdzproces(nazwa);
		cout << "Nazwa: " << po->nazwa << endl;
		cout << "PID: " << po->PID << endl;
		cout << "PPID: " << po->PPID << endl;
	
		if (po->dziecko) {
			cout << "Dziecko: (" << po->dziecko->PID << ") ";
			wsk = po->dziecko;
			while (wsk->m_brat) {
				wsk = wsk->m_brat; cout << wsk->PID << " ";
			}
			cout << endl;
		}
		else { cout << "Dziecko: brak" << endl; }

		if (po->m_brat) { cout << "Mlodszy brat: " << po->m_brat->PID << endl; }
		else { cout << "Mlodszy brat: brak" << endl; }
		if (po->s_brat) { cout << "Straszy brat: " << po->s_brat->PID << endl; }
		else { cout << "Starszy brat: brak" << endl; }
		cout << "Stan: " << po->stan << endl;
		cout << "Rejestr A: " << po->A << endl;
		cout << "Rejestr B: " << po->B << endl;
		cout << "Rejestr C: " << po->C << endl;
		cout << "Licznik rozkazow: " << po->licznik << endl;
		cout << "Piorytet: " << po->priorytet << endl;
	}
	else cout << "Nie ma procesu o takiej nazwie" << endl;

	cout << endl;
}


void pokaz_waitp() {
	vector<PCB::waits>::iterator it;
	for (it = waitP.begin(); it != waitP.end(); it++) {
		cout << it->kto << "	" << it->na_kogo << endl;
	}
}

void pokaz_exitp() {
	vector<PCB::exits>::iterator it;
	for (it = exitP.begin(); it != exitP.end(); it++) {
		cout << it->kto << "	" << it->rezultat << endl;
	}
}


int main() {
	z.init();
	znajdzproces("INIT")->fork("2");
	znajdzproces("INIT")->fork("3");
	znajdzproces("INIT")->fork("4");
	znajdzproces("2")->exec("2","2","LOL");
	znajdzproces("2")->fork("5");
	znajdzproces("2")->exit(0);
	z.pokaz_liste();
	znajdzproces("2")->pokaz_proces("2");
	pokaz_waitp();
	cout << endl;
	z.kill(2);
	pokaz_exitp();
	znajdzproces("2")->waitpid();
	cout << "okx" << endl;
	znajdzproces("2")->pokaz_proces("2");
	z.pokaz_liste();
	pokaz_waitp();
	cout << endl;
	pokaz_exitp();
	return 0;
}