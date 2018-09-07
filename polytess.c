// Base code provided by Dr. Pounds
// Developed by Mark Gomes
// 09/05/18

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {false, true} bool;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef struct vertex
{
	int x;
	int y;
	struct vertex *next;
} vertex;

typedef struct line
{
    float slope;
    float yIntercept;
} line;

void push(vertex *startVertex, vertex input);
void freeVerticesMemory(vertex *startVertex);
bool checkIntersection(vertex input);

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

vertex *startVertex;

// Functions

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
    glClear(GL_COLOR_BUFFER_BIT);  		/*clear the window */

    if (!COLORS_DEFINED) 
	{
       red   = 255;
       green = 0; 
       blue  = 0;
    }

    glColor3ub( red, green, blue ); 
    glFlush(); 						/* clear buffers */
}


void drawVertex( int x, int y )
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

void drawOutline( void )
{
    glClear ( GL_COLOR_BUFFER_BIT );

    glBegin ( GL_LINE_LOOP );
	    vertex *current = startVertex;
    	while (current -> next != NULL)
	    {
            glVertex2i ( current -> next -> x, current -> next -> y );
            current = current -> next;
	    }
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
    	//printf ("%d   %d\n", x, sy);
        drawVertex( x, sy );
		
		//Add point to list of vertices
		vertex input = {x, sy, NULL};
		if(!checkIntersection(input))
		{
			push(startVertex, input);
		}
		else
		{
			printf("Intersection : Point Not Accepted");
		}
    }

  	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        //printf ("%d   %d\n", x, sy);
        //eraseBox( x, sy );

        //Display outline of the polygon
        drawOutline();
    }
  
  	if ( button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN )
    {
        printf ("%d   %d\n", x, sy);
        clearBox();
    }
}


void keyboard( unsigned char key, int x, int y )
{ 
	if ( key == 'q' || key == 'Q') 
	{
		freeVerticesMemory(startVertex);
		exit(0);
	}
}

bool checkIntersection(vertex input)
{
	int count = 0;	//Count number of vertices
    //Find end of linked list vertices
    vertex *current = startVertex;
    while(current -> next != NULL)
    {
        current = current -> next;
		count++;
    }
	
	//Check if enough points entered
	if(count < 3)
	{
		return false;
	}

    //Calculate line for last two vertices
    line newLine; 
    newLine.slope = (input.y - current -> y) / (input.x - current -> x);
    newLine.yIntercept = (newLine.slope * -current -> x) + current -> y;

    //Check new line with all lines for intersection
    current = startVertex -> next;
    line oldLine;
    while(current -> next != NULL)
    {
        //Calculate line for current and next vectices
        oldLine.slope = (current -> next -> y - current -> y) / (current -> next -> x - current -> x);
        oldLine.yIntercept = (oldLine.slope * -current -> x) + current -> y;

        //Check intersection
        float x = (oldLine.yIntercept - newLine.yIntercept) / (newLine.slope - oldLine.slope);
        float y = (newLine.slope * x) + newLine.yIntercept;
        
        //If x is greater than min of two line points and less than max of two line points
        //Same for y; if it is then intersection
        if(x > min(current -> x , current -> next -> x) && x < max(current -> x , current -> next -> x))
		{
			if(y > min(current -> y, current -> next -> y) && y < max(current -> y, current -> next -> y))
			{
				return true;
				break;
			}
		}

        //Move to nect vertex and repeat
        current = current -> next;
    }
	return false;
}

// Add a vertex to end of vertices
void push(vertex *startVertex, vertex input)
{
	//Find end of the linked list vertices
	vertex *current = startVertex;
	while(current -> next != NULL)
	{
		current = current -> next;
	}
	//Add input to end of link list
	
	vertex *newVertex = (vertex*)malloc(sizeof(vertex));
	newVertex -> x = input.x;
	newVertex -> y = input.y;
	newVertex -> next = NULL;
	current -> next = newVertex;

	current = current -> next;
	//Print out inputed vertex
	printf("( %i , %i ) \n", current -> x, current -> y);

	//Print out updated list of vertices
	/*
	current = startVertex;
	while (current -> next != NULL)
	{
		printf("( %i , %i ) \n", current -> next -> x, current -> next -> y);
		current = current -> next;
	}
	*/
}

// Insert an vertex from verticies
void insert(vertex *firstVertex, vertex *secondVertex, vertex input)
{
    vertex *newVertex = (vertex*)malloc(sizeof(vertex));    //Create new vertex from input
    newVertex -> x = input.x;
    newVertex -> y = input.y;

    newVertex -> next = secondVertex;       //Connect new vertex to second vertex

    firstVertex -> next = newVertex;        //Connect first vertex to new vertex
}

// Delete a vertex into verticies


// Free up all allocated memory by list of verticies
void freeVerticesMemory(vertex *startVertex)
{
	vertex *tempVertex = startVertex -> next;
    
	while(startVertex -> next != NULL)
	{
		tempVertex = startVertex -> next;				//Set temp node to current node

		startVertex = startVertex -> next;		//Move pointing node to next node

		free(tempVertex);						//Free up temp node
        tempVertex = NULL;
	}
}

int main(int argc, char** argv)
{
	vertex buffer = {0, 0, NULL};
	startVertex = &buffer;

    myglutInit(argc,argv); 		/* Set up Window */
    myInit(); 					/* set attributes */
    
    glutMouseFunc(mouse);  			/* Define Mouse Handler */
    glutKeyboardFunc(keyboard);	 	/* Define Keyboard Handler */

    glutDisplayFunc(display); 		/* Display callback invoked when window opened */
    glutMainLoop(); 				/* enter event loop */
}
