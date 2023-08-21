/*  SISerator - creates Single Image Stereograms from Heightmaps and Textures (both loaded from user) or Single Image Random Dot Stereograms from Heightmaps
    Copyright (C) 2015  Ansgar Rütten

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef __SIS_Kern
#define __SIS_Kern
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

class SIS_Bild{
	private:
		unsigned char* tDaten;
		unsigned int tBreite, tHoehe;
	public:
		//SIS_Bild(); /*erhaelt als Daten einen NULL Pointer*/
		SIS_Bild(unsigned int, unsigned int); /*Daten werden mit Pseudozufallszahlen gefuellt*/
		SIS_Bild(unsigned int, unsigned int, unsigned char*);
		SIS_Bild(const SIS_Bild&);
		SIS_Bild& operator=(const SIS_Bild&);
		~SIS_Bild();
		/*Daten*/
		void Daten(unsigned int, unsigned char);
		unsigned char Daten(unsigned int) const;
		unsigned char *Datensatz() const;
		void Plus(unsigned int, unsigned char);
		/*Breite*/
		void Breite(unsigned int);
		unsigned int Breite(void) const;
		/*Hoehe*/
		void Hoehe(unsigned int);
		unsigned int Hoehe(void) const;
		/*Zufallswerte*/
		void ZufallsWerte(bool);
};

class SingleImageStereogram{
	protected:
		SIS_Bild *Hoehenkarte, *Textur, *Stereogramm;
		int TexturBreitenNullage, TexturHoehenNullage, StartPunkt;
        int Bildtiefe, MattscheibenAbstand, Verschub, Augenabstand, Bildabstand;
		float HoehenFkt;
		bool KreuzBlickAktiv;
		virtual void ErzeugeStereogram(void) = 0;
		virtual void TexturAufbringen(void);
		void LadeGrundeinstellungen(void);
		void BerechneBildabstand(void);
		void BerechneVerschub(void);
	public:
		SingleImageStereogram(const SingleImageStereogram&);
		SingleImageStereogram(SIS_Bild*, SIS_Bild*);
		virtual ~SingleImageStereogram(void);
		/*Werte auslesen*/
		SIS_Bild* HoleStereogramm(void);
		SIS_Bild* HoleTexturierteHK(void);
		int HoleTexturBreitenNullage(void) const;
		int HoleTexturHoehenNullage(void) const;
		int HoleAugenabstand(void) const;
		int HoleBildtiefe(void) const;
		int HoleBildabstand(void) const;
		int HoleMattscheibenAbstand(void) const;
		int HoleVerschub(void) const;
		int HoleMinVerschub(void) const;
		float HoleHoehenFkt(void) const;
		int HoleStartPunkt(void) const;
		int HoleGesamtGroesse(void) const;
		bool KreuzBlick() const;
		/*Werte setzen*/
		void SetzeTexturBreitenNullage(int);
		void SetzeTexturHoehenNullage(int);
		void SetzeStartPunkt(int);
		void SetzeAugenabstand(int);
		void SetzeBildtiefe(int);
		//void SetzeBildabstand(int);
		void SetzeMattscheibenAbstand(int);
		void SetzeVerschub(int);
		void SetzeHoehenFkt(float);
		SingleImageStereogram& operator=(const SingleImageStereogram&);
		void KreuzBlick(bool);

    protected:
        /*Logbuch*/
        char LogName[16];
        ofstream *Logbuch;
        void logSchreiben(const char*);
        void logSchreiben(const float msg, int i=0);
};

class SIS_Einfach : public SingleImageStereogram
{
    public:
        SIS_Einfach(SIS_Bild*, SIS_Bild*);
        virtual ~SIS_Einfach();
        virtual void ErzeugeStereogram(void);
};

class SIS_Mehrfach : public SIS_Einfach
{
    public:
        SIS_Mehrfach(SIS_Bild*, SIS_Bild*);
        virtual ~SIS_Mehrfach();
        virtual void ErzeugeStereogram(void);
        void Verschmieren(void);
};

class SIS_Doppelbild : public SingleImageStereogram
{
    public:
        SIS_Doppelbild(SIS_Bild*, SIS_Bild*);
        virtual ~SIS_Doppelbild();
        virtual void ErzeugeStereogram(void);
};
#endif
