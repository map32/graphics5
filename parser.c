#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

void readargs(char *args, double *list){
  char *t = strtok(args, " ");
  char **whatever;
  while(t!=NULL){
    *list = strtod(t,whatever);
    //printf("%lf:d \n",*list);
    t = strtok(NULL," ");
    list++;
  }
}

/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 circle: add a circle to the edge matrix - 
	    takes 3 arguments (cx, cy, r)
	 hermite: add a hermite curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 bezier: add a bezier curve to the edge matrix -
	    takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 xrotate: create an x-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 yrotate: create an y-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 zrotate: create an z-axis rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 1 argument (theta)
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * pm,
                  screen s) {

  FILE *f;
  char line[256];
  double args[8];
  int type = -1;
  color c;
  c.red = 255;
  c.blue=0;
  c.green=0;
  
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf("%s\n",line);
    //printf(":%s:%d\n",line,type);
    if (strcmp(line, "line")==0){
      type = 0;
    } else if (strcmp(line, "circle")==0){
      type = 1;
    } else if (strcmp(line, "hermite")==0){
      type = 2;
    } else if (strcmp(line, "bezier")==0){
      type = 3;
    } else if (strcmp(line, "ident")==0){
      ident(transform);
    } else if (strcmp(line, "scale")==0){
      type = 4;
    } else if (strcmp(line, "translate")==0){
      type = 5;
    } else if (strcmp(line, "xrotate")==0){
      type = 6;
    } else if (strcmp(line, "yrotate")==0){
      type = 7;
    } else if (strcmp(line, "zrotate")==0){
      type = 8;
    } else if (strcmp(line, "apply")==0){
      matrix_mult(transform,pm);
    } else if (strcmp(line, "display")==0){
      //printf("fdssdfsf");
      clear_screen(s);
      draw_polygons(pm,s,c);
      display(s);
    } else if (strcmp(line, "save")==0){
      type = 9;
    } else if (strcmp(line, "clear")==0){
      pm->lastcol = 0;
    } else if (strcmp(line, "sphere")==0){
      type = 11;
    } else if (strcmp(line, "box")==0){
      type = 12;
    } else if (strcmp(line, "torus")==0){
      type = 13;
    } else if (strcmp(line, "quit")==0){
      break;
    } else {
      if(type==-1) continue;
      readargs(line,args);
      switch (type){
      case 0:
	add_edge(pm,args[0],args[1],args[2],args[3],args[4],args[5]);
	break;
      case 1:
	add_circle(pm,args[0],args[1],args[2],0.01);
        break;
      case 2:
	add_curve(pm,args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],0.01,0);
        break;
      case 3:
	add_curve(pm,args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],0.01,1);
        break;
      case 4:
	matrix_mult(make_scale(args[0],args[1],args[2]),transform);
        break;
      case 5:
	matrix_mult(make_translate(args[0],args[1],args[2]),transform);
        break;
      case 6:
	matrix_mult(make_rotX(args[0]/180.*M_PI),transform);
        break;
      case 7:
	matrix_mult(make_rotY(args[0]/180.*M_PI),transform);
        break;
      case 8:
	matrix_mult(make_rotZ(args[0]/180.*M_PI),transform);
        break;
      case 9:
	//printf("dfg");
	draw_polygons(pm,s,c);
	save_extension(s,line);
	break;
      case 11:
        add_sphere(pm,args[0],args[1],args[2],0.05);
        break;
      case 12:
	add_box(pm,args[0],args[1],args[2],args[3],args[4],args[5]);
        break;
      case 13:
        add_torus(pm,args[0],args[1],args[2],args[3],0.05);
        break;
      default:
	printf("invalid command/n");
      }
      type = -1;
    }
  }
  print_matrix(transform);
  fclose(f);
}
