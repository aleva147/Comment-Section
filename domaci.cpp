// NAPOMENA 1: F-je za pronalazanje pitanja, i brisanje pitanja, su jedine f-je kojima nisam prolsedjivao pokazivac na konkretno stablo, vec listu
//   svih pitanja i tekst konkretnog pitanja. Razlog je taj sto nisam zeleo da pravim privremeni objekat u main-u sa zadatim tekstom samo kako bih
//   ispostovao formalnost da 'treba svakoj f-ji proslediti pokazivac na konkretno stablo' jer bih time gubio vreme na nesto sto nije neophodno za
//   realizaciju trazene operacije.

// NAPOMENA 2: Uvecanje odgovora sam realizovao tako da se svaki put upise nova najveca ocena do tada radi lakseg testiranja funkcionalnosti programa.
//   Ako je potrebno, moze se uvesti izmena da korisnik unosi za koliko zeli da uveca trenutnu ocenu, ili da se prosto inkrementira trenutna vrednost.

#include <iostream>
#include <string>
#include <queue>
using namespace std;

const int MAX = 10;

class Cvor {
private:
	// Zahtevana polja:
	string sadrzaj;
	Cvor** direktniPotomci = new Cvor * [MAX];	// niz pokazivaca na najvise 10 direktnih odgovora (potomaka)
	int ocenePotomaka[MAX];
	int ocena;
	// Dodatna polja:
	Cvor* pokNaOca = nullptr;  // azuriram kad dodajem cvor u stablo
	int idOca;	// za ispis celog stabla po level orderu, da naglasim konkretno kom odgovoru/pitanju dati cvor pripada
public:
	// Konstruktori
	Cvor(string sadrzaj) : sadrzaj(sadrzaj){
		ocena = 0;
		for (int i = 0; i < MAX; i++) {
			direktniPotomci[i] = nullptr;
			ocenePotomaka[i] = -1;	// spec vrednost koju koristimo na pozicijama gde nema sina
		}
	}
	Cvor(const Cvor& c) = delete;

	// Destruktor
	// - Ideja mi je da destruktor samo azurira veze, a u f-ji za brisanje cvora cu level orderom pozivati delete za njega i svaki njegov potomak 
	//		pa ce se tako dealocirati sva zauzeta memorija. Na taj nacin necu rekurzivno pozivati destruktor, vec iterativno brisati level orderom u f-ji za brisanje.
	~Cvor() {
		// - Za zadat odgovor od kog pocinjemo brisanje, treba azurirati u njegovom roditelju niz pok na sinove i niz ocena sinova.
		// - Za decu zadatog odgovora od kog pocinje brisanje nece se azurirati nista jer je roditelj vec obrisan.
		// - Ukoliko je zadato da se brisanje pocinje od samog pitanja, takodje nista nema da se azurira.
		if (pokNaOca) {
			for (int i = 0; i < MAX; i++) {
				if (pokNaOca->direktniPotomci[i] == this) {
					pokNaOca->direktniPotomci[i] = nullptr;
					pokNaOca->ocenePotomaka[i] = -1;
					break;
				}
			}
		}
	}

	// Op Overload
	Cvor& operator = (const Cvor& c) = delete;

	// Getteri i setteri
	string dohvatiSadrzaj() const { return sadrzaj; }
	Cvor* dohvatiDirektnogPot(int i) const { return direktniPotomci[i]; }
	Cvor* dohvatiPokNaOca() const { return pokNaOca; }
	int dohvatiOcenu() const { return ocena; }
	int dohvatiIdOca() const { return idOca; }

	void dodeliIdOca(int idOca) { this->idOca = idOca; }

	// Ostali
	void dodajSina(Cvor* noviSin, int poz) {
		direktniPotomci[poz] = noviSin;			// azurira niz pokazivaca na sinove u roditelju
		ocenePotomaka[poz] = noviSin->ocena;	// azurira niz ocena na potomke koji se cuva u roditelju
		noviSin->pokNaOca = this;				// azurira pokazivac na oca u novododatom sinu
	}
	void uvecajOcenu(int novaOcena) {
		ocena = novaOcena;									// azurira ocenu izabranog odgovora
		for (int i = 0; i < MAX; i++) {							// nalazenje ovog odgovora u nizu ocena njegovog roditelja i azuriranje na novu ocenu
			if (pokNaOca->dohvatiDirektnogPot(i) == this) {
				pokNaOca->ocenePotomaka[i] = ocena;
			}
		}
	}
	void urediCvor() {
		for (int j = 0; j < MAX - 1; j++) {
			for (int k = j + 1; k < MAX; k++) {
				if (ocenePotomaka[j] < ocenePotomaka[k]) {
					int tmp = ocenePotomaka[j];
					ocenePotomaka[j] = ocenePotomaka[k];
					ocenePotomaka[k] = tmp;

					Cvor* tmpCvor = direktniPotomci[j];
					direktniPotomci[j] = direktniPotomci[k];
					direktniPotomci[k] = tmpCvor;
				}
			}
		}
	}
	void pisi() const {
		cout << sadrzaj << endl;
	}
	void obrisiPokNaOca() { pokNaOca = nullptr; }
};

Cvor* levelOrderPretraga(Cvor* koren, string trazenTekstCvora) {
	queue<Cvor*> Q;
	Q.push(koren);

	Cvor* nadjenCvor = nullptr;
	while (!Q.empty()) {
		Cvor* tren = Q.front();
		Q.pop();

		// Obrada cvora (uporedjivanje teksta)
		if (tren->dohvatiSadrzaj() == trazenTekstCvora) {
			nadjenCvor = tren;
			break;
		}

		// Ostatak level order algoritma (dodavanje sinova u red)
		for (int i = 0; i < MAX; i++) {
			if (tren->dohvatiDirektnogPot(i) != nullptr)
				Q.push(tren->dohvatiDirektnogPot(i));
		}
	}
	// Praznjenje reda
	for (int i = 0; i < Q.size(); i++) {
		Q.pop();
	}

	return nadjenCvor;
}

// Zad 1. Definisanje strukture za proizvoljan broj pitanja (lista pokazivaca na cvorove)
struct Elem {
	Cvor* koren;
	Elem* sled;

	Elem(Cvor* koren, Elem* sled = nullptr) : koren(koren), sled(sled) {}
};

// Zad 2. Dodavanje pitanja
void dodajPitanje(Elem*& pitanja, Elem*& poslPitanje, Cvor* novoPitanje) {
	Elem* noviElem = new Elem(novoPitanje);
	poslPitanje = (!pitanja ? pitanja : poslPitanje->sled) = noviElem;
}

// Zad 3. Dodavanje novog odgovora za izabrano pitanje/odgovor
void dodajOdgovor(Cvor* koren) {
	cout << "Unesi konkretan tekst uokviru izabranog pitanja kome dodeljujes odgovor: ";
	string tekst;
	getline(cin, tekst);

	// Nalazenje cvora u stablu kome treba umetnuti odgovor:
	Cvor* nadjenRoditelj = levelOrderPretraga(koren, tekst);

	// Umetanje cvora:
	if (!nadjenRoditelj) {
		cout << "Ne postoji trazeni tekst uokviru izabranog pitanja." << endl;
	}
	else {
		// Ovo samo ako je nadjen gore trazen tekst
		cout << "Unesi tekst odgovora koji dodajes: ";
		string odgovor;
		getline(cin, odgovor);

		bool uspesnoDodat = false;
		for (int i = 0; i < MAX; i++) {
			if (nadjenRoditelj->dohvatiDirektnogPot(i) == nullptr) {
				uspesnoDodat = true;
				Cvor* noviCvor = new Cvor(odgovor);
				nadjenRoditelj->dodajSina(noviCvor, i);	// azurira i niz pok na sinove i niz ocena u roditelju, i azurira pokNaOca u novododatom sinu
				break;
			}
		}
		if (!uspesnoDodat) {
			cout << "Nije moguce dodati jos odgovora, ima ih maksimalan broj." << endl;
		}
	}
}

// Zad 4. Nalazenje zadatog pitanja
Cvor* pronadjiPitanje(Elem* pitanja, string tekstTrazenogPitanja) {
	for (Elem* tren = pitanja; tren; tren = tren->sled) {
		if (tren->koren->dohvatiSadrzaj() == tekstTrazenogPitanja) {
			return tren->koren;
		}
	}

	cout << "Nema trazenog pitanja." << endl;
	return nullptr;
}

// Zad 5. Pretraga odgovora
Cvor* pronadjiOdgovor(Cvor* koren) {
	// Nalazenje cvora u stablu u kome treba traziti odgovor:
	cout << "Unesi konkretan tekst uokviru izabranog pitanja u kome trazis odgovor: ";	// Ovaj dodatni korak jer se trazilo u zadatku da se naglasi odakle treba poceti trazenje odgovora u pitanju. Moglo bi inace odmah iz korena da se trazi odgovor.
	string tekst;
	getline(cin, tekst);

	Cvor* nadjenaSekcija = levelOrderPretraga(koren, tekst);
	if (!nadjenaSekcija) {
		cout << "Ne postoji trazena sekcija uokviru ovog pitanja." << endl;
		return nullptr;
	}

	// Nalazenje samog odgovora
	cout << "Unesi tekst samog odgovora: ";
	getline(cin, tekst);

	Cvor* nadjenOdgovor = levelOrderPretraga(nadjenaSekcija, tekst);
	if (!nadjenOdgovor)
		cout << "Nije pronadjen trazeni odgovor u izabranoj sekciji ovog pitanja." << endl;

	return nadjenOdgovor;
}

// Zad 5.1. Pronalazak odgovora sa najvecom ocenom u zadatom pitanju
Cvor* pronadjiOdgovorSaMaxOcenom(Cvor* koren) {
	// Level order, obrada je trazenje cvora sa max ocenom
	queue<Cvor*> Q;
	Q.push(koren);

	Cvor* nadjenCvor = nullptr;
	int max = -1;
	while (!Q.empty()) {
		Cvor* tren = Q.front();
		Q.pop();

		// Obrada cvora (uporedjivanje teksta)
		if (tren->dohvatiOcenu() > max && tren != koren) {
			nadjenCvor = tren;
			max = tren->dohvatiOcenu();
		}

		// Ostatak level order algoritma (dodavanje sinova u red)
		for (int i = 0; i < MAX; i++) {
			if (tren->dohvatiDirektnogPot(i) != nullptr)
				Q.push(tren->dohvatiDirektnogPot(i));
		}
	}
	// Praznjenje reda
	for (int i = 0; i < Q.size(); i++) {
		Q.pop();
	}

	if (!nadjenCvor)
		cout << "Zadato pitanje nema nijedan odgovor." << endl;

	return nadjenCvor;
}

// Zad 6. Povecanje ocene nekog odgovora
/* Realizovao kao metod klase */

// Zad 7. Uredjivanje odgovora prema njihvoj oceni za sva pitanja
void uredjivanjeSvihOdgovora(Cvor* koren) {
	// Level order, obrada trenutnog cvora je azuriranje uredjivanje niza pok na potomke u njegovom roditelju, kao i niza ocena u njegovom roditelju
	queue<Cvor*> Q;
	Q.push(koren);

	Cvor* nadjenCvor = nullptr;
	while (!Q.empty()) {
		Cvor* tren = Q.front();
		Q.pop();

		// Obrada roditelja 
		if (tren != koren)
			tren->dohvatiPokNaOca()->urediCvor();

		// Ostatak level order algoritma (dodavanje sinova u red)
		for (int i = 0; i < MAX; i++) {
			if (tren->dohvatiDirektnogPot(i) != nullptr)
				Q.push(tren->dohvatiDirektnogPot(i));
		}
	}
	// Praznjenje reda
	for (int i = 0; i < Q.size(); i++) {
		Q.pop();
	}
}

// Zad 8. Ispis svih pitanja
/* Koristi metod klase za svako stablo */

// Zad 9. Ispis izabranog pitanja u celosti
void ispisCelogPitanja(Cvor* koren) {
	// Level order obilazak, obrada je ispis sadrzaja trenutnog cvora (pri cemu se naglasava cemu odgovor pripada)
	Cvor* tren = koren;
	Cvor* granicnik = nullptr;
	tren->dodeliIdOca(-1);
	
	queue<Cvor*> Q;
	Q.push(tren);
	Q.push(granicnik);	// zbog ispisa, da pratimo kad smo presli na sledeci nivo.
		
	int idNaNivou = 0;	// kad predjemo na sledeci nivo, resetujemo na nulu. Tako cemo na svakom nivou indeksirati cvorove ispocetka 
						//	 i upisati u njihove potomke na kog se oca odnose.
	
	while (Q.front() != granicnik) {
		tren = Q.front();
		Q.pop();
	
		// Ispis trenutnog cvora	
		if (tren->dohvatiIdOca() == -1) {
			cout << "#0 (Polazno pitanje): " << tren->dohvatiSadrzaj() << endl;
		}
		else {
			cout << "#" << idNaNivou << "(Odgovor na stavku #" << tren->dohvatiIdOca() 
				<< " iz prethodnog nivoa): " << tren->dohvatiSadrzaj() << endl;
		}
	
		// Stavljanje potomaka u red uz zabelezivanje kom ocu pripadaju
		for (int i = 0; i < MAX; i++) {
			Cvor* potomak = tren->dohvatiDirektnogPot(i);
			if (potomak != nullptr) {
				potomak->dodeliIdOca(idNaNivou);
				Q.push(potomak);
			}
		}
	
		idNaNivou++;
			
		// Slucaj prelaska na sledeci nivo
		if (Q.front() == granicnik) {
			cout << "-----------------------------------" << endl;
			idNaNivou = 0;
			Q.pop();
			Q.push(granicnik);
		}
	}
}

// Zad 10. Brisanje odgovora i njegovih potomaka
void obrisiOdgovor(Cvor* odgovor) {
	// Level order, obrada: u roditelju odgovora azurirati niz pok na sinove, kao i niz ocena, a za sam odgovor dealocirati memoriju i obrisati sam cvor
	queue<Cvor*> Q;
	Q.push(odgovor);

	while (!Q.empty()) {
		Cvor* tren = Q.front();
		Q.pop();

		// Dodavanje sinova cvora u red
		for (int i = 0; i < MAX; i++) {
			Cvor* sin = tren->dohvatiDirektnogPot(i);
			if (sin != nullptr) {
				Q.push(sin);
				sin->obrisiPokNaOca();	// oca cemo obrisati, pa nam ovo sluzi da ne bismo probali da pristupimo obrisanom ocu za azuriranje niza pok na sinova i niza ocena
			}
		}

		// Obrada (Brisanje trenutnog cvora)
		delete tren;	// poziva destruktor, a onda oslobadja mem koju je zauzeo sam ovaj cvor na heap-u (ne racunajuci mem koju zauzimaju njegovi potomci)
	}
	// Praznjenje reda
	for (int i = 0; i < Q.size(); i++) {
		Q.pop();
	}
}

// Zad 11. Birsanje pitanja i svih njegovih potomaka (preuredjuje listu pitanja i unistava izabrano pitanje)
void obrisiPitanje(Elem*& pitanja, Elem*& poslPitanje, string tekstPitanja) {
	// Pronalazenje pitanja za izbacivanja, i pamcenje pitanja koje mu prethodi u listi zbog uvezivanja
	Elem* q = nullptr;
	Elem* p = pitanja;
	for (; p; p = p->sled) {
		if (p->koren->dohvatiSadrzaj() == tekstPitanja) {
			break;
		}
		q = p;
	}

	if (!p) {
		cout << "Nije pronadjeno pitanje." << endl;
		return;
	}

	// Azuriranje liste pitanja
	if (!q) {					// 1. ako je izbacen prvi element liste, treba azurirati pok na pocetak liste
		if (p == poslPitanje) {	// 1.1 ako je u listi bio samo jedan element, treba azurirati i pok na poslednji element liste
			pitanja = nullptr;
			poslPitanje = nullptr;
		}
		pitanja = p->sled;
	}
	else if (p == poslPitanje) { // 2. ako je izbacen posl element liste, treba azurirati pok na poslednji element liste
		poslPitanje = q;
		poslPitanje->sled = nullptr;
	}
	else {						// 3. ako je neki element iz sredine, samo preuvezujemo da njegov prethodnik pokazuje na njegovog sledbenika
		q->sled = p->sled;
	}

	// Brisanje odgovarajuceg cvora i elementa liste kome pripada.
	obrisiOdgovor(p->koren);
	delete p;
}

int main() {
	bool nastavi = true;
	int unos;
	int nOcena;
	string tekst;
	Cvor* pokCvor = nullptr;

	// Zad 1. Definisanje strukture za kreiranje proizvoljnog broja pitanja (inicijalno prazne)
	Elem* pitanja = nullptr;
	Elem* poslednjePitanje = pitanja;	// da ne bi iterirao kroz celu listu pitanja, koja moze biti besk dugacka, pri dodavanju novog pitanja

	// Meni
	while (nastavi) {
		cout << "Unesi broj za zeljenu operaciju: " << endl
			<< "0 - Kraj programa." << endl
			<< "1 - Dodavanje novog pitanja." << endl
			<< "2 - Dodavanje odgovora na neko pitanje/odgovor." << endl
			<< "3 - Pretraga pitanja." << endl
			<< "4 - Pretraga odgovora." << endl
			<< "5 - Pronalazak odgovora sa najvecom ocenom na zadato pitanje." << endl
			<< "6 - Povecanje ocene odredjenog odgovora." << endl
			<< "7 - Uredjivanje odgovora prema njihovim ocenama za sva pitanja." << endl
			<< "8 - Ispis svih pitanja." << endl
			<< "9 - Ispis pojedinacnog pitanja u celosti." << endl
			<< "10 - Brisanje odgovora i svih njegovih potomaka." << endl
			<< "11 - Brisanje pojedinacnog pitanja i svih povezanih odgovora." << endl << endl;
		cin >> unos;
		cin.ignore();
		switch (unos) {
		case 0:
			nastavi = false;

			cout << endl;
			break;
		case 1:
			cout << "Unesi tekst pitanja koje zelis da dodas: ";
			getline(cin, tekst);

			pokCvor = new Cvor(tekst);
			dodajPitanje(pitanja, poslednjePitanje, pokCvor);

			cout << endl;
			break;
		case 2:
			cout << "Unesi tekst pitanja uokviru kog zelis da dodas odgovor: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor)
				dodajOdgovor(pokCvor);
			
			cout << endl;
			break;
		case 3:
			cout << "Unesi tekst pitanja koje trazis: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) cout << "Pitanje ~" << pokCvor->dohvatiSadrzaj() << "~ je pronadjeno." << endl;

			cout << endl;
			break;
		case 4:
			cout << "Unesi tekst pitanja uokviru kog zelis da trazis odgovor: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) {
				pokCvor = pronadjiOdgovor(pokCvor);
				if (pokCvor) cout << "Odgovor je pronadjen." << endl;
			}

			cout << endl;
			break;
		case 5:
			cout << "Unesi tekst pitanja uokviru kog zelis da trazis odgovor: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) {
				pokCvor = pronadjiOdgovorSaMaxOcenom(pokCvor);
				if (pokCvor) cout << "Nadjeni cvor: " << pokCvor->dohvatiSadrzaj() << ", i njegova ocena je " << pokCvor->dohvatiOcenu() << endl;
			}

			cout << endl;
			break;
		case 6:
			cout << "Unesi tekst pitanja uokviru kog zelis da uvecas odgovor: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) {
				pokCvor = pronadjiOdgovor(pokCvor);
				if (pokCvor) {
					cout << "Unesi novu ocenu koju dodeljujes izabranom odgovoru: ";
					cin >> nOcena;
					cin.ignore();
					pokCvor->uvecajOcenu(nOcena);
				}
			}

			cout << endl;
			break;
		case 7:
			for (Elem* i = pitanja; i; i = i->sled) {
				uredjivanjeSvihOdgovora(i->koren);
			}
			cout << "Uredjivanje zavrseno." << endl;

			cout << endl;
			break;
		case 8:
			for (Elem* i = pitanja; i; i = i->sled) {
				i->koren->pisi();
			}

			cout << endl;
			break;
		case 9:
			cout << "Unesi tekst pitanja koje zelis u celosti da ispises: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) {
				ispisCelogPitanja(pokCvor);
			}

			cout << endl;
			break;
		case 10:
			cout << "Unesi tekst pitanja uokviru kog zelis da obrises odgovor: ";
			getline(cin, tekst);

			pokCvor = pronadjiPitanje(pitanja, tekst);
			if (pokCvor) {
				pokCvor = pronadjiOdgovor(pokCvor);
				if (pokCvor) {
					if (pokCvor->dohvatiPokNaOca() == nullptr)
						cout << "Greska, uneto je pitanje, a ne odgovor." << endl;
					else {
						obrisiOdgovor(pokCvor);
					}
				}
			}

			cout << endl;
			break;
		case 11:
			cout << "Unesi tekst pitanja koje zelis da obrises: ";
			getline(cin, tekst);

			obrisiPitanje(pitanja, poslednjePitanje, tekst);

			cout << endl;
			break;
		default:
			cout << "Nepostojeca opcija." << endl << endl;
		}
	}

	// Dealokacija svega preostalog
	while (pitanja) {
		Elem* staro = pitanja;
		pitanja = pitanja->sled;
		obrisiPitanje(staro, poslednjePitanje, staro->koren->dohvatiSadrzaj());
	}
}