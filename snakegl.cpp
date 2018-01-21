#include<glad/glad.h>
#include<GLFW/glfw3.h>
//This library is for the sleep command
#include<unistd.h> 
#include<bits/stdc++.h>
//This library is for the delay command
#include<time.h>
using namespace std;
//this function includes delay command in program
#define HEIGHT 720
#define BREADTH 1280
#define SNAKE 20
queue <int> state;
int initState=1;



int curr_state=state.front();
float framerate=15;
int board[HEIGHT/SNAKE][BREADTH/SNAKE];
int shaderProgram;
unsigned int vboSnake[HEIGHT/SNAKE][BREADTH/SNAKE], vaoSnake[HEIGHT/SNAKE][BREADTH/SNAKE], eboSnake[HEIGHT/SNAKE][BREADTH/SNAKE];
void delay(float secs)
{
	float end = clock()/CLOCKS_PER_SEC + secs;
	while((clock()/CLOCKS_PER_SEC) < end);
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
	glViewport(0,0,width,height);
}
void activate_airship(int s)
{
	state.push(s);
	cout<<"state="<<state.back()<<endl;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UP && action == GLFW_PRESS && (state.back()==1 || state.back()==3))
        activate_airship(0);
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS&& (state.back()==0 || state.back()==2))
        activate_airship(1);
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS&& (state.back()==3 || state.back()==1))
        activate_airship(2);
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS&& (state.back()==0 || state.back()==2))
        activate_airship(3);

}


//this function detects keyboard input
void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window,true);
	}
}
void gameOver(GLFWwindow *window)
{
	int i,j;
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		for(i=0;i<HEIGHT/SNAKE;i++)
		{
			for(j=0;j<BREADTH/SNAKE;j++)
			{
				if(board[i][j]==1)
				{
					glUseProgram(shaderProgram);
					glBindVertexArray(vaoSnake[i][j]);
					glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
				}
			}
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwSetWindowShouldClose(window,true);
}
void generateCoinNormal(int &coinX,int &coinY)
{
	while(1)
	{
		coinX=rand()%(BREADTH/SNAKE);
		coinY=rand()%(HEIGHT/SNAKE);
		if(board[coinY][coinX]==1)
		{
			continue;
		}
		else
		{
			break;
		}
	}
}
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\n\0";
const char *coinShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\n\0";
int main()
{
	int i,j;
	state.push(initState);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window=glfwCreateWindow(800,600,"window",NULL,NULL);
	if(window==NULL)
	{
		cout<<"Failed to create GLFW window"<<endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout<<"Failed to initialize GLAD"<<endl;
		return -1;
	}
	glViewport(0,0,800,800);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	//Shaders
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int coinShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(coinShader, 1, &coinShaderSource, NULL);
    glCompileShader(coinShader);
    glGetShaderiv(coinShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(coinShader, 512, NULL, infoLog);
        std::cout << "ERROR::COIN::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
	shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    int coinShaderProgram=glCreateProgram();
	glAttachShader(coinShaderProgram, vertexShader);
	glAttachShader(coinShaderProgram, coinShader);
    glLinkProgram(coinShaderProgram);
   	glGetProgramiv(coinShaderProgram, GL_LINK_STATUS, &success);
    if (!success) 
    {
        glGetProgramInfoLog(coinShaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
	glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
	
	
   	glGenBuffers((HEIGHT/SNAKE)*(BREADTH/SNAKE), &eboSnake[0][0]);	
    glGenVertexArrays((HEIGHT/SNAKE)*(BREADTH/SNAKE), &vaoSnake[0][0]);	
    glGenBuffers((HEIGHT/SNAKE)*(BREADTH/SNAKE), &vboSnake[0][0]);	
    unsigned int indices[]=
    {
    	0,1,3,
    	1,2,3
    };
    float x=(1.0/((BREADTH/SNAKE)/2.0)),y=(1.0/((HEIGHT/SNAKE)/2.0));
    cout<<x<<" "<<y<<endl;
    for(i=0;i<HEIGHT/SNAKE;i++)
	{
		for(j=0;j<BREADTH/SNAKE;j++)
		{
			float vertices[]=
			{
				(-1+(x*j)),(1-(y*i)),0.0,
				(-1+(x*j)+x),(1-(y*i)),0.0,
				(-1+(x*j)+x),(1-(y*i)-y),0.0,
				(-1+(x*j)),(1-(y*i)-y),0.0
			};

			glBindVertexArray(vaoSnake[i][j]);
			
			glBindBuffer(GL_ARRAY_BUFFER,vboSnake[i][j]);
			glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboSnake[i][j]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    		glEnableVertexAttribArray(0);

    		glBindBuffer(GL_ARRAY_BUFFER, 0); 
    		glBindVertexArray(0); 
		}
	}
    	
	int nextX,nextY;
	queue <int> xVal,yVal;
	
	for(i=0;i<(HEIGHT/SNAKE);i++)
	{
		for(j=0;j<(BREADTH/SNAKE);j++)
		{
			board[i][j]=0;
		}
	}
	board[(HEIGHT/SNAKE)/2][(BREADTH/SNAKE)/2]=1;
	xVal.push((BREADTH/SNAKE)/2);
	yVal.push((HEIGHT/SNAKE)/2);

	glfwSetKeyCallback(window, key_callback);
	float lastTime=glfwGetTime();
	
	int coinX,coinY,flag=0,score=0;
	generateCoinNormal(coinX,coinY);
	while(!glfwWindowShouldClose(window))
	{
		while(1)
		{
			float currentTime=glfwGetTime();
			if(currentTime-lastTime>=(1.0)/framerate)
			{
				lastTime=currentTime;
				break;
			}
		}
		processInput(window);
		glClearColor(0.2f,0.3f,0.3f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		//Moving the snake
		if(state.empty()==0)
		{
			curr_state=state.front();
			state.pop();
		}
		switch(curr_state)
		{
			case 0:
				nextX=xVal.back();
				nextY=(yVal.back()-1);
				//cout<<nextX<<" "<<nextY<<endl;
				if(nextY==-1)
				{
					nextY=(HEIGHT/SNAKE)-1;
				}
				break;
			case 1:
				nextX=(xVal.back()+1)%(BREADTH/SNAKE);
				nextY=yVal.back();
				break;
			case 2:
				nextX=xVal.back();
				nextY=(yVal.back()+1)%(HEIGHT/SNAKE);
				break;
			case 3:
				nextX=(xVal.back()-1);
				nextY=yVal.back();
				if(nextX==-1)
				{
					nextX=(BREADTH/SNAKE)-1;
				}
				break;
		}
		cout<<nextX<<" "<<nextY<<endl;
		if(board[nextY][nextX]==1)
		{
			cout<<"Game Over\n";
			gameOver(window);
		}
		//updating board and queue
		board[nextY][nextX]=1;
		if(!flag)
		{
			board[yVal.front()][xVal.front()]=0;
			xVal.pop();
			yVal.pop();
		}
		else
		{
			flag=0;
		}
		xVal.push(nextX);
		yVal.push(nextY);
		if(coinX==nextX && coinY==nextY)
		{
			score++;
			framerate+=(0.2);
			cout<<"score="<<score<<endl;
			generateCoinNormal(coinX,coinY);
			flag=1;
		}
		//rendering
		for(i=0;i<HEIGHT/SNAKE;i++)
		{
			for(j=0;j<BREADTH/SNAKE;j++)
			{
				if(board[i][j]==1)
				{
					glUseProgram(shaderProgram);
					glBindVertexArray(vaoSnake[i][j]);
					glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
				}
			}
		}
		glUseProgram(coinShaderProgram);
		glBindVertexArray(vaoSnake[coinY][coinX]);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
		glfwSwapBuffers(window);
		
		glfwPollEvents();
		
		
	}
	glDeleteVertexArrays(1, &vaoSnake[0][0]);
    glDeleteBuffers(1, &vboSnake[0][0]);
    glDeleteBuffers(1, &eboSnake[0][0]);
    
	glfwTerminate();
	return 0;
}