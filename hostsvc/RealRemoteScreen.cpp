//
// Created by lishunan on 16-5-25.
//

#include "RealRemoteScreen.h"

RealRemoteScreen::RealRemoteScreen()
{

		string pre = "D:\\bmps\\test (";
		for (auto i = 1; i < 250; i++)
		{
			files.push_back(pre + std::to_string(i) + ").bmp");
		}
		for (auto& f:files)
		{
			auto x = get_SDL_RWops(f);
			vc.push_back(x);
		}
	

#ifdef  _DEBUG
	int ms = 200;
#else
	int ms = 20;
#endif

	sPW = make_shared<RepeatWork>(ms, true, [this]()
	                              {
		                              print("send frame");
		                              println(this->count);
		                              ++this->count;
		                              if (_index == 249)
			                              _index = 1;
		                              else
			                              ++_index;
		                              RPCResponse msg;
		                              msg.set_ispushmessage(true);
		                              msg.set_resultint(count);
		                              msg.set_pushtype(this->get_type());
		                              //auto x = this->get_SDL_RWops();
		                              auto t = vc[_index];
		                              msg.set_resultbytes(t.second, t.first);
		                              string s;
		                              msg.SerializeToString(&s);
		                              cout << s.size() << endl;
		                              try
		                              {
			                              this->send(s);
		                              }
		                              catch (...)
		                              {
			                              cerr << "push fail" << endl;
		                              }
	                              });
}

RealRemoteScreen::~RealRemoteScreen()
{
	m.lock();
	cout << "destruct RS" << endl;
	sPW->deactivate();
	sPW->close();
	sPW.reset();
	for (auto& x:vc)
	{
		delete[] x.second;
	}
	m.unlock();
	cout << "destruct RS success" << endl;
}
