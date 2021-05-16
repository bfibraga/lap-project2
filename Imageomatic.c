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
	Aluno 1: numero, nome
	Aluno 2: numero, nome

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




/*** TYPE Pixel ***/

/* More Pixel functions, in case you need them */

Pixel pixelPosterize(Pixel pixel, int factor) {

	int divisable = (int) pow(2, 8-factor);

	for (;pixel.red % divisable != 0; pixel.red--);
	for (;pixel.green % divisable != 0; pixel.green--);
	for (;pixel.blue % divisable != 0; pixel.blue--);

	return pixel;
}

Pixel pixelDroplet(double distance){
	double value = 0.7* MAX_COLOR + 0.3 * sin(distance / 20) * MAX_COLOR;
	return pixel(value, value, value);
}


/*** TYPE Image ***/

Int2 imageTranspose(Image img, Int2 n, Image res) {
	if (int2IsError(n)) return int2Error;

	Int2 i;
	n = int2(n.y, n.x);
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = img[i.y][i.x];
	}
	return n;
}

Int2 imageInvertXAxis(Image img, Int2 n, Image res) {
	if (int2IsError(n)) return int2Error;

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = img[n.x - i.x][i.y];		
	}

	return n;
}

Int2 imageRotate90_v2(Image img, Int2 n, Image res) {
	if (int2IsError(n)) return int2Error;

	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = img[n.x - i.x][i.y];		
	}
	n = int2(n.y, n.x);
	return n;
}

//---------------------------------

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
	unsigned int r =0, g = 0, b = 0, lookFile = 0;
	sscanf(cor, "%2x%2x%2x", &r, &g, &b); // reads the string to check if it's a hexcode for a colour
	
	if ((r+g+b)/3 > MAX_COLOR && strlen(cor) == 6){ // if the sum of values divided by 3 becomes greater than MAX_COLOR
		r = 0;					// then it can't be interpreted as a hexcode so we put black by default
		g = 0;					// and we have to look through the cor.txt file
		b = 0;
		lookFile = 1;
	}
	
	Pixel p;
	if(lookFile == 1){
	FILE *f;
	if ((f = fopen(colorsFileName, "r")) == NULL) return int2Error;

	String s;
	int found = 0;
	while ((fgets(s, 255, f) != NULL) && found == 0) { // each line will be "hexcode nameColour"
		String colour;
		sscanf(s, "%2x2x2x %s", &r, &g, &b, &colour); // we select the values to their respective variables
		if (strncmp(colour, cor, strlen(cor)) == 0)  // checks if color is a name of a color
			found = 1
		}
	
	fclose(f);
	}
	p = pixel(r,g,b);
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
	Int2 center = int2Half(n); // calculate the center of the image
	double distance; 
	Int2 i;
	for(i.x = 0; i.x < n.x; i.x++)
	for(i.y = 0; i.y < n.y; i.y++){
		distance = int2Distance(i, n); // calculate the distance between the pixel to the center
		res[i.x][i.y] = pixelDroplet(distance); // we put the correspondent pixel according to a provided formula
	}
	return n;
}

Int2 imageMask(Image img1, Int2 n1, Image img2, Int2 n2, Image res) // pre: int2Equals(n1, n2)
{
	if (int2IsError(n1) || int2IsError(n2)) return int2Error;
	Int2 i;
	Pixel p1, p2;
	for(i.x = 0; i.x < n1.x; i.x++)
	for(i.y = 0; i.y < n1.y; i.y++){
		p1 = img1[i.x][i.y];
		p2 = img2[i.x][i.y];
		res[i.x][i.y] = pixel(p1.red*(p2.red/MAX_COLOR), p1.green*(p2.green/MAX_COLOR), p1.blue*(p2.blue/MAX_COLOR);
	}
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
	return int2Error;
}

Int2 imageRotation90(Image img, Int2 n, Image res)
{
	Image temp;
	n = imageTranspose(img, n, temp);
	
	return imageInvertXAxis(temp, n , res);
}


Int2 imagePosterize(Image img, Int2 n, int factor, Image res)
{
	if (int2IsError(n) || factor < 0 || factor > 8) return int2Error;

	if (factor == 8){
		imageCopy(img, n, res);
		return n;
	}

	#define new_MAX_COLOR pow(2,factor);
	// valores: nmr%(pow(2,8-factor))

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
	return int2Error;
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

