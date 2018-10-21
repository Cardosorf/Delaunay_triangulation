#include <OpenGL/gl.h>     // The GL Header File
#include <cmath>
#include <stdlib.h>
#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header
#include <iostream>
#include <vector>
#include <queue> 
#include <algorithm>
#include <utility> 

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
	Edges() {};
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


class Triangle {

public:
	OrderedPair getV1(void);
	OrderedPair getV2(void);
	OrderedPair getV3(void);
	Triangle(OrderedPair v1, OrderedPair v2, OrderedPair v3);


private:
	OrderedPair v1;
	OrderedPair v2;
	OrderedPair v3;

};

Triangle::Triangle(OrderedPair v1, OrderedPair v2, OrderedPair v3)
{
	this->v1 = v1;
	this->v2 = v2;
	this->v3 = v3;
}

OrderedPair Triangle::getV1(void) {
	return v1;
}

OrderedPair Triangle::getV2(void) {
	return v2;
}

OrderedPair Triangle::getV3(void) {
	return v3;
}


//Checks if an edge belongs to a triangle and if so removes its vertices from the vector of vertices
void isVT(Edges e, vector<OrderedPair>& x, vector<Triangle> t) {
		for (int i = 0; i < t.size(); i++) {
			if (((e.getStart().getAxis_x() == t[i].getV1().getAxis_x() && e.getStart().getAxis_y() == t[i].getV1().getAxis_y()) ||
				(e.getStart().getAxis_x() == t[i].getV2().getAxis_x() && e.getStart().getAxis_y() == t[i].getV2().getAxis_y()) ||
				(e.getStart().getAxis_x() == t[i].getV3().getAxis_x() && e.getStart().getAxis_y() == t[i].getV3().getAxis_y())) &&
				((e.getEnd().getAxis_x() == t[i].getV1().getAxis_x() && e.getEnd().getAxis_y() == t[i].getV1().getAxis_y()) ||
				(e.getEnd().getAxis_x() == t[i].getV2().getAxis_x() && e.getEnd().getAxis_y() == t[i].getV2().getAxis_y()) ||
					(e.getEnd().getAxis_x() == t[i].getV3().getAxis_x() && e.getEnd().getAxis_y() == t[i].getV3().getAxis_y()))) {
				for (int j = 0; j < x.size(); j++) {
					if (x[j].getAxis_x() == t[i].getV1().getAxis_x() && x[j].getAxis_y() == t[i].getV1().getAxis_y()) {
						x.erase(x.begin() + j);
						j = 0;
					}
					else if (x[j].getAxis_x() == t[i].getV2().getAxis_x() && x[j].getAxis_y() == t[i].getV2().getAxis_y()) {
						x.erase(x.begin() + j);
						j = 0;
					}
					else if (x[j].getAxis_x() == t[i].getV3().getAxis_x() && x[j].getAxis_y() == t[i].getV3().getAxis_y()) {
						x.erase(x.begin() + j);
						j = 0;
					}
				}
			}
		}
}


//Checks if and edge belongs to a vector
bool isEdgeConvex(vector<Edges> convex, Edges x) {

	for (int i = 0; i < convex.size(); i++) {
		if (convex[i].getStart().getAxis_x() == x.getStart().getAxis_x() &&
			convex[i].getStart().getAxis_y() == x.getStart().getAxis_y() &&
			convex[i].getEnd().getAxis_x() == x.getEnd().getAxis_x() &&
			convex[i].getEnd().getAxis_y() == x.getEnd().getAxis_y()) {
			return true;
		}
	}

	return false;
}


// ab.ac   Calculates its angle.
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

//Removes vertices from edge.
void remove_vertices_edge(vector<OrderedPair>& temp_vertices, queue<Edges> list_Edges) {
	for (int j = 0; j < (int)temp_vertices.size(); j++) {
		if ((temp_vertices[j].getAxis_x() == list_Edges.front().getStart().getAxis_x() && temp_vertices[j].getAxis_y() == list_Edges.front().getStart().getAxis_y()) || (temp_vertices[j].getAxis_x() == list_Edges.front().getEnd().getAxis_x() && temp_vertices[j].getAxis_y() == list_Edges.front().getEnd().getAxis_y())) {
			temp_vertices.erase(temp_vertices.begin() + j);
			j = 0;
		}
	}
}

//Finds the greater angle and its vertice
pair<float, OrderedPair> greater_angle_vertice(vector<OrderedPair> temp_vertices, queue<Edges> list_Edges) {
	float angle_temp = 0;
	OrderedPair temp;
	for (int i = 0; i < (int)temp_vertices.size(); i++) {
		if (angle_temp < angle(temp_vertices[i], list_Edges.front().getStart(), list_Edges.front().getEnd())) {
			angle_temp = angle(temp_vertices[i], list_Edges.front().getStart(), list_Edges.front().getEnd());
			temp = temp_vertices[i];
		}
	}
	return make_pair(angle_temp, temp);
}

int counting = 0;

bool isEdgeBelongingConvex(Edges& edge,vector<Edges>& convex_edges) {
	for (int i = 0; i < convex_edges.size(); i++) {
		if (((edge.getStart().getAxis_x() == convex_edges[i].getStart().getAxis_x() && edge.getStart().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
			(edge.getStart().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && edge.getStart().getAxis_y() == convex_edges[i].getEnd().getAxis_y())) &&
			((edge.getEnd().getAxis_x() == convex_edges[i].getStart().getAxis_x() && edge.getEnd().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
			(edge.getEnd().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && edge.getEnd().getAxis_y() == convex_edges[i].getEnd().getAxis_y())))
		{
			cout << "SOU ARESTA LIVRE? ANTES " << edge.getFree() << endl;
			edge.setFree(0);
			convex_edges[i].setFree(0);
			cout << "SOU ARESTA LIVRE? DEPOIS " << edge.getFree() << endl;
			counting++;
			return true;
		}
	}
	return false;
}



vector<OrderedPair> pointsArray;


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
		vector<Edges> oldConvex_edges; //edges from the original convex hull
		queue<Edges> list_Edges;

		vector<OrderedPair> temp_vertices = pointsArray;

		vector<Triangle> triangles;

		//Storing edges from the convex hull
		for (int i = 0; i < (int)convex_vertices.size() - 1; i++) {
			Edges temp(convex_vertices[i], convex_vertices[i + 1]);
			convex_edges.push_back(temp);
			oldConvex_edges.push_back(temp);
		}
		convex_edges[0].setFree(1);
		list_Edges.push(convex_edges[0]); //Picking one edge from the convex hull to start the triangulation

		//Removes vertices from the first edge.
		remove_vertices_edge(temp_vertices, list_Edges);

		pair<float, OrderedPair> angle_vertice;
		//Finds the greater angle and its vertice
		angle_vertice = greater_angle_vertice(temp_vertices, list_Edges);

		cout << "elementos da lista " << list_Edges.size() << endl;
		cout << "inicio aresta x " << list_Edges.front().getStart().getAxis_x() << endl;
		cout << "inicio aresta y " << list_Edges.front().getStart().getAxis_y() << endl;
		cout << "fim aresta x " << list_Edges.front().getEnd().getAxis_x() << endl;
		cout << "fim aresta y " << list_Edges.front().getEnd().getAxis_y() << endl;

		
		while (!list_Edges.empty()) {
			cout << "SOU LIVRO? antes " << list_Edges.front().getFree() << endl;
			for (int i = 0; i < convex_edges.size(); i++) {
				if (((list_Edges.front().getStart().getAxis_x() == convex_edges[i].getStart().getAxis_x() && list_Edges.front().getStart().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
					(list_Edges.front().getStart().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && list_Edges.front().getStart().getAxis_y() == convex_edges[i].getEnd().getAxis_y())) &&
					((list_Edges.front().getEnd().getAxis_x() == convex_edges[i].getStart().getAxis_x() && list_Edges.front().getEnd().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
					(list_Edges.front().getEnd().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && list_Edges.front().getEnd().getAxis_y() == convex_edges[i].getEnd().getAxis_y())))
				{
					if (list_Edges.front().getFree() != convex_edges[i].getFree()) {
						list_Edges.front().setFree(0);
						cout << "hello" << endl;
						break;
					}
				}
			}
			if (counting == 2) {
				list_Edges.front().setFree(0);
			}
			cout << "counting " << counting << endl;
			counting = 0;
			cout << "SOU LIVRO? depois " << list_Edges.front().getFree() << endl;

			for (int i = 0; i < convex_edges.size() - 1; i++) {
				float ABx = list_Edges.front().getEnd().getAxis_x() - list_Edges.front().getStart().getAxis_x();
				float ABy = list_Edges.front().getEnd().getAxis_y() - list_Edges.front().getStart().getAxis_y();
				float ADx = convex_edges[i].getEnd().getAxis_x() - list_Edges.front().getStart().getAxis_x();
				float ADy = convex_edges[i].getEnd().getAxis_y() - list_Edges.front().getStart().getAxis_y();
				float ACx = convex_edges[i].getStart().getAxis_x() - list_Edges.front().getStart().getAxis_x();
				float ACy = convex_edges[i].getStart().getAxis_y() - list_Edges.front().getStart().getAxis_y();
				float CDx = convex_edges[i].getEnd().getAxis_x() - convex_edges[i].getStart().getAxis_x();
				float CDy = convex_edges[i].getEnd().getAxis_y() - convex_edges[i].getStart().getAxis_y();
				float CAx = list_Edges.front().getStart().getAxis_x() - convex_edges[i].getStart().getAxis_x();
				float CAy = list_Edges.front().getStart().getAxis_y() - convex_edges[i].getStart().getAxis_y();
				float CBx = list_Edges.front().getEnd().getAxis_x() - convex_edges[i].getStart().getAxis_x();
				float CBy = list_Edges.front().getEnd().getAxis_y() - convex_edges[i].getStart().getAxis_y();

				if ((ABx*ADy - ABy * ADx)*(ABx*ACy - ABy * ACx) < 0 && (CDx*CAy - CDy * CAx)*(CDx*CBy - CDy * CBx) < 0) {
					list_Edges.front().setFree(0);
				}
			}


			if (list_Edges.front().getFree() == 0) {
				list_Edges.pop();
			}
			else{
			//temp_vertices = pointsArray;
			cout << "tam verti antes" << convex_edges.size() << endl;
			cout << "tam tempverti" << temp_vertices.size() << endl;
			isVT(list_Edges.front(), temp_vertices, triangles);
			cout << "tam tempverti" << temp_vertices.size() << endl;
			//Finds the greater angle and its vertice
			angle_vertice = greater_angle_vertice(temp_vertices, list_Edges);
			Edges newestFirst(list_Edges.front().getEnd(), angle_vertice.second);
			newestFirst.setFree(1);
			Edges newestSecond(angle_vertice.second, list_Edges.front().getStart());
			newestSecond.setFree(1);

			cout << angle_vertice.second.getAxis_x() << endl;

			if (!isEdgeBelongingConvex(newestFirst, convex_edges)) {
				//newestFirst.setFree(1);
				convex_edges.push_back(newestFirst);
				list_Edges.push(newestFirst);
			}
			cout << "newestFirst " << newestFirst.getFree() << endl;

			if (!isEdgeBelongingConvex(newestSecond, convex_edges)) {
				//newestSecond.setFree(1);
				convex_edges.push_back(newestSecond);
				list_Edges.push(newestSecond);
			}
			cout << "newestSecond " << newestSecond.getFree() << endl;

			cout << "tam verti depois" << convex_edges.size() << endl;
			if(list_Edges.front().getFree() == 1)
				triangles.push_back(Triangle(list_Edges.front().getEnd(), angle_vertice.second, list_Edges.front().getStart()));
			cout << "triangulos " << triangles.size() << endl;
			list_Edges.pop();


			/*
			if (!list_Edges.empty()) {
				for (int i = 0; i < convex_edges.size(); i++) {
					if (((list_Edges.front().getStart().getAxis_x() == convex_edges[i].getStart().getAxis_x() && list_Edges.front().getStart().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
						(list_Edges.front().getStart().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && list_Edges.front().getStart().getAxis_y() == convex_edges[i].getEnd().getAxis_y())) &&
						((list_Edges.front().getEnd().getAxis_x() == convex_edges[i].getStart().getAxis_x() && list_Edges.front().getEnd().getAxis_y() == convex_edges[i].getStart().getAxis_y()) ||
						(list_Edges.front().getEnd().getAxis_x() == convex_edges[i].getEnd().getAxis_x() && list_Edges.front().getEnd().getAxis_y() == convex_edges[i].getEnd().getAxis_y())))
					{
						list_Edges.front().setFree(0);
					}
				}
			}
			*/


			

			

			/*
			while (list_Edges.front().getFree() == 0) {
				list_Edges.pop();
			}*/
			//cout << list_Edges.front().getFree() << endl;
			temp_vertices = pointsArray;
			}
		}


		
		cout << triangles.size() << endl;
		for (int i = 0; i < (int)triangles.size(); i++) {
			glBegin(GL_LINE_STRIP);
			glVertex2f(triangles[i].getV1().getAxis_x(), triangles[i].getV1().getAxis_y());
			glVertex2f(triangles[i].getV2().getAxis_x(), triangles[i].getV2().getAxis_y());
			glVertex2f(triangles[i].getV3().getAxis_x(), triangles[i].getV3().getAxis_y());
			
			//getchar();
			glEnd();
			glFlush();
			glutSwapBuffers();
			glutSwapBuffers();
		}
		
		vector<OrderedPair> oldpoints = convex_Hull(pointsArray);

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < (int)oldpoints.size(); i++) {
		glVertex2f(oldpoints[i].getAxis_x(), oldpoints[i].getAxis_y());
		}
		glEnd();
		glFlush();
		glutSwapBuffers();
		glutSwapBuffers();
		
	}
}

//Print the header with my name, dre and instructions
void header() {
	//Sets the position of the text in the window using the x and y coordinates
	glRasterPos2f(-1, 0.9);
	string name = "Rafael Pais Cardoso\n";
	string dre = "DRE: 116140788";
	string instruction1 = "Left mouse button -> creates points";
	string instruction2 = "Right mouse button -> Delaunay Triangulation";
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