#pragma once
#include <string>

using namespace std;
class MePaint
{
public:
	void MainPaint(int, char**);
	void createShaderAndProgram();
	void sendDataToShader();
	const string loadShaderAsString(const string fileName);
	~MePaint();
};

