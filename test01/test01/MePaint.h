#pragma once
#include <string>

using namespace std;
class MePaint
{
public:
	void MainPaint(int, char**);
	//void keyboardFun(unsigned char key, int x, int y);
	void createShaderAndProgram();
	void sendDataToShader();
	const string loadShaderAsString(const string fileName);
	~MePaint();
};

