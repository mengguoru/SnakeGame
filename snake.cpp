#include <windows.h> 
#include <glut.h>  
#include <time.h>

char title[] = "Snake Game";  
int windowWidth = 640;     
int windowHeight = 480;     
int windowPosX = 50;      
int windowPosY = 50;      
int refreshMillis = 500;
int borderX = 0, borderY = 0;
int unitX = 20, unitY = 20;

bool fullScreenMode = true; 

//for snake size
struct node {
	int x, y;
	node *pre, *next;
	node() {};

	node(int x, int y) :x(x), y(y), pre(nullptr), next(nullptr) {};
};
node *_fo;

struct snake {
	node *head, *tail;
	snake() :head(nullptr), tail(nullptr) {};
	void add(int x, int y)
	{
		if (nullptr == head)
			head = tail = new node(x, y);
		else {
			node *tmp = new node(x, y);
			tail->next = tmp;
			tmp->pre = tail;
			tail = tmp;
		}
	}
};
snake *sa;

enum Direction{up,down,left,right};
Direction dir;
int _dir[4][2] = {
	{0,1},{0,-1},{-1,0},{1,0}
};

void initGL() {
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

	borderX = windowWidth / unitX - 1, borderY = windowHeight / unitY - 1;
}
bool coverFood(int x, int y)
{
	if (!(3 <= x&&x <= borderX-3) || !(3 <= y&&y <= borderY-3))
		return true;
	for (node *p = sa->head; p->next != nullptr; p = p->next)
	{
		if (p->x == x || p->y == y)
			return true;
	}
	return false;
}
void spawnFood()
{
	srand(time(NULL));
	int x, y;
	do {
		x = rand() % borderX;
		y = rand() % borderY;
	} while (!coverFood(x, y));
	_fo->x = x;
	_fo->y = y;
}
void initSnake(int initSize)
{
	sa = new snake();
	_fo = new node();
	int tmpX = 5, tmpY = 5;
	for (int i = 0; i < initSize; i++)
	{
		sa->add(tmpX,tmpY-i);
	}
	spawnFood();
	dir = up;
}
//treat as grid
void drawGrid(int x1, int y1)
{
	int pivotX = unitX, pivotY = unitY;
	int x = pivotX + unitX*x1, y = pivotY + unitY*y1;
	glVertex2i(x - unitX / 2, y + unitY / 2);
	glVertex2i(x + unitX / 2, y + unitY / 2);
	glVertex2i(x + unitX / 2, y - unitY / 2);
	glVertex2i(x - unitX / 2, y - unitY / 2);
}
void checkEat() {
	if (sa->head->x + _dir[dir][0] == _fo->x&&sa->head->y + _dir[dir][1] == _fo->y)
	{
		node *tmp = new node(_fo->x, _fo->y);
		sa->head->pre = tmp;
		tmp->next = sa->head;
		sa->head = tmp;

		spawnFood();
	}
}
void checkFinish()
{
	if (!(0 < sa->head->x&&sa->head->x < borderX) || !(0<sa->head->y&&sa->head->y<borderY))
		exit(EXIT_FAILURE);
}
void calcul()
{
	checkEat();
	switch (dir)
	{
	case up:
		sa->tail->y = sa->head->y + 1;
		sa->tail->x = sa->head->x;
		break;
	case down:
		sa->tail->y = sa->head->y - 1;
		sa->tail->x = sa->head->x;
		break;
	case left:
		sa->tail->x = sa->head->x - 1;
		sa->tail->y = sa->head->y;
		break;
	case right:
		sa->tail->x = sa->head->x + 1;
		sa->tail->y = sa->head->y;
		break;
	}
	node *tmp2 = sa->tail->pre;
	sa->tail->pre->next = nullptr;
	sa->tail->pre = nullptr;
	sa->head->pre = sa->tail;
	sa->tail->next = sa->head;

	sa->head = sa->tail;
	sa->tail = tmp2;
	//check finish
	checkFinish();
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer
	
	//draw snake
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	for (int i = 0; i < borderX; i++)
	{
		for (int j = 0; j < borderY; j++)
		{
			if ((i != 0 && i != borderX - 1&&j!=0&&j!=borderY-1))
			{
				continue;
			}
			drawGrid(i,j);
		}
	}

	glColor3f(1, 1, 0);
	for (node *p = sa->head; p->next != nullptr; p = p->next)
	{
		if (p == sa->head)
		{
			glColor3f(0, 1, 1);
		}else
			glColor3f(1, 1, 0);
		drawGrid(p->x, p->y);
	}

	glColor3f(0, 1, 0);
	drawGrid(_fo->x, _fo->y);
	glEnd();

	glutSwapBuffers(); 

	//calcul();
}

void reshape(GLsizei width, GLsizei height) {
	double Height = windowHeight, Width = windowWidth;
	if (Height == 0)                
		Height = 1;

	glViewport(0, 0, Width, Height);      

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0.0f, (GLdouble)Width, 0.0f, (GLdouble)Height);
}

void Timer(int value) {
	glutPostRedisplay();    
	calcul();
	glutTimerFunc(refreshMillis, Timer, 0); 
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:    
		exit(0);
		break;
	}
}

void specialKeys(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:    
		fullScreenMode = !fullScreenMode;         
		if (fullScreenMode) {                     
			windowPosX = glutGet(GLUT_WINDOW_X); 
			windowPosY = glutGet(GLUT_WINDOW_Y);
			windowWidth = glutGet(GLUT_WINDOW_WIDTH);
			windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
			glutFullScreen();                     
		}
		else {                                         
			glutReshapeWindow(windowWidth, windowHeight); 
			glutPositionWindow(windowPosX, windowPosX);  
		}
		break;
	case GLUT_KEY_UP:
		if (dir != down)
			dir = up;
		break;
	case GLUT_KEY_DOWN:
		if (dir != up)dir = down;
		break;
	case GLUT_KEY_LEFT:
		if (dir != right)dir = left;
		break;
	case GLUT_KEY_RIGHT:
		if (dir != left)dir = right;
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_DOUBLE); 
	glutInitWindowSize(windowWidth, windowHeight);  
	glutInitWindowPosition(windowPosX, windowPosY);
	glutCreateWindow(title);      
	glutDisplayFunc(display);     
	glutReshapeFunc(reshape);     
	glutTimerFunc(0, Timer, 0);   
	glutSpecialFunc(specialKeys); 
	glutKeyboardFunc(keyboard);  

	initGL();                     
	initSnake(5);
	glutMainLoop();               

	return 0;
}