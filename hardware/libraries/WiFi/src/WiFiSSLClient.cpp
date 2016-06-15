#include "WiFi.h"
#include "WiFiSSLClient.h"

extern "C" {
  #include "wl_definitions.h"
  #include "wl_types.h"
  #include "string.h"
}

WiFiSSLClient::WiFiSSLClient(){
    _is_connected = false;
	_sock = -1;
	//memset(&sslclient, 0, sizeof(sslclient_context));
	sslclient.socket = -1;
}

WiFiSSLClient::WiFiSSLClient(uint8_t sock) {
    _sock = sock;
	//memset(&sslclient, 0, sizeof(sslclient_context));
	sslclient.socket = sock;
    if(sock >= 0)
        _is_connected = true;
}

uint8_t WiFiSSLClient::connected() {
  	if (sslclient.socket < 0) {
		_is_connected = false;
    	return 0;
  	}
	else {
		if(_is_connected)
			return 1;
		else{
			stop();
			return 0;
		}
	}
}

int WiFiSSLClient::available() {
	int ret = 0;
	if(!_is_connected)
		return 0;
  	if (sslclient.socket >= 0)
  	{	
      	ret = ssldrv.availData(&sslclient);
		if(ret == 0){
			_is_connected = false;
			return 0;
		}
		else{
			return 1;
		}
  	}
}

int WiFiSSLClient::read() {
  	uint8_t b[1];
	
  	if (!available())
    	return -1;

  	if(ssldrv.getData(&sslclient, b))
  		return b[0];
	else{
		_is_connected = false;
	}
	return -1;
}

int WiFiSSLClient::read(uint8_t* buf, size_t size) {

  	uint16_t _size = size;
	int ret;
	ret = ssldrv.getDataBuf(&sslclient, buf, _size);
  	if (ret <= 0){
		_is_connected = false;
  	}
  	return ret;
}

void WiFiSSLClient::stop() {

  	if (sslclient.socket < 0)
    	return;

  	ssldrv.stopClient(&sslclient);
	_is_connected = false;
	
  	sslclient.socket = -1;
	_sock = -1;
}

size_t WiFiSSLClient::write(uint8_t b) {
	  return write(&b, 1);
}

size_t WiFiSSLClient::write(const uint8_t *buf, size_t size) {
  	if (sslclient.socket < 0)
  	{
	  	return 0;
  	}
  	if (size == 0)
  	{
      	return 0;
  	}

  	if (!ssldrv.sendData(&sslclient, buf, size))
  	{
		_is_connected = false;
      	return 0;
  	}
	
  	return size;
}

WiFiSSLClient::operator bool() {
  	return sslclient.socket >= 0;
}

int WiFiSSLClient::connect(const char* host, uint16_t port) {
	IPAddress remote_addr;
	
	if (WiFi.hostByName(host, remote_addr))
	{
		return connect(remote_addr, port);
	}
	return 0;
}

int WiFiSSLClient::connect(IPAddress ip, uint16_t port) {
	int ret = 0;
	
	ret = ssldrv.startClient(&sslclient, ip);

    if (ret < 0) {
        _is_connected = false;
        return 0;
    } else {
        _is_connected = true;
    }

    return 1;
}
int WiFiSSLClient::peek() {
	uint8_t b;

	if (!available())
		return -1;

	ssldrv.getData(&sslclient, &b);

	return b;
}
void WiFiSSLClient::flush() {
	while (available())
		read();
}