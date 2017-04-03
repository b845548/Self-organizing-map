#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

struct data_v{
  double * v;
  int id;
  double norm;
};
typedef struct data_v data_v;

struct data_base {
	int nbVector;
	int nbColumn;
	data_v * vector;
	int * suffledIndex;
	char ** dictionnary;
	int nb_dictionnary;
};
typedef struct data_base data_base;

struct node {
	double * w; // 4;
	double act; // distance uclidien
	int etiq;
};
typedef struct node node;

struct parametre{
double alpha,alpha_init;
int it_current,it_total;
int phase1;
float phase2;
int rayon_voisinage;
int rayon_voisinage_init;
};
typedef struct parametre parametre;

struct network {
	int width;
	int height;
	int nb_nodes;
	node * nodes;// node ** 50% DE
	parametre param;
};
typedef struct network network;

struct  best_matching_unit {
	int minX;
	int minY;
	struct best_matching_unit * next;
};
typedef struct best_matching_unit best_matching_unit;


struct best_matching_unit_Header{
	int nbl;
	struct best_matching_unit * begin;
	struct best_matching_unit * end;
};
typedef struct best_matching_unit_Header best_matching_unit_Header;








double normalisation(double * vec,int nbl){
	int i;
	double res=0.0;
	for(i=0;i<nbl;i++)
		res+=vec[i]*vec[i];	

	return (double)sqrt(res);

}

int * genSuffledVector(int nbl){ // generer un vecteur avec des valeur de 0 à nbl
	int rd,tmp,i;
	int * vec;
	vec=(int *)malloc(sizeof(int)*nbl);
	
	for(i=0;i<nbl;i++)
		vec[i]=i;
	
	for(i=0;i<nbl;i++){
		rd=rand()%nbl;
		tmp=vec[rd];
		vec[rd]=vec[i];
		vec[i]=tmp;
	}
	return vec;


}
int findDictionnary(char * mot,char ** dictionnary,int nb){
int index=0;
int i;
for(i=0;i<nb;i++)
	if(strcmp(mot,dictionnary[i])==0)
		return i;
	return -1;

}
int isDouble(char * str){
int i,len=strlen(str);
for(i=0;i<len;i++)
	if(strchr(".1234567890",str[i])==NULL)
		return 0;

return 1;
}

void initData(data_base * db,char * fileName){
	data_v * data;
	FILE * f;
	int i,j,nb_line,index;
	char buf[255],*p,* ptr;
	f=fopen(fileName,"r");
	for(nb_line=0;NULL!=fgets(buf,255,f);nb_line++){
		if(nb_line==0){
			ptr=strtok(buf,",");
			db->nbColumn=0;
			for(;ptr!=NULL;){
				if(isDouble(ptr))
					db->nbColumn++;
				ptr=strtok(NULL,",");
			}		
		}
	}
	fseek(f,0,SEEK_SET);
	data=(data_v *)malloc(sizeof(data_v)*(nb_line-1));
	db->dictionnary=(char **)malloc(sizeof(char*)*(100));
	db->nb_dictionnary=0;

	for(j=0;NULL!=fgets(buf,255,f);j++){
		data[j].v=(double *)malloc(sizeof(double)*db->nbColumn);
		ptr=strtok(buf,",");
		for(i=0;ptr!=NULL;i++){
			if(i<db->nbColumn){
				data[j].v[i]=strtod(ptr, &p);	
			}else{
			   data[j].norm=normalisation(data[j].v,db->nbColumn); 
			   if((index=findDictionnary(ptr,db->dictionnary,db->nb_dictionnary))==-1){
				db->dictionnary[db->nb_dictionnary]=strdup(ptr);
                data[j].id=db->nb_dictionnary;
				db->nb_dictionnary++;
				}else 
				data[j].id=index;

						

	 	}
		ptr=strtok(NULL,",");      
	  	}
	}
  	fclose(f);
	db->vector=data;
	db->nbVector=nb_line-1;
	db->suffledIndex=genSuffledVector(db->nbVector);
	for (i = 0; i < db->nb_dictionnary; i++){
		printf("ID %d : %s\n",i,db->dictionnary[i]);
	}	
}
void printNetwork(network net){
	int i;
	for (i = 0; i < net.nb_nodes; i++){
		printf("%f\n",net.nodes[i].act);
	}	
}

double randomNumber(double borneInf,double borneSup){
	return borneInf+(borneSup-borneInf)*((double)rand() / (double)RAND_MAX);
}

void genWeight(double * intv,int nbl,double * w){
	int i;
	for(i=0;i<nbl;i++)
		w[i]=(double)randomNumber(intv[i]+intv[i]/2,intv[i]-intv[i]/2);
}

double * averageVector(data_v * data, int nbLigne, int nbColumn) {
   	int i,j;
	double * avtb;
	avtb=(double *)malloc(sizeof(double)*nbColumn);
	memset(avtb,0,sizeof(double)*nbColumn);
	for(j=0;j<nbLigne;j++)
		for(i=0;i<nbColumn;i++)
			avtb[i]+=data[j].v[i];

	for(i=0;i<nbColumn;i++)
		avtb[i]=(double)avtb[i]/(double)nbLigne;
	return avtb;
} 



double distanceEuclid(double * p, double * q,int nbl){
	double result=0.0,tmp;
	int i;
		for(i=0;i<nbl;i++)
			result+=(p[i] - q[i])*(p[i] - q[i]);
		
	return (double)sqrt(result);

}



int tailleCarre(int nb){
	int i;
	int number;
	int nonSqrt;
	double num;
	while(1){
		num=sqrt(nb++);
		nonSqrt=(int)num;
		if((double)nonSqrt-num==0)
			break;
	}
	return nonSqrt;
}



void initNetwork(data_base * db,network * net){
	
	int i,rv,total,j;	
	double * average;
	net->height=tailleCarre((int)sqrt(db->nbVector)*5);
	net->width=net->height;
	net->nb_nodes=net->width*net->height;
	net->nodes=(node *)malloc(sizeof(node)*net->nb_nodes);
	for(rv=1,total=0;total<net->nb_nodes/2;total+=rv*8,rv++);// 50% de node
	net->param.rayon_voisinage=rv; 
	net->param.rayon_voisinage_init=rv;
	net->param.alpha=(double)0.2*((double)rand() / (double)RAND_MAX)+0.7;
	net->param.alpha_init=net->param.alpha;
	net->param.it_current=0;
	net->param.it_total=500*net->nb_nodes;
	net->param.phase1=net->param.it_total/4;
	net->param.phase2=1.0;
	average=averageVector(db->vector,db->nbVector, db->nbColumn);
	for (i = 0; i < net->nb_nodes; i++){
		net->nodes[i].w=(double *)malloc(sizeof(double)*db->nbColumn);
		genWeight(average,db->nbColumn,net->nodes[i].w);
		net->nodes[i].act=distanceEuclid(net->nodes[i].w,db->vector[db->suffledIndex[i]].v, db->nbColumn);
		net->nodes[i].etiq = -1;
	}
	
}


void initBMU(network * net, best_matching_unit_Header *bmu){
	best_matching_unit * nouveau;
	int mini=0;
	int i;
	for(i=1;i<net->nb_nodes;i++){
		if(net->nodes[mini].act>net->nodes[i].act)
			mini=i;
	}
	
	
	bmu->nbl=0;
	bmu->begin=NULL;
	
	
	for(i=0;i<net->nb_nodes;i++){
		if(net->nodes[mini].act==net->nodes[i].act){
			nouveau=(best_matching_unit *)malloc(sizeof(best_matching_unit));
			nouveau->minX=i%net->width;
			nouveau->minY=i/net->width;
			nouveau->next=bmu->begin;
			bmu->begin=nouveau;
			if(bmu->nbl==0)
				bmu->end=bmu->begin;
			bmu->nbl++;

		}
	}
	
	
}
void freeBMU(best_matching_unit_Header *bmu){
	best_matching_unit * tmp;
	tmp=bmu->begin;
	while(tmp!=bmu->end){
		if(tmp)
			free(tmp);
		tmp=tmp->next;
	}
	bmu->nbl=0;
}

void voisinage(network * net, best_matching_unit_Header * bmu, data_v * randomData,int nb_data_v){
	int i,j,k,x,y,r=net->param.rayon_voisinage;
	node * winner_node;
	best_matching_unit * winner;
	int randomNumber=rand()%bmu->nbl;
	winner=bmu->begin;
	winner_node=&net->nodes[winner->minX+winner->minY*net->width];
	for(i=0;i<bmu->nbl;i++){
		if(i==randomNumber){
			winner_node=&net->nodes[winner->minX+winner->minY*net->width];
			break;
		}				
		winner=winner->next;
	}

	
	for(i=-r;i<=r;i++)
		for(j=-r;j<=r;j++)
			for(k=0;k<nb_data_v;k++){
				x=winner->minX+j;
				y=winner->minY+i;	
				if(x<net->width&&y<net->height&&x>=0&&y>=0)
					net->nodes[x+y*net->width].w[k]+=net->param.alpha*(randomData->v[k]-net->nodes[x+y*net->width].w[k]);
			}
//	printf("%d %d\n",winner->minX,winner->minY);

}
void parametrage(parametre *p){
	p->alpha=p->alpha_init*(1.0-(p->it_current/(double)p->it_total))*p->phase2;
	if(p->it_current%p->phase1==0){
		p->phase2*=0.1;	
		p->rayon_voisinage=(int)p->rayon_voisinage_init*(1-p->it_current/(double)p->it_total);
	}
};

void apprentisage(data_base * db,network * net){
	int i;
	data_v * randomData;
	best_matching_unit_Header bmu;
	for(;net->param.it_current<net->param.it_total;net->param.it_current++){
		randomData=&db->vector[rand()%db->nbVector];
		for(i=0;i<net->nb_nodes;i++)
			net->nodes[i].act=distanceEuclid(net->nodes[i].w,randomData->v, db->nbColumn);
		initBMU(net,&bmu);
		voisinage(net,&bmu,randomData,db->nbColumn);
		parametrage(&net->param);
		freeBMU(&bmu);
	}
}


void initEtiq(data_base * db,network * net){
	int i,j,k;
	int max,min;
	float act,save;
	for(i=0;i<net->nb_nodes;i++){
		for(j=0,min=0,save=1000;j<db->nbVector;j++){
			act=distanceEuclid(db->vector[j].v, net->nodes[i].w, db->nbColumn);
			if(save == act)
				printf("%d %d \n",db->vector[min].id,db->vector[j].id);
	
			if(save > act){
				save=act;
				min=j;
			}	
		}
		net->nodes[i].etiq=db->vector[min].id;
	}
}
void printCarte(network * net){
int i,j;
	for(j=0;j<net->height;j++){
		for(i=0;i<net->width;i++)
		if(net->nodes[i+j*net->width].etiq==-1)
		printf(" ");
		else		
		printf("%d",net->nodes[i+j*net->width].etiq);
			printf("\n");
	}
}
int main(int args,char ** argv){
	data_base db;
	network net;
	srand(time(NULL));
	initData(&db,"iris.data");
	initNetwork(&db,&net);
	apprentisage(&db,&net);
	initEtiq(&db,&net);
	printCarte(&net);
	return 0;	
}

/*
nb_node=5*sqrt(nb_vec);
for(RV=0,i=0;i<nb_node;i+=8,RV++);//RV=(nb_node/2)8;
nb_it_total=5*nb_node;
t=nb_it_actuel;
alphaInit=0.7-0.9;
alpha=alphaInit;
phase1 = 1/4*t;
alpha=alphaInit(1-t/nb_it_total);
for voisin
Wij = Wij + alpha*(Xi-Wij)
*/
