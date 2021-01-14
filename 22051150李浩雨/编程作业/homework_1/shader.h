#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>

using namespace std;

class Shader {
public:
	Shader(const char* vertexPath,const char* fragmentPath);
	void use();
	//Shader Program ID
	unsigned int ID;
private:
	void checkCompileErrors(unsigned int ID, string type);
};

#endif // !_SHADER_H_
