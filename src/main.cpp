/*
This is a simple OpenGL program using SDL2 for window management and input handling. It initializes an OpenGL context,
sets up vertex data for a triangle, and enters a main loop to handle input and rendering. 
*/

/* Compilation on Linux:
g++ src/main.cpp src/glad.c -o build/prog -I./include -I"../common/third party/glm-master" -g -lSDL2 -ldl
*/

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

// Globals
int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenglContext = nullptr;
bool gQuit = false; // if true, quit the main loop

GLuint gVertexArrayObject = 0; // VAO for vertex attributes
GLuint gVertexBufferObject = 0; // VBO for vertex positions
GLuint gIndexBufferObject = 0;
GLuint gGraphicsPipelineShaderProgram = 0; // shader program object

float gOffset = 0.0f; 
float gRotate = 0.0f;

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv  Error Handling Routines  vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
static void GLClearAllErrors() {
    while(glGetError() != GL_NO_ERROR) {
        // 循环体是空的，因为我们只关心把错误取出来丢掉
    }
}

// Returns true if we have an error
static bool GLCheckErrorStatus(const char* function, int line) {
    while(GLenum error = glGetError()) {
        std::cout << "OpenGL Error: " << error 
                  << "\tLine: " << line
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}
#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  Error Handling Routines  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

std::string LoadShaderAsString(const std::string& filename) {
    // standard C++ way to read a text file into a string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if(myFile.is_open()) {
        while(std::getline(myFile, line)) {
            result += line + '\n';
        }

        myFile.close();
    }

    return result;

}   


void InitializeProgram() {
    // 1) 初始化 SDL 的视频子系统（创建窗口、处理输入等都依赖它）
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        std::cout << "SDL2 could not initialize video subsystem\n";
        exit(1);
    }

    // 2) 在创建窗口/Context 之前，配置 OpenGL Context 的属性
    //    注意：这些属性是“对 SDL 的请求”，最终是否满足取决于驱动/平台支持。
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // 3) 创建 SDL 窗口。
    //    这里传入 SDL_WINDOW_OPENGL 表示这个窗口将用于 OpenGL 渲染。
    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window",
                            0, 0,
                            gScreenWidth, gScreenHeight,
                            SDL_WINDOW_OPENGL);
    
    if(gGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL Window was not able to be created\n";
        exit(1);
    }

    // 4) 基于该窗口创建 OpenGL Context（后续所有 gl* 调用都作用于当前 context）
    gOpenglContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

    if(gOpenglContext == nullptr) {
        std::cout << "OpenGL context not available\n";
        exit(1);
    }

    // 5) 初始化 GLAD：加载 OpenGL 函数指针。
    //    没有这一步，许多 gl* 函数在运行时可能是空指针。
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        exit(1);
    }
    
}


void VertexSpecification() {
    // lives on cpu
    const std::vector<GLfloat> vertexData{

        /* 0 - Vertex */
        -0.5f, -0.5f, 0.0f,
        1.0f, 0.0f, 0.0f, 
        /* 1 - Vertex */
        0.5f, -0.5f, 0.0f,  
        0.0f, 1.0f, 0.0f,
        /* 2 - Vertex */  
        -0.5f,  0.5f, 0.0f,   
        0.0f, 0.0f, 1.0f,   
        /* 3 - Vertex */  
        0.5f, 0.5f, 0.0f,  
        1.0f, 0.0f, 0.0f  

    };


    /* -------------------- Start setting things on the GPU ----------------------------------------------------------*/

    // create vao 
    glGenVertexArrays(1, &gVertexArrayObject);

    glBindVertexArray(gVertexArrayObject);

    // create vbo for vertexData, bind it to GL_ARRAY_BUFFER, and upload data
    glGenBuffers(1, &gVertexBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER,       vertexData.size() * sizeof(GLfloat), 
                 vertexData.data(), GL_STATIC_DRAW);

    // Enable vertex attribute and Describe vertex attribute layout
    // for position attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);

    // for color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)(sizeof(GLfloat) * 3));

    // 按索引绘制的顶点索引数据（每三个索引构成一个三角形）
    const std::vector<GLuint> indexBufferData {2, 0, 1, 3, 2, 1}; 

    // create index buffer object (IBO) for indexed drawing 
    glGenBuffers(1, &gIndexBufferObject);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferData.size() * sizeof(GLuint), 
                 indexBufferData.data(), GL_STATIC_DRAW);


    // Unbind vao and vbo to prevent accidental modification 
    glBindVertexArray(0); // 解绑vao
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(1);

}

GLuint CompileShader(GLuint shaderType, const std::string& shadersource) {

    GLuint shaderObject;

    if(shaderType == GL_VERTEX_SHADER) {
        shaderObject = glCreateShader(GL_VERTEX_SHADER);
    }
    else if(shaderType == GL_FRAGMENT_SHADER) {
        shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    }

    const char* src = shadersource.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);
    
    return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
    // 1) 创建顶点着色器对象，编译顶点着色器源代码
    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
    // 2) 创建片段着色器对象，编译片段着色器源代码
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

    // 3) 创建 shader program 对象，把上述两个 shader attach 上去，并链接（link）成一个可用的程序
    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);
    glValidateProgram(programObject); // optional but good practice

    // 4) 链接成功后，可以删除 shader 对象（它们已经被 program 吸收了）
    glDetachShader(programObject, myVertexShader);
    glDetachShader(programObject, myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

void CreateGraphicsPipeline() {

    std::string vertexShaderSource = LoadShaderAsString("/home/summer/openglLearning/shaders/vertex_shader.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("/home/summer/openglLearning/shaders/fragment_shader.glsl");


    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}



void Input() {
    // SDL_Event 是一个联合体，用于承载“事件队列”里取出的各种事件（窗口/键盘/鼠标等）。
    SDL_Event e;
    // SDL_PollEvent：非阻塞轮询。
    // 返回非 0 表示取到一个事件并写入 e；返回 0 表示当前队列为空。
    while(SDL_PollEvent(&e) != 0) {
        // SDL_QUIT：用户点击窗口关闭按钮/系统请求退出等。
        if(e.type == SDL_QUIT) {
            std::cout << "Goodbye!\n";
            gQuit = true;
        }
        
    }

    // Retrieve keyboard state
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if(state[SDL_SCANCODE_UP]) {
        gOffset += 0.01f;
        std::cout << "g_uOffset: " << gOffset << std::endl;
    }
    if(state[SDL_SCANCODE_DOWN]) {
        gOffset -= 0.01f;
        std::cout << "g_uOffset: " << gOffset << std::endl;
    }
    if(state[SDL_SCANCODE_LEFT]) {
        gRotate -= 1.0f;
        std::cout << "g_uRotate: " << gRotate << std::endl;
    }
    if(state[SDL_SCANCODE_RIGHT]) {
        gRotate += 1.0f;
        std::cout << "g_uRotate: " << gRotate << std::endl;
    }


}

void PreDraw() {

    // 通常会在这里做“每帧渲染前”的准备工作
    // - 设置 OpenGL 状态（深度测试/混合/剔除等）
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClearColor(1.f, 1.f, 0.f, 1.f); // 黄色背景
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // 清除深度缓冲和颜色缓冲

    // - 绑定 shader program
    glUseProgram(gGraphicsPipelineShaderProgram);

    // 构造一个模型变换矩阵：先平移，再旋转
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, gOffset));

    // 围绕 y 轴旋转 45 度
    model           = glm::rotate(model, glm::radians(gRotate), glm::vec3(0.0f, 1.0f, 0.0f));

    // Retrieve the location of the uniform variable "u_ModelMatrix" in the shader program, and set it to the translation matrix
    GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");

    if(u_ModelMatrixLocation >= 0) { 
        glUniformMatrix4fv(u_ModelMatrixLocation, // location of the uniform variable
                           1, // count: how many matrices we are sending (1 in this case)
                           GL_FALSE, // whether to transpose the matrix (OpenGL expects column-major order, and glm::mat4 is already in that format, so we pass GL_FALSE)
                           &model[0][0] // glm::mat4 在内存中是列主序存储的，所以传地址时直接传第一列的地址即可
                         );
    }
    else {
        std::cout << "Could not find u_ModelMatrixLocation\n";
        exit(EXIT_FAILURE);
    }

    // 构造一个透视投影矩阵
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 
                                             (float)gScreenWidth / (float)gScreenHeight,
                                             0.1f,
                                             10.0f
                                            );

    // Retrieve the location of the uniform variable "u_Perspective" 
    GLint u_ProjectionLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_Projection");

    if(u_ProjectionLocation >= 0) { 
        glUniformMatrix4fv(u_ProjectionLocation, // location of the uniform variable
                           1, // count: how many matrices we are sending (1 in this case)
                           GL_FALSE, // whether to transpose the matrix (OpenGL expects column-major order, and glm::mat4 is already in that format, so we pass GL_FALSE)
                           &perspective[0][0] // glm::mat4 在内存中是列主序存储的，所以传地址时直接传第一列的地址即可
                         );
    }
    else {
        std::cout << "Could not find u_ProjectionLocation\n";
        exit(EXIT_FAILURE);
    }


}

void Draw() {

    // - 绑定 VAO/VBO
    glBindVertexArray(gVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

    // Render data
    glDrawElements(GL_TRIANGLES, 
                   6, // 这里是索引的数量，不是顶点数量。我们有 6 个索引（2 个三角形），所以传 6。  
                   GL_UNSIGNED_INT, 
                   0); // 索引绘制：从当前绑定的 GL_ELEMENT_ARRAY_BUFFER 里读取索引数据，每三个索引构成一个三角形，绘制两组三角形（共六个顶点）
    
    glUseProgram(0); // unbind shader program
}

void MainLoop() {
    // 主循环：
    // 1) 处理输入事件
    // 2) 每帧渲染前准备
    // 3) 发出绘制指令
    // 4) 交换前后缓冲，把画面显示到窗口上
    while(!gQuit) {
        Input();

        PreDraw();

        Draw();

        // 双缓冲交换：把“后缓冲”呈现到屏幕（前缓冲）
        SDL_GL_SwapWindow(gGraphicsApplicationWindow);


    }
}

void CleanUp() {
    // 按“创建的逆序”回收资源：先销毁窗口，再关闭 SDL。
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}



int main(int argc, char* args[]) {

    // 1. 初始化 SDL2 和 OpenGL context
    InitializeProgram();

    // 2. 设置顶点数据和属性
    VertexSpecification();

    // 3. 创建图形管线（编译/链接 shader 等）
    CreateGraphicsPipeline();

    // 4. 进入主循环
    MainLoop();

    // 5. 清理资源并退出
    CleanUp();


    return 0;
}