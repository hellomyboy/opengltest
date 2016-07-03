#include "MePaint.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

using namespace std;

MePaint paint;

int main(int argc, char* argv[]) {
	paint.MainPaint(argc, argv);

	return 0;
}