#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


typedef struct {
	long int PixelClock; // Hz
	int HRes, HSyncStart, HSyncEnd, HTotal;
	int VRes, VSyncStart, VSyncEnd, VTotal;
} modeline;

typedef struct {
	long int PixelClock; // Hz
	long int HorizFreq; // Hz
	long int VertFreq; // Hz
	int hfporch;
	int hsync;
	int hbporch;
	int hres;
	int vfporch;
	int vsync;
	int vbporch;
	int vres;
} tgxdata;

typedef struct {
	int EIS;
	int EIE;
	int EPE;
	int HCS;
	int HCE;
	int HSS;
	int HSE;
	int HBS;
	int HBE;
	int HRE;
	int VSS;
	int VSE;
	int VBS;
	int VBE;
/* 	int pll; */
/* 	int diff; */
/* 	int pll; */
/* 	int pfc; */
/* 	int ucl; */	
} ffbdata;

int parseModeline(const char* in, modeline *mode) {
	int HRes, HSyncStart, HSyncEnd, HTotal;
	int VRes, VSyncStart, VSyncEnd, VTotal;
	float pc;
	int res = sscanf(in, "%f %d %d %d %d %d %d %d %d\n",
	       &pc, &HRes, &HSyncStart, &HSyncEnd, &HTotal, &VRes, &VSyncStart, &VSyncEnd, &VTotal);
	if (res != 9)
		return 0;
	mode->HRes = HRes;
        mode->HSyncStart = HSyncStart;
        mode->HSyncEnd = HSyncEnd;
        mode->HTotal = HTotal;
        mode->VRes = VRes;
        mode->VSyncStart = VSyncStart;
        mode->VSyncEnd = VSyncEnd;
        mode->VTotal = VTotal;
	mode->PixelClock = lrint(1000000.f * pc);
	return 1;
}

int parseTGX(const char* in, tgxdata *tgx) {
	int res = sscanf(in, "%ld,%ld,%ld,%d,%d,%d,%d,%d,%d,%d,%d",
			 &tgx->PixelClock,
			 &tgx->HorizFreq,
			 &tgx->VertFreq,
			 &tgx->hfporch,&tgx->hsync,&tgx->hbporch,&tgx->hres,
			 &tgx->vfporch,&tgx->vsync,&tgx->vbporch,&tgx->vres);
	if (res != 11)
		return 0;
	return 1;
}

int modeline2tgx(const modeline *mode, tgxdata* tgx) {
	tgx->PixelClock = mode->PixelClock;
	tgx->HorizFreq = lround((1. * mode->PixelClock) / mode->HTotal);
	tgx->VertFreq = lround((1. * mode->PixelClock) / (mode->HTotal * mode->VTotal));
	tgx->hfporch = mode->HSyncStart - mode->HRes;
	tgx->hsync = mode->HSyncEnd - mode->HSyncStart;
	tgx->hbporch = mode->HTotal - mode->HSyncEnd;
	tgx->hres = mode->HRes;
	tgx->vfporch = mode->VSyncStart - mode->VRes;
	tgx->vsync = mode->VSyncEnd - mode->VSyncStart;
	tgx->vbporch = mode->VTotal - mode->VSyncEnd;
	tgx->vres = mode->VRes;
	return 1;
}

int tgx2modeline(const tgxdata *tgx, modeline *mode) {
	mode->PixelClock = tgx->PixelClock;
	mode->HRes = tgx->hres;
        mode->HSyncStart = tgx->hres + tgx->hfporch;
        mode->HSyncEnd = tgx->hres + tgx->hfporch + tgx->hsync;
        mode->HTotal = tgx->hfporch + tgx->hres + tgx->hsync + tgx->hbporch;
        mode->VRes = tgx->vres;
        mode->VSyncStart = tgx->vres + tgx->vfporch;
        mode->VSyncEnd = tgx->vres + tgx->vfporch + tgx->vsync;
        mode->VTotal = tgx->vfporch + tgx->vres + tgx->vsync + tgx->vbporch;
	return 1;
}

int tgx2ffb(const tgxdata* tgx, ffbdata *ffb) {
	int VBE = tgx->vsync - 1 + tgx->vbporch;
	int VBS = VBE + tgx->vres;
	int VSE = tgx->vsync - 1;
	int VSS = VSE + tgx->vbporch + tgx->vres + tgx->vfporch;

	int x = (tgx->hres > 1280 ? 4 : 2);

	int HBE = (tgx->hsync + tgx->hbporch) / x - 1;
	int HBS = (tgx->hsync + tgx->hbporch + tgx->hres) / x - 1;
	int HSE = (tgx->hsync) / x - 1;
	int HSS = (tgx->hsync + tgx->hbporch + tgx->hres + tgx->hfporch) / x - 1;

	int HRE = HSS - HSE - 1;
	int HCE = HBS - 4;
	int HCS = HBE - 4;

	int EPE = 0;
	int EIE = 0;
	int EIS = 0;

	ffb->EIS = EIS;
	ffb->EIE = EIE;
	ffb->EPE = EPE;
	ffb->HCS = HCS;
	ffb->HCE = HCE;
	ffb->HSS = HSS;
	ffb->HSE = HSE;
	ffb->HBS = HBS;
	ffb->HBE = HBE;
	ffb->HRE = HRE;
	ffb->VSS = VSS;
	ffb->VSE = VSE;
	ffb->VBS = VBS;
	ffb->VBE = VBE;
	return 1;
}

void printFFB(const ffbdata* ffb) {
	printf("%x %x %x %x %x %x %x %x %x %x %x %x %x %x",
	       ffb->EIS,ffb->EIE,ffb->EPE,ffb->HCS,ffb->HCE,ffb->HSS,ffb->HSE,ffb->HBS,ffb->HBE,ffb->HRE,ffb->VSS,ffb->VSE,ffb->VBS,ffb->VBE
	       );
	
}

void printModeline(const modeline *mode) {
	printf("%f %d %d %d %d %d %d %d %d",
	       mode->PixelClock / 1000000.,
	       mode->HRes, mode->HSyncStart, mode->HSyncEnd, mode->HTotal,
	       mode->VRes, mode->VSyncStart, mode->VSyncEnd, mode->VTotal);
}

void printTGX(const tgxdata *tgx) {
	printf("%ld,%ld,%ld,%d,%d,%d,%d,%d,%d,%d,%d,COLOR,0OFFSET",
	       tgx->PixelClock,
	       tgx->HorizFreq,
	       tgx->VertFreq,
	       tgx->hfporch,tgx->hsync,tgx->hbporch,tgx->hres,
	       tgx->vfporch,tgx->vsync,tgx->vbporch,tgx->vres);
}



int main(int argc, char **argv) {
	modeline mode;
	tgxdata tgx;
	ffbdata ffb;
	int res;
	char opt;
/* 	int res = parseModeline(argv[1], &mode); */
/* 	if (res) { */
/* 		printModeline(&mode); */
/* 		printf("\n"); */
/* 		modeline2tgx(&mode, &tgx); */
/* 		printTGX(&tgx); */
/* 		printf("\n"); */
/* 		tgx2modeline(&tgx, &mode); */
/* 		printModeline(&mode); */
/* 		printf("\n"); */
/* 		tgx2ffb(&tgx, &ffb); */
/* 		printFFB(&ffb); */
/* 		printf("\n"); */
		
/* 	} */
	
	while ((opt = getopt(argc, argv, "m:t:")) != -1) {
		switch (opt) {
		case 'm':
			res = parseModeline(optarg, &mode);
			if (res) {
				modeline2tgx(&mode, &tgx);
				printTGX(&tgx);
				printf("\n");
				tgx2ffb(&tgx, &ffb);
				printFFB(&ffb);
				printf("\n");
			} else {
				fprintf(stderr, "Modeline parse error\n");
			}
			break;
		case 't':
			res = parseTGX(optarg, &tgx);
			if (res) {
				tgx2modeline(&tgx, &mode);
				printModeline(&mode);
				printf("\n");
				tgx2ffb(&tgx, &ffb);
				printFFB(&ffb);
				printf("\n");
			} else {
				fprintf(stderr, "TGX parse error\n");
			}
			break;
		default: /* '?' */
			exit(-1);
		}
	}
	
	
	return 0;
}
