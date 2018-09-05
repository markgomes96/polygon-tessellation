// Base code provided by Dr. Pounds
// Developed by Mark Gomes
// 09/05/18

#include <GL/glut.h>
#include <stdio.h>

typedef struct vertex
{
	int x;
	int y;
	struct vertex *next;
} vertex;

// These are defined in a global scope

GLubyte red, green, blue;
int COLORS_DEFINED;

// Specity the values to place and size the window on the screen

const int WINDOW_POSITION_X = 100;
const int WINDOW_POSITION_Y = 100;
const int WINDOW_MAX_X = 800;
const int WINDOW_MAX_Y = 800;

// Specify the coordinate ranges for the world coordinates in the 2D Frame

const float WORLD_COORDINATE_MIN_X = 0.0;
const float WORLD_COORDINATE_MAX_X = 800.0;
const float WORLD_COORDINATE_MIN_Y = 0.0;
const float WORLD_COORDINATE_MAX_Y = 800.0;

// Global variables to store vertex information

vertex * startvertex;

void myglutInit( int argc, char** argv )
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); /* default, not needed */
    glutInitWindowSize(WINDOW_MAX_X,WINDOW_MAX_Y); /* set pixel window */
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y); /* place window top left on display */
    glutCreateWindow("Polygon Tesselation"); /* window title */
}


void myInit(void)
{
	/* standard OpenGL attributes */

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
	glColor3f(1.0, 0.0, 0.0); /* draw in red */
	glPointSize(10.0);

	COLORS_DEFINED = 0;

	/* set up viewing window with origin lower left */

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(WORLD_COORDINATE_MIN_X, WORLD_COORDINATE_MAX_X,
                 WORLD_COORDINATE_MIN_Y, WORLD_COORDINATE_MAX_Y);
    glMatrixMode(GL_MODELVIEW);
}


void display( void )
{
	/* define a point data type */

    //typedef GLfloat point[2];     

    // point p; 							/* A point in 2-D space */

    glClear(GL_COLOR_BUFFER_BIT);  		/*clear the window */

    if (!COLORS_DEFINED) 
	{
       red   = 255;
       green = 0; 
       blue  = 0;
    }

    glColor3ub( red, green, blue ); 

    /* define point */

    //p[0] = 100; 
    //p[1] = 100;
   
    /* plot new point */
    /*
    glBegin(GL_POINTS);
    	glVertex2fv(p); 
    glEnd();
    */
    glFlush(); 						/* clear buffers */
}


void drawBox( int x, int y )
{
    typedef GLfloat point[2];     
    point p;
 
    glColor3ub( red, green, blue );
   
    p[0] = x;
    p[1] = y;  
    
        glBegin(GL_POINTS);
            glVertex2fv(p); 
        glEnd();
        glFlush();
}


void eraseBox( int x, int y )
{
    typedef GLfloat point[2];     
    point p;

    glColor3f( 1.0, 1.0, 1.0 );

    p[0] = x;
    p[1] = y;  

    glBegin(GL_POINTS);
      glVertex2fv(p); 
    glEnd();
    glFlush();
}


void clearBox()
{
       glClear(GL_COLOR_BUFFER_BIT); 
       glFlush();
}


void mouse( int button, int state, int x, int y )
{ 
	int sy = WORLD_COORDINATE_MAX_Y - y;
	
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
    	printf ("%d   %d\n", x, sy);
        drawBox( x, y );
		
		//Add point to list of vertices
		vertex input = {x, sy, NULL};
		pushvertex(startvertex, input);
    }

  	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        printf ("%d   %d\n", x, sy);
        eraseBox( x, y );
    }
  
  	if ( button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN )
    {
        printf ("%d   %d\n", x, sy);
        clearBox();
    }
}


void keyboard( unsigned char key, int x, int y )
{ 
  if ( key == 'q' || key == 'Q') exit(0);
}

void pushvertex(vertex **startvertex, vertex input)
{
	//Find end of the linked list
	vertex *current = startvertex;
	while(current -> next != NULL)
	{
		current = current -> next;
	}

	//Add input to end of link list
	vertex newvertex;
	current -> next = newvertex;
	current -> next -> x = input.x;
	current -> next -> y = input.y;
	current -> next -> next = NULL;

	//Print out updated list of vertices
	current = startvertex -> next;
	do
	{
		printf("( %i , %i ) \n", current -> x, current ->y);
		if(current -> next != NULL)
		{
			current = current -> next;
		}
	} while (current -> next != NULL)
}

int main(int argc, char** argv)
{
	vertex buffer = {0, 0, NULL};
	startvertex = buffer;

    myglutInit(argc,argv); 		/* Set up Window */
    myInit(); 					/* set attributes */
    
    glutMouseFunc(mouse);  			/* Define Mouse Handler */
    glutKeyboardFunc(keyboard);	 	/* Define Keyboard Handler */

    glutDisplayFunc(display); 		/* Display callback invoked when window opened */
    glutMainLoop(); 				/* enter event loop */
}