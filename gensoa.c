#include <stdio.h>
#include <stdlib.h>

static int cmpint(const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}
   
int main(int argc, char **argv) {
	const int device_data[] = {
		0x50775d8, 0x4d3f640, 0x46cf710, 0x3d27848, 0xcdfe600, 0xb43e940, 0x80befc0, 0x6f94740, 0x66ff300, 0x5a1f4a0, 0x337f980, 0x2d0fa50, -1 };

	const int user_data[] = {
		78750000, 50000000, 154000000, 148500000, 170000000, 202500000, -1 };
	const int max = 100;
	int* data = malloc(sizeof(int) * max);
	int count = 0, i, j;

	for (i = 0 ; count < max && device_data[i] != -1 ; i++) {
		data[count++] = device_data[i];
	}
	for (i = 0 ; count < max && user_data[i] != -1 ; i++) {
		data[count++] = user_data[i];
	}

	qsort(data, count, sizeof(int), cmpint);

	char prefix[max+1];
	prefix[0]=0;
	for (i = 0 ; i < count ; i++) {
		printf("%sdup %x = if\n", prefix, data[i]);
		printf("%s ics%d\n", prefix, (int)data[i]/1000000);
		if (i < (count - 1))
			printf("%selse\n", prefix);
		prefix[i]=' ';
		prefix[i+1]=0;
	}
	for (i = count ; i > 0 ; i --) {
		prefix[i-1]=0;
		printf("%sthen\n", prefix);
	}
}
