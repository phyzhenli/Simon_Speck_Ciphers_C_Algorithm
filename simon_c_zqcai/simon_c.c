#include<stdio.h>
#include<stdint.h>

uint16_t cyclic_left(uint16_t x, unsigned n) { return ((x << n) | (x >> (16 - n))); }
uint16_t cyclic_right(uint16_t x, unsigned n) { return ((x >> n) | (x << (16 - n))); }

void key_generate(uint16_t* k)
{
	uint16_t c = 0xfffc;
	uint64_t z = 0b0001100111000011010100100010111110110011100001101010010001011111;
	uint16_t temp = 0;

	for (int i = 4; i < 32; i++)
	{
		temp = cyclic_right(k[i - 1], 3);
		temp = temp ^ k[i - 3];
		temp = temp ^ cyclic_right(temp, 1);
		k[i] = k[i - 4] ^ temp ^ (c ^ ((z >> (i - 4)) & 1));
	}

}

void simon_encrypt(uint16_t* k, uint8_t* text, uint8_t* ciphertext)
{
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t temp = 0;

	x = ((uint16_t)text[0] << 8) | ((uint16_t)text[1]); // 0x4142
	y = ((uint16_t)text[2] << 8) | ((uint16_t)text[3]); // 0x4344

	for (int i = 0; i < 32; i++)
	{
		temp = cyclic_left(x, 1) & cyclic_left(x, 8);
		temp = temp ^ y ^ cyclic_left(x, 2);
		y = x;
		x = temp ^ k[i];
	}

	ciphertext[0] = (uint8_t)(y >> 8);
	ciphertext[1] = (uint8_t)(y & 0xff);
	ciphertext[2] = (uint8_t)(x >> 8);
	ciphertext[3] = (uint8_t)(x & 0xff);
}

void simon_decrypt(uint16_t* k, uint8_t* ciphertext, uint8_t* plaintext)
{
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t temp = 0;

	x = ((uint16_t)ciphertext[0] << 8) | ((uint16_t)ciphertext[1]); // 0x4142
	y = ((uint16_t)ciphertext[2] << 8) | ((uint16_t)ciphertext[3]); // 0x4344

	for (int i = 31; i >= 0; i--)
	{
		temp = cyclic_left(x, 1) & cyclic_left(x, 8);
		temp = temp ^ y ^ cyclic_left(x, 2);
		y = x;
		x = temp ^ k[i];
	}

	plaintext[0] = (uint8_t)(y >> 8);
	plaintext[1] = (uint8_t)(y & 0xff);
	plaintext[2] = (uint8_t)(x >> 8);
	plaintext[3] = (uint8_t)(x & 0xff);
}

int main()
{
	uint16_t key[32] = { 0x1918, 0x1110, 0x0908, 0x0100 };
	uint8_t text[4] = { 0x78, 0x56, 0x90, 0x43 };
	uint8_t ciphertext[4] = { 0 };
	uint8_t plaintext[4] = { 0 };
	printf("The text is: 0x%02x%02x%02x%02x\n", text[0], text[1], text[2], text[3]);
	key_generate(key);
	simon_encrypt(key, text, ciphertext);
	printf("The ciphertext is: 0x%02x%02x%02x%02x\n", ciphertext[0], ciphertext[1], ciphertext[2], ciphertext[3]);
	simon_decrypt(key, ciphertext, plaintext);
	printf("The plaintext is: 0x%02x%02x%02x%02x\n", plaintext[0], plaintext[1], plaintext[2], plaintext[3]);
	return 0;
}
/*
int main()
{
	uint16_t key[32] = { 0x1918, 0x1110, 0x0908, 0x0100 };
	uint8_t text[4] = { 0 };
	uint8_t ciphertext[4] = { 0 };
	uint8_t plaintext[4] = { 0 };
	
	key_generate(key);
	
	FILE* fid_text;
	FILE* fid_ciphertext;
	FILE* fid_plaintext;
	errno_t err_text;
	errno_t err_ciphertext;
	errno_t err_plaintext;
	
	err_text = fopen_s(&fid_text, "text.txt", "rb");
	err_ciphertext = fopen_s(&fid_ciphertext, "ciphertext.txt", "wb");
	err_plaintext = fopen_s(&fid_plaintext, "plaintext.txt", "wb");
	
	int count = 0;
	count = fread(text, sizeof(uint8_t), 4, fid_text);
	while (count == 4)
	{
		simon_encrypt(key, text, ciphertext);
		fwrite(ciphertext, sizeof(uint8_t), 4, fid_ciphertext);
		simon_decrypt(key, ciphertext, plaintext);
		fwrite(plaintext, sizeof(uint8_t), 4, fid_plaintext);
		count = fread(text, sizeof(uint8_t), 4, fid_text);
	}
	if (count)
	{
		for (int i = count; i < 4; i++) text[i] = count;
		simon_encrypt(key, text, ciphertext);
		fwrite(ciphertext, sizeof(uint8_t), count, fid_ciphertext);
		simon_decrypt(key, ciphertext, plaintext);
		fwrite(plaintext, sizeof(uint8_t), count, fid_plaintext);
	}
	fclose(fid_text);
	fclose(fid_ciphertext);
	fclose(fid_plaintext);

	return 0;
}*/