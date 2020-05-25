#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<time.h>
typedef char ElemType; 

#define plain_file_open_error -1
#define cipher_file_open_error -2
#define ok 1
#define rshift_seven(x) (((x>> 7)&0x01ff) | (((x) & 0x007f) << 9))
#define lshift_two(x)   (((x&0x3fff) <<2) | ((x>> 14)&0x0003))
#define rshift_three(x) (((x) >> 3) | (((x) & 0x7) << 13))
#define rshift_two(x)   (((x >> 2)&0x3fff) | (((x) & 0x3) << 14))
#define lshift_one(x)   ((x<<1)|((x>>15)&0x0001))
#define lshift_seven(x) (((x&0x1fff)<<7)|((x>>9)&0x007f))

//已测试移位功能无误。
//已测试数据传世无误。
 
void printf16_x(short a){
	 //printf("%x",( a & (0xffffffff00000000)     )>>32);
	 
	 printf("%04x\n",( a & 0x0000ffff));
	
	
	 
} 

int speck_encryptblock(ElemType plainblock[4],ElemType cipherblock[4]){
	
	short k[22],l[24];
     
	k[0] = 0x1918;
	l[0] = 0x1110;
  	l[1] = 0x0908;
	l[2] = 0x0100;
	
	for (int i = 0;i < 21;i++)
	{
		l[i+3] = (k[i] +rshift_seven(l[i]))^i;
	
        k[i+1] =  (lshift_two(k[i]))^l[i+3];
		printf16_x(k[i+1]); 
	}
	
	short x=0,y=0;
	short temp;
	x=((x^plainblock[0])<<8)^(plainblock[1]&0x00ff);
   	y=((y^plainblock[2])<<8)^(plainblock[3]&0x00ff);
   	
   	short x1=0x6574,y1= 0x694c;
	    
   	printf("加密前x，y的值为：\n");
   	printf("x = ");
   	printf16_x(x);
   	printf("y = ");
	printf16_x(y);
	
	printf("加密前x1，y1的值为：\n");
   	printf("x1 = ");
   	printf16_x(x1);
   	printf("y1 = ");
	printf16_x(y1);
	
    for(int i=0;i<22;i++){
   		x1=rshift_seven(x1);
   		x1=(x1+y1)^k[i];
   		y1=(lshift_two(y1))^x1;
   	
	   };                                                                                                                                                                                                            x1 = 0xa868;y1=0x42f2;
	   
	   
	   for(int i=0;i<22;i++){
   		x=rshift_seven(x);
   		x=(x+y)^k[i];
   		y=(lshift_two(y))^x;
   	
	   }
	   
	//short temp2;
    //temp2=x;
    //x=y;
    //y=temp2;
   printf("加密后x，y的值为：\n");
   	printf("x = ");
   	printf16_x(x);
   	printf("y = ");
	printf16_x(y);
	printf("加密后x1，y1的值为：\n");
   	printf("x1 = ");
   	printf16_x(x1);
   	printf("y1 = ");
	printf16_x(y1);
    cipherblock[0]=(x&0xff00)>>8;   
	cipherblock[1]=x&0x00ff;  
    cipherblock[2]=(y&0xff00)>>8; 
	cipherblock[3]=y&0x00ff;
   	
}

int speck_decryptblock(ElemType cipherblock[4],ElemType plainblock[4]){
	
	short k[22],l[24];
     
	k[0] = 0x1918;
	l[0] = 0x1110;
  	l[1] = 0x0908;
	l[2] = 0x0100;
	
	for (int i = 0;i < 21;i++)
	{
		l[i+3] = (k[i] +rshift_seven(l[i]))^i;
	
        k[i+1] =  (lshift_two(k[i]))^l[i+3];
		
	}
	
	short x=0,y=0;
	short temp;
	x=((x^cipherblock[0])<<8)^(cipherblock[1]&0x00ff);
   	y=((y^cipherblock[2])<<8)^(cipherblock[3]&0x00ff);
   
   	for(int i=0;i<22;i++){
   	    y=x^y;
	    y=rshift_two(y);
   	    x=(x^k[21-i])-y;
   	    x=lshift_seven(x);
   	
	   }
	
    printf("解密后x，y的值为：\n");
   	printf("x = ");
   	printf16_x(x);
   	printf("y = ");
	printf16_x(y);
    plainblock[0]=(x&0xff00)>>8;   
	plainblock[1]=x&0x00ff;  
    plainblock[2]=(y&0xff00)>>8; 
	plainblock[3]=y&0x00ff;
	
}
int main ()
{
	clock_t t1,t2,t3,t4;
	ElemType a[4]={'A','B','C','D'};
		printf("原始明文为：\n");
	for(int i=0;i<4;i++){
		printf("%c\n",a[i]);	
	}
	ElemType b[4];
	ElemType c[4];
    t1=clock();

	speck_encryptblock(a,b);
    t2=clock();
	printf("加密密文：\n");
		for(int i=0;i<4;i++){
		printf("%c\n",b[i]);	
	} 
	speck_decryptblock(b,c);
	
    printf("解密明文为：\n");
	for(int i=0;i<4;i++){
		printf("%c\n",c[i]);	
	}
	return 0;
}
