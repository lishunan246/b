//
// Created by lishunan on 4/30/16.
//

#ifndef DESKTOP_REMOTESCREEN_H
#define DESKTOP_REMOTESCREEN_H

#include <fstream>
#include "RepeatWork.h"
#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "ProxyObjectBase.hpp"
#include "RealObjectBase.hpp"


using namespace GkcHostSvc;


class RemoteScreen
{
protected:
	string _type = "RemoteScreen";
public:
	virtual ~RemoteScreen()
	{
	}

	virtual string get_screen() = 0;
	virtual void active() = 0;
	virtual void deactive() = 0;
};

class ProxyRemoteScreen : public RemoteScreen, public ProxyObjectBase
{
	int SCREEN_WIDTH = 512;
	int SCREEN_HEIGHT = 512;
	string _screen;
	SDL_Window* _window;
	SDL_Surface* _screenSurface,*g;
	shared_ptr<thread> pthread;
	const char* BuFile[3] = { "G:\\btn1.bmp","G:\\1080.bmp","G:\\btn2.bmp" };
	SDL_Renderer *Render = NULL;
	SDL_Surface *BackGround = NULL, *ButtonSur[3] = { NULL, NULL, NULL };
	SDL_Texture *BackText = NULL, *ButtonText = NULL;
	SDL_Rect ButtRect = { 0,0,64,64 };
	bool quit = false;
public:
	explicit ProxyRemoteScreen(pConnection connection) : ProxyObjectBase(connection, RemoteScreen::_type)
	{
		connection->registerHandler(RemoteScreen::_type, get_message_handler());
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			cerr << "SDL_Init() Failed: " << SDL_GetError() << endl;
			exit(1);
		}
		_window = SDL_CreateWindow("App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (_window == nullptr)
		{
			cerr << "can't create window " << SDL_GetError() << endl;
		}
		//pthread = make_shared<thread>([this]()
		{
			const Uint32 FPS = 20;
			SDL_Event Mouse;
			Uint32 _FPS_Timer = 0, Buttonnow = 0;

			createRender();
			loadBackGround();
			drawBackGround();
			loadButtonPicture();
			updateButton(0);
			while (!quit) {
				while (SDL_PollEvent(&Mouse)) {
					switch (Mouse.type) {
					case SDL_QUIT:
						quit = true;
						break;
					case SDL_MOUSEBUTTONDOWN:
						cout << Mouse.button.x << Mouse.button.y;
						if (Mouse.button.button == SDL_BUTTON_LEFT)
							if (isOnButton(Mouse.button.x, Mouse.button.y))
								if (Buttonnow != 2) {
									updateButton(2);
									Buttonnow = 2;
									string tmp = get_screen();
									ofstream outfile("tmp.bmp", ofstream::binary | ofstream::trunc);
									outfile << tmp;
									outfile.close();
									SDL_FreeSurface(BackGround);
									BackGround = SDL_LoadBMP("tmp.bmp");
								}
						break;
					case SDL_MOUSEMOTION:
						if (isOnButton(Mouse.button.x, Mouse.button.y)) {
							if (Buttonnow != 1) {
								updateButton(1);
								Buttonnow = 1;
							}
						}
						else if (Buttonnow != 0) {
							updateButton(0);
							Buttonnow = 0;
						}
						break;
					case SDL_MOUSEBUTTONUP:
						if (Buttonnow != 0) {
							updateButton(0);
							Buttonnow = 0;
						}
						break;
					}
				}
				if (SDL_GetTicks() - _FPS_Timer<1000 / FPS)
					SDL_Delay(1000 / FPS - SDL_GetTicks() + _FPS_Timer);
				_FPS_Timer = SDL_GetTicks();
			}
		}//);
	}

	SDL_Window* getWindow() {
		return _window;
	}

	void createRender() {
		Render = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
	}

	void loadBackGround() {
		BackGround = SDL_LoadBMP("G:\\4m.bmp");
	}

	void loadButtonPicture() {
		for (int i=0; i<3; i++)
			ButtonSur[i] = SDL_LoadBMP(BuFile[i]);
	}

	void setQuit() {
		quit = true;
	}

	bool getQuit() {
		return quit;
	}

	bool drawBackGround() {
		SDL_DestroyTexture(BackText);
		BackText = SDL_CreateTextureFromSurface(Render, BackGround);
		if (BackText == NULL) return false;
		SDL_RenderClear(Render);
		SDL_RenderCopy(Render, BackText, NULL, NULL);
		SDL_RenderPresent(Render);
		return true;
	}

	bool isOnButton(Uint32 x, Uint32 y) {
		if (x >= 0 && x <= 64)
			if (y >= 0 && y <= 64) return true;
		return false;
	}

	bool updateButton(Uint32 ButtonMode) {
		drawBackGround();
		//const char* OpenButton = BuFile[ButtonMode];
		//ButtonSur = SDL_LoadBMP(OpenButton);
		Uint32 color_key = SDL_MapRGB(ButtonSur[ButtonMode]->format, 255, 255, 255);
		SDL_SetColorKey(ButtonSur[ButtonMode], SDL_TRUE, color_key);
		if (ButtonSur == NULL) return false;
		SDL_DestroyTexture(ButtonText);
		ButtonText = SDL_CreateTextureFromSurface(Render, ButtonSur[ButtonMode]);
		//if (BackText == NULL) return false;
		SDL_RenderCopy(Render, ButtonText, NULL, &ButtRect);
		SDL_RenderPresent(Render);
		return true;
	}

	void display(string gs)
	{
		
		auto now = std::time(nullptr);
		auto name = std::to_string(now) + ".bmp";

		ofstream outfile(name, ofstream::binary | ofstream::trunc);

		outfile << gs;
		outfile.close();

		g = SDL_LoadBMP(name.c_str());

		if (g == nullptr)
			cerr << "load fail" << SDL_GetError() << endl;

		_screenSurface = SDL_GetWindowSurface(_window);
		SDL_FillRect(_screenSurface, nullptr, SDL_MapRGB(_screenSurface->format, 0xff, 0xff, 0xff));
		SDL_BlitSurface(g, nullptr, _screenSurface, nullptr);
		SDL_UpdateWindowSurface(_window);
		SDL_FreeSurface(g);
		g = nullptr;
		std::remove(name.c_str());
	}

	void active() override
	{
		call("active");
	}

	void deactive() override
	{
		call("deactive");
	}

	string get_screen() override
	{
		call("get_screen");
		_screen = _response.resultbytes();
		cout << _response.ByteSize() << "transferred" << endl;
		return _screen;
	}

	void close()
	{
		pthread->join();
		SDL_DestroyWindow(_window);
		SDL_Quit();
		pthread.reset();
	}

	messageHandler get_message_handler()
	{
		return [this](RPCResponse response)
			{
				//auto p2 = static_pointer_cast<ProxyRemoteScreen>(p);
				auto x = response.resultbytes();
				this->display(x);
			};
	}
};

class RealRemoteScreen : public RemoteScreen, public RealObjectBase
{
	shared_ptr<RepeatWork> sPW;
	std::mutex m, m2;
	vector<string> files = {PATH + "4m.bmp", PATH + "1080.bmp"};
public:
	RealRemoteScreen()
	{
		sPW = make_shared<RepeatWork>(500, true, [this]()
		                              {
			                              this->m.lock();
			                              RPCResponse msg;
			                              msg.set_ispushmessage(true);
			                              msg.set_pushtype(this->get_type());
			                              auto x = this->get_screen();
			                              msg.set_resultbytes(x);
			                              string t;
			                              msg.SerializeToString(&t);
			                              cout << t.size() << endl;
										  try {
											  this->send(t);
										  }
										  catch (...) {
											  cerr << "push fail" << endl;
										  }


										  this->m.unlock();
		                              });
	}

	~RealRemoteScreen()
	{
		m.lock();
		cout << "destruct RS" << endl;
		sPW->deactive();
		sPW->close();
		sPW.reset();
		m.unlock();
	}

	void active() override
	{
		sPW->active();
	}

	void deactive() override
	{
		cout << "deactive" << endl;
		sPW->deactive();
	}

	string get_screen() override
	{
		static int i = 0;
		if (i == 0)
			i = 1;
		else
			i = 0;
		m2.lock();
		ifstream fin(files[i], ios::binary);
		if (!fin.is_open())
		{
			cerr << "fail openfile" << endl;
			return "";
		}
		auto x = fin.rdbuf();
		ostringstream ostrm;
		ostrm << x;
		fin.close();
		m2.unlock();
		return ostrm.str();
	}

	ObjectService get_service() const override
	{
		return [](const RPCRequest* rpcRequest, RPCResponse* response, const shared_ptr<RealObjectBase>& p)
			{
				auto xp = dynamic_pointer_cast<RealRemoteScreen>(p);
				auto method = rpcRequest->method();
				if (method == "active")
				{
					xp->active();

					response->set_success(true);
				}
				else if (method == "deactive")
				{
					xp->deactive();

					response->set_success(true);
				}
				else if (method == "get_screen")
				{
					auto x = xp->get_screen();

					response->set_success(true);
					response->set_resultbytes(x);
				}
				else
				{
					cerr << "unknow method" << endl;
				}
			};
	}

	const string& get_type() override
	{
		return _type;
	}

	function<shared_ptr<RealObjectBase>(void)> get_creator() override
	{
		return []()
			{
				return make_shared<RealRemoteScreen>();
			};
	}
};


#endif //DESKTOP_REMOTESCREEN_H
