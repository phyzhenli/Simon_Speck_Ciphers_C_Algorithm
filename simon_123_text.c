#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
#include<windows.h>
#define plain_file_open_error -1
#define cipher_file_open_error -2
#define ok 1
#define unsigned short short
typedef char ElemType;

#define rshift_two(x)   (((x >> 2)&0x3fff) | (((x) & 0x3) << 14))
#define rshift_three(x) (((x >> 3)&0x1fff) | (((x) & 0x7) << (13)))
#define rshift_one(x)   (((x >> 1)&0x7fff) | (((x) & 0x1) << 15))
#define lshift_one(x)   ((x<<1)|((x>>15)&0x0001))
#define lshift_eight(x) ((x<<8)|((x>>8)&0x00ff))
#define lshift_two(x)   ((x<<2)|((x>>14)&0x0003))

void printf16_x(short a)
{
	printf("%04x", (a & 0x0000ffff));
}


int simon_encryptblock(ElemType plainblock[4], ElemType cipherblock[4]) 
{
	short k[32];
	k[0] = 0x1918;
	k[1] = 0x1110;
	k[2] = 0x0908;
	k[3] = 0x0100;
	uint64_t z = 0b0001100111000011010100100010111110110011100001101010010001011111;
	short c = 0xfffc;

	short tmp;
	for (int i = 4; i < 32; i++)
	{
		tmp = rshift_three(k[i - 1]);
		tmp = tmp ^ k[i - 3];
		tmp ^= rshift_one(tmp);
		k[i] = k[i - 4] ^ tmp ^ (c ^ ((z >> ((i - 4) % 62)) & 1));
		//printf16_x(k[i]);  //output key
	}

	short x = 0, y = 0;
	short temp;
	x = ((x^plainblock[0]) << 8) ^ (plainblock[1] & 0x00ff);
	y = ((y^plainblock[2]) << 8) ^ (plainblock[3] & 0x00ff);

	for (int i = 0; i < 32; i++)
	{
		temp = (lshift_one(x) & lshift_eight(x)) ^ y ^ lshift_two(x);
		y = x;
		x = temp ^ k[i];
	};

	short temp2;
	temp2 = x;
	x = y;
	y = temp2;

	printf("x = ");
	printf16_x(x);
	printf("\n");
	printf("y = ");
	printf16_x(y);
	printf("\n");

	cipherblock[0] = (x & 0xff00) >> 8;
	cipherblock[1] = x & 0x00ff;
	cipherblock[2] = (y & 0xff00) >> 8;
	cipherblock[3] = y & 0x00ff;

	return 0;
}


int simon_encrypt(const char *plainfile, const char *cipherfile) {
	FILE *plain, *cipher;
	int count, iii;
	ElemType plainblock[4], cipherblock[4];

	if ((plain = fopen(plainfile, "rb")) == NULL) {
		return plain_file_open_error;
	}
	if ((cipher = fopen(cipherfile, "wb")) == NULL) {
		return cipher_file_open_error;
	}
	while (!feof(plain)) {
		if ((count = fread(&plainblock, sizeof(char), 4, plain)) == 4) {
			simon_encryptblock(plainblock, cipherblock);
			fwrite(cipherblock, sizeof(char), 4, cipher);
		}
	}
	if (count) {
		memset(plainblock + count, '\0', 3 - count);
		plainblock[3] = 4 - count;
		simon_encryptblock(plainblock, cipherblock);
		fwrite(cipherblock, sizeof(char), 4, cipher);
	}
	fclose(plain);
	fclose(cipher);
	return ok;
}

int simon_decryptblock(ElemType cipherblock[4], ElemType plainblock[4])
{

	short k[32];
	k[0] = 0x1918;
	k[1] = 0x1110;
	k[2] = 0x0908;
	k[3] = 0x0100;
	uint64_t z = 0b0001100111000011010100100010111110110011100001101010010001011111;
	short c = 0xfffc;

	short tmp;
	for (int i = 4; i < 32; i++)
	{
		tmp = rshift_three(k[i - 1]);
		tmp = tmp ^ k[i - 3];
		tmp ^= rshift_one(tmp);
		k[i] = k[i - 4] ^ tmp ^ (c ^ ((z >> ((i - 4) % 62)) & 1));

	}
	short x = 0, y = 0;
	short temp;

	x = ((x^cipherblock[0]) << 8) ^ (cipherblock[1] & 0x00ff);
	y = ((y^cipherblock[2]) << 8) ^ (cipherblock[3] & 0x00ff);

	for (int i = 0; i < 32; i++)
	{
		// Shift, AND , XOR ops
		temp = (lshift_one(x) & lshift_eight(x)) ^ y ^ lshift_two(x);

		// Feistel Cross
		y = x;

		// XOR with Round Key
		x = temp ^ k[31 - i];
	}
	short temp2;
	temp2 = x;
	x = y;
	y = temp2;

	printf("x = ");
	printf16_x(x);
	printf("\n");
	printf("y = ");
	printf16_x(y);
	printf("\n");

	plainblock[0] = (x & 0xff00) >> 8;
	plainblock[1] = x & 0x00ff;
	plainblock[2] = (y & 0xff00) >> 8;
	plainblock[3] = y & 0x00ff;

	return 0;
}

int simon_decrypt(const char *cipherfile, const char *plainfile) {
	FILE *plain, *cipher;
	int count=0, times = 0;
	long filelen;
	ElemType plainblock[4], cipherblock[4];
	if ((cipher = fopen(cipherfile, "rb")) == NULL) {
		return cipher_file_open_error;
	}
	if ((plain = fopen(plainfile, "wb")) == NULL) {
		return plain_file_open_error;
	}
	fseek(cipher, 0, SEEK_END);
	filelen = ftell(cipher);
	rewind(cipher);
	while (1) {
		fread(cipherblock, sizeof(char), 4, cipher);
		simon_decryptblock(cipherblock, plainblock);
		times += 4;
		if (times < filelen) {
			fwrite(plainblock, sizeof(char), 4, plain);
		}
		else {
			break;
		}
	}
	if (plainblock[3] < 4) {
		for (count = 4 - plainblock[3]; count < 4; count++) {
			if (plainblock[count] != '\0') {
				break;
			}
		}
	}
	if (count == 3) {
		fwrite(plainblock, sizeof(char), 4 - plainblock[3], plain);
	}
	else {
		fwrite(plainblock, sizeof(char), 4, plain);
	}
	fclose(plain);
	fclose(cipher);
	return ok;
}
int main()
{
	clock_t a, b;
	a = clock();
	simon_encrypt("1.txt", "2.txt");
	b = clock();
	printf("the total time for encryption is %dms\n", b - a);
	system("pause");

	a = clock();
	simon_decrypt("2.txt", "3.txt");
	b = clock();
	printf("the total time for decryption is %dms\n", b - a);
	return 0;
}
