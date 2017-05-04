#ifndef __VECTOR_IS_INCLUDED__
#define __VECTOR_IS_INCLUDED__

#define CO(V)	V.x,V.y,V.z

struct Vector{
	double x,y,z;
	Vector();
	Vector(double,double,double);
	bool scan();
	double mag2();
	double mag();
	void norm();
	Vector unit();
	void show();
	void showln();
};

Vector operator+(Vector a,Vector b);
Vector operator-(Vector a);
Vector operator-(Vector a,Vector b);
Vector operator*(Vector a,Vector b);
Vector operator*(double a,Vector b);
Vector operator*(Vector b,double a);
Vector operator/(Vector b,double a);

double DOT(Vector a, Vector b);
Vector CROSS(Vector a, Vector b);
double BOX(Vector a, Vector b, Vector c);

double projectLength(Vector proj, Vector on);
Vector projectVector(Vector proj, Vector on);
Vector rotateVector(Vector rotatee, Vector axis, double angle);

extern Vector I;
extern Vector J;
extern Vector K;
extern Vector Origin;

#endif
