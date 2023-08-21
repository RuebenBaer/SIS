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

#ifndef __Vektor_
#define __Vektor_

class Vektor{
  private:
    float* Koordinate;
  public:
    /*Konstruktoren*/
    Vektor();
    Vektor(const Vektor&);
    Vektor(float, float, float);
    Vektor(float*);
    /*Destruktor*/
    ~Vektor();
    /*Koordinaten fuellen*/
    void SetKoordinaten(float, float, float);
    void SetKoordinaten(float*);
    void SetKoordinaten(int, float);/*Index, Wert*/
    /*Koordinaten auslesen*/
    float x(void){return(Koordinate[0]);};
    float y(void){return(Koordinate[1]);};
    float z(void){return(Koordinate[2]);};
    float GetKoordinaten(int i)const{return(Koordinate[i]);} ;
    /*Vektorrechnungen*/
    Vektor operator+(const Vektor&);
    Vektor& operator+=(Vektor&);
    Vektor operator-(const Vektor&);
    Vektor& operator-=(Vektor&);
    Vektor operator*(float);
    Vektor& operator*=(float);
    Vektor operator/(float);
    Vektor& operator/=(float);
    float operator*(const Vektor&);
    float operator*=(const Vektor&);
    Vektor& operator=(const Vektor&);
    Vektor Kreuz(const Vektor&);
    float Laenge(void);
    Vektor drehen(double alpha, double beta=0, double gamma=0);
  };

#endif
