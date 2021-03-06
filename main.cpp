//Albert Su
///Pong
//Hit spacebar to start

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <SDL_mixer.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


GLuint LoadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}

int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1100, 750, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 1100, 750);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	GLuint paddle = LoadTexture("paddle.png");
	GLuint ballTexture = LoadTexture("ball.png");
	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	float lastFrameTicks = 0.0f;

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	Mix_Chunk *someSound;
	someSound = Mix_LoadWAV("effect.wav");

	Mix_Music *music;
	music = Mix_LoadMUS("music.wav");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//right paddle
	float rightY = 0.0f;
	//left paddle
	float leftY = 0.0f;
	//ball center coordinates
	float ballX = 0.0f;
	float ballY = 0.0f;

	float ballYSpeed = 0.0f;
	//1 moves to the right, 0 to the left
	bool ballDir = 1;
	//starting position, ball and paddles set in middle
	bool reset = 1;

	Mix_PlayMusic(music, -1);
	SDL_Event event;
	bool done = false;
	while (!done)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}



		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (ballDir == 1)
			ballX += elapsed*2.0f;
		else
			ballX += -elapsed*2.0f;

		if (ballY >= 1.95f || ballY < -1.95f)
		{
		Mix_PlayChannel(-1, someSound, 0);
		ballYSpeed = -ballYSpeed;
		}

		ballY += ballYSpeed;
		//checks collision with right paddle
		if (ballX > 3.35 && rightY > ballY - 0.45f && rightY < ballY + 0.45f)
		{
			Mix_PlayChannel(-1, someSound, 0);
			ballDir = 0;
			ballYSpeed += (ballY - rightY)*elapsed;
		}
		//checks collision with left paddle
		if (ballX <-3.35 && leftY >ballY - 0.45f && leftY < ballY + 0.45f)
		{
			Mix_PlayChannel(-1, someSound, 0);
			ballDir = 1;
			ballYSpeed += (ballY - leftY)*elapsed;
		}

		if (ballX > 3.5 || ballX < -3.5)
			reset = 1;

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_DOWN])
		{
			if (rightY>-1.6)
				rightY += -elapsed*2.0f;
		}
		if (keys[SDL_SCANCODE_UP])
		{
			if (rightY<1.6)
				rightY += elapsed*2.0f;
		}
		if (keys[SDL_SCANCODE_S])
		{
			if (leftY>-1.6)
				leftY += -elapsed*2.0f;
		}
		if (keys[SDL_SCANCODE_W])
		{
			if (leftY<1.6)
				leftY += elapsed*2.0f;
		}
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, paddle);
		float vertices[] = { 3.4f, -0.4f + rightY, 4.0f, -0.4f + rightY, 4.0f, 0.4f + rightY, 3.4f, -0.4f + rightY, 4.0f, 0.4f + rightY, 3.4f, 0.4f + rightY };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, paddle);
		float vertices2[] = { -4.0f, -0.4f + leftY, -3.4f, -0.4f + leftY, -3.4f, 0.4f + leftY, -4.0f, -0.4f + leftY, -3.4f, 0.4f + leftY, -4.0f, 0.4f + leftY };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		if (reset)
		{
			ballX = 0.0;
			ballY = 0.0;
			rightY = 0.0;
			leftY = 0.0;
			ballYSpeed = 0.0;
			if (keys[SDL_SCANCODE_SPACE])
				reset = 0;
		}
		glBindTexture(GL_TEXTURE_2D, ballTexture);
		float vertices3[] = { -0.05f + ballX, -0.05f + ballY, 0.05f + ballX, -0.05f + ballY, 0.05f + ballX, 0.05f + ballY, -0.05f + ballX, -0.05f + ballY, 0.05f + ballX, 0.05f + ballY, -0.05f + ballX, 0.05f + ballY };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);
		float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
