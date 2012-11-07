#include "shaderobject.h"
#include <stdio.h>

#ifdef WIN32
#define OPEN_FILE(a,b,c) fopen_s(&a, b, c);
#else
#define OPEN_FILE(a,b,c) a = fopen(b, c);
#endif

namespace inviwo {

const std::string ShaderObject::logSource_ = "ShaderObject";

ShaderObject::ShaderObject(GLenum shaderType, std::string fileName) :
    fileName_(fileName),
    shaderType_(shaderType)
{
    id_ = glCreateShader(shaderType);
}

ShaderObject::~ShaderObject() {
    //free(source_);
}

void ShaderObject::initialize() {
    // TODO: remove absolute paths
    loadSource(IVW_DIR+"modules/opengl/glsl/"+fileName_);
    upload();
    compile();
}

void ShaderObject::deinitialize() {}

void ShaderObject::loadSource(std::string fileName) {
    FILE* file;
    char* fileContent = NULL;
    long len;

    if (fileName.length() > 0) {
        OPEN_FILE(file, fileName.c_str(), "rb");
        if (file != NULL) {
            fseek(file, 0L, SEEK_END);
            len = ftell(file);
            fseek(file, 0L, SEEK_SET);
            fileContent = (char*)malloc(sizeof(char)*(len));
            if(fileContent != NULL){
                fread(fileContent, sizeof(char), len, file);
                fileContent[len] = '\0';
            }
            fclose(file);
        }
    }
    source_ = fileContent;
}

void ShaderObject::upload() {
    glShaderSource(id_, 1, &source_, 0);
    LGL_ERROR;
}

std::string ShaderObject::getCompileLog() {
    GLint maxLogLength;
    glGetShaderiv(id_, GL_INFO_LOG_LENGTH , &maxLogLength);
    LGL_ERROR;

    if (maxLogLength > 1) {
        GLchar* compileLog = new GLchar[maxLogLength];
        ivwAssert(compileLog!=0, "could not allocate memory for compiler log");
        GLsizei logLength;
        glGetShaderInfoLog(id_, maxLogLength, &logLength, compileLog);
        std::istringstream compileLogStr(compileLog);
        delete[] compileLog;
        return compileLogStr.str();
    } else return "";
}

void ShaderObject::compile() {
    glCompileShader(id_);
    std::string compilerLog = getCompileLog();
    if (!compilerLog.empty())
        LogInfo(compilerLog);
}

} // namespace
