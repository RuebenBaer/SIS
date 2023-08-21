/* DONE (Ansi#1#): Default Werte einlesen*/
/* DONE (Ansi#1#): Absturz in Verschubschleife untersuchen*/
/* DONE (Ansi#2#): Texturnulllage auf sinnvolle Werte beschränken!*/
/* DONE (Ansi#2#): Aufbringen der Textur auf das Stereogramm vor dem "Verschieben"*/
/* DONE (Ansi#1#): Stereogram Konstruktor mit nur einem Bild entfernen*/
/* DONE (Ansi#2#): OffsetHoehe einpflegen*/

#include "SIS_Kern.h"
#include "ErrorObjekt.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

/************************************************************************************************/
/*SingleImageStereogram*/
/************************************************************************************************/

SingleImageStereogram::SingleImageStereogram(const SingleImageStereogram &Obj)
{
    try
	{
		Stereogramm = new SIS_Bild(1,1);
	}
	catch(Error* e)
	{
		throw;
	}

	Hoehenkarte = Obj.Hoehenkarte;
	Textur = Obj.Textur;
	TexturBreitenNullage = Obj.TexturBreitenNullage;
	Augenabstand = Obj.Augenabstand;
	Bildtiefe = Obj.Bildtiefe;
	MattscheibenAbstand = Obj.MattscheibenAbstand;
	HoehenFkt = Obj.HoehenFkt;
	KreuzBlickAktiv = Obj.KreuzBlickAktiv;
}

SingleImageStereogram& SingleImageStereogram::operator=(const SingleImageStereogram &Obj)
{
    try
	{
		Stereogramm = new SIS_Bild(1,1);
	}
	catch(Error* e)
	{
		throw;
	}

	Hoehenkarte = Obj.Hoehenkarte;
	Textur = Obj.Textur;
	TexturBreitenNullage = Obj.TexturBreitenNullage;
	Augenabstand = Obj.Augenabstand;
	Bildtiefe = Obj.Bildtiefe;
	MattscheibenAbstand = Obj.MattscheibenAbstand;
	HoehenFkt = Obj.HoehenFkt;
	KreuzBlickAktiv = Obj.KreuzBlickAktiv;
	return *this;
}

SingleImageStereogram::SingleImageStereogram(SIS_Bild* HK, SIS_Bild* TTR)
{
	Hoehenkarte = HK;
	Textur = TTR;
	LadeGrundeinstellungen();
	try
	{
		Stereogramm = new SIS_Bild(1,1);
	}
	catch(Error* e)
	{
		throw;
	}
}

SingleImageStereogram::~SingleImageStereogram(void)
{
    logSchreiben("~SingleImageStereogramm\n");
    logSchreiben("Logbuch geschlossen.\n");
    delete Logbuch;
	delete Stereogramm;
}

void SingleImageStereogram::LadeGrundeinstellungen(void)
{
	char Zeile[50];
	bool Pruefer[5]={0,0,0,0,0};
	ifstream GrundEinstellungsDatei("Grundeinstellungen.cfg", ios::in);
	if(GrundEinstellungsDatei.good())
	{
		cout<<"Grundeinstellungen.cfg existiert"<<endl;
		while(!GrundEinstellungsDatei.eof())
		{
			GrundEinstellungsDatei.getline(Zeile,50,' ');
			if(!strcmp(Zeile,"w_Augenabstand"))
			{
				GrundEinstellungsDatei.getline(Zeile,50);
				Augenabstand=atoi(Zeile);
				Pruefer[0]=1;
				cout<<"Pruefer[0]: "<<Pruefer[0]<<endl;
				continue;
			}
			if(!strcmp(Zeile,"w_MattscheibenAbstand"))
			{
				GrundEinstellungsDatei.getline(Zeile,50);
				MattscheibenAbstand=atoi(Zeile);
				Pruefer[1]=1;
				cout<<"Pruefer[1]: "<<Pruefer[1]<<endl;
				continue;
			}
			if(!strcmp(Zeile,"w_Bildtiefe"))
			{
				GrundEinstellungsDatei.getline(Zeile,50);
				Bildtiefe=atoi(Zeile);
				Pruefer[2]=1;
				cout<<"Pruefer[2]: "<<Pruefer[2]<<endl;
				continue;
			}
			if(!strcmp(Zeile,"w_TexturNullage"))
			{
				GrundEinstellungsDatei.getline(Zeile,50);
				TexturBreitenNullage=atoi(Zeile);
				Pruefer[3]=1;
				cout<<"Pruefer[3]: "<<Pruefer[3]<<endl;
				continue;
			}
			if(!strcmp(Zeile,"w_Verschub"))
			{
				GrundEinstellungsDatei.getline(Zeile,50);
				Verschub=atoi(Zeile);
				Pruefer[4]=1;
				cout<<"Pruefer[4]: "<<Pruefer[4]<<endl;
				continue;
			}
		}
		if(!(Pruefer[0]))Augenabstand=227;
		if(!(Pruefer[1]))MattscheibenAbstand=1890;
		if(!(Pruefer[2]))Bildtiefe=378;
		if(!(Pruefer[3]))TexturBreitenNullage=0;
		if(!(Pruefer[4]))Verschub=190;
		/*Beschraenkung von HoehenFkt*/
		//if(HoehenFkt>(Bildtiefe/255.0))HoehenFkt=(Bildtiefe/255.0);
		/*Verschub ausrechnen*/
		//Verschub = Augenabstand*Bildtiefe/(MattscheibenAbstand+Bildtiefe);
		GrundEinstellungsDatei.close();
	}else{
		cout<<"Grundeinstellungen.cfg existiert nicht"<<"\nErstelle die Datei neu"<<endl;
		ofstream NeueEinstellungsDatei("Grundeinstellungen.cfg", ios::out|ios_base::app);
		if(NeueEinstellungsDatei.good())
		{
			NeueEinstellungsDatei<<"w_Augenabstand 227"<<"\n";
			NeueEinstellungsDatei<<"w_MattscheibenAbstand 1890"<<"\n";
			NeueEinstellungsDatei<<"w_Bildtiefe 378"<<"\n";
			NeueEinstellungsDatei<<"w_TexturNullage 0"<<"\n";
			NeueEinstellungsDatei<<"w_Verschub 190"<<"\n";
			NeueEinstellungsDatei.close();
		}
		LadeGrundeinstellungen();
	}
	KreuzBlickAktiv = 0;
	BerechneBildabstand();
	return;
}

int SingleImageStereogram::HoleTexturBreitenNullage(void) const
{
	return TexturBreitenNullage;
}

int SingleImageStereogram::HoleTexturHoehenNullage(void) const
{
	return TexturHoehenNullage;
}

int SingleImageStereogram::HoleAugenabstand(void) const
{
	return Augenabstand;
}

int SingleImageStereogram::HoleBildtiefe(void) const
{
	return Bildtiefe;
}

int SingleImageStereogram::HoleBildabstand(void) const
{
	return Bildabstand;
}

int SingleImageStereogram::HoleMattscheibenAbstand(void) const
{
	return MattscheibenAbstand;
}

int SingleImageStereogram::HoleVerschub(void) const
{
	return Verschub;
}

int SingleImageStereogram::HoleMinVerschub(void) const
{
    int lokalerVerschub, MinVerschub = Hoehenkarte->Breite();
    float lokHoehenFkt = (float)(Bildtiefe)/255.0f;

    for (unsigned int Reihe=0; Reihe < Hoehenkarte->Hoehe(); Reihe++)
	{
   		for (unsigned int Spalte = StartPunkt; Spalte < Hoehenkarte->Breite(); Spalte++)
		{
			/*try*/int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
			/*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
			if(KreuzBlickAktiv)
			{
			    lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
            }else{
                lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
            }
            if(lokalerVerschub < MinVerschub)
            {
                MinVerschub = lokalerVerschub;
            }
        }
	}
	return MinVerschub;
}

int SingleImageStereogram::HoleStartPunkt(void) const
{
	return StartPunkt;
}

float SingleImageStereogram::HoleHoehenFkt(void) const
{
	return HoehenFkt;
}

int SingleImageStereogram::HoleGesamtGroesse(void) const
{
    int Rueckgabe = (Hoehenkarte->Breite() + Verschub)*
                    Hoehenkarte->Hoehe()*3;
    return(Rueckgabe);
}

void SingleImageStereogram::SetzeTexturBreitenNullage(int Wert)
{
	TexturBreitenNullage = Wert;
	return;
}

void SingleImageStereogram::SetzeTexturHoehenNullage(int Wert)
{
	TexturHoehenNullage = Wert;
	return;
}

void SingleImageStereogram::SetzeStartPunkt(int Wert)
{
    StartPunkt = Wert;
    if(StartPunkt<0)StartPunkt=0;
    if(Hoehenkarte!=0)
    {
        if(StartPunkt>=(int)(Hoehenkarte->Breite()))StartPunkt=(int)(Hoehenkarte->Breite()-1);
    }
    return;
}

void SingleImageStereogram::SetzeAugenabstand(int Wert)
{
	Augenabstand = Wert;
    BerechneBildabstand();
	return;
}

void SingleImageStereogram::SetzeBildtiefe(int Wert)
{
	Bildtiefe = Wert;
    BerechneBildabstand();
	return;
}

/*Funktion rausgenommen - unerwartete Ergebnisse
void SingleImageStereogram::SetzeBildabstand(int Wert)
{
	Bildabstand= Wert;
	BerechneVerschub();
	return;
}*/

void SingleImageStereogram::SetzeMattscheibenAbstand(int Wert)
{
	MattscheibenAbstand = Wert;
    BerechneBildabstand();
	return;
}

void SingleImageStereogram::SetzeVerschub(int Wert)
{
	Verschub = Wert;
    BerechneBildabstand();
	return;
}

void SingleImageStereogram::BerechneBildabstand(void)
{
    if(KreuzBlickAktiv)
    {
        Bildabstand = (int)((float)Augenabstand*(float)MattscheibenAbstand/(float)(Augenabstand+Verschub)+(float)Bildtiefe);

    }else{
        Bildabstand = (int)((float)Augenabstand*(float)MattscheibenAbstand/(float)(Augenabstand-Verschub));
    }
    return;
}

void SingleImageStereogram::BerechneVerschub(void)
{
    if(KreuzBlickAktiv)
    {
        if((Bildabstand-Bildtiefe)==0)Bildabstand++;

        Verschub = (int)((float)Augenabstand*(((float)MattscheibenAbstand/(float)(Bildabstand-Bildtiefe))-1.0));

    }else{
        Verschub = (int)((float)Augenabstand*(1.0-((float)MattscheibenAbstand/(float)(Bildabstand))));
    }
    return;
}

void SingleImageStereogram::SetzeHoehenFkt(float Wert)
{
	HoehenFkt = Wert;
    BerechneBildabstand();
	return;
}

SIS_Bild* SingleImageStereogram::HoleStereogramm(void)
{
	try
	{
		ErzeugeStereogram();
	}
	catch(Error* e)
	{
		throw;
	}
	return Stereogramm;
}

SIS_Bild* SingleImageStereogram::HoleTexturierteHK(void)
{
    TexturAufbringen();
    return Stereogramm;
}

void SingleImageStereogram::TexturAufbringen(void)
{
	try
	{
		Stereogramm->Hoehe(Hoehenkarte->Hoehe());
		Stereogramm->Breite(Hoehenkarte->Breite() + Verschub);
	}
	catch(Error*e)
	{
		e->anhaengen((char *)("\nStereogramm vergrößern gescheitert"));
		throw;
	}

	int TexturReihe, TexturSpalte;
	int offsetBreite, offsetHoehe;

	offsetHoehe = TexturHoehenNullage;
    while(offsetHoehe < 0)
	{
        offsetHoehe += Textur->Hoehe();
    }
	while(offsetHoehe >= (int)Textur->Hoehe())
	{
        offsetHoehe -= Textur->Hoehe();
    }
    offsetBreite = TexturBreitenNullage;
    while(offsetBreite < 0)
	{
        offsetBreite += Textur->Breite();
    }
	while(offsetBreite >= (int)Textur->Breite())
	{
        offsetBreite -= Textur->Breite();
    }

	/*Hoehenoffset Anfang*/
	TexturReihe =  Textur->Hoehe() - offsetHoehe;
	for(unsigned int Reihe = 0; Reihe < Stereogramm->Hoehe(); Reihe++)
	{
		if(TexturReihe>=(int)Textur->Hoehe())TexturReihe = 0;
		/*Breitenoffset Anfang*/
		TexturSpalte = Textur->Breite() - offsetBreite;
		for(unsigned int Spalte = 0; Spalte < Stereogramm->Breite(); Spalte++)
		{
			if(TexturSpalte>=(int)Textur->Breite())TexturSpalte = 0;
			try{
				Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3, Textur->Daten((TexturSpalte+TexturReihe*Textur->Breite())*3));
				Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3 + 1, Textur->Daten((TexturSpalte+TexturReihe*Textur->Breite())*3 + 1));
				Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3 + 2, Textur->Daten((TexturSpalte+TexturReihe*Textur->Breite())*3 + 2));
			}
			catch(Error* e)
			{
				e->anhaengen((char*)"\nFehler bei Textur aufbringen (Vorwärts)");
				throw;
			}
			TexturSpalte++;
		}
		TexturReihe++;
	}
	return;
}

void SingleImageStereogram::KreuzBlick(bool Wert)
{
    KreuzBlickAktiv = Wert;
    BerechneBildabstand();
    return;
}

bool SingleImageStereogram::KreuzBlick(void) const
{
    return KreuzBlickAktiv;
}

/*Logbuch*/
void SingleImageStereogram::logSchreiben(const char*msg)
{
    Logbuch->open(LogName,ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}

void SingleImageStereogram::logSchreiben(const float msg, int i)
{
    Logbuch->open(LogName,ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        Logbuch->setf( ios::fixed, ios::floatfield );
        Logbuch->precision(i);
        *Logbuch<<msg;
        Logbuch->close();
    }
    return;
}
/*Logbuch ENDE*/

/************************************************************************************************/
/*SIS_Bild*/
/************************************************************************************************/

SIS_Bild::SIS_Bild(unsigned int h, unsigned int b)
{
	tHoehe = h;
	tBreite = b;
	cout<<"Groesse SIS: "<<b*h*3<<endl;
	try{
		tDaten = new unsigned char[tHoehe*tBreite*3];
	}
	catch(...)
	{
		throw new Error((char*)"Unbekannter Fehler bei new unsigned char []!!");
	}
	/*Zufallspunktbild erzeugen*/
	if(tDaten!=NULL)
	{
		srand(time(NULL));
		for(unsigned int i = 0; i<(tHoehe*tBreite*3);i++) tDaten[i]=rand()%255;
	}else{
		tHoehe = tBreite = 0;
		throw new Error((char*)"Fehler bei Speicheranforderung");
	}
}

SIS_Bild::SIS_Bild(unsigned int h, unsigned int b, unsigned char* DatenPuffer)
{
	tHoehe = h;
	tBreite = b;
	try{
		tDaten = new unsigned char[tHoehe*tBreite*3];
	}
	catch(...)
	{
		throw new Error((char*)"Fehler bei new unsigned char []!!");
	}
	if(tDaten!=NULL)
	{
		for(unsigned int i = 0; i<(tHoehe*tBreite*3);i++) tDaten[i] = DatenPuffer[i];
	}else{
		tHoehe = tBreite = 0;
		throw new Error((char*)"Fehler bei Speicheranforderung");
	}
}

SIS_Bild::SIS_Bild(const SIS_Bild& BildPuffer)
{
	tHoehe = BildPuffer.Hoehe();
	tBreite = BildPuffer.Breite();
	try{
		tDaten = new unsigned char[tHoehe*tBreite*3];
	}
	catch(...)
	{
		throw new Error((char*)"Fehler bei new unsigned char []!!");
	}
	if(tDaten!=NULL)
	{
		/*try*/for(unsigned int i = 0; i<(tHoehe*tBreite*3);i++) tDaten[i] = BildPuffer.Daten(i);
	}else{
		tHoehe = tBreite = 0;
		throw new Error((char*)"Fehler bei Speicheranforderung");
	}
}

SIS_Bild& SIS_Bild::operator=(const SIS_Bild& BildPuffer)
{
	tHoehe = BildPuffer.Hoehe();
	tBreite = BildPuffer.Breite();
	try{
		tDaten = new unsigned char[tHoehe*tBreite*3];
	}
	catch(...)
	{
		throw new Error((char*)"Fehler bei new unsigned char []!!");
	}
	if(tDaten!=NULL)
	{
		/*try*/for(unsigned int i = 0; i<(tHoehe*tBreite*3);i++) tDaten[i] = BildPuffer.Daten(i);
	}else{
		tHoehe = tBreite = 0;
		throw new Error((char*)"Fehler bei Speicheranforderung");
	}
	return *this;
}

SIS_Bild::~SIS_Bild()
{
	delete []tDaten;
}

void SIS_Bild::Daten(unsigned int i, unsigned char DatenPuffer)
{
	if(i<(tHoehe*tBreite*3))
	{
		tDaten[i] = DatenPuffer;
	}else{
		throw new Error((char*)("Datenfeldgrenze ueberschritten - schreiben"));
	}
	return;
}

void SIS_Bild::Plus(unsigned int i, unsigned char DatenPuffer)
{
	if(i<(tHoehe*tBreite*3))
	{
		tDaten[i] += DatenPuffer;
	}else{
		throw new Error((char*)"Datenfeldgrenze ueberschritten - addieren");
	}
	return;
}

unsigned char SIS_Bild::Daten(unsigned int i) const
{
	if(i<(tHoehe*tBreite*3))
	{
		return(tDaten[i]);
	}else{
		throw new Error((char*)"Datenfeldgrenze ueberschritten - lesen");
	}
}


unsigned char *SIS_Bild::Datensatz() const
{
	return(tDaten);
}

void SIS_Bild::Breite(unsigned int Wert)
{
	unsigned char* tempDaten = new unsigned char[Wert*tHoehe*3];
	if(tempDaten!=NULL)
	{
		for(unsigned int Zeile=0; Zeile<tHoehe; Zeile++)
		{
			for(unsigned int Spalte=0; Spalte<(tBreite<Wert ? tBreite : Wert);Spalte++)
			{
				tempDaten[((Zeile*Wert+Spalte)*3)] = tDaten[((Zeile*tBreite+Spalte)*3)];
				tempDaten[(Zeile*Wert+Spalte)*3+1] = tDaten[(Zeile*tBreite+Spalte)*3+1];
				tempDaten[(Zeile*Wert+Spalte)*3+2] = tDaten[(Zeile*tBreite+Spalte)*3+2];
			}
		}
	}else{
		throw new Error((char*)"Fehler bei Speicheranforderung in 'Breite'");
		return;
	}
	delete []tDaten;
	tDaten = tempDaten;
	tBreite = Wert;
	return;
}

unsigned int SIS_Bild::Breite(void) const
{
    return(tBreite);
}
void SIS_Bild::Hoehe(unsigned int Wert)
{
	unsigned char* tempDaten = new unsigned char[Wert*tBreite*3];
	if(tempDaten!=NULL)
	{
		for(unsigned int Spalte=0; Spalte<tBreite; Spalte++)
		{
			for(unsigned int Zeile=0; Zeile<(tHoehe<Wert ? tHoehe : Wert);Zeile++)
			{
				tempDaten[(Zeile*tBreite+Spalte)*3] = tDaten[(Zeile*tBreite+Spalte)*3];
				tempDaten[(Zeile*tBreite+Spalte)*3+1] = tDaten[(Zeile*tBreite+Spalte)*3+1];
				tempDaten[(Zeile*tBreite+Spalte)*3+2] = tDaten[(Zeile*tBreite+Spalte)*3+2];
			}
		}
	}else{
		throw new Error((char*)"Fehler bei Speicheranforderung in 'Hoehe'");
		return;
	}
	delete []tDaten;
	tDaten = tempDaten;
	tHoehe = Wert;
	return;
}
unsigned int SIS_Bild::Hoehe(void) const
{
	return tHoehe;
}

void SIS_Bild::ZufallsWerte(bool bw = 1)
{
	unsigned char *tempDaten = new unsigned char[tHoehe*tBreite*3];
	if(tempDaten!=NULL)
	{
		srand(time(NULL));
		if(bw)
		{
			unsigned char Wert;
			for(unsigned int i = 0; i<(tHoehe*tBreite*3);i+=3)
			{
				Wert = rand()%255;
				tempDaten[i]=tempDaten[i+1]=tempDaten[i+2]=Wert;
			}
		}else{
			for(unsigned int i = 0; i<(tHoehe*tBreite*3);i++) tempDaten[i]=rand()%255;
		}
	}else{
		tHoehe = tBreite = 0;
		throw new Error((char*)"Fehler bei Speicheranforderung");
		return;
	}
	delete tDaten;
	tDaten = tempDaten;
	return;
}

/*************/
/*SIS_Einfach*/
/*************/

SIS_Einfach::SIS_Einfach(SIS_Bild* HK, SIS_Bild* TTR):SingleImageStereogram(HK, TTR)
{
	/*Log starten*/
	strcpy(LogName, "SIS_Einfach.log");
	Logbuch = new ofstream(LogName, ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

SIS_Einfach::~SIS_Einfach()
{
    logSchreiben("~SIS_Einfach\n");
    //delete Logbuch;
    //delete Stereogramm;
}

void  SIS_Einfach::ErzeugeStereogram(void)
{
    logSchreiben("SIS_Einfach: ErzeugeStereogram\n");
	int lokalerVerschub, von, nach, alterVerschub;

    /*Berechnung maxVerschub für Kreuzblick und Parallelblick alternativ*/
	alterVerschub = Verschub;

	/*Textur auf Stereogramm aufbringen*/
	try
	{
		TexturAufbringen();
	}
	catch(Error* e)
	{
		throw;
	}

    float lokHoehenFkt = (float)(Bildtiefe)/255.0f;

	/*Stereogramm Erzeugen*/
	for (unsigned int Reihe=0; Reihe < Hoehenkarte->Hoehe(); Reihe++)
	{
		int obereGrenze = (Stereogramm->Breite()*(Reihe+1)*3);
		int untereGrenze = (Stereogramm->Breite()*(Reihe)*3);

        //cout<<"\nReihe: "<<Reihe<<endl;
   		for (unsigned int Spalte = StartPunkt; Spalte < Hoehenkarte->Breite(); Spalte++)
		{
			try
            {
				int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
				/*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
				if(KreuzBlickAktiv)
				{
                    lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
                }else{
                    lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
                }

				von = (Reihe*Stereogramm->Breite()+Spalte)*3;
				nach = von + lokalerVerschub*3;

				if(!((nach<untereGrenze)&&(nach>=obereGrenze)))
				{
					Stereogramm->Daten(nach,Stereogramm->Daten(von));
					Stereogramm->Daten(nach+1,Stereogramm->Daten(von+1));
					Stereogramm->Daten(nach+2,Stereogramm->Daten(von+2));

					if(lokalerVerschub>alterVerschub)
					{
					    //nicht überschriebene Pixel mit Material von vorne füllen
                        for(int i=0; i<(lokalerVerschub-alterVerschub); i++)
                        {
                            Stereogramm->Daten(nach-(1+i)*3,Stereogramm->Daten(von-(1+i)*3));
        					Stereogramm->Daten(nach+1-(1+i)*3,Stereogramm->Daten(von+1-(1+i)*3));
        					Stereogramm->Daten(nach+2-(1+i)*3,Stereogramm->Daten(von+2-(1+i)*3));
                        }
                    }
                }
                alterVerschub = lokalerVerschub;
			}
			catch(Error* e)
			{
                logSchreiben("Fehler bei Reihe: ");
                logSchreiben((float)Reihe, 0);
                logSchreiben(" - Spalte: ");
                logSchreiben((float)Spalte, 0);
                logSchreiben("(vorwärts)\n");
                e->anhaengen((char*)"\nStereogramm vorwärts erzeugen\n");
                throw;
			}
		}

		if((StartPunkt)>0)
		{
			for (int Spalte = (StartPunkt); Spalte >= 0; Spalte--)
			{
				try{
					int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
                    /*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
                    if(KreuzBlickAktiv)
                    {
                        lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
                    }else{
                        lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
                    }

					nach = (Reihe*Stereogramm->Breite()+Spalte)*3;
					von	= nach + lokalerVerschub*3;

					if(!((von<untereGrenze)&&(von>=obereGrenze)))
					{
						Stereogramm->Daten(nach,Stereogramm->Daten(von));
						Stereogramm->Daten(nach+1,Stereogramm->Daten(von+1));
						Stereogramm->Daten(nach+2,Stereogramm->Daten(von+2));
					}
				}
				catch(Error* e)
				{
                    logSchreiben("Fehler bei Reihe: ");
                    logSchreiben((float)Reihe, 0);
                    logSchreiben(" - Spalte: ");
                    logSchreiben((float)Spalte, 0);
                    logSchreiben("(rückwärts)\n");
					e->anhaengen((char*)"\nStereogramm rückwärts erzeugen\n");
					throw;
                }
			}
		}
	}
	return;
}

/*************/
/*SIS_Mehrfach*/
/*************/

SIS_Mehrfach::SIS_Mehrfach(SIS_Bild* HK, SIS_Bild* TTR):SIS_Einfach(HK, TTR)
{
	/*Log starten*/
	strcpy(LogName, "SIS_Mehrfach.log");
	Logbuch = new ofstream(LogName, ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

SIS_Mehrfach::~SIS_Mehrfach()
{
    logSchreiben("~SIS_Mehrfach\n");
}

void SIS_Mehrfach::ErzeugeStereogram(void)
{
    logSchreiben("SIS_Mehrfach: ErzeugeStereogram\n");
	/*Textur auf Stereogramm aufbringen*/
	try
	{
		TexturAufbringen();
	}
	catch(Error* e)
	{
		throw;
	}
	try
	{
	    Verschmieren();
	}
	catch(Error* e)
	{
		throw;
	}

	int lokalerVerschub, von, nach, alterVerschub;

    /*Berechnung maxVerschub für Kreuzblick und Parallelblick alternativ*/
	alterVerschub = Verschub;

    float lokHoehenFkt = (float)(Bildtiefe)/255.0f;

	/*Stereogramm Erzeugen*/
	for (unsigned int Reihe=0; Reihe < Hoehenkarte->Hoehe(); Reihe++)
	{
		int obereGrenze = (Stereogramm->Breite()*(Reihe+1)*3);
		int untereGrenze = (Stereogramm->Breite()*(Reihe)*3);

        //cout<<"\nReihe: "<<Reihe<<endl;
   		for (unsigned int Spalte = StartPunkt; Spalte < Hoehenkarte->Breite(); Spalte++)
		{
			try
            {
				int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
				/*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
				if(KreuzBlickAktiv)
				{
                    lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
                }else{
                    lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
                }

				von = (Reihe*Stereogramm->Breite()+Spalte)*3;
				nach = von + lokalerVerschub*3;

				if(!((nach<untereGrenze)&&(nach>=obereGrenze)))
				{
					Stereogramm->Daten(nach,Stereogramm->Daten(von));
					Stereogramm->Daten(nach+1,Stereogramm->Daten(von+1));
					Stereogramm->Daten(nach+2,Stereogramm->Daten(von+2));

					if(lokalerVerschub>alterVerschub)
					{
					    //nicht überschriebene Pixel mit Material von vorne füllen
                        for(int i=0; i<(lokalerVerschub-alterVerschub); i++)
                        {
                            Stereogramm->Daten(nach-(1+i)*3,Stereogramm->Daten(von-(1+i)*3));
        					Stereogramm->Daten(nach+1-(1+i)*3,Stereogramm->Daten(von+1-(1+i)*3));
        					Stereogramm->Daten(nach+2-(1+i)*3,Stereogramm->Daten(von+2-(1+i)*3));
                        }
                    }
                }
                alterVerschub = lokalerVerschub;
			}
			catch(Error* e)
			{
                logSchreiben("Fehler bei Reihe: ");
                logSchreiben((float)Reihe, 0);
                logSchreiben(" - Spalte: ");
                logSchreiben((float)Spalte, 0);
                logSchreiben("(vorwärts)\n");
                e->anhaengen((char*)"\nStereogramm vorwärts erzeugen\n");
                throw;
			}
		}

		if((StartPunkt)>0)
		{
			for (int Spalte = (StartPunkt); Spalte >= 0; Spalte--)
			{
				try{
					int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
                    /*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
                    if(KreuzBlickAktiv)
                    {
                        lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
                    }else{
                        lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
                    }

					nach = (Reihe*Stereogramm->Breite()+Spalte)*3;
					von	= nach + lokalerVerschub*3;

					if(!((von<untereGrenze)&&(von>=obereGrenze)))
					{
						Stereogramm->Daten(nach,Stereogramm->Daten(von));
						Stereogramm->Daten(nach+1,Stereogramm->Daten(von+1));
						Stereogramm->Daten(nach+2,Stereogramm->Daten(von+2));
					}
				}
				catch(Error* e)
				{
                    logSchreiben("Fehler bei Reihe: ");
                    logSchreiben((float)Reihe, 0);
                    logSchreiben(" - Spalte: ");
                    logSchreiben((float)Spalte, 0);
                    logSchreiben("(rückwärts)\n");
					e->anhaengen((char*)"\nStereogramm rückwärts erzeugen\n");
					throw;
                }
			}
		}
	}
	return;
}

void SIS_Mehrfach::Verschmieren(void)
{
    logSchreiben("SIS_Mehrfach: Verschmieren\n");
    /*Ueberblenden der Textur in Streifen*/
    int* Puffer = new int[(Verschub*Stereogramm->Hoehe()*3)];
    int* Wiederholung = new int[(Verschub*Stereogramm->Hoehe()*3)];

    if((!Puffer)||(!Wiederholung))
    {
        return;
    }
    for(int i = 0; i<(Verschub*Stereogramm->Hoehe()*3); i++)
    {
        Puffer[i] = 0;
        Wiederholung[i] = 0;
    }

    int pufferSpalte = Verschub-(StartPunkt%Verschub);
    for(unsigned int Spalte = 0; Spalte<Stereogramm->Breite(); Spalte++)
    {
        if(pufferSpalte >= Verschub)
        {
            pufferSpalte = 0;
        }
        for(unsigned int Reihe = 0; Reihe<Stereogramm->Hoehe(); Reihe++)
        {
            Puffer[(pufferSpalte+Reihe*Verschub)*3] += (int)Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3);
            Puffer[(pufferSpalte+Reihe*Verschub)*3+1] += (int)Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3+1);
            Puffer[(pufferSpalte+Reihe*Verschub)*3+2] += (int)Stereogramm->Daten((Spalte+Reihe*Stereogramm->Breite())*3+2);

            Wiederholung[(pufferSpalte+Reihe*Verschub)*3] += 1;
            Wiederholung[(pufferSpalte+Reihe*Verschub)*3+1] += 1;
            Wiederholung[(pufferSpalte+Reihe*Verschub)*3+2] += 1;
        }
        pufferSpalte++;
    }
    /*ENDE Ueberblenden der Textur in Streifen*/
    /*Aufbringen auf Stereogramm*/
    logSchreiben("Beginn Schreiben Stereogramm: ");
    logSchreiben("StartPunkt: ");
    logSchreiben((float)StartPunkt, 0);
    logSchreiben(" - Verschub: ");
    logSchreiben((float)Verschub, 0);
    logSchreiben("\n");
    for(unsigned int Spalte = StartPunkt; Spalte<(StartPunkt + Verschub); Spalte++)
    {
        for(unsigned int Reihe = 0; Reihe<Stereogramm->Hoehe(); Reihe++)
        {
            try
            {
                Stereogramm->Daten((Spalte + Reihe * Stereogramm->Breite())*3, (char)(Puffer[(Spalte-StartPunkt+Reihe*Verschub)*3]/Wiederholung[(Spalte-StartPunkt+Reihe*Verschub)*3]));
                Stereogramm->Daten((Spalte + Reihe * Stereogramm->Breite())*3+1, (char)(Puffer[(Spalte-StartPunkt+Reihe*Verschub)*3+1]/Wiederholung[(Spalte-StartPunkt+Reihe*Verschub)*3+1]));
                Stereogramm->Daten((Spalte + Reihe * Stereogramm->Breite())*3+2, (char)(Puffer[(Spalte-StartPunkt+Reihe*Verschub)*3+2]/Wiederholung[(Spalte-StartPunkt+Reihe*Verschub)*3+2]));
            }
            catch(Error *e)
            {
                logSchreiben("Fehler bei Reihe: ");
                logSchreiben((float)Reihe, 0);
                logSchreiben(" - Spalte: ");
                logSchreiben((float)Spalte, 0);
                logSchreiben(" (Schreiben Stereogramm in 'Verschmieren')\n");
				e->anhaengen((char*)"\nSIS_Mehrfach::Verschmieren()\n");
				throw;
            }
        }
    }
    /*ENDE Aufbringen auf Stereogramm*/
    delete[] Puffer;
    delete[] Wiederholung;
    return;
}

/****************/
/*SIS_Doppelbild*/
/****************/

SIS_Doppelbild::SIS_Doppelbild(SIS_Bild* HK, SIS_Bild* TTR):SingleImageStereogram(HK, TTR)
{
	/*Log starten*/
	strcpy(LogName, "SIS_Doppelbild.log");
	Logbuch = new ofstream(LogName, ios_base::out|ios_base::app);
    if(Logbuch->good())
    {
        *Logbuch<<"Logbuch gestartet\n";
        Logbuch->close();
    }
}

SIS_Doppelbild::~SIS_Doppelbild()
{
    logSchreiben("~SIS_Doppelbild\n");
}

void SIS_Doppelbild::ErzeugeStereogram(void)
{
    logSchreiben("SIS_Doppelbild: ErzeugeStereogram\n");
    SIS_Bild* DoppelBild;
    int* HoehenPuffer;
	/*Textur auf Stereogramm aufbringen*/
	try
	{
		TexturAufbringen();
	}
	catch(Error* e)
	{
        e->anhaengen((char*)" - Doppelbild\n");
        logSchreiben(e->message());
		throw;
	}

    /*Puffer für Erzeugung*/
    int vmin = HoleMinVerschub();
    int deltaB = ((Verschub - vmin) / 2);
    try
    {
        DoppelBild = new SIS_Bild(Hoehenkarte->Hoehe(), (Hoehenkarte->Breite()+deltaB)*2);
    }
    catch(Error* e)
    {
        e->anhaengen((char*)" - Doppelbild Puffer\n");
        logSchreiben(e->message());
        delete DoppelBild;
        throw;
    }
    for (int i=0; i<(DoppelBild->Breite()*DoppelBild->Hoehe()*3); i++)
    {
        DoppelBild->Daten(i, 0);
    }
    HoehenPuffer = new int[(Hoehenkarte->Breite()+deltaB)*2];
    if(!HoehenPuffer)
    {
        logSchreiben((char*)"HoehenPuffer konnte nicht erzeugt werden\n");
        return;
    }
    logSchreiben((char*)"HoehenPuffer erzeugt\n");

	/*Stereogramm Erzeugen*/
	int von, nach_links, nach_rechts, lokalerVerschub;
    float lokHoehenFkt = (float)(Bildtiefe)/255.0f;

	for (unsigned int Reihe=0; Reihe < Hoehenkarte->Hoehe(); Reihe++)
	{
        for (int i=0; i<(Hoehenkarte->Breite()+deltaB)*2; i++)
        {
            HoehenPuffer[i]=-1;
        }
		int obereGrenze = (DoppelBild->Breite()*(Reihe+1)*3);
		int untereGrenze = (DoppelBild->Breite()*(Reihe)*3);

		int alterVerschub = Verschub;

   		for (unsigned int Spalte = 0; Spalte < Hoehenkarte->Breite(); Spalte++)
		{
			try
            {
				int Pixelhoehe = (int)(Hoehenkarte->Daten((Reihe*Hoehenkarte->Breite()+Spalte)*3)*lokHoehenFkt);
				/*Lokalen Verschub für Kreuzblick und Parallelblick alternativ*/
				if(KreuzBlickAktiv)
				{
                    lokalerVerschub = (int)((float)Augenabstand*((float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)-1));
                }else{
                    lokalerVerschub = (int)((float)Augenabstand*(1-(float)MattscheibenAbstand/(float)(Bildabstand-Pixelhoehe)));
                }
                lokalerVerschub = (lokalerVerschub-vmin)/2;

				von = (Reihe*Stereogramm->Breite()+Spalte)*3;
				nach_links = (Reihe*DoppelBild->Breite()+Spalte)*3 - lokalerVerschub*3 + deltaB *3;
				nach_rechts = (Reihe*DoppelBild->Breite()+Spalte)*3 + (Hoehenkarte->Breite() + lokalerVerschub)*3 + deltaB *3;

				if(!((nach_links<untereGrenze)&&(nach_rechts>=obereGrenze)))
				{
				    if(Pixelhoehe>HoehenPuffer[Spalte - lokalerVerschub + deltaB])
                    {
                        HoehenPuffer[Spalte-lokalerVerschub+deltaB]=Pixelhoehe;
                        try
                        {
                            DoppelBild->Daten(nach_links,Stereogramm->Daten(von));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_links, 0);logSchreiben((char*)"nach_links");
                        }
                        try
                        {
                            DoppelBild->Daten(nach_links+1,Stereogramm->Daten(von+1));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_links+1, 0);logSchreiben((char*)"nach_links+1");
                        }
                        try
                        {
                            DoppelBild->Daten(nach_links+2,Stereogramm->Daten(von+2));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_links+1, 0);logSchreiben((char*)"nach_links+2");
                        }
                        if((lokalerVerschub<alterVerschub)&&(Spalte>0))
                        {
                            //nicht überschriebene Pixel mit Material von vorne füllen
                            for(int i=0; i<(alterVerschub-lokalerVerschub); i++)
                            {
                                DoppelBild->Daten(nach_links-(1+i)*3, Stereogramm->Daten(von+(1+i)*3));
                                DoppelBild->Daten(nach_links+1-(1+i)*3, Stereogramm->Daten(von+1+(1+i)*3));
                                DoppelBild->Daten(nach_links+2-(1+i)*3, Stereogramm->Daten(von+2+(1+i)*3));
                            }
                        }
                    }
                    if(Pixelhoehe>HoehenPuffer[Spalte + Hoehenkarte->Breite() + lokalerVerschub + deltaB])
                    {
                        HoehenPuffer[Spalte + Hoehenkarte->Breite() + lokalerVerschub + deltaB]=Pixelhoehe;
                        try
                        {
                            DoppelBild->Daten(nach_rechts,Stereogramm->Daten(von));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_rechts, 0);logSchreiben((char*)"nach_rechts");
                        }
                        try
                        {
                            DoppelBild->Daten(nach_rechts+1,Stereogramm->Daten(von+1));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_rechts+1, 0);logSchreiben((char*)"nach_rechts+1");
                        }
                        try
                        {
                            DoppelBild->Daten(nach_rechts+2,Stereogramm->Daten(von+2));
                        }
                        catch(Error* e)
                        {
                            logSchreiben(nach_rechts+2, 0);logSchreiben((char*)"nach_rechts+2");
                        }
                        if(lokalerVerschub>alterVerschub)
                        {
                            for(int i=0; i<(lokalerVerschub-alterVerschub); i++)
                            {
                                DoppelBild->Daten(nach_rechts-(1+i)*3, Stereogramm->Daten(von-(1+i)*3));
                                DoppelBild->Daten(nach_rechts+1-(1+i)*3, Stereogramm->Daten(von+1-(1+i)*3));
                                DoppelBild->Daten(nach_rechts+2-(1+i)*3, Stereogramm->Daten(von+2-(1+i)*3));
                            }
                        }
                    }
                    alterVerschub = lokalerVerschub;
                }
			}
			catch(Error* e)
			{
                logSchreiben("Fehler bei Reihe: ");logSchreiben((float)Reihe, 0);logSchreiben(" - Spalte: ");
                logSchreiben((float)Spalte, 0);logSchreiben("(vorwärts): ");logSchreiben(e->message());logSchreiben("\n");
                e->anhaengen((char*)"\nStereogramm vorwärts erzeugen\n");
                delete DoppelBild;
                throw;
			}
		}
	}
	delete Stereogramm;
	delete[]HoehenPuffer;
	Stereogramm = DoppelBild;
	logSchreiben((char*)"Erfolgreich erstellt");
	return;
}
