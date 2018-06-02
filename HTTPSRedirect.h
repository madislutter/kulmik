/*  HTTPS with follow-redirect
 *  Created by Sujay S. Phadke, 2016
 *  All rights reserved.
 *
 */

#include <WiFiClientSecure.h>

class HTTPSRedirect : public WiFiClientSecure {
  private:
    const int httpsPort;
    const char* redirFingerprint;
    bool fpCheck = false;
    bool keepAlive = true;
    bool verboseInfo = false;
    String createGETrequest(const char*, const char*);
    String createPOSTrequest(const char*, const char*, String payload);
    String findRedirUrl(const char* redirHost);
    
  public:
    HTTPSRedirect(const int, const char*, bool);
    HTTPSRedirect(const int);
    ~HTTPSRedirect();

    bool getRedir(String url, const char* host, const char* redirHost);
    bool getRedir(const char* url, const char* host, const char* redirHost);
    bool postRedir(String url, const char* host, const char* redirHost, String payload);
    bool postRedir(const char* url, const char* host, const char* redirHost, String payload);
};
