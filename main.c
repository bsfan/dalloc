#include <stdio.h>
#define FRAGCOUNT 1000
#define UI_LINELEN 100
int M_BlockSize[FRAGCOUNT+1];
int M_FirstFree;
void M_Print_InsertEnter(int llen,int offset){
	if(offset%llen){
		return;
	}
	printf("\n0x%010i:",offset);
}
void M_Print(){

	int offset=0;
	int head=0;
	int nextHead=0;
	int nowfree=M_FirstFree;
	while(1){
		nextHead=head+M_BlockSize[head];
		for(offset=head;offset<nextHead;offset++){
			M_Print_InsertEnter(UI_LINELEN,offset);
			if(nowfree){
				printf("_");
			}else{
				printf("#");
			}
		}
		nowfree=!nowfree;
		head=nextHead;
		if(head>=FRAGCOUNT){
			break;
		}
	}
}
void M_NewBlock(int newHead,int newSize){  
	//   |##prev##|__cur__________________|##next##|
	//
	//			 |
	//			\|/
	//
	//   |##prev##|__cur___|##new#####|___|##next##|

	//find curHead;   
	int curHead,curSize,prevHead;
	for(curHead=0;curHead+M_BlockSize[curHead]<=newHead;curHead+=M_BlockSize[curHead]){
		prevHead=curHead;
	}
	curSize=M_BlockSize[curHead];

	if(newSize+newHead-curHead<curSize){//   ??|??cur??|##new#####|_(3)_|##next##|
		M_BlockSize[newHead+newSize]=curSize-(newHead-curHead)-newSize;//(3)
	}else//   ??|##new##########|##(next)##|
	{
		newSize+=M_BlockSize[curHead+curSize];//newSize+=nextSize
	}

	if(newHead!=curHead){//   ..|##prev##|__cur___|##new#####???
		M_BlockSize[curHead]=newHead-curHead;//Shrink cur
		M_BlockSize[newHead]=newSize;//Build new

	}else{//   ..|##prev##|##(new)#####????
		if(curHead==0){//  0 |##(new)#####????
			M_FirstFree=!M_FirstFree;
			M_BlockSize[curHead]=newSize;
		}else{//   ..|##prev##|##(new)#####?????
			M_BlockSize[prevHead]+=newSize;
		}
	}
}
void M_Init_Block(int head,int len){
	M_BlockSize[head]=len;
}
void M_Init(){
	M_Init_Block(0,FRAGCOUNT);
	M_Init_Block(FRAGCOUNT,0);//overflow protection
	M_FirstFree=1;
}
int M_CheckFreeStatus(int head,int size,int free)
{
	//find curHead;   
	int curHead,curSize,curFree;
	curFree=M_FirstFree;
	for(curHead=0;curHead+M_BlockSize[curHead]<=head;curHead+=M_BlockSize[curHead]){
		curFree=!curFree;
	}
	curSize=M_BlockSize[curHead];
	if(curFree!=free){
		return 0;
	}
	if(head+size>curHead+curSize){
		return 0;
	}
	return 1;
}
int M_Allocate_FirstFit(int newSize){
	int curSize;
	int curHead=0;
	int curFree=M_FirstFree;
	while(1){
		curSize=M_BlockSize[curHead];
		if(curFree&&curSize>=newSize){
			M_NewBlock(curHead,newSize);
			return curHead;
		}
		curHead+=curSize;
		curFree=!curFree;
		if(curHead>=FRAGCOUNT){
			break;
		}
	}
	return -1;
}
int M_Allocate_BestFit(int newSize){
	int curSize;
	int curHead=0;
	int curFree=M_FirstFree;
	int bestSize=FRAGCOUNT+1;
	int bestHead=-1;
	while(1){
		curSize=M_BlockSize[curHead];
		if(curFree&&curSize>=newSize){
			if(curSize<bestSize){
				bestSize=curSize;
				bestHead=curHead;
			}
		}
		curHead+=curSize;
		curFree=!curFree;
		if(curHead>=FRAGCOUNT){
			break;
		}
	}
	if(bestHead!=-1){
		M_NewBlock(bestHead,newSize);
	}
	return bestHead;
}
int main(){
	int i,j;
	char key;
	M_Init();
	while(1){
		M_Print();
		printf("\n");
		printf("0:Allocate(FixedArea)   1:Allocate(FirstFit)   2:Allocate(BestFit) \nf:Free   r:Reset(Initial)   q:Quit >");
		while((key=getchar())=='\n');
		switch(key){
		case 'q':
			return 0;
		case 'r':
			M_Init();
			printf("OK!\n");
			break;
		case 'f':
			printf("Head:");
			scanf("%i",&i);
			printf("Size:");
			scanf("%i",&j);
			if(!M_CheckFreeStatus(i,j,0)){
				printf("ERROR:This area contains free fragments!\n");
				break;
			}
			M_NewBlock(i,j);
			printf("OK!\n");
			break;
		case '0':
			printf("New head:");
			scanf("%i",&i);
			printf("New size:");
			scanf("%i",&j);
			if(!M_CheckFreeStatus(i,j,1)){
				printf("ERROR:This area contains used fragments!\n");
				break;
			}
			M_NewBlock(i,j);
			printf("OK!\n");
			break;
		case '1':
			printf("New size:");
			scanf("%i",&i);
			if((j=M_Allocate_FirstFit(i))==-1){
				printf("FAILED!\n");
				break;
			}
			printf("OK! New head=0x%010i\n",j);
			break;
		case '2':
			printf("New size:");
			scanf("%i",&i);
			if((j=M_Allocate_BestFit(i))==-1){
				printf("FAILED!\n");
				break;
			}
			printf("OK! New head=0x%010i\n",j);
			break;
		}
	}
	return 0;
}

