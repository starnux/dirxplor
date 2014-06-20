/*
  KtrAscii from Ktr.h 
  Ktr.h Copyright (c) 2003 Ktr & Dada66 
  www.ktr.fr.fm 
*/ 


void DrawSprite(short x,short y,char* sprite,unsigned char mask,unsigned char height, short SEG1)
{ 
   asm{ 
    mov cx, x; 
    and cl, 0x7;
    mov si, 0x3FF; 
    sub si, y; 
    and x, 0xFFF8; 
    shl x, 0x3; 
    sub si, x; 
    mov es, SEG1; 
    mov dh, mask; 
    xor dl, dl; 
    shr dx, cl; 
    xor dx, 0xFFFF; 
    mov di, sprite; 
    mov al, height;} 
boucle: 
asm{  mov bl, ds:[di]; 
    shl bx, 0x8; 
    inc di; 
    dec al; 
    shr bx, cl; 
    and es:[si], dh; 
    or es:[si], bh; 
    } 
    if(_SI > 63) 
    { 
       asm push si; 
       asm sub si, 0x40; 
       asm and es:[si], dl; 
       asm or es:[si], bl; 
       asm pop si; 
    } 
    asm dec si; 
    asm cmp al, 0; 
    asm jne boucle; 
} 

unsigned char ktrascii[][5] = 
{  0,  0,  0,  0,  0, 
  64, 64, 64,  0, 64, 
160,160,  0,  0,  0, 
160,224,160,224,160, 
224,192,224, 96,224, 
128, 32, 64,128, 32, 
  64,160, 96,160, 96, 
192,192,  0,  0,  0, 
  32, 64, 64, 64, 32, 
128, 64, 64, 64,128, 
224,224,224,224,224, 
   0, 64,224, 64,  0, 
   0,  0,  0, 64,128, 
   0,  0,224,  0,  0, 
   0,  0,  0,  0,128, 
  32, 64, 64, 64,128, 
224,160,160,160,224, 
  64,192, 64, 64,224, 
224, 32,224,128,224, 
224, 32,224, 32,224, 
160,160,224, 32, 32, 
224,128,224, 32,224, 
224,128,224,160,224, 
224,160, 32, 32, 32, 
224,160,224,160,224, 
224,160,224, 32,224, 
   0, 64,  0, 64,  0, 
   0, 64,  0, 64,128, 
  32, 96,224, 96, 32, 
   0,224,  0,224,  0, 
128,192,224,192,128, 
  64,160, 32, 64, 64, 
224, 32,224,160,224, 
  64,160,160,224,160, 
192,160,192,160,192, 
  96,128,128,128, 96, 
192,160,160,160, 192, 
224,128,192,128,224, 
224,128,192,128,128, 
  96,128,160,160, 96, 
160,160,224,160,160, 
224, 64, 64, 64,224, 
224, 32, 32,160,224, 
160,192,128,192,160, 
128,128,128,128,224, 
160,224,160,160,160, 
160,224,224,160,160, 
  64,160,160,160, 64, 
192,160,192,128,128, 
  64,160,160, 64, 32, 
192,160,192,192,160, 
224,128,224, 32,224, 
224, 64, 64, 64, 64, 
160,160,160,160,224, 
160,160,160,160, 64, 
160,160,160,224,160, 
160,160, 64,160,160, 
160,160, 64, 64,128, 
224, 32, 64,128,224, 
192,128,128,128,192, 
128, 64, 64, 64, 32, 
  96, 32, 32, 32, 96, 
  64,160,  0,  0,  0, 
   0,  0,  0,  0,224, 
128, 64,  0,  0,  0, 
   0, 64,160,  0,  0, 
   0,  0,160, 64,  0}; 

/*affiche une police (KTR)*/
void putchar3(unsigned char x, unsigned char y, unsigned char lettre)
{
   lettre -= 32;
   DrawSprite(x, y, ktrascii[lettre], 224, 5,0x1a60);
}

void print3(unsigned char x, unsigned char y, unsigned char string[])
{int i;
for(i = 0 ; string[i] != 0 ; i++) putchar3(x + 4*i+10, y, string[i]);
}

void printline(int line,char * string){
print3(0,line*6+8,string);}

