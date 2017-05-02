#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<assert.h>

struct data_v{
  double * v;
  int id;
  double norm;
};
typedef struct data_v data_v;

struct data_base {
	int nbVector;
	int nbColumn;
	int * suffledIndex;
	char ** dictionnary;
	int nb_dictionnary;
	data_v * vector;
};
typedef struct data_base data_base;

struct node {
	double * w; // 4;
	double act; // distance uclidien
	int etiq;
};
typedef struct node node;

struct parametre{
	int phase1;
	double phase2;
	double alpha,alpha_init;
	int it_current,it_total;
	int rayon_voisinage,rayon_voisinage_init;
	};
typedef struct parametre parametre;

struct network {
	int width;
	int height;
	int nb_nodes;
	node * nodes;
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







