/*!\file window.c
 * \brief GL4D-Primitives et modèle simple de balle avec rebonds. Il
 * faut ajouter la prise en compte du temps, du rayon de la balle et
 * des bugs de type blocage de la balle (quand la gestion du temps sera
 * ajoutée).
 *
 * gestion de gravité, correction de bug et multi-balles ajoutée le 13/02/17
 *
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr
 * \date February 02 2017
 */
#include <assert.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include "som.h"

extern void initData(data_base * db,char * fileName);
extern void initNetwork(data_base * db,network * net);
extern void apprentisage(data_base * db,network * net);
extern void initEtiq(data_base * db,network * net);
static data_base db;
static network net;
extern void printCarteName(network * net);
extern double magnitude(double * vec,int nb);
typedef struct mobile_t mobile_t;

struct mobile_t {
  float x, y, vx, vy, ax, ay;
  int r;
  GLuint color;
};

static mobile_t * _mobiles = NULL;
static int _nbMobiles = 1;
static float _G[] = {0, -980};

void mobileInit(int n) {
int i,j;
double mag;
GLuint color;
	srand(time(NULL));
	initData(&db,"iris.data");
	initNetwork(&db,&net);
	apprentisage(&db,&net);
	initEtiq(&db,&net);
	printCarteName(&net);
//	printCarteVal(&net);
  _nbMobiles = net.nb_nodes;
  _mobiles = malloc(_nbMobiles * sizeof *_mobiles);
  assert(_mobiles);

  for(j = 0; j < net.height; j++) {

  for(i = 0; i < net.width; i++) {
    _mobiles[i+j*net.width].r = 10.0;
    _mobiles[i+j*net.width].x = i*20;
    _mobiles[i+j*net.width].y = j*20;
    _mobiles[i+j*net.width].vx = gl4dmSURand() * gl4dpGetWidth() / 4.0;
    _mobiles[i+j*net.width].vy = gl4dmURand() * gl4dpGetHeight() / 4.0;
    _mobiles[i+j*net.width].ax = _G[0];
    _mobiles[i+j*net.width].ay = _G[1];
    mag=magnitude(net.nodes[i+j*net.width].w,db.nbColumn);
    color=(GLuint)120*(net.nodes[i+j*net.width].etiq);
	_mobiles[i+j*net.width].color = (255<<24)+((int)(mag*40)<<16)+(50<<8)+((int)(mag*40));
	}
}

/*
    _mobiles[i].x = gl4dmURand() * gl4dpGetWidth();
    _mobiles[i].y = gl4dmURand() * gl4dpGetHeight();
    _mobiles[i].vx = gl4dmSURand() * gl4dpGetWidth() / 4.0;
    _mobiles[i].vy = gl4dmURand() * gl4dpGetHeight() / 4.0;
    _mobiles[i].ax = _G[0];
    _mobiles[i].ay = _G[1];
    _mobiles[i].r = 5.0 + gl4dmURand() * gl4dpGetWidth() / 20.0;
    _mobiles[i].color = rand();
*/
} 

void mobileMove(void) {
  static GLuint t0 = 0;
  GLuint t;
  GLfloat dt;
  int i;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;
  for(i = 0; i < _nbMobiles; i++) {
    _mobiles[i].x += _mobiles[i].vx * dt;
    _mobiles[i].y += _mobiles[i].vy * dt;
    _mobiles[i].vx += _mobiles[i].ax * dt;
    _mobiles[i].vy += _mobiles[i].ay * dt;
    if((_mobiles[i].x - _mobiles[i].r < 0 && _mobiles[i].vx < 0) || 
       (_mobiles[i].x + _mobiles[i].r >= gl4dpGetWidth() && _mobiles[i].vx > 0)) {
      _mobiles[i].vx = -_mobiles[i].vx;
      _mobiles[i].vx -= _mobiles[i].vx * 0.1;
      _mobiles[i].vy -= _mobiles[i].vy * 0.05;
    }
    if((_mobiles[i].y - _mobiles[i].r < 0 && _mobiles[i].vy < 0) || 
       (_mobiles[i].y + _mobiles[i].r >= gl4dpGetHeight() && _mobiles[i].vy > 0)) {
      _mobiles[i].vy = -_mobiles[i].vy;
      _mobiles[i].vy -= _mobiles[i].vy * 0.1;
      _mobiles[i].vx -= _mobiles[i].vx * 0.05;
    }
  }
}

void mobileDraw(void) {
  int i;
GLuint color;
double mag;
int centreX=50;
  for(i = 0; i < _nbMobiles; i++) {
    mag=magnitude(net.nodes[i].w,db.nbColumn);
    color=(GLuint)120*(net.nodes[i].etiq);
	_mobiles[i].color = (255<<24)+((int)(mag*40)<<16)+(50<<8)+((int)(mag*40));
    gl4dpSetColor(_mobiles[i].color);
    gl4dpFilledCircle(_mobiles[i].x+ centreX, _mobiles[i].y+50, _mobiles[i].r);
  }

  for(i = 0; i < _nbMobiles; i++) {
    mag=magnitude(net.nodes[i].w,db.nbColumn);
    color=(GLuint)140*(net.nodes[i].etiq);
	_mobiles[i].color = (255<<24)+(color<<16)+(50<<8)+((int)(mag*40));
    gl4dpSetColor(_mobiles[i].color);
    gl4dpFilledCircle(_mobiles[i].x+ centreX+net.width*20+50*1, _mobiles[i].y+50, _mobiles[i].r);
  }
  for(i = 0; i < _nbMobiles; i++) {
    color=(GLuint)120*(net.nodes[i].etiq);
	_mobiles[i].color = (255<<24)+(color<<16)+(50<<8)+color;
    gl4dpSetColor(_mobiles[i].color);
    gl4dpFilledCircle(_mobiles[i].x+ centreX + 2*net.width*20+50*2, _mobiles[i].y+50, _mobiles[i].r);
  }

}

static void idle(void) {
//  mobileMove();
}

static void draw(void) {
  gl4dpClearScreenWith (RGB(121, 150, 150));
  mobileDraw();
  gl4dpUpdateScreen(NULL);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments utilisés.*/
static void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
/*!\brief créé la fenêtre, un screen 2D, place la fonction display et
 * appelle la boucle principale.*/
int main(int argc, char ** argv) {
  /* fenêtre positionnée en (10, 10), ayant une dimension de (512, 512) et un titre "GL4D-Primitives" */
  if(!gl4duwCreateWindow(argc, argv, "GL4D-BaBalle", 10, 10, 800,400, SDL_WINDOW_SHOWN))
    return 1;
  gl4dpInitScreen();
  mobileInit(100);
  atexit(quit);
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
