#include <iostream>
#include <fstream>
#include <SDL.h>
#include <SDL_image.h>
#include <Box2D/Box2D.h>
#include <vector>

// Ported by @valt to SDL from https://www.youtube.com/user/FamTrinli voleyball tutorial in SFML
// todo wall sizes adjusmtment needed metres to pixels... 

enum {BACKGROUND = 0, BLOBBY, BALL};
const int sizeItem = 16;
const int sizeX = 800, sizeY = 600;
const int FPS = 60;
const int ANIMATION_DELAY = 500;
SDL_Rect board = { 0, 0, sizeX, sizeY };
Uint32 start;

const float SCALE = 30.f; // pixels to meters
const float DEG = 57.29577f;
b2Vec2 Gravity(0.f, 9.8f);
b2World World(Gravity);

void setWall(int x, int y, int w, int h)
{
	b2PolygonShape gr;
	gr.SetAsBox(w / SCALE, h / SCALE);
	b2BodyDef bdef;
	bdef.position.Set(x / SCALE, y / SCALE);
	b2Body *b_ground = World.CreateBody(&bdef);
	b_ground->CreateFixture(&gr, 1);

}

int main(int argc, char ** argv) {
	SDL_Window *win = nullptr;
	SDL_Renderer *renderer = nullptr;
	// Error checks
	std::cout << "SDL_Init\n";
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("Voleyball", 100, 50, sizeX, sizeY, 0);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow error\n";
	}
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cout << "SDL_CreateRenderer error\n";
	}
	// Load bitmaps
	std::vector<std::string> Bitmaps;
	Bitmaps.push_back("img/background.png");
	Bitmaps.push_back("img/blobby.png");
	Bitmaps.push_back("img/ball.png");
	// Create textures from bitmaps
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)){
		std::cout << "SDL_image could not initialize! SDL_image Error:" << IMG_GetError() << std::endl;
	}
	std::vector<SDL_Texture *> Textures;
	for (auto bitmap : Bitmaps) {
		Textures.push_back(IMG_LoadTexture(renderer, bitmap.c_str()));
		if (Textures.back() == nullptr) {
			std::cout << bitmap.c_str() << " SDL_CreateTextureFromSurface error\n";
		}
		else
			std::cout << bitmap.c_str() << " SDL_CreateTextureFromSurface OK\n";
	}
	/////////box2d///////////
	setWall(400, 520, 2000, 10);
	setWall(400, 450, 10, 170);
	setWall(0, 0, 10, 2000);
	setWall(800, 0, 10, 2000);

	b2PolygonShape shape;
	shape.SetAsBox(30 / SCALE, 30 / SCALE);
	b2BodyDef bdef;
	bdef.type = b2_dynamicBody;
	///players///////////////
	b2Body *pBody[2];
	for (int i = 0; i<2; i++) {
		bdef.position.Set(20 * i, 2);
		b2CircleShape circle;
		circle.m_radius = 32 / SCALE;
		circle.m_p.Set(0, 13 / SCALE);
		pBody[i] = World.CreateBody(&bdef);
		pBody[i]->CreateFixture(&circle, 5);
		circle.m_radius = 25 / SCALE;
		circle.m_p.Set(0, -20 / SCALE);
		pBody[i]->CreateFixture(&circle, 5);
		pBody[i]->SetFixedRotation(true);
	}
	pBody[0]->SetUserData("player1");
	pBody[1]->SetUserData("player2");
	/// ball /////////////
	bdef.position.Set(5, 1);
	b2CircleShape circle;
	circle.m_radius = 32 / SCALE;
	b2Body *b = World.CreateBody(&bdef);
	b2FixtureDef fdef;
	fdef.shape = &circle;
	fdef.restitution = 0.95;
	fdef.density = 0.2;
	b->CreateFixture(&fdef);
	b->SetUserData("ball");
	/////////////////////////
	
	while (1) {
		start = SDL_GetTicks();
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			b2Vec2 vel;
			b2Vec2 pos;
			if (e.type == SDL_QUIT) {
				break;
			}
			/* Look for a keypress */
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_a:
					vel = pBody[0]->GetLinearVelocity();
					vel.x = -5;
					pBody[0]->SetLinearVelocity(vel);
					break;
				case SDLK_d:
					vel = pBody[0]->GetLinearVelocity();
					vel.x = 5;
					pBody[0]->SetLinearVelocity(vel);
					break;
				case SDLK_w:
					pos = pBody[0]->GetPosition();
					vel = pBody[0]->GetLinearVelocity();
					if (pos.y*SCALE >= 463)  vel.y = -13;
					pBody[0]->SetLinearVelocity(vel);
					break;
				case SDLK_LEFT:
					vel = pBody[1]->GetLinearVelocity();
					vel.x = -5;
					pBody[1]->SetLinearVelocity(vel);
					break;
				case SDLK_RIGHT:
					vel = pBody[1]->GetLinearVelocity();
					vel.x = 5;
					pBody[1]->SetLinearVelocity(vel);
					break;
				case SDLK_UP:
					pos = pBody[1]->GetPosition();
					vel = pBody[1]->GetLinearVelocity();
					if (pos.y*SCALE >= 463)  vel.y = -13;
					pBody[1]->SetLinearVelocity(vel);
					break;
				default:
					std::cout << "Key not supported\n";
					break;
				}
			}
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.sym) {
				case SDLK_a:
					vel = pBody[0]->GetLinearVelocity();
					vel.x = -0;
					pBody[0]->SetLinearVelocity(vel);
					break;
				case SDLK_d:
					vel = pBody[0]->GetLinearVelocity();
					vel.x = 0;
					pBody[0]->SetLinearVelocity(vel);
					break;
				case SDLK_LEFT:
					vel = pBody[1]->GetLinearVelocity();
					vel.x = 0;
					pBody[1]->SetLinearVelocity(vel);
					break;
				case SDLK_RIGHT:
					vel = pBody[1]->GetLinearVelocity();
					vel.x = 0;
					pBody[1]->SetLinearVelocity(vel);
					break;
				};
			}
		}
		World.Step(1 / 60.f, 8, 3);
		//ball max speed
		b2Vec2 vel = b->GetLinearVelocity();
		if (vel.Length()>15) b->SetLinearVelocity(15 / vel.Length() * vel);

		SDL_RenderClear(renderer);
		// board texture
		auto texture = Textures[BACKGROUND];
		SDL_RenderCopy(renderer, texture, nullptr, &board);

		for (b2Body* it = World.GetBodyList(); it != 0; it = it->GetNext())
		{
			b2Vec2 pos = it->GetPosition();
			float angle = it->GetAngle();
			if (it->GetUserData() == "ball")
			{
				auto texture = Textures[BALL];
				SDL_Rect b = { pos.x*SCALE,  pos.y*SCALE, 64, 64};
				SDL_RenderCopyEx(renderer, texture, nullptr, &b, angle*DEG, nullptr, SDL_FLIP_NONE);
			}
			if (it->GetUserData() == "player1")
			{
				auto texture = Textures[BLOBBY];
				SDL_Rect b = { pos.x * SCALE, pos.y * SCALE, 75, 89 };
				SDL_RenderCopyEx(renderer, texture, nullptr, &b, angle*DEG, nullptr, SDL_FLIP_NONE);
			}
			if (it->GetUserData() == "player2")
			{
				auto texture = Textures[BLOBBY];
				SDL_Rect b = { pos.x * SCALE , pos.y * SCALE, 75, 89 };
				SDL_RenderCopyEx(renderer, texture, nullptr, &b, angle*DEG, nullptr, SDL_FLIP_NONE);
			}
		}
		SDL_RenderPresent(renderer);
		// Animation delay
		if (ANIMATION_DELAY / FPS > SDL_GetTicks() - start)
			SDL_Delay(ANIMATION_DELAY / FPS - (SDL_GetTicks() - start));
	}
	IMG_Quit();
	for (auto texture : Textures)
		SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "SDL_Quit\n";
	return 0;
}