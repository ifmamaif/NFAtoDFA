#ifndef IMmyLibraries
	#define IMmyLibraries 
	#include<stdio.h>
	#include<stdlib.h>
#endif
#ifndef IMmyStructs
	#define IMmyStructs	
	// automat = // <Sigma,S,s0,delta,F> 
	// Sigma este alfabetul de intrare (o multime finita si nevida de Symboluri).
	// S este o multime finita si nevida de stari.
	// s0 este Statea initiala, element al lui S.
	// delta este functia de tranzitie
	// F este multimea starilor finale, o submultime a lui S.
	struct automat { 				
		int n,nf,s;	 	// n=numarul de stari , nf=numarul de stari finale , s=numarul de Symboluri						
		char *Sigma;				
		struct SuperState {
			int State;
			struct SuperState *Next;		
		}**Delta;				
		int *F;				
	};		
	struct list {
		struct SuperState CurrentState;
		struct SuperState *Delta;
		struct list *Next;
	};
#endif
#ifndef IMmyFunctions
	#define IMmyFunctions
	int Showautomat(struct automat);	
	int OrderSuperState(struct SuperState *);
	int OrderNFA(struct automat);
	int FreeSuperState(struct SuperState *);
	int CopySuperState(struct SuperState *,struct SuperState);
	int NFAtoDFA(struct automat);
#endif
#ifndef IMmyMain
	#define IMmyMain
int main(int argc, char *argv[]){
	struct automat NFA;
	FILE * file = fopen(argv[1], "r");
    if(file==NULL)printf("This file doesn't exist!"),exit(0);		
	{ 	//BEGIN READ first line		
		fscanf(file,"%d %d %d",&NFA.n,&NFA.nf,&NFA.s); 	
		NFA.Delta=(struct SuperState **)malloc(NFA.n*sizeof(struct SuperState *));
		int i=0;	
		for(;i<(NFA.n);i++){			
			NFA.Delta[i]=(struct SuperState *)malloc(NFA.s*sizeof(struct SuperState ));			
			int j=0;
			for(;j<NFA.s;j++){
				NFA.Delta[i][j].State=-1;
				NFA.Delta[i][j].Next=NULL;
			}
		}	
		NFA.F=(int*)malloc(NFA.nf*sizeof(int));					
		NFA.Sigma=(char*)malloc((NFA.s+1)*sizeof(char));
		NFA.Sigma[0]='\0';
	}	//END READ first line	
	{ 	//BEGIN READ second line
		int i=0;
		for(;i<NFA.nf;i++){			
			fscanf(file,"%d",&(NFA.F[i]));
		}
	}	//END READ second line
	{ 	//BEGIN READ rest of file
		int st=0,end=0; //State=State curenta , end=urmatoarea State
		char c[3]; // Simbolul de tranzitie
		int pozition=ftell(file);
		while (fscanf(file,"%d",&st)>0 && fread(c,sizeof(char),3,file)>0 && fscanf(file,"%d",&end)>0)	{
			int i=0,ok=1;
			while(ok==1 && NFA.Sigma[i]!='\0')	{
				if(NFA.Sigma[i]==c[1])	{
					ok=0;
				}
				else
					i++;
			}
			if(ok==1){
				NFA.Sigma[i]=c[1];
				NFA.Sigma[i+1]='\0';
			}
		}
		fseek(file,pozition,SEEK_SET);
		while (fscanf(file,"%d",&st)>0 && fread(c,sizeof(char),3,file)>0 && fscanf(file,"%d",&end)>0)	{	
			int i=0;
			for(;i<NFA.s && NFA.Sigma[i] && NFA.Sigma[i]!=c[1];i++)	{
			}
			if(NFA.Delta[st][i].State	<	0)	{
				NFA.Delta[st][i].State=end;
			}
			else	{
				struct SuperState *road;
				road=&NFA.Delta[st][i];
				while(road->Next!=NULL)	{
					road=road->Next;
				}
				road->Next=(struct SuperState *)malloc(sizeof(struct SuperState ));
				road->Next->State=end;
				road->Next->Next=NULL;
			}
		}		 
	}	//END READ rest of file
	OrderNFA(NFA);
	Showautomat(NFA);
	NFAtoDFA(NFA);
	///FreeMemAutomat(NFA);	
	fclose(file);
	return 0;
}

#endif
#ifndef IMmyDefiningfunctions
	#define IMmyDefiningfunctions
	int Showautomat(struct automat NFA){
		int i,j;
		for(i=0;i<NFA.n;i++)	{			
			for(j=0;j<NFA.s;j++)	{
				if(NFA.Delta[i][j].State>=0)	{
					printf("%d %c ",i,NFA.Sigma[j]);
					struct SuperState *road;
					road=&NFA.Delta[i][j];
					while(road!=NULL)	{
						printf("%d",road->State);
						road=road->Next;
					}
					printf("\n");
				}
			}
		}
		printf("\n");
		return 1;
	}
	int OrderSuperState(struct SuperState *ThisSuperState)	{
		if(ThisSuperState->Next==NULL)	{
			return 0;
		}
		int n=0;
		struct SuperState *road;
		road=ThisSuperState;
		while(road!=NULL){
			road=road->Next;
			n++;
		}
		int i=0,ok=1;
		while(i<n && ok==1)	{
			ok=0;
			road=ThisSuperState;
			while(road->Next!=NULL)	{
				if(road->State>road->Next->State)	{
					int aux=road->State;
					road->State=road->Next->State;
					road->Next->State=aux;
					ok=1;
				}
				road=road->Next;
			}
			i++;
		}
		return 1;
	}
	int OrderNFA(struct automat NFA){
		int i,j;
		for(i=0;i<NFA.n;i++)	{
			for(j=0;j<NFA.s;j++)	{
				OrderSuperState(&NFA.Delta[i][j]);
			}
		}
		return 1;
	}
	int FreeSuperState(struct SuperState *ThisSuperState)	{
		while(ThisSuperState!=NULL){
			struct SuperState *tofree;
			tofree=ThisSuperState;
			ThisSuperState=ThisSuperState->Next;
			free(tofree);
		}
		return 1;
	}
	int CopySuperState(struct SuperState *To,struct SuperState From)	{
		if((From).State	<	0)	{
			return 0;
		}
		if((*To).State	<	0)	{			
			(*To).State=(From).State;
			(*To).Next=NULL;
			struct SuperState *roadTo,*roadFrom;
			roadTo=To;
			roadFrom=(From).Next;
			while(roadFrom!=NULL)	{
				roadTo->Next=(struct SuperState *)malloc(sizeof(struct SuperState));
				roadTo->Next->State=roadFrom->State;
				roadTo=roadTo->Next;
				roadFrom=roadFrom->Next;
			}
			roadTo->Next=NULL;
			return 2;
		}
		else {
			To->State=(From).State;
			struct SuperState *roadTo,*roadFrom;
			roadTo=To;
			roadFrom=(From).Next;
			while(roadFrom!=NULL)	{
				if(roadTo->Next==NULL)	{
					roadTo->Next=(struct SuperState *)malloc(sizeof(struct SuperState));
					roadTo->Next->State=roadFrom->State;
					roadTo=roadTo->Next;
				}
				else	{
					roadTo->Next->State=roadFrom->State;
					roadTo=roadTo->Next;
				}					
				roadFrom=roadFrom->Next;
			}
			FreeSuperState(roadTo->Next);
			roadTo->Next=NULL;
			return 3;
		}		
		return 1;
	}
	int EqualSuperState(struct SuperState SuperState1,struct SuperState SuperState2){
		struct SuperState *road1,*road2;
		road1=&SuperState1;
		road2=&SuperState2;
		while(road1!=NULL && road2!=NULL){
			if(road1->State!=road2->State){
				return 0;
			}
			road1=road1->Next;
			road2=road2->Next;
		}
		if(road1!=NULL){
			return 0;
		}
		if(road2!=NULL){
			return 0;
		}
		return 1;
	}
	int ExistSuperStateInList(struct list *List,struct SuperState ThisSuperState){
		struct list *roadList;
		roadList=List;
		while(roadList!=NULL){
			if(EqualSuperState((roadList->CurrentState),ThisSuperState)==1){
				return 1;
			}
			roadList=roadList->Next;
		}		
		return 0;
	}
	int ShowSuperState(struct SuperState ThisSuperState){
		printf("%d",ThisSuperState.State);
		struct SuperState *road;
		road=ThisSuperState.Next;
		while(road!=NULL)	{
			printf(",%d",road->State);
			road=road->Next;
		}
		return 1;
	}
	int ShowList(struct automat NFA,struct list *List){
		struct list *road;
		road=List;
		while(road!=NULL){
			int i;
			for(i=0;i<NFA.s;i++){
				ShowSuperState(road->CurrentState);
				printf(" %c ",NFA.Sigma[i]);
				ShowSuperState(road->Delta[i]);
				printf("\n");
			}
			road=road->Next;
		}
		return 1;
	}
	int CreateList(struct list **List,struct automat NFA){
		(*List)=(struct list *)malloc(sizeof(struct list));
		(*List)->CurrentState.State=0;
		(*List)->CurrentState.Next=NULL;
		(*List)->Delta=(struct SuperState *)malloc(NFA.s*sizeof(struct SuperState ));
		int i;
		for(i=0;i<NFA.s;i++)	{
			(*List)->Delta[i].State=-1;
			(*List)->Delta[i].Next=NULL;
			CopySuperState(&(*List)->Delta[i],NFA.Delta[0][i]);
		}
		(*List)->Next=NULL;
		return 1;
	}
	int UnionSuperState(struct SuperState *To,struct SuperState From){ 
		if(From.State<0){
			return 0;
		}
		if((*To).State<0)	{
			CopySuperState(To,From);
			return 2;
		}
		struct SuperState *roadTo,*roadFrom;
		roadTo=To;
		roadFrom=&From;
		while(roadTo->Next!=NULL	&&	roadFrom->Next!=NULL){
			if((roadTo->State)==(roadFrom->State)){
				roadTo=roadTo->Next;
				roadFrom=roadFrom->Next;
			}
			else if((roadTo->State)<(roadFrom->State)){
				roadTo=roadTo->Next;
			}
			else {	//	if((roadTo->State)>(roadFrom->State))
				struct SuperState *aux;
				aux=(struct SuperState *)malloc(sizeof(struct SuperState));
				aux->State=roadTo->State;
				aux->Next=roadTo->Next;
				roadTo->State=roadFrom->State;
				roadTo->Next=aux;
				roadTo=roadTo->Next;
				roadFrom=roadFrom->Next;
			}
		}		
		while(roadFrom->Next!=NULL){
			roadTo->State=roadFrom->State;
			if(roadTo->Next==NULL){
				roadTo->Next=(struct SuperState*)malloc(sizeof(struct SuperState));
			}
			roadFrom=roadFrom->Next;
		}
		roadTo->State=-1;
		roadTo->Next=NULL;
		return 1;
	}
	struct SuperState move(struct automat NFA,struct SuperState ThisSuperState,int i)	{
		struct SuperState Rezult;
		Rezult.State=-1;
		Rezult.Next=NULL;
		struct SuperState *roadSuperState;
		roadSuperState=&ThisSuperState;
		while(roadSuperState!=NULL){
			UnionSuperState(&Rezult,NFA.Delta[roadSuperState->State][i]);
			roadSuperState=roadSuperState->Next;
		}
		return Rezult;
	}
	struct list* mark(struct list *List,struct automat NFA,struct SuperState ThisSuperState)	{
		struct list *road;
		road=List;
		while(road->Next!=NULL)	{
			road=road->Next;
		}
		road->Next=(struct list *)malloc(sizeof(struct list));
		road->Next->CurrentState.State=-1;
		road->Next->CurrentState.Next=NULL;
		CopySuperState(&(road->Next->CurrentState),ThisSuperState);		
		road->Next->Delta=(struct SuperState *)malloc(NFA.s * sizeof(struct SuperState));
		int i;
		for(i=0;i<NFA.s;i++)	{
			road->Next->Delta[i].State=-1;
			road->Next->Delta[i].Next=NULL;
		}
		road->Next->Next=NULL;
		return road->Next;
	}
	int NFAtoDFA(struct automat NFA){
		struct list *List=NULL;
		CreateList(&List,NFA); // copiem starea initiala din NFA
		struct list *roadlist;
		roadlist=List;		
		///ShowList(NFA,List);
		while(roadlist!=NULL){
			int i;
			for(i=0;i<NFA.s;i++){
				if(roadlist->Delta[i].State	>=	0)	{
					if(ExistSuperStateInList(List,(roadlist->Delta[i]))==0)	{
						struct list *construct=NULL;
						construct=mark(List,NFA,(roadlist->Delta[i]));
						int j;
						for(j=0;j<NFA.s;j++){
							CopySuperState(&construct->Delta[j],move(NFA,construct->CurrentState,j));
						}
					}
				}
				
			}
			roadlist=roadlist->Next;
		}
		///printf("\nDone\n");
		ShowList(NFA,List);
		return 1;
	}
	
#endif





