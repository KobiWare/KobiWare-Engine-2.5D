#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533 //one degree in radians

float px, py, pdx, pdy, pa; //player xy, player delta xy, player angle

void drawPlayer(){
     glColor3f(1, 1, 0);
     glPointSize(8);
     glBegin(GL_POINTS);
     glVertex2i(px, py);
     glEnd(); //draw yellow dot representing player

     glLineWidth(3);
     glBegin(GL_LINES);
     glVertex2i(px, py);
     glVertex2i(px + pdx * 5, py + pdy * 5); //draw thing that casts rays out
     glEnd();
}

int mapX = 8, mapY = 8, mapS = 64;
int map[]= {
     1,1,1,1,1,1,1,1,
     1,0,1,0,0,0,0,1,
     1,0,1,0,0,0,0,1,
     1,0,1,0,0,0,0,1,
     1,0,0,0,0,0,0,1,
     1,0,0,0,0,1,0,1,
     1,0,0,0,0,0,0,1,
     1,1,1,1,1,1,1,1,
};

void drawMap2D() {
     int x, y, xo, yo;
     for (y = 0; y < mapY;y++){
          for (x = 0; x < mapX; x++){
               if(map[y*mapX+x]==1){glColor3f(1,1,1);} else {glColor3f(0,0,0);} //loops thjrough every coordinate and sets the 2d box according to if box is 1 or 0
               xo = x * mapS; //x offset is equal to x multiplied by the full map size
               yo = y * mapS; //y offset is equal to y multiplied by the full map size
               glBegin(GL_QUADS);
               glVertex2i(xo+1, yo+1);
               glVertex2i(xo+1, yo + mapS-1);
               glVertex2i(xo-1 + mapS, yo + mapS-1);
               glVertex2i(xo-1 + mapS, yo+1); //create grid like look to the 2d map
               glEnd();
          }
     }
}

float dist(float ax,float ay, float bx, float by, float ang){
     return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay))); //calculate the distance 
}

void drawRays2D() {
     int r, mx, my, mp, dof;
     float rx, ry, ra, xo, yo, disT;
     ra = pa-DR*30; //convert ray angle to player angle minus 30 degrees in radians
     if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;} //if radians is less than zero, add 2 pi, if greater subtract 2 pi
     for (r = 0; r < 90;r++){ //run through for each ray
          dof = 0; //depth of field is equal to 0 at beginning
          float disH = 1000000, hx = px, hy = py; //horizontal x and horizontal y are equal to current player x and y
          float aTan = -1 / tan(ra); //angle tangent is equal to -1 / tan(ra)
          if(ra>PI){ry=(((int)py>>6)<<6)-0.0001; //if the ray angle is greater than pi, ray y is equal to ((player y / 64)*64)-0.0001
          rx=(py-ry)*aTan+px;  //ray x is equal to (player y - ray y) multiplied by aTan and then adding player x
          yo=-64; //y offset is equal to -64
          xo=-yo*aTan;} //xo offset is equal to negative y offset multiplied by aTan
          if(ra<PI){ry=(((int)py>>6)<<6)+64;  //if ray angle is less than pi, ray y is equal to py / 64 * 64 + 64
          rx=(py-ry)*aTan+px;  //ray x is equal to player y - ray y multiplied by aTan plus player x
          yo=64; //y offset is equal to 64
          xo=-yo*aTan;} //x offset is equal to negative y offset multiplied by aTan
          if(ra==0 || ra==PI){rx=px; ry=py; dof=8;} //if the ray angle is 0 or PI, ray x is equal to player x and ry is equal to player 1 and dof is equal to 8
          while(dof<8){
               mx = (int)(rx) >> 6; my = (int)(ry)>>6; mp=my*mapX+mx;
               if(mp>0 && mp<mapX*mapY && map[mp]==1){hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=8;}
               else{rx+=xo; ry+=yo;dof+=1;}
          }

          dof = 0;
          float disV = 1000000, vx = px, vy = py;
          float nTan = -tan(ra);
          if(ra>P2 && ra<P3){rx=(((int)px>>6)<<6)-0.0001; ry=(px-rx)*nTan+py; xo=-64; yo=-xo*nTan;}
          if(ra<P2 || ra>P3){rx=(((int)px>>6)<<6)+64; ry=(px-rx)*nTan+py; xo=64; yo=-xo*nTan;}
          if(ra==0 || ra==PI){rx=px; ry=py; dof=8;}
          while(dof<8){
               mx = (int)(rx) >> 6; my = (int)(ry)>>6; mp=my*mapX+mx;
               if(mp>0 && mp<mapX*mapY && map[mp]==1){vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof=8;}
               else{rx+=xo; ry+=yo;dof+=1;}
          }
          if(disV<disH){rx=vx; ry=vy; disT=disV;}
          if(disH<disV){rx=hx; ry=hy; disT=disH;}
          glColor3f(1, 0, 0);
          glLineWidth(3);
          glBegin(GL_LINES);
          glVertex2i(px, py);
          glVertex2i(rx, ry);
          glEnd();

          int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
          int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

          float ca=pa-ra; if(ca<0) {ca+=2*PI;} if(ca>2*PI) {ca-=2*PI;} disT=disT*cos(ca);
          float lineH = (mapS * screenHeight)/disT; if(lineH>screenHeight){lineH=screenHeight;}
          float lineO=(screenHeight/4)-lineH/2;
          glLineWidth(16);glBegin(GL_LINES);glVertex2i(r*8+530,lineO);glVertex2i(r*8+530,lineH+lineO);
          glEnd();

          ra += DR; if(ra<0) {ra+=2*PI;} if(ra>2*PI) {ra-=2*PI;}
     }
}

void display() {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     drawMap2D();
     drawPlayer();
     drawRays2D();
     glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y){
     if(key=='a') {pa-=0.1; if(pa<=0){pa+=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
     if(key=='d') {pa+=0.1; if(pa>2*PI){pa-=2*PI;} pdx=cos(pa)*5; pdy=sin(pa)*5;}
     if(key=='w') {px+=pdx; py+=pdy;}
     if(key=='s') {px-=pdx; py-=pdy;}
     glutPostRedisplay();
}

void init() {
     glClearColor(0.3, 0.3, 0.3, 0);
     gluOrtho2D(0, 1024, 512, 0);
     px = 300;
     py = 300;
     pdx = cos(pa) * 5;
     pdy = sin(pa) * 5;
}

int main(int argc, char* argv[]){
     glutInit(&argc, argv);
     int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
     int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
     glutInitWindowSize(screenWidth, screenHeight);
     glutCreateWindow("KobiWare 2.5D Engine");
     init();
     glutDisplayFunc(display);
     glutKeyboardFunc(buttons);
     glutMainLoop();
}