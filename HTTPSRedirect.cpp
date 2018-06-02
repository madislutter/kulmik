/*  HTTPS with follow-redirect
 *  Created by Sujay S. Phadke, 2016
 *  All rights reserved.
 *
 */
#include "HTTPSRedirect.h"

HTTPSRedirect::HTTPSRedirect(const int p, const char* fp, bool c) 
    : httpsPort(p), redirFingerprint(fp), fpCheck(c){
}

HTTPSRedirect::HTTPSRedirect(const int p) 
    : httpsPort(p){
      fpCheck = false;
}

HTTPSRedirect::~HTTPSRedirect(){ 
}

String HTTPSRedirect::createGETrequest(const char* url, const char* host){
  return String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: ESP8266\r\n" +
         "Connection: close" +
         "\r\n\r\n";
  
}

String HTTPSRedirect::createPOSTrequest(const char* url, const char* host, String payload) {
  return String("POST ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: ESP8266\r\n" +
         "Connection: close\r\n" +
         "Content-type: text/plain\r\n" +
         "Content-length: " + payload.length() + "\r\n" +
         "\r\n" +
         payload + "\r\n\r\n";
}

bool HTTPSRedirect::getRedir(String url, const char* host, const char* redirHost){
  return getRedir(url.c_str(), host, redirHost);
}

bool HTTPSRedirect::getRedir(const char* url, const char* host, const char* redirHost){
  if (!connected())
    return false;

  print(createGETrequest(url, host));
  String redirUrl = findRedirUrl(redirHost);
  flush();

  if (redirUrl.length() == 0 || !connect(redirHost, httpsPort))
    return false;

  print(createGETrequest(redirUrl.c_str(), redirHost));
  flush();
  find("\r\n\r\n");

  return true;
}

bool HTTPSRedirect::postRedir(String url, const char* host, const char* redirHost, String payload){
  return postRedir(url.c_str(), host, redirHost, payload);
}

bool HTTPSRedirect::postRedir(const char* url, const char* host, const char* redirHost, String payload){
  if (!connected()) {
    return false;
  }

  print(createPOSTrequest(url, host, payload));
  String redirUrl = findRedirUrl(redirHost);
  flush();

  if (redirUrl.length() == 0) {
    return false;
  }

  if (!connect(redirHost, httpsPort)) {
    return false;
  }

  print(createGETrequest(redirUrl.c_str(), redirHost));
  flush();
  find("\r\n\r\n");

  return true;
}

String HTTPSRedirect::findRedirUrl(const char* redirHost) {
  String line;

  while (connected()) {
    line = readStringUntil('\n');
    if (line == "\r")
      break;
    if (find("Location: ")) {
      find((char *)redirHost);
      return readStringUntil('\n');
    }
  }

  return "";
}
