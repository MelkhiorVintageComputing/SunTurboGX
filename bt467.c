#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	float freq;
	int p2; // 0 or 1 (or 2, but not used in default TGX)
	int p5; // c, d or e (i.e. 8 + 4/5/6)
	int p10;
	int p11;
	int p13;
} dacp;

/* local notation : */

/*      p1 p2 p3 p4 p5 p6 p7 p8 p9 pa pb pc pd */
/* documentation : */
/*      12 11 10  9  8  7  6  5  4  3  2  1  0 */

/* 47.25 0  1  0  a  c  f  f  1  8  2  0  0  5 */

/* p5 is VCO gain from table page E42 : 8 (N[2] set) + 4/5/6*/
/* pd/p13 is R[0-3] */
/* pc/p12 is (unused?) R[4-6] */
/* pb/p11 is A[0-3] */
/* pa/p10 is M[0-3] */
/* p9 is (unused?) M[4-5] / FBKPOL / DBLFREQ */
/* p8 is (unused?) N[0-3] */
/* p7,p6 is (unused?) N2[0-7] */
/* p5 is VCO gain (V[0-2]) from table page E42 : 8 (includes N[2] set) + 4/5/6 */
/* p4 is P[0-2] */
/* p3 is ... */
/* p2 is S[0-1] / AUX_CLK / AUX_N1 */
/* p1 is RESERVED / JAMPLL / DACRST / SELXTAL */


/* this is from the device itself */
const dacp data[] = {
	{ 47.25 , 1, 0xc, 2, 0, 5 },
	{ 54.   , 1, 0xc, 2, 2, 4 },
	{ 64.125, 1, 0xc, 2, 1, 3 },
	{ 74.25 , 1, 0xd, 4, 3, 5 },
	{ 81.00 , 1, 0xd, 5, 0, 6 },
	{ 84.375, 1, 0xd, 3, 1, 3 },
	{ 94.5  , 1, 0xd, 2, 0, 2 },
	{ 108.0 , 1, 0xd, 4, 2, 3 },
	{ 117.0 , 1, 0xd, 3, 2, 2 },
	{ 135.0 , 1, 0xe, 5, 4, 3 },
	{ 189.0 , 0, 0xd, 2, 0, 2 },
	{ 216.0 , 0, 0xd, 4, 2, 3 },

	{ 0.0, -1, -1, -1, -1, -1}
};

void printDacp(const dacp data) {
	const float base = 13.5;
	int warn = data.p11 > data.p10;
	printf("%f (%f [%9.0f: % 8x] %d %d %d %d %d [%x %x %x %x %x] = %f%s)\n", data.freq/base,
	       data.freq,
	       data.freq*1000000.f,
	       (int)lrint(data.freq*1000000.f),
	       data.p2,
	       data.p5,
	       data.p10,
	       data.p11,
	       data.p13,
	       data.p2,
	       data.p5,
	       data.p10,
	       data.p11,
	       data.p13,
#define p2 data.p2
#define p5 data.p5
#define p10 data.p10
#define p11 data.p11
#define p13 data.p13
/* 	       1.f* (p11 == 0 ? ((p10+1)*7) : ((p10+1)*6+p11)), */
/* 	       (2.f* (p11 == 0 ? ((p10+1)*7) : ((p10+1)*6+p11)))/((p13+1.f)*(powf(2.f,p2))), */
	       base * (2.f* (p11 == 0 ? ((p10+1)*7) : ((p10+1)*6+p11)))/((p13+1.f)*(powf(2.f,p2))),
	       warn ? " OUPS THIS IS WRONG" : ""
	       );
	if (!warn)
		printf(": ics%d 0 %x 0 a %x f f 1 %x %x %x %x %x ;\n",
		       (int)floorf(data.freq), p2, p5, 8 | (p10 >> 4), p10 & 0xF, p11, (p13 >> 4), p13 & 0xF);
#undef p2
#undef p5
#undef p10
#undef p11
#undef p13
}

dacp makedacp(const float freq, const int p2, const int p5, const int p10, const int p11, const int p13) {
	dacp data;
	data.freq = freq;
	data.p2 = p2;
	data.p5 = p5;
	data.p10 = p10;
	data.p11 = p11;
	data.p13 = p13;
	return data;
}

inline float dacpRatioFormula(const int p2, const int p10, const int p11, const int p13) {
	return (2.f* (p11 == 0 ? ((p10+1)*7) : ((p10+1)*6+p11)))
		/
		((p13+1.f)*(powf(2.f,p2)));
}

dacp* testValueApprox(const float x, const float error) {
	dacp *data = malloc(sizeof(dacp));
	const float base = 13.5;
	int found = 0;
	int p2, p5, p10, p11, p13;
	int max13;
/* p11 is A */
/* p10 is M */
	data[found] = makedacp( 0.0, -1, -1, -1, -1, -1 );
	for (max13 = 7 ; !found && max13 <= 127 ; max13 += 120) {
	for (p2 = 0 ; p2 <= 2 ; p2 ++) {
		/* beware if p10 > 15, since it should spill in p9... */
		for (p10 = 0; p10 <= 63 ; p10 ++) {
			for (p11 = 0 ; p11 <= p10 && p11 <= 7; p11 ++) {
				/* beware if p13 > 7, since it should spill in p12... should be able to go to 127 */
				for (p13 = 0 ; p13 <= max13 ; p13 ++) {
					float ratio = dacpRatioFormula(p2, p10, p11, p13);
					float val = base * ratio;
					float full = val * powf(2.f,p2);
					int high = full > 270.f;
					if (!high && (fabsf(val-x) <= error)) {
						p5 = full <= 120.f ? 0xc : full <= 200 ? 0xd : 0xe;
						printf("%f (%f) %d %x %d %d %d\n", val, fabsf(val-x), p2, p5, p10, p11, p13);
						data[found] = makedacp( val, p2, p5, p10, p11, p13 );
						found++;
						data = realloc(data, sizeof(dacp)*(1+found));
						data[found] = makedacp( 0.0, -1, -1, -1, -1, -1 );
					}
				}
			}
		}
	}
	}
	if (!found) {
		free(data);
		data = NULL;
	}
	return data;
}

dacp* testValue(const float x) {
	float error = .01f;
	int p2, p10, p11, p13;
	dacp* data = NULL;
	while (!data && error <= 1.001f) {
		printf("Looking for %f +- %f...\n", x, error);
		/* p11 is A */
		/* p10 is M */
		data = testValueApprox(x, error);
		if (!data)
			error *= 10.f;
	}
	return data;
}

void printAllDacp(const dacp *data) {
	int i;
	for (i = 0 ; data[i].freq > 0. ; i++) {
		printDacp(data[i]);
	}
}

void checkValue(const float x) {
	dacp *data_extra = testValue(x);
	printf("For value %f\n", x);
	if (data_extra) {
		printAllDacp(data_extra);
		free(data_extra);
	}
}


int main(int argc, char **argv) {
	int i;
/*
	checkValue(78.75f);
	checkValue(148.5f);
	checkValue(154.f);
	checkValue(138.5f);
	checkValue(50.f);
	checkValue(162.f);
	checkValue(170.f);
*/
	for (i = 1 ; i < argc ; i++) {
		checkValue((float)atof(argv[i]));
	}

	printf("From the TGX+\n");
	printAllDacp(data);



	return 0;
}
