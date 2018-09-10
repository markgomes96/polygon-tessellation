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

typedef struct point
{
	int x;
	int y;
	struct point *next;
} point;

/*
typedef struct line
{
    float slope;
    float yIntercept;
} line;
*/

typedef struct vector
{
	int x;
	int y;
	int z;
} vector;

void push(point *startPoint, point input);
void freeVerticesMemory(point *startPoint);
bool checkIntersection(point p1, point p2, point p3, point p4);
int dotProuct(vector v1, vector v2);
vector crossProduct(vector v1, vector v2);

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

// Global variables to store point information

point *startPoint;
int count = 0;

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


void drawPoint( int x, int y )
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
	    point *current = startPoint;
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
        drawPoint( x, sy );
		
		//Add point to list of vertices
		point input = {x, sy, NULL};

		if(count < 3)
		{
			//Find end of linked list vertices
    		point *current = startPoint;
    		while(current -> next != NULL)
    		{
        		current = current -> next;
    		}
			point *lastPoint = current;
			current = startPoint -> next;
			bool intersected = false;

			while(current -> next != NULL)			//Check if new point causes intersection
			{
				if(checkIntersection(*current, *(current -> next), *lastPoint, input))
				{
					intersected = true;
					break
				}

				current = current -> next;		//Move to next point and repeat
			}
		}

		if(count < 3)
		{
			push(startPoint, input);
			count++;
		}
		else if(!intersected)
		{
			push(startPoint, input);
			count++;
		}
		else
		{
			printf("Intersection Detected : Point Not Accepted");
		}
    }

  	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        //printf ("%d   %d\n", x, sy);
        //eraseBox( x, sy );

        //Display outvector of the polygon
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
		freeVerticesMemory(startPoint);
		exit(0);
	}
}

/*
bool checkIntersection(point input)
{
	int count = 0;	//Count number of vertices
    //Find end of linked list vertices
    point *current = startPoint;
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

    //Calculate vector for last two vertices
    vector newLine; 
    newLine.slope = (input.y - current -> y) / (input.x - current -> x);
    newLine.yIntercept = (newLine.slope * -current -> x) + current -> y;

    //Check new vector with all vectors for intersection
    current = startPoint -> next;
    vector oldLine;
    while(current -> next != NULL)
    {
        //Calculate vector for current and next vectices
        oldLine.slope = (current -> next -> y - current -> y) / (current -> next -> x - current -> x);
        oldLine.yIntercept = (oldLine.slope * -current -> x) + current -> y;

        //Check intersection
        float x = (oldLine.yIntercept - newLine.yIntercept) / (newLine.slope - oldLine.slope);
        float y = (newLine.slope * x) + newLine.yIntercept;
        
        //If x is greater than min of two vector points and less than max of two vector points
        //Same for y; if it is then intersection
        if(x > min(current -> x , current -> next -> x) && x < max(current -> x , current -> next -> x))
		{
			if(y > min(current -> y, current -> next -> y) && y < max(current -> y, current -> next -> y))
			{
				return true;
				break;
			}
		}

        //Move to nect point and repeat
        current = current -> next;
    }
	return false;
}
*/

bool checkIntersection(point p1, point p2, point p3, point p4)
{
	float a0Det = 0;
	float a1Det = 0;
	float ADet = 0;
	float uA = 0;
	float uB = 0;

	ADet = ( (p2.x - p1.x) * (-(p4.y - p3.y)) ) - ( (p2.y - p1.y) * (-(p4.x - p3.x)) );
	if(ADet == 0)		//Check if lines are parallel
	{
		float slope = (p2.y - p1.y) / (p2.x - p1.x);
		float yIntA = (slope * -p1.x) + p1.y;
		float yIntB = (slope * -p3.x) + p3.y;
		
		if(yIntA == yIntB)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	a0Det = ( (p3.x - p1.x) * (-(p4.y - p3.y)) ) - ( (p3.y - p1.y) * (-(p4.x - p3.x)) );
	a1Det = ( (p2.x - p1.x) * (p3.y - p1.y) ) - ( (p2.y - p1.y) * (p3.x - p1.x) );

	uA = a0Det / ADet;
	uB = a1Det / ADet;

	if( (uA >= 0 && uA <= 1) && (uB >= 0 && uA <= 1) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

int dotProduct(vector v1, vector v2)
{
	int dp;
	dp = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	return dp;
}

vector crossProduct(vector v1, vector v2)
{
	vector cp;
	cp.x = (v1.y * v2.z) - (v2.y * v1.z);
	cp.y = (v1.x * v2.z) - (v2.x * v1.z);
	cp.z = (v1.x * v2.y) - (v2.x * v1.y);

	return cp;
}

//Add a point to end of vertices
void push(point *startPoint, point input)
{
	//Find end of the linked list vertices
	point *current = startPoint;
	while(current -> next != NULL)
	{
		current = current -> next;
	}
	//Add input to end of link list
	
	point *newPoint = (point*)malloc(sizeof(point));
	newPoint -> x = input.x;
	newPoint -> y = input.y;
	newPoint -> next = NULL;
	current -> next = newPoint;

	current = current -> next;
	//Print out inputed point
	printf("( %i , %i ) \n", current -> x, current -> y);

	//Print out updated list of vertices
	/*
	current = startPoint;
	while (current -> next != NULL)
	{
		printf("( %i , %i ) \n", current -> next -> x, current -> next -> y);
		current = current -> next;
	}
	*/
}

// Insert an point from verticies
void insert(point *firstPoint, point *secondPoint, point input)
{
    point *newPoint = (point*)malloc(sizeof(point));    //Create new point from input
    newPoint -> x = input.x;
    newPoint -> y = input.y;

    newPoint -> next = secondPoint;       //Connect new point to second point

    firstPoint -> next = newPoint;        //Connect first point to new point
}

// Delete a point into verticies


// Free up all allocated memory by list of verticies
void freeVerticesMemory(point *startPoint)
{
	point *tempPoint = startPoint -> next;
    
	while(startPoint -> next != NULL)
	{
		tempPoint = startPoint -> next;				//Set temp node to current node

		startPoint = startPoint -> next;		//Move pointing node to next node

		free(tempPoint);						//Free up temp node
        tempPoint = NULL;
	}
}

int main(int argc, char** argv)
{
	point buffer = {0, 0, NULL};
	startPoint = &buffer;

    myglutInit(argc,argv); 		/* Set up Window */
    myInit(); 					/* set attributes */
    
    glutMouseFunc(mouse);  			/* Define Mouse Handler */
    glutKeyboardFunc(keyboard);	 	/* Define Keyboard Handler */

    glutDisplayFunc(display); 		/* Display callback invoked when window opened */
    glutMainLoop(); 				/* enter event loop */
}
