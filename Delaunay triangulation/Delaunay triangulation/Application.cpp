#include <OpenGL/gl.h>     // The GL Header File
#include <cmath>
#include <stdlib.h>
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <iostream>
#include <vector>
#include <queue> 

#define _USE_MATH_DEFINES
//Width and height of the window. If necessary, change these values.
#define WIDTH 800
#define HEIGHT 600
#define PI 3.14159265

using namespace std;



//Creates a class to handle OrderedPairs
class OrderedPair {

public:
	float getAxis_x(void);
	float getAxis_y(void);
	OrderedPair() {};
	OrderedPair(float x_value, float y_value) {
		x = x_value;
		y = y_value;
	}

private:
	float x;
	float y;
};

//Sets the ordered pair
/*
OrderedPair::OrderedPair(float x_value, float y_value) {
	x = x_value;
	y = y_value;
}
*/
float OrderedPair::getAxis_x(void) {
	return x;
}

float OrderedPair::getAxis_y(void) {
	return y;
}


class Edges {

public:
	OrderedPair getStart(void);
	OrderedPair getEnd(void);
	int getFree(void);
	void setFree(int value);
	Edges(OrderedPair start, OrderedPair end);

private:
	OrderedPair start;
	OrderedPair end;
	int free = 0; //visited edge

};

Edges::Edges(OrderedPair start, OrderedPair end)
{
	this->start = start;
	this->end = end;
}


OrderedPair Edges::getStart(void) {
	return start;
}

OrderedPair Edges::getEnd(void) {
	return end;
}

int Edges::getFree(void) {
	return free;
}

void Edges::setFree(int value) {
	free = value;
}

int firstClick = 0;
bool polygon = false;
vector<OrderedPair> pointsArray;

vector<OrderedPair> convex_Hull(vector<OrderedPair> pointsArray) {
	OrderedPair leftmost = pointsArray[0];
	for (int i = 0; i < (int)pointsArray.size(); i++) {
		if (pointsArray[i].getAxis_x() < leftmost.getAxis_x())
			leftmost = pointsArray[i];
	}

	OrderedPair previous = leftmost;
	vector<OrderedPair> temp;
	temp.push_back(leftmost);
	OrderedPair next(leftmost.getAxis_x(), -1);

	while (true) {
		float val;
		OrderedPair pivottemp = pointsArray[0];

		for (int i = 0; i < (int)pointsArray.size(); i++) {

			//Cross Product: ab * ac
			val = ((previous.getAxis_y() - pointsArray[i].getAxis_y()) * (previous.getAxis_x() - next.getAxis_x())) - ((previous.getAxis_y() - next.getAxis_y()) * (previous.getAxis_x() - pointsArray[i].getAxis_x()));

			//if Cross Product > 0, then this point may be one of those from the convex hull if the following val are < 0.
			if (val > 0) {
				next = pointsArray[i];
			}

		}

		temp.push_back(next);
		pivottemp = next;
		next = previous;
		previous = pivottemp;

		if (temp.front().getAxis_x() == temp.back().getAxis_x() && temp.front().getAxis_y() == temp.back().getAxis_y()) {
			break;
		}

	}

	return temp;

}

// ab.ac
float angle(OrderedPair a, OrderedPair b, OrderedPair c) {
	float temp = 0;
	float abx, aby, acx, acy;
	float inner_product;
	float mab, mac;

	abx = b.getAxis_x() - a.getAxis_x();
	aby = b.getAxis_y() - a.getAxis_y();
	acx = c.getAxis_x() - a.getAxis_x();
	acy = c.getAxis_y() - a.getAxis_y();

	inner_product = (abx * acx) + (aby * acy);

	mab = sqrt(pow(abx, 2) + pow(aby, 2));
	mac = sqrt(pow(acx, 2) + pow(acy, 2));

	return acos((inner_product) / (mab*mac)) * 180.0 / PI;
}


void mouse(int button, int state, int x, int y) {

	float dx, dy;
	int intercept = 0;


	glColor3f(1.0, 1.0, 1.0);
	//Left button click draws points on the screen and each point is stored inside a vector. 
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		pointsArray.push_back(OrderedPair((2.*x) / WIDTH - 1, -(2.*y) / HEIGHT + 1));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(6);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POINTS);
		for (int i = 0; i < (int)pointsArray.size(); i++) {
			glVertex2f(pointsArray[i].getAxis_x(), pointsArray[i].getAxis_y());
		}
		glEnd();
		glFlush();
		glutSwapBuffers();
	}

	//Finds the points wich belong to the convex hull.
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && pointsArray.size() > 2) {
		vector<OrderedPair> convex_vertices;

		convex_vertices = convex_Hull(pointsArray);

		vector<Edges> convex_edges;
		
		//Edges temp(convex_vertices[0], convex_vertices[1]);

		queue<Edges> list_Edges;

		vector<OrderedPair> temp_vertices = pointsArray;

		//Storing edges from the convex hull
		for (int i = 0; i < (int)convex_vertices.size()-1; i++) {
			Edges temp(convex_vertices[i], convex_vertices[i+1]);
			convex_edges.push_back(temp);
		}

		//convex_edges[0].getStart().getAxis_x();
		//convex_edges[1].getStart().getAxis_x();
		//cout << "true" << endl;
		//cout << (int)convex_edges.size() << endl;
		list_Edges.push(convex_edges[0]); //Picking one edge from the convex hull
		
		cout << temp_vertices.size() << "  " << convex_vertices.size() << endl;
		/*
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			cout << temp_vertices[i].getAxis_x() << "  " << temp_vertices[i].getAxis_y() << endl;
		}*/

		//temp_vertices.erase(temp_vertices.begin() + 1); //Removes the first edge.
		
		//Removes vertices from the first edge.
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			for (int j = 0; j < (int)temp_vertices.size(); j++) {
				if ((temp_vertices[j].getAxis_x() == convex_edges[0].getStart().getAxis_x() && temp_vertices[j].getAxis_y() == convex_edges[0].getStart().getAxis_y())) {
					temp_vertices.erase(temp_vertices.begin() + j);
				}

				if ((temp_vertices[j].getAxis_x() == convex_edges[0].getEnd().getAxis_x() && temp_vertices[j].getAxis_y() == convex_edges[0].getEnd().getAxis_y())) {
					temp_vertices.erase(temp_vertices.begin() + j);
				}
			}
		}

		//Removes the first edge.
		/*
		//tah ruim
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			for (int j = 0; j < (int)convex_edges.size(); j++) {
				if (temp_vertices[i].getAxis_x() == convex_edges[j].getStart().getAxis_x() && temp_vertices[i].getAxis_y() == convex_edges[j].getStart().getAxis_y()) {
					temp_vertices.erase(temp_vertices.begin() + i);
					list_Edges.push(convex_edges[j]);
					break;
				}
			}
		}
		*/
		cout << temp_vertices.size() << "  " << convex_vertices.size() << endl;

		float angle_temp = 0;
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			if (angle_temp < angle(temp_vertices[i], list_Edges.front().getStart(), list_Edges.front().getEnd())) {
				angle_temp = angle(temp_vertices[i], list_Edges.front().getStart(), list_Edges.front().getEnd());
			}
		}
		
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			if (angle_temp == angle(temp_vertices[i], list_Edges.front().getEnd(), list_Edges.front().getStart())) {
				Edges newest1(list_Edges.front().getEnd(), temp_vertices[i]);
				newest1.setFree(1);
				convex_edges.push_back(newest1);
				list_Edges.push(newest1);

				Edges newest2(temp_vertices[i], list_Edges.front().getStart());
				newest2.setFree(1);
				convex_edges.push_back(newest2);
				list_Edges.push(newest2);
				list_Edges.pop();
			}
		}

		for (int i = 0; i < (int)convex_edges.size(); i++) {
			cout << convex_edges[i].getFree() << endl;
		}


		/*
		for (int i = 0; i < (int)temp_vertices.size(); i++) {
			cout << temp_vertices[i].getAxis_x() << "  " << temp_vertices[i].getAxis_y() << endl;
		}
		*/
		/*
		for (int i = 0; i < (int)convex_vertices.size(); i++) {
			for (int j = 0; j < (int)convex_edges.size(); j++) {
				if(convex_vertices[i].getAxis_x() == convex_edges[j].getStart().getAxis_x()){
					cout << "true" << endl;
				}
			}
		}
		* /

		/*
		for (int i = 0; (int)convex_vertices.size(); i++) {
			Edges temp();


		}
		*/

		/*
		for (int i = 0; i < (int)convex_vertices.size(); i++) {
			cout <<  convex_vertices[i].getAxis_x() << "  " <<  convex_vertices[i].getAxis_y() << endl;
		}

		for (int i = 0; i < (int)convex_edges.size(); i++) {

			cout << convex_edges[i].getStart().getAxis_x() << endl;
			cout << convex_edges[i].getStart().getAxis_y() << endl;
			cout << convex_edges[i].getEnd().getAxis_x() << endl;
			cout << convex_edges[i].getEnd().getAxis_y() << endl;
			
			
		}
		*/
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < (int)convex_edges.size(); i++) {
			glVertex2f(convex_edges[i].getStart().getAxis_x(), convex_edges[i].getStart().getAxis_y());
			glVertex2f(convex_edges[i].getEnd().getAxis_x(), convex_edges[i].getEnd().getAxis_y());
		}

		/*
		for (int i = 0; i < (int)convex_vertices.size(); i++) {
			glVertex2f(convex_vertices[i].getAxis_x(), convex_vertices[i].getAxis_y());
		}
		*/
		glEnd();
		glFlush();
		glutSwapBuffers();

	}

}

//Print the header with my name, dre and instructions
void header() {
	//Sets the position of the text in the window using the x and y coordinates
	glRasterPos2f(-1, 0.9);
	string name = "Rafael Pais Cardoso\n";
	string dre = "DRE: 116140788";
	string instruction1 = "Left mouse button -> creates connected points by lines";
	string instruction2 = "Right mouse button -> Gift wrap the points";
	string instruction3 = "Enjoy :)";
	int len_name = (int)name.length();
	int len_dre = (int)dre.length();
	int len_instruction1 = (int)instruction1.length();
	int len_instruction2 = (int)instruction2.length();
	int len_instruction3 = (int)instruction3.length();

	//Loops to display character by character
	for (int i = 0; i < len_name; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, name[i]);
	}

	glRasterPos2f(-1, 0.8);
	for (int i = 0; i < len_dre; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, dre[i]);
	}

	glRasterPos2f(-1, 0.7);
	for (int i = 0; i < len_instruction1; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction1[i]);
	}

	glRasterPos2f(-1, 0.6);
	for (int i = 0; i < len_instruction2; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction2[i]);
	}

	glRasterPos2f(-1, 0.5);
	for (int i = 0; i < len_instruction3; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, instruction3[i]);
	}
}

void display()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glLineWidth(4.0);
	header();
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Rafael Pais Cardoso");
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMainLoop();
	return EXIT_SUCCESS;
}