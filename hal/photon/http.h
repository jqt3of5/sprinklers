extern const int EEPROM_HOST_ADDR;

struct IpAddr
{
  byte addr[4];
};

void http_handler(const char* url, ResponseCallback* cb, void* cbArg, Reader*body, Writer*result, void*reserved);
