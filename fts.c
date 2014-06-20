/*
  Functions.h by Superna
  Code made for DirXplor
  By Superna and 2072
  www.na-prod.com
*/
//Functions from Casio2.h v2.2
void setpage(short page){  // Met la page voulue Libthium
	asm{
	mov ax,page
	shl ax,6
	add ax,0x1A20
	mov dl,ah
	shr dl,4
	shl ax,4
	out 0x05,ax
	mov al,dl
	out 0x07,al } }
void clearpage(short page) { // Efface l'ecran Superna
asm {
        mov ax,page
        shl ax,6
        add ax,0x1A20
        mov es,ax
        xor ax,ax
        xor di,di
        mov cx,0x200
        cld
        rep stosw
     }
}

int getpage(void) { //Prens la page courante (Moi)
	asm{ mov ax,0x4E
	     mov es,ax
	     xor si,si
	     mov ax,es:[si]
	     sub ax,0x1a20
             shr ax,6 }
	return _AX; }

void disp_bmp (unsigned char BmpTab[],int page) { // Plus rapide Par Moi, Whyp et Roeoender
asm{ mov ax,page
     shl ax,6
     add ax,0x1a20
     mov es,ax
     #ifdef __cplusplus
     mov si,offset BmpTab
     #else
     mov si,BmpTab
     #endif
     xor di,di
     mov cx,0x200
     cld
     rep movsw } }
#include <conio.h> //mais pas pour longtemps
int waitkey() //Attends une touche pressee et la retourne
	{int v=getch();
	if (v==0) v=getch();
	return v;}
void copypage(int source ,int target){asm{ //Copy la page source a la page target - Superna
	push ds
	mov ax,target
	shl ax,6
	add ax,0x1A20
	mov es,ax
	mov di,0
	mov ax,source
	shl ax,6
	add ax,0x1A20
	mov ds,ax
	mov si,0
	cld
	mov cx,512
	rep movsw
	pop ds};}
void invhLine2(int x1, int x2, int y, int page)//inverse une ligne horizontale par dada66
{
	asm mov cx, page  //numero de page   //4
	asm shl cx, 6     //multiplication par 64 //2
	asm add cx, 0x1A20  //ajout de la page 0  //2
	asm mov es, cx  //on le met dans es    //2
	asm mov cx, x1;
	asm and cl, 0x7;
	asm mov si, 0x3ff;
	asm sub si, y;
	asm and x1, 0xfff8;
	asm shl x1, 0x3;
	asm sub si, x1;
	asm shr x1, 0x3;
	asm mov ah, 0xff;
	asm shr ah, cl;
	asm mov bx, x1;
	asm mov cx, x2;
boucle:	asm add bl, 0x8;
	asm cmp cl, bl;
	asm jb fin;
	asm xor es:[si], ah;
	asm mov ah, 0xff;
	asm sub si, 0x40;
	asm jmp boucle;
fin:    asm mov bl, 0x7f;
	asm and cl, 0x7;
	asm shr bl, cl;
	asm not bl;
	asm and ah, bl;
	asm xor es:[si], ah;
}
void hline2(int x1, int x2, int y,int page , int state) //par dada66
{
	asm mov ax,page
	asm shl ax,6
	asm add ax,0x1A20
	asm mov es, ax;
	asm mov cx, x1;
	asm and cl, 0x7;
	asm mov si, 0x3ff;
	asm sub si, y;
	asm and x1, 0xfff8;
	asm shl x1, 0x3;
	asm sub si, x1;
	asm shr x1, 0x3;
if(state)
{
	asm { 
	mov ah, 0xff
	shr ah, cl
	mov bx, x1
	mov cx, x2 }
boucle:	asm { 
	add bl, 0x8
	cmp cl, bl
	jb fin
	or es:[si], ah
	mov ah, 0xff
	sub si, 0x40
	jmp boucle}
fin:    asm { 
	mov bl, 0x7f
	and cl, 0x7
	shr bl, cl
	not bl
	and ah, bl
	or es:[si], ah }
}
else {
	asm { 
	mov ah, 0xff 
	shr ah, cl
	not ah
	mov bx, x1
	mov cx, x2 }
boucle2:	asm {
	add bl, 0x8
	cmp cl, bl
	jb fin2
	and es:[si], ah
	xor ah, ah
	sub si, 0x40
	jmp boucle2}
fin2:    asm {
	mov bl, 0x7f
	and cl, 0x7
	shr bl, cl
	and es:[si], bl }
}
}
void fillRect(int x1,int y1,int x2,int y2,int page,int state) //Rempli un rectangle Duobab
	{int j;
	for(j=y1;j<=y2;j++)
		hline2(x1, x2, j, page, state);} // dada66

void invRect(int x1,int y1,int x2,int y2,int page) //Inverse un rectangle Moi
	{int j;
	for(j=y1;j<=y2;j++)
		invhLine2(x1, x2, j, page);} // dada66


//Fonctions utiles a DirXplor
void fast_copy(unsigned char *p_src,unsigned char *p_dst,int len) /* Fast copy routine by tonton1664 */
{
	while (len) {
              *p_dst=*p_src;
              p_dst++;p_src++;len--;
 }
}

char equal(const char* s1, const char* s2) {
     int i;
     for(i = 0; s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]; i++);
           return (s1[i] == s2[i]);
     }

void invline(char line){
     invRect(2,line*6+19,125,line*6+25,tpage); }

void invline2(char line){
     invRect(9,line*6+8,121,line*6+5+8,tpage); }

void invline3(char line){
     invRect(2,line*6+8,125,line*6+8+5,tpage); }

void delspace(){
     int i;
     //espace principal
     for(i=8;i<50;i++)
        hline2(9,121,i,tpage,0);
     //Barre de defilement
     for(i=14;i<44;i++)
        hline2(123,126,i,tpage,0);
     }
void newsize(long size){
     unsigned int i;
     for(i=51;i<56;i++)
        hline2(63,119,i,tpage,0);
     if(size==0) sprintf(tmp,"?");
     else sprintf(tmp,"%lu O",size);
     print3(54,51,tmp);
     }

void showpage(){
     copypage(tpage,ipage);
     }
//Grosses Fonctions

int listdrive(char* addr, int k){  //liste les fichiers du repertoire addr
  int i;
  DIR *dir;
  char* type;
  struct dirent *ent;
  char path[13];
  if ((dir = opendir(addr)) == NULL)
     return k;  //si le chemin n'existe pas
  if(addr[0]=='A') outportb(0x59,0xA0);
  while ((ent = readdir(dir)) != NULL){        //on lit en boucle
        if(k>=100) goto endsearch;   //si plus de 100, on arrete->protection
        fast_copy(ent->d_name, list[k].name, sizeof(ent->d_name));   //on copie le nom
        fast_copy(addr, list[k].dir, sizeof(list[k].dir));   //et le debut du chemin
        if(list[k].name[0] != '.')//pour eviter les dossiers '.' et '..' change la condition ca peut refuser les noms qui commencent par '.'
        {
                for(type = list[k].name; type[0] != '\0' && type[0] != '.'; type++);
                if(type[0] == '.')
	                   type++;
                if(equal(type,com) || equal(type,exe))
                           list[k].type='E'; //on definit les types ici Executable (exe,com)
                else if(equal(type,bmp)|| equal(type,csp))
                           list[k].type='I';//images (bmp,csp,csi)
                else if(equal(type,csv))
                           list[k].type='V';  //video (csv)
                else
                           list[k].type='?';    // (et le reste)
                //On verifie la validité de ce fichier
                sprintf(tmp,"%s%s",list[k].dir,list[k].name); //On le passe
                if((i=open(tmp,O_BINARY))==-1) list[k].type='N';
                else close(i);
                k++;
                if(type[0] == '\0')//c'est un dossier
                    {
	                fast_copy(addr, path, sizeof(path));
	                for(i=0;path[i] != '\0'; i++);
	                fast_copy(list[k - 1].name, path + i, sizeof(path) - i);
	                for(i=0;path[i] != '\0'; i++);
	                path[i] = '\\';
	                path[i + 1] = '\0';
	                k--;
	                k = listdrive(path, k);
                }
        }
  }
endsearch:
  closedir(dir);
  return k;
}

void execute(char pathp[]){ //By 2072
    char command[128];          //buffer for the command line
    char *args[64];             //table of arguments
    int i;
    int numberofargs    =0;     //no comment
    int a               =0;
	 int cline_size;             //size of the command line
	 cline_size=strlen(pathp);   //cline_size is set
	 if (cline_size > 128)       //Error if the command line size is more than 128
		  return;
	 //This, below, scan the command line to find space characters
    for (i=0; i <= cline_size; i++) {
	if (pathp[i]!=(char)32) {               //if there is no space
	    if (a==0)                           //if it is the first character of the arg
		args[numberofargs]=&command[i]; //copy the address of the first arg
	    command[i]=pathp[i];                //copy the arg in command
				a++;                                //a is increased till a space is found
	} else if (a > 0 && args[numberofargs][a - 1]!=(char)32) {      //if a space is found after
									//an argument and if the
									//previous character wasn't a
									//space
				command[i]='\0';    //this end the argument
	    numberofargs++;     //a new argument has been found
	    a=0;                //reset the index of character of the argument
		  }
	 }
	 args[numberofargs + 1]=NULL;    //This is for the last argument
	 execv(args[0], args);   //execute the progs with the argument list
	 return;
}

//Voila les fonctions Findfirst et FindNext comme pour les fichiers
int b_findfirst(struct memory_zone * file,unsigned short type){
        char far * start;
        start=tell_mem_zone_seg(type);
        //Debut de la memoire Casio
        file->b_segment=FP_SEG(start);
        file->b_offset=FP_OFF(start);
        //on prends la taille
        if((file->b_size=peek(file->b_segment,file->b_offset))==0)
                        goto nop;
        //On va chercher le type
        file->b_ztype=peekb(file->b_segment,file->b_offset+2);
        //et le nom
        movedata(file->b_segment,file->b_offset+3,_DS,(unsigned)file->b_name,8);
        file->b_name[8]=NULL;
        if(file->b_ztype!=type) goto next;
//On l'a le premier bougre
return 1;
nop:
//ah ben en a pu
return -1;
next:
//Ben la on change de type, on peut toujours continuer
return -2;
}

char b_findnext(struct memory_zone * file){
        int oldtype=file->b_ztype;
        //On est sur le nouveau fichier basic
        file->b_offset+=file->b_size;
        //on prends la taille
        if((file->b_size=peek(file->b_segment,file->b_offset))==0)
                        goto nop;
        //On va chercher le type
        file->b_ztype=peekb(file->b_segment,file->b_offset+2);
        //et le nom
        movedata(file->b_segment,file->b_offset+3,_DS,(unsigned)file->b_name,8);
        file->b_name[8]=NULL;
        if(file->b_ztype!=oldtype) goto next;
//On l'a trouvé le bougre
return 1;
nop:
//Ben en a pu
return -1;
next:
//Ben la on change de type, on peut toujours continuer
return -2;
}

void listxlectron(){
        //On va lister les fichiers Xlectron avec mon Module pour MemZones
        int numitem=1,i,toppos=0,curpos=0,maxdown;
	if(b_findfirst(&xlectron,8)!=1){
     	        sprintf(list[0].name,"%s",nofiles);
     	        list[0].type=0;
     	        goto endlist;}
        sprintf(list[0].name,"%s",xlectron.b_name);
        list[0].type=8;
        while(b_findnext(&xlectron)==1){
                sprintf(list[numitem].name,"%s",xlectron.b_name);
                list[numitem].type=8;
                numitem++; }
endlist:
        disp_bmp(xlecpanel,tpage);
        for(i=0;i<7;i++){
     	if((i+toppos)<numitem){    //verifie si on est pas au bout de la liste
     	        print3(0,i*6+8,list[i].name);             //et on ecrit le chemin du fichier
     	        maxdown=i;
                }
     	}
changecur:
        invline3(curpos);
        showpage();
waitxlec:
        switch(waitkey()){
                case 1:case 64:return;
                case 80: //down
                       if(curpos<maxdown){
                                invline3(curpos);
                                curpos++;
                                goto changecur; }
                       if(curpos==6&&numitem>6){
                                toppos++;
                                goto endlist; }
                       goto waitxlec;
                case 72: //up
                       if(curpos>0) {
                                invline3(curpos);
                                curpos--;
                                goto changecur; }
                       if(curpos==0&&toppos>0){
                                toppos--;
                                goto endlist; }
                default:goto waitxlec;
        }

}

