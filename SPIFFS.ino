const uint8_t uidWidth = 20;
const uint8_t saldoWidth = 6;
const uint8_t barWidth = 40;

bool convertToode(String input, char* buf) {
  if (input.length() < 5)
    return false;
  int inputPointer = 0;
  int bufferPointer = 0;

  // BAR
  while (input.charAt(inputPointer) != ',')
    buf[bufferPointer++] = input.charAt(inputPointer++);
  // Right-pad BAR
  while (bufferPointer < barWidth)
    buf[bufferPointer++] = ' ';

  while (inputPointer < input.length())
    buf[bufferPointer++] = input.charAt(inputPointer++);
  buf[bufferPointer] = '\0';

  return true;
}

bool convertLiige(String input, char* buf) {
  if (input.length() < 5)
    return false;
  int inputPointer = 0;
  int bufferPointer = 0;
  // UID
  while (input.charAt(inputPointer) != ',')
    buf[bufferPointer++] = input.charAt(inputPointer++);
  // Right-pad UID
  while (bufferPointer < uidWidth)
    buf[bufferPointer++] = ' ';

  // Nimi
  buf[bufferPointer++] = input.charAt(inputPointer++);
  while (input.charAt(inputPointer) != ',')
    buf[bufferPointer++] = input.charAt(inputPointer++);

  // Saldo
  int saldoBeginPos = bufferPointer;
  buf[bufferPointer++] = input.charAt(inputPointer++);
  while (input.charAt(inputPointer) != ',')
    buf[bufferPointer++] = input.charAt(inputPointer++);
  // Right-pad saldo
  while (bufferPointer - saldoBeginPos < saldoWidth+1)
    buf[bufferPointer++] = ' ';

  // onAdmin ja muu
  while (inputPointer < input.length())
    buf[bufferPointer++] = input.charAt(inputPointer++);
  buf[bufferPointer] = '\0';

  return true;
}

bool olemeSuletud() {
  File f = SPIFFS.open(seadedFail, "r");
  String txt = f.readStringUntil('\n');
  f.close();

  return txt[0] == '1';
}

void sulgeTehinguteks() {
  File f = SPIFFS.open(seadedFail, "w");
  f << "1";
  f.close();
}

void avaTehinguteks() {
  File f = SPIFFS.open(seadedFail, "w");
  f << "0";
  f.close();
}

// Kasutame vaid nende tehingute jaoks, mida tabelisse salvestada ei õnnestunud
bool kirjutaTehing(Liige &l, Toode &t) {
  File f = SPIFFS.open(tehingudFail, "a");
  f << l.nimi << "," << t.nimi << endl;
  f.close();
}

String salvestamataTehingud() {
  File f = SPIFFS.open(tehingudFail, "r");
  String tehingud = readAll(f);
  f.close();

  return tehingud;
}

bool kustutaSalvestamataTehingud() {
  SPIFFS.open(tehingudFail, "w").close();
}

bool kirjutaLiige(Liige &l) {
  File f = SPIFFS.open(liikmedFail, "r+");
  if (!f || !seekTo(","+l.nimi+",", f)) // nimi on komade vahel, et matchiks kogu fieldi
    return false;
  f << liige2csv(l);
  f.close();

  return true;
}

bool kirjutaToode(Toode &t) {
  File f = SPIFFS.open(tootedFail, "r+");
  if (!f || !seekTo(","+t.nimi+",", f)) // nimi on komade vahel, et matchiks kogu fieldi
    return false;
  f << toode2csv(t);
  f.close();

  return true;
}

String toode2csv(Toode &t) {
  String txt;
  txt.reserve(88);

  txt += t.barcode;
  while (txt.length() < barWidth)
    txt += " ";
  txt += "," + t.nimi + "," + t.hind + "," + t.kaal;

  return txt;
}

String liige2csv(Liige &l) {
  String txt;
  txt.reserve(80);

  txt += l.nfc_id;
  while (txt.length() < uidWidth)
    txt += " ";
  txt += "," + l.nimi + "," + l.saldo;
  while (txt.length() - uidWidth - l.nimi.length() - 2 < saldoWidth)
    txt += " ";

  if (l.isAdmin) {
    txt += ",1";
  } else {
    txt += ",0";
  }

  return txt;
}

Toode csv2toode(String csv) {
 int endOfBar = csv.indexOf(',');
 int endOfNimi = csv.indexOf(',', endOfBar+1);
 int endOfHind = csv.indexOf(',', endOfNimi+1);

 String triipkood = csv.substring(0, endOfBar);
 triipkood.trim();
 String nimi = csv.substring(endOfBar+1, endOfNimi);
 String hind = csv.substring(endOfNimi+1, endOfHind);
 String kaal = csv.substring(endOfHind+1);

 return {nimi, hind.toInt(), triipkood, kaal.toInt()};
}

Liige csv2liige(String csv) {
  int endOfNfcUid = csv.indexOf(',');
  int endOfNimi = csv.indexOf(',', endOfNfcUid+1);
  int endOfSaldo = csv.indexOf(',', endOfNimi+1);

  String nfc_uid = csv.substring(0, endOfNfcUid);
  nfc_uid.trim();
  String nimi = csv.substring(endOfNfcUid+1, endOfNimi);
  String saldo = csv.substring(endOfNimi+1, endOfSaldo);
  String isAdmin = csv.substring(endOfSaldo+1);

  return {nimi, saldo.toInt(), nfc_uid, isAdmin == "1" ? true : false};
}

bool findToodeByBAR(String bar, Toode &t) {
  while (bar.length() < barWidth)
    bar += " ";

  File f = SPIFFS.open(tootedFail, "r");
  if (!f || !seekTo(bar, f))
    return false;
  t = csv2toode(f.readStringUntil('\n'));
  f.close();

  return true;
}

bool findToodeByName(String nimi, Toode &t) {
  File f = SPIFFS.open(tootedFail, "r");
  if (!f || !seekTo("," + nimi + ",", f))
    return false;
  t = csv2toode(f.readStringUntil('\n'));
  f.close();

  return true;
}

bool findLiigeByUID(String uid, Liige &l) {
  while (uid.length() < uidWidth)
    uid += " ";

  File f = SPIFFS.open(liikmedFail, "r");
  if (!f || !seekTo(uid, f))
    return false;
  l = csv2liige(f.readStringUntil('\n'));
  f.close();

  return true;
}

bool findLiigeByName(String nimi, Liige &l) {
  File f = SPIFFS.open(liikmedFail, "r");
  if (!f || !seekTo("," + nimi + ",", f))
    return false;
  l = csv2liige(f.readStringUntil('\n'));
  f.close();

  return true;
}

String readAll(Stream& s) {
  String txt;
  const int buf_len = 2048;
  size_t bytes_read = 0;
  char buf[buf_len] = {0};
  while (s.available()) {
    bytes_read = s.readBytes(buf, buf_len-1);
    buf[bytes_read] = '\0';
    txt += buf;
  }

  return txt;
}

bool seekTo(String needle, File& f) {
  String txt = readAll(f);
  long pos = txt.indexOf(needle);
  if (pos == -1)
    return false;

  // Leia rea algus
  while (pos > 0 && txt.charAt(pos-1) != '\n')
    pos--;

  f.seek(pos, SeekSet);
  return true;
}

bool seekTo(int lineIdx, File &f) {
  String txt = readAll(f);
  long pos = 0;
  for (int i = 0; i < lineIdx; i++) {
    if (pos < 0)
      return false;
    pos = txt.indexOf('\n', pos) + 1;
  }

  f.seek(pos, SeekCur);
  return true;
}

void appendToFile(String fileName, String txt) {
  File f = SPIFFS.open(fileName, "a");
  f << txt << endl;
  f.close();
}

byte fillBufferLiikmed(int startIdx, byte amount, char* buffer, byte bufLen) {
  File f = SPIFFS.open(liikmedFail, "r");
  String txt = readAll(f);
  f.close();

  byte amountFound = 0;
  long lineStart = 0;
  for (int i = 0; i < startIdx; i++) // Leia startIdx-s \n täht
    lineStart = txt.indexOf('\n', lineStart) + 1;
  if (lineStart == -1)
    return 0;

  long lineEnd = txt.indexOf('\n', lineStart);
  String line, nimi;
  int bPointer = 0, nimiStart, nimiEnd;
  while (amountFound < amount) {
    line = txt.substring(lineStart, lineEnd);
    if (line.length() < 2)
      break;

    nimiStart = line.indexOf(',') + 1;
    nimiEnd = line.indexOf(',', nimiStart);
    nimi = line.substring(nimiStart, nimiEnd);
    for (int i = 0; i < nimi.length(); i++)
      buffer[bPointer++] = nimi[i];
    buffer[bPointer++] = '\0';
    amountFound++;

    lineStart = lineEnd+1;
    lineEnd = txt.indexOf('\n', lineStart);
    if (lineEnd == -1)
      break;
  }

  return amountFound;
}

byte fillBufferTooted(int startIdx, byte amount, char* buffer, byte bufLen) {
  File f = SPIFFS.open(tootedFail, "r");
  String txt = readAll(f);
  f.close();

  byte amountFound = 0;
  long lineStart = 0;
  for (int i = 0; i < startIdx; i++) // Leia startIdx-s \n täht
    lineStart = txt.indexOf('\n', lineStart) + 1;
  if (lineStart == -1)
    return 0;

  long lineEnd = txt.indexOf('\n', lineStart);
  String line, nimi;
  int bPointer = 0, nimiStart, nimiEnd;
  while (amountFound < amount) {
    line = txt.substring(lineStart, lineEnd);
    if (line.length() < 2)
      break;

    nimiStart = line.indexOf(',') + 1;
    nimiEnd = line.indexOf(',', nimiStart);
    nimi = line.substring(nimiStart, nimiEnd);
    for (int i = 0; i < nimi.length(); i++)
      buffer[bPointer++] = nimi[i];
    buffer[bPointer++] = '\0';
    amountFound++;

    lineStart = lineEnd+1;
    lineEnd = txt.indexOf('\n', lineStart);
    if (lineEnd == -1)
      break;
  }

  return amountFound;
}
