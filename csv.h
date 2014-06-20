/*
  Csv.h by Na-prod
  Programmed for DirXplor
  Based on CsvRead.asm from Tonton1664
  Csv.h Copyright (c) 2003 Na-Prod
  www.na-prod.com
*/
int csvread(char * file){
	unsigned long size,test;
	int handle;
	int i,errnum;
	char nfrm,vitesse=31,mode;
	unsigned char tmp[1024];
	if( ( handle=open(file,O_RDONLY|O_BINARY) )==-1 ) {
		errnum=0; //erreur fichier
		goto error; }

	lseek(handle,0,0);

	if(read(handle,tmp,10)==-1){
		errnum=1; //erreur lecture
		goto error; }

	if(tmp[0]!='C'||tmp[1]!='S'||tmp[2]!='V'){//mauvais ident
		errnum=2; //erreur Format
		goto error; }
	if(tmp[3]!=1||tmp[5]==0){//mauvaise version&&frames nuls
		errnum=2; //erreur Format
		goto error; }

	mode=tmp[4];
	nfrm=tmp[5];
	vitesse=31-tmp[6];
	print3(25,10,"CSV PLAYER");
	print3(6,20,"FILE INFORMATION :");
	invline(0);
	sprintf(tmp,"FRAMES: %i",nfrm);
	print3(8,26,tmp);
	sprintf(tmp,"VITESSE: %i",vitesse);
	print3(8,32,tmp);
	print3(6,44,"PRESS EXE/F1 TO LAUNCH..");
	print3(6,50,"PRESS ESC/F6 TO QUIT...");
	showpage();
wait:
	errnum=waitkey();
	if(errnum==1||errnum==64) { close(handle);
			return; }
	if(errnum!=28&&errnum!=59) goto wait;

	lseek(handle,10,0); //position apres le header

	clearpage(tpage+1);
	setpage(tpage+1);
	asm mov al,mode;
	asm out 2,al;
	size=10;
	for(i=0;i<nfrm;i++){
		errnum=read(handle,tmp,1024);
		if(errnum==-1){
			errnum=1; //erreur lecture
			goto error; }

		disp_bmp(tmp,tpage+1);
		for(errnum=0;errnum<vitesse;errnum++)
			asm{	hlt
				hlt };
		size+=1024;
		test=tell(handle);
		if(test!=size){
			errnum=1; //erreur lecture
			goto error; }
		}

	asm{	mov al,0xc3
		out 2,al }

	setpage(ipage);

	goto wait;
error: //Gestion des erreurs
	setpage(ipage);
	close(handle);
	asm{
		mov al,0xc3
		out 2,al }
	disp_bmp(panel,tpage);
     	print3(25,10,"ERROR");
	switch(errnum){
		case 0:
			print3(8,26,"ERREUR DE FICHIER");
			break;
		case 1:
			print3(8,26,"ERREUR DE LECTURE");
			break;
		case 2:
			print3(8,26,"FORMAT INCORRECT");
			break;
		default:
			print3(8,26,"ERREUR INCONNUE");
			break; }
	invline(1);
        print3(6,38," PRESS A KEY...");
	showpage();
	waitkey();
	return;
}






