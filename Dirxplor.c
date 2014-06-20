#define VERSION "VB15"
/*
         v1.4
        X     X
   OO    X   X      RRR
   O O    X X       R  R
   O O i R X  P l o RRR
   O O    X X       R  R
   OO    X   X      R   R
        X     X
    by Na-Prod Casio Software

  DirXplor by Na-prod
  Programmed by Superna
  DirXplor Copyright (c) 2003 Na-Prod
  www.na-prod.com for News & Updates
  ----------------------------------
  Casio2.h by Na-prod
  Part of the CDK Project
  Casio2.h Copyright (c) 2003 Na-Prod
  www.na-prod.com
  ----------------------------------
  DadaKtrAscii from Ktr.h and Dada66
  Ktr.h Copyright (c) 2003 Ktr
  www.ktr.fr.fm
  ----------------------------------
  Bmp.h Code from Whyp
  Code took from SysXpl4 sources
  SysXpl Copyright (c) 2002 Whyp
  www.graphx-prog.fr.st
  ----------------------------------
  Csv.h by Na-prod
  Programmed for DirXplor
  Based on CsvRead.asm from Tonton1664
  Csv.h Copyright (c) 2003 Na-Prod
  www.na-prod.com
  ---------------------------------------------
  DirXplor is programmed for free
  The four thing i'll oblige will be :
  - I'll only accept changes in DirXplor if i'm OK
  - My name must be in your program ans sources
  - The sources must be distributed freely almost the
    part of the sources were's DirXplor's code
  - DirXplor must only be distributed in it's original package
    or made by me
  Owerwise this program is free to use and to distribute
  This program cannot be sold or sold modified.
  Enjoy this code and this program !
*/
#include "2ascii.h"
#include "data.h"
#include "memzones.c"
#include <dirent.h>
#include <stdio.h>
#include <dos.h>
//#include <mem.h> replaced by fast_copy
#include <io.h>
#define O_BINARY 0x8000
#define O_RDONLY 1
//#include <string.h> replaced by equal & fast_copy


//Global Data
struct item{
	char name[13];
	char dir[13];
	char type;}
	list[100];
struct lnkstruct{
        int status;
        char path[30]; }
        lnk[3];
char tmp[30];
unsigned int size;
int numitem;
int ipage,tpage; //les n° de pages i->au debut t->d'edition
char *exe="EXE",*com="COM",*bmp="BMP",*csp="CSP",*csv="CSV";
char *nofiles="NO FILE ";
struct memory_zone xlectron;

#include "fts.c" //All functions needed

//Anti-Error Function
void far handler()
{
  _hardresume(_HARDERR_FAIL);
}

#include "bmp.h" //bmp par Whyp !
#include "csv.h" //Csv for DirXplor


void main(void){
     char lect='L';       //le nom du lecteur courant
     char dir[30]="X:\\"; //le string pour les noms de fichier
     char buf[2];         //le string pour le type
     char toppos=0,poscur=0,downpos,maxdown;
     int i;
     short key;
     struct memory_zone bf;
     //Initialisation
     ipage=getpage();
     tpage=ipage+1;
     clearpage(ipage);
     disp_bmp(back,tpage);
     buf[1] = '\0';
     _harderr(handler); //pour ne pas afficher l'erreur lors de la recherche des fichiers
     //Initialisation de la sauvegarde du dernier lect en MemZone
     for(i=0;i<3;i++) lnk[i].status=0;
savestart:   //si il existe pas, il est créé avec L: par defaut
     if((i=search_mem_zone(0xC,"DIRXPLOR",&bf))!=112) {
                if(i!=-1) { create_mem_zone(0xC,"DIRXPLOR",i, DELETE_ZONE);
                            goto savestart; }
                if (create_mem_zone(0xC,"DIRXPLOR",112, CREATE_ZONE) == -2){
                   goto savestart; }
                else {
                   search_mem_zone(0xC,"DIRXPLOR",&bf);
                   lect='L';
                   write_mem_zone(&bf,&lect,0,1);
                   write_mem_zone(&bf,lnk,1,96);
                   goto cont;}  }
     else {
cont:
     read_mem_zone(&bf, &lect, 0,1);
     read_mem_zone(&bf,lnk,1,96); }
//*********remise a zero de laffichage et des fichiers********
start:
     //Afficher "Loading..."
     disp_bmp(back,tpage);
     putchar3(33,51,lect);
     print3(30,25,"LOADING...");
     showpage();
     //a chaque changement de lecteur, on stocke la lettre en mem
     write_mem_zone(&bf,&lect,0,1);
     //et on ecris le path correctement
     sprintf(dir,"%c:\\",lect);
     //on recherche les fichiers
     numitem = listdrive(dir, 0);
     if(numitem==0){    //s'il ne trouve pas de fichiers dans le lecteur
     	sprintf(list[0].dir," ");
     	sprintf(list[0].name,"%s",nofiles);
     	list[0].type='?';
     	numitem=1;}
     toppos=0;
     poscur=0;
//*******rafraichir l'affichage********
clr:
     //Afficher le fond
     disp_bmp(back,tpage);
     //affichage du lecteur en bas
     putchar3(33,51,lect);

//******reaffichage des fichier avec de nouveaux parametres*****
top:
     delspace();
     for(i=0;i<7;i++){
     	if((i+toppos)<numitem){    //verifie si on est pas au bout de la liste
     	        if(list[i+toppos].type!='N'){
                        buf[0] = list[i+toppos].type;    //indique le type du fichier
     	                putchar3(2,i*6+8,buf[0]);        //en b15 une lettre
     	                sprintf(dir,"%s%s",list[i+toppos].dir,list[i+toppos].name);
     	                print3(0,i*6+8,dir);             //et on ecrit le chemin du fichier
     	                maxdown=i; }
     	        else{
                        buf[0] = list[i+toppos].type;    //indique le type du fichier
     	                putchar3(2,i*6+8,'?');        //en b15 une lettre
     	                print3(0,i*6+8,nofiles);             //et on ecrit le chemin du fichier
     	                maxdown=i;
     	                }
                }
     	}

     //Downpos: Numero de fichier en bas de la liste
     if(maxdown<6) downpos=maxdown;
     else { downpos=toppos+6;}
     //Afiche la pos dans la liste dans la barre
     //car 35, si rien en haut : (123,14), au milieu, (123,27) si tt en bas (123,39)
     if(toppos>0&&downpos<(numitem-1))  putchar3(123,27,35);
     else if(toppos>0&&downpos>=(numitem-1))  putchar3(123,39,35);
     else if(toppos==0&&downpos<(numitem-1))  putchar3(123,14,35);
//*******Changement de fichier*********
cur:
     invline2(poscur);
     //on prepare l'adresse standart
     sprintf(dir,"%s%s\0",list[poscur+toppos].dir,list[poscur+toppos].name);
     //on recherche la taille
     if((i=open(dir,O_BINARY))==-1) newsize(0);  //ou l'ouvre
     else { newsize(filelength(i)); //pour avoir sa taille
            close(i);}
     showpage();
//*******on attends les touches******
keywait:
     key=waitkey();
     //Regarde les touches appuy‚es
     switch(key){
        case 72:if(poscur>0) {
                                invline2(poscur);            //mont‚e standart
        /*up*/			poscur--;
     			        goto cur; }
     	        if(poscur==0&&toppos>0) {
     	                                invline2(poscur);   //reenroulement
     				        toppos--;
     				        goto top; }
     	        break;
        case 80:if(poscur<maxdown) {
                                invline2(poscur);   //Descente normale
     /*down*/		        poscur++;
     			        goto cur; }
     	        if(poscur==maxdown&&downpos<(numitem-1)){ //Deroulement
     			        invline2(poscur);
     			        toppos++;
     			        goto top; }
     	        break;
        case 75:if(lect>'A'&&lect<='Q') {
     /*gauche*/			lect--;
     		        	goto start;}
     	        if(lect=='A'||lect>'Q') {
     		              	lect='Q';
     			        goto start;}
                              	break;
        case 77:if(lect<'Q') {
     /*droite*/			lect++;
              			goto start;}
     	        if(lect=='Q'||lect>'Q') {
     		              	lect='A';
               			goto start;}
     	        break;
        case 28:case 0x3b:   //touche EXE/F1 --> execution selon le type
     	                switch(list[toppos+poscur].type){
     		                case 'E':   //executable
                                     disp_bmp(panel,tpage);
                                     print3(25,10,"EXE/COM EXECUTION");
     			             print3(6,20,"FILE INFORMATION :");//on montre les infos (ici sera le choix de l'argument)
                                     invline(0);
                                     print3(8,26,dir);
     			             print3(6,38,"PRESS EXE/F1 TO LAUNCH..");
	                             print3(6,44,"PRESS ESC/F6 TO QUIT...");
     			             showpage();
     			        waitexe:
     			             switch(waitkey()){
     			                case 64:case 1:goto clr;
     			                case 28:case 59:break;
     			                default:goto waitexe;
     			             }
     			        exec:
     			             gotoxy(1,1);
     			             clearpage(ipage);
     			             setdisk(dir[0]-65); //on change de disque (securit‚)
     			             chdir(list[poscur+toppos].dir);   //on se place dans le bon repertoire
     			             execute(dir);     //et on execute
     			             disp_bmp(panel,tpage);
     			             print3(25,10,"ERROR");   //ah ben la y'a eu une erreur, assez rare
     			             print3(6,32,"PRESS A KEY...");
                                     showpage();                    //de tomber ici
     			             waitkey();
     			             goto clr;
     		                case 'I':   //Image BMP/CSI/CSP (en b15 seul le BMP est g‚r‚)
     			             disp_bmp(panel,tpage); //On pr‚pare le chemin du bmp
     			             bmpview(dir);   //on le visualise grace au module de whyp
     			            goto clr;
     		                case 'V':      //les fameuses vid‚os
     			             disp_bmp(panel,tpage); //on prepare le chemin
     			             csvread(dir); //et on la visualise, prenez vos pop corn, c'est partit !!
     			             goto clr;
     		                case '?':case 'F':  //les inconnus et les dossiers ne sont pas ouvrables lol
     				     break;
     		                }
                        break;
        case 'A':case 'B':case 'C':
        case 'D':case 'E':case 'F':
        case 'G':case 'I':case 'J':
        case 'L':                     //voila une ft importante, vous appuyer sur la touche avec la
     	     lect=(char)key;       //lettre du lecteur et hop, magic system, comme linl en gros
     	     goto start;
        case '7':case '8':case '9':
     	     lect=(char)key+22;
     	     goto start;
        case '4':case '5':
     	     lect=(char)key+28;
     	     goto start;
        /*case 48:shootpage(ipage);break; */
        case 1:case 0x40:return;    //a tchao bonsoir ! (esc ou f6)
        case '1':case '2':case '3':
             if(lnk[key-49].status==1) {
                    sprintf(dir,"%s",lnk[key-49].path);
                    goto exec; }
             break;
        case 60: //on attribue un exe a 1 soit 2 soit 3 avec F2
             if(list[poscur+toppos].type!='E') goto keywait;
             i=0;
waitattrib:
             disp_bmp(panel,tpage);
             print3(25,10,"KEY ATTRIBUTION");
     	     print3(6,20,"FILE INFORMATION :");//on montre les infos (ici sera le choix de l'argument)
             invline(0);
             print3(8,26,dir);
     	     print3(6,32,"ASSIGN TO KEY :<  >");
             putchar3(83,32,49+i); //1
             sprintf(tmp,"CURRENT:%s",lnk[i].path);
             print3(6,38,tmp);
     	     print3(6,44,"PRESS EXE/F1 TO ACCEPT..");
	     print3(6,50,"PRESS ESC/F6 TO QUIT...");
             showpage();
     	     key=waitkey();
     	     switch(key){
     			case 64:case 1:goto clr;
     			case 28:case 59:break;
     			case 77: if(i<2) i++;
                                 goto waitattrib;
                        case 75: if(i>0) i--;
                        default:goto waitattrib;
     			        }
     	     lnk[i].status=1;
     	     fast_copy(dir,lnk[i].path,sizeof(dir));   //on copie le path
             write_mem_zone(&bf,lnk,1,96);
             goto clr;
        case 61:
             listxlectron();
             goto start;
        case 62:
             tmp[0]='A';
waitdrv:
             disp_bmp(panel,tpage);
             print3(25,10,"DRIVE OPEN");
     	     print3(6,20,"CHOOSE DRIVE LETTER :");
             invline(0);
     	     sprintf(dir,"< %c >",tmp[0]);
             print3(44,32,dir);
     	     print3(6,44,"PRESS EXE/F1 TO ACCEPT..");
	     print3(6,50,"PRESS ESC/F6 TO QUIT...");
             showpage();
     	     key=waitkey();
     	     switch(key){
     			case 64:case 1:goto clr;
     			case 28:case 59:break;
     			case 77: if(tmp[0]<'Z') tmp[0]++;
                                 goto waitdrv;
                        case 75: if(tmp[0]>'A') tmp[0]--;
                        default:goto waitdrv;
     			        }
     	     lect=tmp[0];
     	     goto start;
        }
     goto keywait;
}
