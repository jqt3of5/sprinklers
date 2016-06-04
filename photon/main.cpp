#pragma SPARK_NO_PREPROCESSOR
#include <Particle.h>
#include <softap_http.h>

const int EEPROM_HOST_ADDR = 0;

struct Page
{
  char *url;
  char *mime_type;
  int (*handler)(char ** urls, char ** keys, char ** values, char*&response);
};

int handle_config(char ** urls, char ** keys, char**values, char * &response)
{
  for (int i = 0; keys[i] != nullptr; ++i)
  {
    if (!strcmp(keys[i], "ip"))
    {
      char ip[4] = {0};
      ip[0] = atoi(strtok(values[i], "."));
      for (int i = 1; i < 4; ++i)
      {
        ip[i] = atoi(strtok(0, "."));
      }
      EEPROM.put(EEPROM_HOST_ADDR, ip);
    }
  }

  char ip_test[4] = {0};
  EEPROM.get(EEPROM_HOST_ADDR, ip_test);
  response = new char[20]();
  sprintf(response, "%d.%d.%d.%d", ip_test[0], ip_test[1], ip_test[2], ip_test[3]);

  return 200;
}

int handle_reset(char **urls, char **keys, char ** values, char *& response)
{
  response = new char[3]();
  strcpy(response, "OK");

  return 200;
}

Page pages[] = {
  {"config", "text/html", handle_config},
  {"reset", "text/html", handle_reset},
  {nullptr}

};

void parse_url(const char * url, char ** &urls, char ** &keys, char ** &values)
{
  char * url_part = strtok((char *)url, "?");
  char * query_part = strtok(nullptr, "?");

  urls = new char*[10]();
  char * part = strtok(url_part, "/");
  for (int i = 0; i < 10 && part != nullptr; i++)
  {
    urls[i] = part;
    part = strtok(nullptr, "/");
  }

  char ** params = new char*[10]();
  char * param = strtok(query_part, "&");
  for (int i = 0; i < 10 && param != nullptr; i++)
  {
    params[i] = param;
    param = strtok(nullptr, "&");
  }

  keys = new char*[10]();
  values = new char*[10]();
  for (int i = 0; i < 10 && params[i] != nullptr; ++i)
  {
    keys[i] = strtok(params[i], "=");
    values[i] = strtok(nullptr, "=");
  }
}

void http_handler(const char* url, ResponseCallback* cb, void* cbArg, Reader*body, Writer*result, void*reserved)
{
  char ** urls, **keys, **values;
  parse_url(url, urls, keys, values);

  for (int i = 0; i < 2; ++i)
  {
    if (!strcmp(pages[i].url, urls[0]))
    {
      char * response;
      int responseCode = pages[i].handler(urls, keys, values, response);

      cb(cbArg, 0, responseCode, pages[i].mime_type, nullptr);
      result->write(response);
      free(response);
    }
  }
  free(urls);
  free(keys);
  free(values);
}

STARTUP(softap_set_application_page_handler(http_handler, nullptr));

TCPClient client;
void setup()
{
  pinMode(D7, OUTPUT);

  byte addr[4] = {0};
  EEPROM.get(EEPROM_HOST_ADDR, addr);
  if (addr[0] == 0xFF)
  {
    //If no address is set, start listening.
    WiFi.listen();
  }
  else if (!client.connect(addr, 8081))
  {
    //If connecting failed, fall back to listening mode
    WiFi.listen();
  }

}

void loop()
{
  //char addr[4] = {0};
  //EEPROM.get(EEPROM_HOST_ADDR, addr);
  if (client.connected())
  {
    //client.printf("%d.%d.%d.%d", addr[0], addr[1],addr[2],addr[3]);
    while (client.available())
    {
      client.read();
    }
  }
}
