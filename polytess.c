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

typedef struct vector
{
	int x;
	int y;
	int z;
} vector;

typedef struct triangle
{
	point v1;
	point v2;
	point v3;
	float area;
} triangle;

void push(point *startPoint, point input);
void freeVerticesMemory(point *startPoint);
bool isSamePoint(point p1, point p2);
bool checkIntersection(point p1, point p2, point p3, point p4);
void tesselatePolygon(bool drawFlag);
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
int verticesCount = 0;

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
	    point *current = startPoint -> next;
    	while (current -> next != NULL)
	    {
            glVertex2i ( current -> x, current -> y );
            current = current -> next;
	    }
		glVertex2i ( current -> x, current -> y);
    glEnd();

    glFlush();
}

void drawPolygon( void )
{
	glClear ( GL_COLOR_BUFFER_BIT );
	
	glBegin ( GL_POLYGON );
		point *current = startPoint -> next;
		while (current -> next != NULL)
		{
			glVertex2i ( current ->  x, current -> y );
			current = current -> next;
		}
		glVertex2i ( current -> x, current -> y);
	glEnd();
	
	glFlush();
}

void mouse( int button, int state, int x, int y )
{ 
	int sy = WORLD_COORDINATE_MAX_Y - y;
	
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
		//Add point to list of vertices
		point input = {x, sy, NULL};

        bool intersected = false;
		if(verticesCount > 2)
		{
			//Find end of linked list vertices
    		point *current = startPoint;
    		while(current -> next != NULL)
    		{
        		current = current -> next;
    		}
			point *lastPoint = current;
			current = startPoint -> next;

			while(current -> next -> next != NULL)			//Check if new point causes intersection
			{
				if(checkIntersection(*current, *(current -> next), *lastPoint, input))
				{
					intersected = true;
                    printf("Intersection Detected : Point Not Accepted\n");
					break;
				}

				current = current -> next;		//Move to next point and repeat
			}

            if(!intersected)
            {
                push(startPoint, input);
                verticesCount++;
                drawPoint( x, sy );
            }
		}
        else
        {
            push(startPoint, input);
            verticesCount++;
            drawPoint( x, sy );
        }
    }

  	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        //Display outline of the polygon
        if(verticesCount > 1)
        	drawOutline();
    }
}


void keyboard( unsigned char key, int x, int y )
{ 
	// Draw polygon with no tesselation
	if ( key == 'f' || key == 'F')
	{
		if(verticesCount > 2)
			drawPolygon();
		else
			printf("Please input more points\n");
	}
	
	// List out triangles used in tesselation
	if ( key == 't' || key == 'T')
	{
		if(verticesCount > 2)
			tesselatePolygon(false);
		else
			printf("Please input more points\n");
	}

	// Draw polygon using tesselation
	if ( key == 'p' || key == 'P')
	{
		if(verticesCount > 2)
			tesselatePolygon(true);
		else
			printf("Please input more points\n");
	}

	// Return to initial polygon outline
	if ( key == 'i' || key == 'I')
	{
		if(verticesCount > 2)
			drawOutline();
		else
			printf("Please input more points\n");
	}

	// Exit the program
	if ( key == 'q' || key == 'Q') 
	{
		printf("Goodbye...\n");
		freeVerticesMemory(startPoint);
		exit(0);
	}
}

void tesselatePolygon(bool drawFlag)
{
	triangle triangleList[verticesCount-1];
    int ti = 0;
    int vertCount = verticesCount;
    int pi = 0;

    //Move all points over to an array
    point pointList[verticesCount];
	point intersectPL[verticesCount+1];

    point *current = startPoint -> next;
    int index = 0;
    while(current -> next != NULL)
    {
        pointList[index] = *current;
        index++;
        current = current -> next;
    }
	pointList[index] = *current;

	current = startPoint -> next;
	index = 0;
	while(current -> next != NULL)
	{
		intersectPL[index] = *current;
		index++;
		current = current -> next;
	}
	intersectPL[index] = *current;
	intersectPL[index+1] = *(startPoint -> next);

	//print out all points in pointlist
	printf("Vertices count : %d \n", vertCount);
	for(int i = 0; i < vertCount; i++)
	{
		printf("%d : (%d, %d) \n", i, pointList[i].x, pointList[i].y);
	}

    //Earclipping algorithm
    point fp, mp, ep;				//first point, midpoint, endpoint
    vector v1, v2, cp;				//vectors to calculate the crossproduct
	bool intersectFlag;
    while(vertCount > 3)
    {
		//Get next 3 points based on point index
		fp = pointList[pi];
		mp = pointList[pi+1];
		ep = pointList[pi+2];

        //check the cross product to see if points go counter clockwise
        v1 = (vector){.x = (fp.x - mp.x), .y = (fp.y - mp.y), .z = 0};
        v2 = (vector){.x = (ep.x - mp.x), .y = (ep.y - mp.y), .z = 0};
        cp = crossProduct(v1, v2);
        if(cp.z < 0)
        {
			//Check if line from last point to first point intersects any other line
			intersectFlag = false;
			/*
			current = startPoint -> next;
			while(current -> next != NULL)
			{
				if(checkIntersection(*current, *(current -> next), ep, fp))
				{
					intersectFlag = true;
					break;
				}
				current = current -> next;
			}
			*/
			
			for(int i = 0; i < verticesCount; i++)
			{
				//check if sames line is being tested
				if((isSamePoint(ep, intersectPL[i]) || isSamePoint(ep, intersectPL[i+1])) && (isSamePoint(fp, intersectPL[i]) || isSamePoint(fp, intersectPL[i+1])))
				{
					printf("Two lines are the same \n");
				}
				else if(checkIntersection(intersectPL[i], intersectPL[i+1], ep, fp))
				{
					intersectFlag = true;
					printf("intersect flag: %d \n", intersectFlag);
					break;
				}
				printf("intersect flag: %d \n", intersectFlag);
			}

			printf("Final intersect flag: %d \n", intersectFlag);

			//exit(0);

            //check if the lines intersect
            if(!intersectFlag)
            {
                //add triangle to triangle list
                triangleList[ti] = (triangle){.v1 = fp, .v2 = mp, .v3 = ep, .area = 0};
                ti++;

                //remove the midpoint
                //pointList[pi+1] = NULL;
				vertCount--;

				//print all points in array
				printf("Point Array check 1\n");
				for(int i = 0; i < verticesCount; i++)
				{
					printf("%d : point (%d, %d) \n", i, pointList[i].x, pointList[i].y);
				}
	
                //move up all the points that aren't null
                for(int i = pi+1; i < vertCount; i++)
                {
                    pointList[i] = pointList[i+1];
                }
				pointList[vertCount] = (point){.x = 0, .y = 0, .next = NULL};

				//print all points in array
				printf("Point Array check 2\n");
				for(int i = 0; i < verticesCount; i++)
				{
					printf("%d : point (%d, %d) \n", i, pointList[i].x, pointList[i].y);
				}

				printf("fire 1 : vertCount: %d  ;   ti: %d \n", vertCount, ti);

				//return to first 3 points
				pi = 0;
            }
			else
			{
				//move to the next set of 3 points
				pi++;
			}
        }
        else if(cp.z == 0)
        {
            //remove the midpoint
            //pointList[pi+1] = NULL;
			vertCount--;
			printf("fire 2 : vertCount: %d \n", vertCount);

            //move up all points that aren't null
            for(int i = pi+1; i < vertCount; i++)
            {
                pointList[i] = pointList[i+1];
            }
        }
        else
        {
            //move to the next set of 3 points 
            pi++;
			printf("fire 3 : pi : %d \n", pi);
        }
    }

	//Add last 3 vertices
	triangleList[ti] = (triangle){.v1 = pointList[0], .v2 = pointList[1], .v3 = pointList[2], .area = 0};
	ti++;

	//draw the list of triangles if drawFlag is set
	if(drawFlag)
	{
		printf("ti : %d \n", ti);
		for(int i = 0; i < ti; i++)
		{
			printf("triangle %d : (%d, %d) ; (%d, %d) ; (%d, %d) \n", i, triangleList[i].v1.x, triangleList[i].v1.y, 
				triangleList[i].v2.x, triangleList[i].v2.y, triangleList[i].v3.x, triangleList[i].v3.y);
		}

		glClear ( GL_COLOR_BUFFER_BIT );

		for(int i = 0; i < ti; i++)
		{
			glBegin ( GL_POLYGON );
				glVertex2i ( triangleList[i].v1.x, triangleList[i].v1.y );
				//printf("point (%d, %d)   ", triangleList[i].v1.x, triangleList[i].v1.y);

				glVertex2i ( triangleList[i].v2.x, triangleList[i].v2.y );	
				//printf("point (%d, %d)   ", triangleList[i].v2.x, triangleList[i].v2.y);
		
        		glVertex2i ( triangleList[i].v3.x, triangleList[i].v3.y );		
				//printf("point (%d, %d) \n", triangleList[i].v3.x, triangleList[i].v3.y);
			glEnd();
		}

		glFlush();
	}   
}

bool isSamePoint(point p1, point p2)
{
	if(p1.x == p2.x && p1.y == p2.y)
		return true;
	else
		return false;
}

bool checkIntersection(point p1, point p2, point p3, point p4)
{
	float ADet = 0;
	float tADet = 0;
	float tBDet = 0;
	float tA = 0;
	float tB = 0;

	printf("point 1 : (%d, %d)	point 2 : (%d, %d)	point 3 : (%d, %d)	point 4 : (%d, %d) \n", p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y);

	ADet = (float)( (p2.x - p1.x) * (-(p4.y - p3.y)) ) - ( (p2.y - p1.y) * (-(p4.x - p3.x)) );
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

	tADet = (float)( (p3.x - p1.x) * (-(p4.y - p3.y)) ) - ( (p3.y - p1.y) * (-(p4.x - p3.x)) );
	tBDet = (float)( (p2.x - p1.x) * (p3.y - p1.y) ) - ( (p2.y - p1.y) * (p3.x - p1.x) );

	tA = (float)tADet / (float)ADet;
	tB = (float)tBDet / (float)ADet;

	if( (tA >= 0 && tA <= 1) && (tB >= 0 && tB <= 1) )
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
