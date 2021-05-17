/*
	Linguagens e Ambientes de Programação - Projeto de 2020/2021

	Imageomatic module body

largura maxima = 100 colunas
tab = 4 espaços
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789

	Este ficheiro constitui apenas um ponto de partida para o
	seu trabalho. Todo este ficheiro pode e deve ser alterado
	à vontade, a começar por este comentário.


 IDENTIFICAÇÃO DOS AUTORES -
	Aluno 1: 57747, Bruno Braga
	Aluno 2: 57833, Bruno Cabrita

Comentarios:

?????????????????????????
?????????????????????????
?????????????????????????
?????????????????????????
?????????????????????????
?????????????????????????

*/

#include "Imageomatic.h"



/*** TYPE Int2 ***/

/* More Int2 functions, in case you need them */

Int2 int2Subtract(Int2 a, Int2 b) {
	return int2(a.x - b.x, a.y - b.y);
}

/*** TYPE Pixel ***/

/* More Pixel functions, in case you need them */

/*Returns a pixel posterized by given factor*/
Pixel pixelPosterize(Pixel pixel, int factor) {

	int mask = 0xff << (8 - factor);
	pixel.red = pixel.red & mask;
	pixel.green = pixel.green  & mask;
	pixel.blue = pixel.blue & mask;

	return pixel;
}

void sum_color(double sum_color[], Pixel add) {
	sum_color[0] += add.red;
	sum_color[1] += add.green;
	sum_color[2] += add.blue;
}

/*** TYPE Image ***/

void initialization(void)
{
	// This function is automatically called when the interpreter starts.
	// If you need to perform some initialization, this is the place
	// to write the initialization code.
}


Int2 imageCopy(Image img, Int2 n, Image res)
{
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = img[i.x][i.y];
	}
	return n;
}

Int2 imagePaint(String cor, Int2 n, Image res)
{	
	if (int2IsError(n)) return int2Error;

	FILE *f;
	if ((f = fopen(colorsFileName, "r")) == NULL) return int2Error;

	Pixel p;
	String s;
	while (fgets(s, 255, f) != NULL) {
		if (strncmp(&s[7], cor, strlen(cor)) == 0) { // checks if color is a name of a color
			unsigned int dec_color;
			sscanf(s, "%x %s", &dec_color, s);
			Byte b[3] = {
				(dec_color&0xff0000)>>16,
				(dec_color&0x00ff00)>>8,
				(dec_color&0x0000ff)
			};
			for (int i = 0 ; i < 3 ; i++) printf("%x\n", b[i]);

			p = pixel(b[0],b[1],b[2]);
			break;
		}
	}

	fclose(f);

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = p;
	}

	return n;
}

Int2 imageNegative(Image img, Int2 n, Image res)
{
	if (int2IsError(n)) return int2Error;

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		Pixel neg = pixel(MAX_COLOR - img[i.x][i.y].red, MAX_COLOR - img[i.x][i.y].green, MAX_COLOR - img[i.x][i.y].blue);
		res[i.x][i.y] = neg;
	}

	return n;
}

Int2 imageDroplet(Int2 n, Image res)
{
	if (int2IsError(n)) return int2Error;

	Int2 i;
	Int2 center = int2Half(n);
	double dist;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		dist = int2Distance(i,center);
		res[i.x][i.y] = pixelGray(0.7* MAX_COLOR + 0.3 * sin(dist / 20) * MAX_COLOR);
	}

	return n;
}



Int2 imageMask(Image img1, Int2 n1, Image img2, Int2 n2, Image res) // pre: int2Equals(n1, n2)
{
	if (int2IsError(n1) || int2IsError(n2)) return int2Error;
	
	Int2 i;
	for(i.y = 0; i.y < n1.y; i.y++)
	for(i.x = 0; i.x < n1.x; i.x++) {
		Pixel current = img1[i.x][i.y];
		float mask[3] = {(float)img2[i.x][i.y].red/MAX_COLOR, 
						(float)img2[i.x][i.y].green/MAX_COLOR, 
						(float)img2[i.x][i.y].blue/MAX_COLOR };
		res[i.x][i.y] = pixel(current.red*mask[0], current.green*mask[1], current.blue*mask[2]);
	}

	return n1;
}

Int2 imageGrayscale(Image img, Int2 n, Image res)
{	
	if (int2IsError(n)) return int2Error;

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		Pixel grey = pixelGray(pixelGrayAverage(img[i.x][i.y]));
		res[i.x][i.y] = grey;
	}

	return n;
}

Int2 imageBlur(Image img, Int2 n, int nivel, Image res)
{
	if (int2IsError(n) || nivel < 0) return int2Error;

	if (nivel == 0) {
		imageCopy(img, n, res);
		return n;
	}

	#define ZERO2 int2(0,0)

	Int2 i, j;
	 
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		double sum[3] = {0,0,0}; // R=0 ; G=0 ; B=0
		double count = 0;
		for (j.x = i.x-nivel ; j.x <= i.x+nivel ; j.x++)
		for (j.y = i.y-nivel ; j.y <= i.y+nivel ; j.y++) {
			if (int2Between(ZERO2, j, n)) {
				sum_color(sum, img[j.x][j.y]);
				count++;
			}
		}
		res[i.x][i.y] = pixel(sum[0]/count, sum[1]/count, sum[2]/count);
	}

	return n;
}

Int2 imageRotation90(Image img, Int2 n, Image res) {
	if (int2IsError(n)) return int2Error;

	Int2 i;
	Int2 j = int2(n.y, n.x);
	for(i.y = 0; i.y < j.y; i.y++)
	for(i.x = 0; i.x < j.x; i.x++) {
		res[i.x][i.y] = img[n.y-i.y][n.x-i.x];		
	}

	return j;
}


Int2 imagePosterize(Image img, Int2 n, int factor, Image res)
{
	if (int2IsError(n) || factor < 0 || factor > 8) return int2Error;

	if (factor == 8){
		imageCopy(img, n, res);
		return n;
	}

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		Pixel temp = img[i.x][i.y];
		Pixel resp = pixelPosterize(temp, factor);
		res[i.x][i.y] = resp;
	}

	return n;
}

Int2 imageHalf(Image img, Int2 n, Image res)
{
	if (int2IsError(n)) return int2Error;

	Int2 i;
	Int2 j = int2(n.x/2, n.y/2);
	for(i.y = 0; i.y < j.y; i.y++)
	for(i.x = 0; i.x < j.x; i.x++) {
		Int2 old = int2(i.x*2, i.y*2);
		res[i.x][i.y] = img[old.x][old.y];
	}
	n = j;

	return n;
}

Int2 imageFunctionPlotting(DoubleFun fun, int scale, Int2 n, Image res)
{
	if (int2IsError(n)) return int2Error;

	

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		if (i.x == n.x/2) {
			res[i.x][i.y] = black;
		} else if (i.y == n.y/2) {
			res[i.x][i.y] = black;
		} else {
			double resX = (i.x-n.x/2)/n.x;
			double resY = (i.y-n.x/2)/n.y;
			double result = -fun(resX)*scale;

		if (resY == (int)(result)){
			res[i.x][i.y] = black;
		} else res[i.x][i.y] = white;
		}
	}

	return n;
}

Int2 imageOrderedDithering(Image img, Int2 n, Image res)
{
	#define INDEX_SIDE  8
	Byte indexMatrix[INDEX_SIDE][INDEX_SIDE] = {
					{ 0, 32,  8, 40,  2, 34, 10, 42}, 
					{48, 16, 56, 24, 50, 18, 58, 26},
					{12, 44,  4, 36, 14, 46,  6, 28},
					{60, 28, 52, 20, 62, 30, 54, 22},
					{ 3, 35, 11, 43,  1, 33,  9, 41},
					{51, 19, 59, 27, 49, 17, 57, 25},
					{15, 47,  7, 39, 13, 45,  5, 37},
					{63, 31, 55, 23, 61, 29, 53, 21}
			};
	return int2Error;
}

Int2 imageSteganography(Image img, Int2 n, String s, Image res)
{
	return int2Error;
}


void imageTests(void)
{
	static Image img, img2, res;
	Int2 n;

	// Q
	n = imageLoad("img/frutos.png", img);
	n = imageGrayscale(img, n, res);
	imageStore("img/cinzento.png", res, n);

	// N
	n = imageLoad("img/frutos.png", img);
	n = imageNegative(img, n, res);
	imageStore("img/negativo.png", res, n);	

	// H
	n = imageLoad("img/frutos.png", img);
	n = imageHalf(img, n, res);
	imageStore("img/metade.png", res, n);

	// P
	n = int2(512, 512);
	n = imagePaint("green", n, res);
	imageStore("img/pintar.png", res, n);

	// R
	n = imageLoad("img/frutos.png", img);
	n = imageRotation90(img, n, res);
	imageStore("img/rotacao_90.png", res, n);

	// O
	n = imageLoad("img/frutos.png", img);
	n = imagePosterize(img, n, 3, res);
	imageStore("img/poster.png", res, n);

	// G
	n = int2(512, 512);
	n = imageDroplet(n, res);
	imageStore("img/goticula.png", res, n);

	// D
	n = imageLoad("img/frutos.png", img);
	n = imageBlur(img, n, 5, res);
	imageStore("img/desfocado.png", res, n);

	// M
	n = imageLoad("img/frutos.png", img);
	n = imageDroplet(n, img2);
	n = imageMask(img, n, img2, n, res);
	imageStore("img/mascarar.png", res, n);

	// F
	n = int2(512, 512);
	n = imageFunctionPlotting(sin, 50, n, res);
	imageStore("img/funcao.png", res, n);

	// T
	n = imageLoad("img/frutos.png", img);
	n = imageOrderedDithering(img, n, res);
	imageStore("img/matizacao.png", res, n);

	// E
	n = imageLoad("img/frutos.png", img);
	n = imageSteganography(img, n, "atacamos ao amanhecer", res);
	imageStore("img/esteganografia.png", res, n);
}

