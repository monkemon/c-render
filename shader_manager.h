#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define LOG_SIZE_BYTES 512

GLuint compileShader(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    if (shader==GL_INVALID_VALUE || shader==GL_INVALID_OPERATION)
    {
        printf("Failed to load shader source!\n");
    }
    glCompileShader(shader);

    int success;
    GLsizei log_len; 
    GLchar info_log[LOG_SIZE_BYTES + 1];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, LOG_SIZE_BYTES, &log_len, info_log);
        info_log[log_len] = 0;
        printf("Shader output:\n%s\n", info_log);
    }
    return shader;
}


char* load_file(const char* f_path, size_t* out_f_len)
{
    FILE* f_ptr;
    char* buffer;
    struct stat st;
    size_t read_bytes = 0;

    if (stat(f_path, &st) != 0)
    {
        perror("stat");
        return NULL;
    }

    f_ptr = fopen(f_path, "rb");
    if (!f_ptr)
    {
        perror("fopen");
        return NULL;    
    }

    // buffer [ FILE_LEN + 1 ]
    buffer = (char*) calloc(st.st_size + 1, sizeof(char));
    if (!buffer)
    {
        fprintf(stderr, "calloc() of %ld bytes failed!\n",
            ( st.st_size + 1) * sizeof(char)
        );
        fclose(f_ptr);
        return NULL;
    }

    read_bytes = fread(buffer, 1, st.st_size, f_ptr);
    if (read_bytes != st.st_size)
    {
        fprintf(stderr, "fread() of file [%s] failed! stat size: %ld, read size: %ld\n",
            f_path,
            st.st_size,
            read_bytes
        );
        fclose(f_ptr);
        return NULL;
    }

    // Null terminate the last element
    fprintf(stdout, "fread() - read %ld bytes\n", read_bytes);
    buffer[read_bytes] = 0;

    *out_f_len = read_bytes;
    return buffer;    
}


GLuint loadShaderFromFile(const char* filepath, GLenum type)
{
    char* file_raw;
    size_t file_size;
    
    file_raw = load_file(filepath, &file_size);

    GLuint res_shader = compileShader(file_raw, type);

    free(file_raw);

    return res_shader;
}


GLuint createShaderProgramFromFile(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = loadShaderFromFile(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = loadShaderFromFile(fragmentSource, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

#endif //SHADER_MANAGER_H