const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
String url = "/macros/s/TABLE_ID_HERE/exec?";

// Kui Drive'i salvestamine ei õnnestu, siis salvestab SPIFFSi.
//bool salvestaTehing(Liige &l, Toode &t) {
//  l.saldo -= t.hind;
//  kirjutaLiige(l);
//  if (post("tehingud", l.nimi + "," + t.nimi)) {
//    return true;
//  } else {
//    kirjutaTehing(l, t); // Tabelisse salvestamine ei õnnestunud, jäta tehing meelde.
//    return false;
//  }
//}

bool salvestaTehing(Liige &l, Toode &t) {
  l.saldo -= t.hind;
  delay(10);
  kirjutaLiige(l);
  kirjutaTehing(l, t); // Tabelisse salvestamine ei õnnestunud, jäta tehing meelde.
  delay(10);
  return false;
}

bool salvestaSalvestamataTehingud() {
  String tehingud = salvestamataTehingud();
  if (tehingud.length() == 0)
    return true;
  if (post("tehingud", tehingud)) {
    kustutaSalvestamataTehingud();
    return true;
  }
  return false;
}

bool salvestaUID(Liige &l) {
  kirjutaLiige(l);
  return post("liikmed", l.nimi + "," + l.nfc_id);
}

bool salvestaTriipkood(Toode &t) {
  kirjutaToode(t);
  return post("tooted", t.nimi + "," + t.barcode);
}

bool vargusAlgas() {
  return post("vargus", "");
}

bool vargusTimeout() {
  return post("vargus", "timeout");
}

bool vargusVaigistatud(String nimi) {
  return post("vargus", nimi);
}

bool post(String endpoint, String payload) {
  yield();
  unsigned long t0 = millis();
  Serial << F("POST to ") << endpoint << ", with " << payload << endl;
  Serial << F("Free: ") << ESP.getFreeHeap() << endl;
  HTTPSRedirect client(443);

  int tries = 0;
  bool connected = false;
  while (!connected && tries < 3) {
    connected = client.connect(host, 443);
    tries++;
    Serial << F("Try nr ") << tries << F(". Time spent: ") << millis() - t0 << endl;
  }

  if (connected) {
    if (!client.postRedir(url+endpoint, host, googleRedirHost, payload)) {
      Serial << F("postRedir failed. free heap: ") << ESP.getFreeHeap() << endl;
      return false;
    }
  
    if (client.readStringUntil('\n') == "ok") {
      Serial << F("ok. total time: ") << millis() - t0 << endl;
      client.stop();
      return true;
    }
    Serial << F("not ok") << endl;
    client.stop();
    return false;
  }

  Serial << F("could not connect") << endl;
  client.stop();
  return false;
}

bool uuendaLiikmed() {
  HTTPSRedirect client(443);
  if (!client.connect(host, 443)) {
    Serial << F("connect failed. free heap: ") << ESP.getFreeHeap() << endl;
    return false;
  }
  if (!client.getRedir(url+"liikmedCSV", host, googleRedirHost)) {
    Serial << F("getRedir failed. free heap: ") << ESP.getFreeHeap() << endl;
    return false;
  }

  File f = SPIFFS.open(liikmedFail, "w");
  if (!f)
    return false;
  char buf[128] = {0};
  while (client.available() && convertLiige(client.readStringUntil('\n'), buf))
    f << buf << endl;
  f.close();

  return true;
}

bool uuendaTooted() {
  HTTPSRedirect client(443);
  if (client.connect(host, 443) != 1 || !client.getRedir(url+"tootedCSV", host, googleRedirHost))
    return false;

  File f = SPIFFS.open(tootedFail, "w");
  if (!f)
    return false;
  char buf[128] = {0};
  while (client.available() && convertToode(client.readStringUntil('\n'), buf))
    f << buf << endl;
  f.close();

  return true;
}

// Lahutab liikmete saldodest salvestamata tehingute summad. Kasutame selleks, et pärast restarti liikmete saldod õiged oleksid. Tabelist küsitud saldodes ei ole arvestatud külmiku mälus olevaid tehinguid.
void korrigeeriSaldod() {
  String tehingud = salvestamataTehingud();
  String tehing;
  Liige l;
  Toode t;

  int lineStart = 0;
  int lineEnd = tehingud.indexOf('\n');
  int koma = 0;
  while (lineEnd > 0) {
    tehing = tehingud.substring(lineStart, lineEnd);
    koma = tehing.indexOf(',');
    if (findLiigeByName(tehing.substring(0, koma), l) && findToodeByName(tehing.substring(koma+1), t)) {
      l.saldo -= t.hind;
      kirjutaLiige(l);
    }
    lineStart = lineEnd+1;
    lineEnd = tehingud.indexOf('\n', lineStart);
  }
}

