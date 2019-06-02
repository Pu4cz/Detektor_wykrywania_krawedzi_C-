//Murawski Patryk codeblocks GNU GCC Compiler

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const int maska1[2][2] = { {1, 0}, {0, -1} };
const int maska2[2][2] = { {0, 1}, {-1, 0} };

struct BITMAPFILEHEADER
{
	int bfType;
	int bfSize;
	int bfReserved1;
	int bfReserved2;
	int bfOffBits;
};

struct BITMAPINFOHEADER
{
	int biSize;
	int biWidth;
	int biHeight;
	int biPlanes;
	int biBitCount;
	int biCompression;
	int biSizeImage;
	int biXpelsPerMeter;
	int biYpelsPerMeter;
	int biCrlUses;
	int biCrlImportant;
};

void wypelnij_struktury(fstream &outputf, BITMAPFILEHEADER &bmpfile, BITMAPINFOHEADER &bmpinfo)
{

	outputf.read((char*)(&bmpfile.bfType), 2);
	outputf.read((char*)(&bmpfile.bfSize), 4);
	outputf.read((char*)(&bmpfile.bfReserved1), 2);
	outputf.read((char*)(&bmpfile.bfReserved2), 2);
	outputf.read((char*)(&bmpfile.bfOffBits), 4);
	outputf.read((char*)(&bmpinfo.biSize), 4);
	outputf.read((char*)(&bmpinfo.biWidth), 4);
	outputf.read((char*)(&bmpinfo.biHeight), 4);
	outputf.read((char*)(&bmpinfo.biPlanes), 2);
	outputf.read((char*)(&bmpinfo.biBitCount), 2);
	outputf.read((char*)(&bmpinfo.biCompression), 4);
	outputf.read((char*)(&bmpinfo.biSizeImage), 4);
	outputf.read((char*)(&bmpinfo.biXpelsPerMeter), 4);
	outputf.read((char*)(&bmpinfo.biYpelsPerMeter), 4);
	outputf.read((char*)(&bmpinfo.biCrlUses), 4);
	outputf.read((char*)(&bmpinfo.biCrlImportant), 4);

}

void przepisz_naglowek(fstream &outputf, fstream &inputf, BITMAPFILEHEADER bmpfile) {

	outputf.seekg(0);
	inputf.seekp(0);

	char buf;

	for (int i = 0; i < bmpfile.bfOffBits; i++)
	{
		outputf.get(buf);
		inputf.put(buf);
	}
}

void konwolucja_linii(fstream&inputf, BITMAPINFOHEADER bmpinfo, int zera, char ** linie) {

	char buf1, buf2;
	for (int i = 0; i < 3; i++) { //3 bajty brzegu wiersza
		inputf.put(linie[1][i]);
	}

	for (int i = 3; i < (bmpinfo.biWidth - 1) * 3; i++) {

		buf1 = maska1[0][0] * (int)linie[0][i - 3] + maska1[0][1] * (int)linie[0][i] + maska1[1][0] * (int)linie[1][i - 3] + maska1[1][1] * (int)linie[1][i]; //konwulcja z 1 mask¹

		buf2 = maska2[0][0] * (int)linie[0][i - 3] + maska2[0][1] * (int)linie[0][i] + maska2[1][0] * (int)linie[1][i - 3] + maska2[1][1] * (int)linie[1][i]; //konwulcja z 2 mask¹

		buf1 = (abs((int)buf1) + abs((int)buf2)) / 2; //uœredniona wartoœæ obliczonych ró¿nic
		inputf.put(buf1);
	}
	for (int i = 3; i > 0; i--) { //3 bajty brzegu wiersza
		inputf.put(linie[1][bmpinfo.biWidth * 3 - i]);
	}
	for (int i = 0; i < zera; i++) { //wypelnianie zerami koñcówki wiersza
		inputf.put('\x00');
	}
}

void wykrywanie_krawedzi(fstream &outputf, fstream &intpuf, BITMAPINFOHEADER bmpinfo) {

	int ile_zer = 0;
	int szerokosc_wiersza = bmpinfo.biWidth * 3;
	while (szerokosc_wiersza % 4 != 0) {
		szerokosc_wiersza++;
		ile_zer++;
	}
	char ** linie = new char *[2];
	for (int i = 0; i < 2; i++) {
		linie[i] = new char[szerokosc_wiersza];
	}
	outputf.read((char *)linie[0], sizeof(char)*szerokosc_wiersza);	//wczytanie 2 pocz¹tkowych linii
	outputf.read((char *)linie[1], sizeof(char)*szerokosc_wiersza);

	intpuf.write(linie[0], sizeof(char)*szerokosc_wiersza); //zapis pierwszej linii
	for (int i = 0; i < bmpinfo.biHeight - 2; i++) {
		konwolucja_linii(intpuf, bmpinfo, ile_zer, linie);
		for (int j = 0; j < sizeof(char)*szerokosc_wiersza; j++) { //podmiana wartoœci miêdzy liniami
			linie[0][j] = linie[1][j];
		}
		if (i != (bmpinfo.biHeight - 3))
			outputf.read((char*)linie[1], sizeof(char)*szerokosc_wiersza);		//wczytanie kolejnej linii
	}
	intpuf.write(linie[1], sizeof(char)*szerokosc_wiersza); //zapis ostatniej linii


}

int main() {

	fstream outputf, inputf;
	BITMAPFILEHEADER bmpfile;
	BITMAPINFOHEADER bmpinfo;

	outputf.open("sonar_aktywny3.bmp", ios::in | ios::binary);
	if (outputf.good() == false) {
		cout << "blad otwarcia pliku!";
		return 0;
	}
	inputf.open("wykrywanie_krawedzi.bmp", ios::out | ios::binary);
	if (inputf.good() == false) {
		cout << "blad otwarcia pliku!";
		return 0;
	}

	wypelnij_struktury(outputf, bmpfile, bmpinfo);
	przepisz_naglowek(outputf, inputf, bmpfile);
	wykrywanie_krawedzi(outputf, inputf, bmpinfo);

	return 0;
}
