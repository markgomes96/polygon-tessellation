// Base code provided by Dr. Pounds
// Developed by Mark Gomes
// This program handles polygon creation and tesselation
// It will accept CCW and CW drawn polygons
// 09/26/18

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef enum {false, true} bool;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef struct point		//stores data for single point on screen
{
	int x;
	int y;
	struct point *next;
} point;

typedef struct vector		//stores data for a 3D vector used for calcualtions
{
	int x;
	int y;
	int z;
} vector;

typedef struct triangle		//stores vertexes for a triangle used for tesselation
{
	point v1;
	point v2;
	point v3;
	double area;
} triangle;

void push(point input);
bool checkPointValid(point input, bool lastPoint);
void freeVerticesMemory(point *startPoint);
bool sharePoint(point p1, point p2);
double vectorAngle(point p1, point p2, point p3);
bool checkIntersection(point p1, point p2, point p3, point p4);
void tesselatePolygon(bool drawFlag);
int dotProduct(vector v1, vector v2);
double vectorMagnitude(vector v1);
vector crossProduct(vector v1, vector v2);
void delay(float secs);
int sign(int num);

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

point *startPoint;			//head pointer for linked list of points
int verticesCount = 0;		//number of vertices points accepted
bool allPointsInputed = false;

// Functions

void myglutInit( int argc, char** argv )
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 		/* default, not needed */
    glutInitWindowSize(WINDOW_MAX_X,WINDOW_MAX_Y); 		/* set pixel window */
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y); 		/* place window top left on display */
    glutCreateWindow("Polygon Tesselation");	 /* window title */
}


void myInit(void)
{
	/* standard OpenGL attributes */

	glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
	glColor3f(1.0, 0.0, 0.0); /* draw in red */
	glPointSize(10.0);
	glLineWidth(3.0);

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

void drawPoint( int x, int y )		//Draw point onto the screen
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

void drawOutline( void )		//Draw outline of polygon with line loops
{
    glClear ( GL_COLOR_BUFFER_BIT );
	glColor3f (1.0, 0.0, 0.0);			//change color to red

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

void drawPolygon( void )		//Draw filled polygon without tesselation
{
	glClear ( GL_COLOR_BUFFER_BIT );
	glColor3f (1.0, 0.0, 1.0);			//change color to purple
	
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
	int sy = WORLD_COORDINATE_MAX_Y - y;	//converts mouse coordinates to screen coordinates
	
	point input = {x, sy, NULL};

	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
		//try to add point to list of vertices
        if(verticesCount > 2)
		{
			bool validPoint = false;
			if(!allPointsInputed)
			{
            	validPoint = checkPointValid(input, false);
			}
			else
			{
				validPoint = checkPointValid(input, true);
			}

			if(validPoint)
			{
				push(input);
				verticesCount++;
				drawPoint( x , sy );
			}
		}
        else
        {
            push(input);
            verticesCount++;
            drawPoint( x, sy );
        }
    }

  	if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        //Display outline of the polygon
        if(verticesCount >= 2)	
		{
			if(checkPointValid(input, true))
			{
				push(input);
				verticesCount++;
				drawPoint( x, sy );
				allPointsInputed = true;		//Add last point and allow user to draw polygons
        		drawOutline();
			}
		}
		else
		{
			printf("Please input more points \n");
		}
    }
}


void keyboard( unsigned char key, int x, int y )
{ 
	if(allPointsInputed)
	{
		// Draw polygon with no tesselation
		if ( key == 'f' || key == 'F')
		{
			drawPolygon();
		}
	
		// List out triangles used in tesselation
		if ( key == 't' || key == 'T')
		{
			tesselatePolygon(false);
		}

		// Draw polygon using tesselation
		if ( key == 'p' || key == 'P')
		{
			tesselatePolygon(true);
		}

		// Return to initial polygon outline
		if ( key == 'i' || key == 'I')
		{
			drawOutline();
		}

		// Exit the program
		if ( key == 'q' || key == 'Q') 
		{
			printf("Goodbye...\n");
			freeVerticesMemory(startPoint);
			exit(0);
		}
	}
}

bool checkPointValid(point input, bool isLastPoint)
{
	bool intersected = false;

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
        //checks intersection between last point and new point 
		if(checkIntersection(*current, *(current -> next), *lastPoint, input))
		{
			intersected = true;
			printf("( %i, %i ) -- *Intersection Detected : Point Not Accepted* \n", input.x, input.y);
			break;
		}

		current = current -> next;		//Move to next point and repeat
	}

    if(!intersected && isLastPoint)     //check intersection between new point and start point
    {
    	current = startPoint -> next -> next;
        while(current -> next != NULL)
        {
        	if(checkIntersection(*current, *(current -> next), input, *(startPoint -> next)))
            {
            	intersected = true;
                printf("( %i, %i ) -- *Intersection Detected : Point Not Accepted* \n", input.x, input.y);
                break;
            }

            current = current -> next;
        }
    }
	
	return !intersected;
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

	point *current = startPoint -> next;	//create array of all the points to implement ear clipping algorithm
	int index = 0;
	while(current -> next != NULL)
	{
		pointList[index] = *current;
		index++;
		current = current -> next;
	}
	pointList[index] = *current;

	current = startPoint -> next;			//create array with all points and 1st point at end to check intersections
	index = 0;
	while(current -> next != NULL)
	{
		intersectPL[index] = *current;
		index++;
		current = current -> next;
	}
	intersectPL[index] = *current;
	intersectPL[index+1] = *(startPoint -> next);

	//Earclipping algorithm
	point fp, mp, ep;				//first point, midpoint, endpoint
	vector v1, v2, cp;				//vectors to calculate the crossproduct
	bool intersectFlag = false;
	int direction = 0;			//get direction of first 3 points
	while(vertCount > 3)
	{
		//Get next 3 points based on point index
		fp = pointList[pi];
		mp = pointList[pi+1];
		ep = pointList[pi+2];

        	//check the cross product to see if points go counter clockwise
        	v1 = (vector){.x = (mp.x - fp.x), .y = (mp.y - fp.y), .z = 0};
        	v2 = (vector){.x = (mp.x - ep.x), .y = (mp.y - ep.y), .z = 0};
       		cp = crossProduct(v1, v2);
		
		/*
 		*Check start direction first, store as 1 or -1
 		*/
		if(direction == 0)
		{
			direction = sign(cp.z);
		}

		//check if current 3 points are going in initial polygon direction
        	if(sign(cp.z) == direction)
        	{
			//flag to check if line cuases an intersections
			intersectFlag = false;
			
			for(int i = 0; i < verticesCount; i++)
			{
				//makes sure lines with same points aren't tested for intersection
                		if(sharePoint(ep, intersectPL[i]) || sharePoint(ep, intersectPL[i+1]) || sharePoint(fp, intersectPL[i]) || sharePoint(fp, intersectPL[i+1]))
               			{
             				//lines share a point
             				printf("share point \n");
                		}
				else if(checkIntersection(intersectPL[i], intersectPL[i+1], ep, fp))	//checks if interior line intersects anterior lines
				{
					printf("intersect anterior line \n");
					intersectFlag = true;
					break;
				}
			}
	
			if(!intersectFlag)		//check if interior angle is smaller than anterior angle
			{
				v1 = (vector){.x = (ep.x - mp.x), .y = (ep.y - mp.y), .z = 0};
				v2 = (vector){.x = (ep.x - pointList[pi+3].x), .y = (ep.y - pointList[pi+3].y), .z = 0};

				printf("fire1 \n");

				if(sign(crossProduct(v1, v2).z) == direction)			//check if next two lines are CCW
				{
					printf("inner triangle : %1f		outer triangle : %1f", vectorAngle(mp, ep, fp), vectorAngle(mp, ep, pointList[pi+3]));

					if(vectorAngle(mp, ep, fp) > vectorAngle(mp, ep, intersectPL[pi+3]))		//check if line is an interior line
					{
						intersectFlag = true;
					}
				}
			}

			printf("Intersected : %s \n", intersectFlag ? "true" : "false");
		
			//check if the lines intersect
			if(!intersectFlag)
			{
				//add triangle to triangle list
                		triangleList[ti] = (triangle){.v1 = fp, .v2 = mp, .v3 = ep, .area = 0};
                		ti++;

                		//remove the midpoint
				vertCount--;
	
                		//move up all the points that aren't null
                		for(int i = pi+1; i < vertCount; i++)
                		{
                    			pointList[i] = pointList[i+1];
                		}
				pointList[vertCount] = (point){.x = 0, .y = 0, .next = NULL};

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
			vertCount--;

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
        	}
	}

	//Add last 3 vertices
	triangleList[ti] = (triangle){.v1 = pointList[0], .v2 = pointList[1], .v3 = pointList[2], .area = 0};
	ti++;

	//either draw the tesselated filled polygon or draw triangle outlines and list out the triangle information
	glClear ( GL_COLOR_BUFFER_BIT );
	if(drawFlag)
	{	
		glColor3f(0.0, 0.0, 1.0);	//change color to blue
		for(int i = 0; i < ti; i++)
		{
			glBegin ( GL_POLYGON );
				glVertex2i ( triangleList[i].v1.x, triangleList[i].v1.y );
				glVertex2i ( triangleList[i].v2.x, triangleList[i].v2.y );	
        		glVertex2i ( triangleList[i].v3.x, triangleList[i].v3.y );		
			glEnd();
		}

		glFlush();
	}
	else	//list out tesslated triangle information
	{
		glColor3f(0.0, 1.0, 0.0);	//change color to green
		printf("Number of Triangles : %d \n", ti);
		for(int i = 0; i < ti; i++)
		{
			//calculate the area
			vector v1 = (vector){.x = (triangleList[i].v2.x - triangleList[i].v1.x), .y = (triangleList[i].v2.y - triangleList[i].v1.y), .z = 0};
			vector v2 = (vector){.x = (triangleList[i].v2.x - triangleList[i].v3.x), .y = (triangleList[i].v2.y - triangleList[i].v3.y), .z = 0};
			triangleList[i].area = ( abs( (double)(crossProduct(v1, v2).z) / 2.0 ) );

			//print the triangle information
			printf("triangle %d : (%d, %d) ; (%d, %d) ; (%d, %d)	--	 Area : %.2f \n", (i+1), triangleList[i].v1.x, triangleList[i].v1.y, 
					triangleList[i].v2.x, triangleList[i].v2.y, triangleList[i].v3.x, triangleList[i].v3.y, triangleList[i].area);

			//display triangle outline on screen
			glBegin ( GL_LINE_LOOP );
				glVertex2i ( triangleList[i].v1.x, triangleList[i].v1.y );
				glVertex2i ( triangleList[i].v2.x, triangleList[i].v2.y );
				glVertex2i ( triangleList[i].v3.x, triangleList[i].v3.y );
			glEnd();

			glFlush();
			delay(0.3);		//draws triangles with delay for cool effect
		}
	}
}

void delay(float secs)		//delays program by certain amount of time
{
	clock_t endTime = clock() + (CLOCKS_PER_SEC * secs);
	clock_t currentTime = 0;

	while(clock() < endTime) {}
}

int sign(int num)
{
	if(num > 0)
		return 1;
	else if (num < 0)
		return -1;
	else
		return 0;
}

bool sharePoint(point p1, point p2)		//determines if two points are the same
{
    if(p1.x == p2.x || p1.y == p2.y)
        return true;
    else
        return false;
}

double vectorAngle(point fp, point mp, point ep)
{
    //find the angle of two vectors sharing middle point
	vector v1 = (vector){.x = (mp.x - fp.x), .y = (mp.y - fp.y), .z = 0};
	vector v2 = (vector){.x = (mp.x - ep.x), .y = (mp.y - ep.y), .z = 0};

	double va = acos( ((double)(dotProduct(v1, v2))) / (vectorMagnitude(v1) * vectorMagnitude(v2)) );
	return va;
}   

bool checkIntersection(point p1, point p2, point p3, point p4)
{
	float ADet = 0;
	float tADet = 0;
	float tBDet = 0;
	float tA = 0;
	float tB = 0;

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

double vectorMagnitude(vector v1)
{
    double vm;
    vm  = sqrt((v1.x * v1.x) + (v1.y * v1.y));
    return vm;
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
void push(point input)
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
	printf("( %i , %i ) -- *Point Accepted* \n", current -> x, current -> y);
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
