#include "UniformBlock.h"
#include <iostream>

using namespace std;

UniformBlock::UniformBlock(const GLuint programID, const GLchar * blockName)
	:programID(programID)
{
	blockIndex = glGetUniformBlockIndex(programID, blockName);
	if (blockIndex == GL_INVALID_INDEX) {
		std::cout << "init uniform data error, invalid uniform name:" << blockName<<","<<blockIndex<<std::endl;
	}
}


void UniformBlock::sendData(const GLchar * elem[], const GLfloat *val[], const GLuint eleSize[], const GLint uniformNum)
{
	//get uniform block size and malloc memory
	GLint blockSize;
	glGetActiveUniformBlockiv(programID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
	GLbyte *buff = (GLbyte *)malloc(blockSize);

	// get element index of uniform block
	GLuint *indices = (GLuint *)malloc(uniformNum * sizeof(GLuint));
	glGetUniformIndices(programID, uniformNum, elem, indices);

	//get element memory offset of uniform block
	GLint *offset = (GLint *)malloc(uniformNum * sizeof(GLint));
	glGetActiveUniformsiv(programID, uniformNum, indices, GL_UNIFORM_OFFSET, offset);

	//copy input data to buffer
	for (int i = 0; i < uniformNum; i++) {
		memcpy(buff + offset[i], val[i], eleSize[i]);
	}

	//send data to uniform block
	GLuint uniformBuffID;
	glGenBuffers(1, &uniformBuffID);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffID);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, buff, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, blockIndex, uniformBuffID);

	free(offset);
	free(indices);
	free(buff);
}