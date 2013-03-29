#include "stdlib.h"
#include "include/glut.h"
#include <iostream>
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

//#include "App.h"
//#include "App2.h"
//#include "App3.h"
#include "App4.h"

#include "windows.h"
using namespace std;

UdpListeningReceiveSocket *s = NULL;

static unsigned long __stdcall OpenNetworkConnection(void *ptr);

#define USE_NETWORK 1

int CleanupEverything() {
	cout << "Cleaning up..." << endl;
	if (s) s->Break();
	theApp.quit();
	return 0;
}

int main(int argc, char **argv) {

	onexit(CleanupEverything);

	bool online = false;

#if USE_NETWORK
	online = true;
#endif

	bool ok = theApp.init(online);

	if (!ok) {
		cout << "Failed to initialize App. Exiting.." << endl;
		return -1;
	}

	if (online) {
		DWORD threadId = 0;
		::CreateThread(0, 0, &OpenNetworkConnection, 0, 0, &threadId);
		::Sleep(0);
	}

	glutMainLoop();

	system("pause");

	return 0;
}

static unsigned long __stdcall OpenNetworkConnection(void *ptr) {
	s = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, PORT_UDP_IN),  theApp.getThis());

	cout << "Listening to UDP port = " << PORT_UDP_IN << endl;
	s->RunUntilSigInt();

	return 0;
}