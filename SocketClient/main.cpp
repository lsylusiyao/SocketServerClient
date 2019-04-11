#include "SocketClient.h"
using namespace std;
int main() {
	auto s = new SocketClass("127.0.0.1", 7999);
	system("pause");
	s->Init_Client();
	string a;
	//while (1)
	{
		cin >> a;
		s->SendCmd(a);
		s->Receive();
	}
	//getchar();
	return 0;
}