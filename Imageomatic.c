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

/*Returns a pixel posterized by given factor as int
	pre: pixel != NULL && factor >= 0*/
Pixel pixelPosterize(Pixel pixel, int factor) {

	int mask = 0xff << (8 - factor);
	pixel.red = pixel.red & mask;
	pixel.green = pixel.green & mask;
	pixel.blue = pixel.blue & mask;

	return pixel;
}

/*Returns a blured pixel by given imagem, its resolution, current pixel position and a blur level as int
	pre: img != NULL && !int2IsError(n) && !int2IsError(current) && nivel >= 0*/
Pixel pixelBlured(Image img, Int2 n, Int2 current, int nivel){

	/*Summation of the 3 color components by given pixel
		pre: add != NULL
		pro: sum_color[0] + sum_color[1] + sum_color[2] <= 3*MAX_COLOR*/
	void sum_color(double sum_color[], Pixel add) {
		sum_color[0] += add.red;
		sum_color[1] += add.green;
		sum_color[2] += add.blue;
	}

	double sum_rgb[3] = {0,0,0}; // Index: R=0 ; G=1 ; B=2
	double count = 0;
	Int2 j;
	for (j.x = current.x-nivel ; j.x <= current.x+nivel ; j.x++)
	for (j.y = current.y-nivel ; j.y <= current.y+nivel ; j.y++) {
		if (int2Between(int2Zero, j, n)) {
			sum_color(sum_rgb, img[j.x][j.y]);
			count++;
		}
	}
	return pixel(sum_rgb[0]/count, sum_rgb[1]/count, sum_rgb[2]/count);
}

/*** TYPE String ***/

/*Converts string s to 6 bits ASCII representation
	pre: s != NULL*/
void convertStringToEncodedString(String s, String res){
	int length = strlen(s);
	int i;
	for(i = 0; i < length; i++){
		char character = s[i];
		if ( character >= 'a' && character <= 'z')
			character -= 32;
		else if ( (character > '?' && character > '_') || character == '@')
			character = '?';
		res[i] = character;
	}
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
	unsigned int found = 0;
	unsigned int b[3] = {0,0,0};
	String s, listaCor;
	Pixel p;
	FILE *f;
	if ((f = fopen(colorsFileName, "r")) == NULL) return int2Error;
	while (fgets(s, 255, f) != NULL && found == 0) {
		//unsigned int dec_color;
		sscanf(s, "%2x%2x%2x %s", &b[0], &b[1], &b[2], listaCor);
		if (strcmp(listaCor, cor) == 0) { // checks if color is a name of a color
			//printf("%x", dec_color);
		//b[0] = (dec_color&0xff0000)>>16;
		//	b[1] = (dec_color&0x00ff00)>>8;
		//	b[2] = dec_color&0x0000ff;
			found = 1;
		}
	}

	fclose(f);
	
	if ((found == 0) ){
		if (sscanf(cor, "%2x%2x%2x%s", &b[0], &b[1], &b[2], s) != 3){
			b[0] = 0;
			b[1] = 0;
			b[2] = 0;
		}
	}
		
	p = pixel(b[0],b[1],b[2]);
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
	
	#define PRECISION_TYPE double

	Int2 i;
	for(i.y = 0; i.y < n1.y; i.y++)
	for(i.x = 0; i.x < n1.x; i.x++) {
		Pixel current = img1[i.x][i.y];
		PRECISION_TYPE mask[3] = {img2[i.x][i.y].red/(PRECISION_TYPE)MAX_COLOR, 
						img2[i.x][i.y].green/(PRECISION_TYPE)MAX_COLOR, 
						img2[i.x][i.y].blue/(PRECISION_TYPE)MAX_COLOR };
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

	Int2 i;
	 
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		res[i.x][i.y] = pixelBlured(img, n, i, nivel); 
	}

	return n;
}

Int2 imageRotation90(Image img, Int2 n, Image res) {
	
	if (int2IsError(n)) return int2Error;

	Int2 i, nr = int2(n.y, n.x);
	for(i.y = 0; i.y < nr.y; i.y++)
	for(i.x = 0; i.x < nr.x; i.x++) {
		res[i.x][i.y] = img[i.y][nr.x-(i.x+1)];		
	}

	return nr;
}


Int2 imagePosterize(Image img, Int2 n, int factor, Image res)
{
	if (int2IsError(n) || factor < 0 || factor > 8) return int2Error;

	if (factor == 8) {
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

	Int2 i, nh = int2Half(n);
	for(i.y = 0; i.y < nh.y; i.y++)
	for(i.x = 0; i.x < nh.x; i.x++) {
		res[i.x][i.y] = img[i.x*2][i.y*2];
	}

	return nh;
}

Int2 imageFunctionPlotting(DoubleFun fun, int scale, Int2 n, Image res)
{
	if (int2IsError(n)) return int2Error;

	#define PRECISION_TYPE double

	Int2 i, center = int2Half(n);
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		if (i.x == center.x || i.y == center.y) {
			res[i.x][i.y] = black;
		} else res[i.x][i.y] = white;
		
	}
	
	for(i.x = 0; i.x < n.x; i.x++){
		PRECISION_TYPE resX = (i.x - center.x)/(PRECISION_TYPE)scale;
		PRECISION_TYPE resY = fun(resX)*scale;
		res[i.x][(int) (center.y - resY)] = black;
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

	if (int2IsError(n)) return int2Error;

	#define PRECISION_TYPE double
	
	Int2 i;
	for(i.y = 0; i.y < n.y; i.y++)
	for(i.x = 0; i.x < n.x; i.x++) {
		Int2 index = int2(i.x % INDEX_SIDE, i.y % INDEX_SIDE);
		PRECISION_TYPE average = (PRECISION_TYPE) pixelGrayAverage(img[i.x][i.y])/4;  // [0;63,75]
		res[i.x][i.y] = indexMatrix[index.x][index.y] < average ? white : black;
	}
	return n;
}

char decFromPixel(Pixel pixel){
    char c = 0;
    c = c | ((pixel.red & 0b11) << 4);
    c = c | ((pixel.green & 0b11) << 2);
    c = c | (pixel.blue & 0b11);
    if(c == 0) return '\0';
    else if (c <= 0x1f) return c + 0x40;
    else return c;
}

void decode(Image img, Int2 n, String out){
    Int2 i;
    for(i.y = 0; i.y < n.y; i.y++)
    for(i.x = 0; i.x < n.x; i.x++) {
        char c = decFromPixel(img[i.x][i.y]);
        *out = c;
        if(c == '\0') {
            return;
        }
        out++;
    }
}

Pixel removeRightMostBits(Pixel current, Byte rgb[], char c) {
	rgb[0] = (current.red << 2) | ((c & 0b110000) >> 4);
	rgb[1] = (current.green << 2) | ((c & 0b001100) >> 2);
	rgb[2] = (current.blue << 2) | (c & 0b000011);
	return pixel(rgb[0], rgb[1], rgb[2]);
}

Int2 imageSteganography(Image img, Int2 n, String s, Image res)
{
	if (int2IsError(n)) return int2Error;

	#define P res[i.x][i.y]

	int counter = 0, length = strlen(s);
	char c;
	//Byte newR, newG, newB;
	Byte new_rgb[3];
	Int2 i = int2Zero;
	Pixel p;
	String encoded;
	convertStringToEncodedString(s, encoded);
	imageCopy(img, n, res);

	/*while(counter < length){
		c = encoded[counter++];
		p = res[i.x][i.y];
		newR = (p.red << 2) | ((c & 0b110000) >> 4);
		newG = (p.green << 2) | ((c & 0b001100) >> 2);
		newB = (p.blue << 2) | (c & 0b000011);
		res[i.x][i.y] = pixel(newR, newG, newB);
		i.x++;
		if(i.x == n.x){
			i.x = 0;
			i.y++;
		}
		if(i.y == n.y){
			i.x = n.x-1;
			break;
		}
	}
 
	p = res[i.x][i.y];		
	newR = (p.red << 2);
	newG = (p.green << 2);
	newB = (p.blue << 2);
	res[i.x][i.y] = pixel(newR, newG, newB);*/

	for (i.y = 0 ; i.y < n.y && counter < length ; i.y++)
	for (i.x = 0 ; i.x < n.x-1 && counter < length ; i.x++) {
		c = encoded[counter++];
		p = res[i.x][i.y];
		res[i.x][i.y] = removeRightMostBits(p, new_rgb, c);
	}
	printf("\n%d %d\n", i.x, i.y);
	p = res[i.x+1][i.y];
	new_rgb[0] = (p.red<<2);
	new_rgb[1] = (p.green<<2);
	new_rgb[2] = (p.blue<<2);
	res[i.x+1][i.y] = pixel(new_rgb[0], new_rgb[1], new_rgb[2]);

	return n;
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
	n = imagePaint("lime", n, res);
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

