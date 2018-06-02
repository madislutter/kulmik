#include <NTPClient.h>
#include <Time.h>
#include <Timezone.h>

TimeChangeRule eeDST = {"eeDST", Last, Sun, Mar, 3, 180}; // UTC+3 alates märtsi viimasest pühapäevast kell 03:00
TimeChangeRule eeSTD = {"eeSTD", Last, Sun, Oct, 4, 120}; // UTC+2 alates oktoobri viimasest pühapäevast kell 04:00
Timezone myTZ(eeDST, eeSTD);

void setupNTP() {
  NTP.init("ee.pool.ntp.org");
  NTP.setPollingInterval(90000); // 25h. Kuna võrguühendus toimib vaid käivitumisel, siis keelame pmst pollingu. Iga 24h tunni tagant on restart.
}

void printTime() {
  if (timeStatus() != timeSet)
    return;
  time_t t = myTZ.toLocal(now());
  char BUFFER[21];
  sprintf(BUFFER, "%02u:%02u:%02u  %02u.%02u.%04u", hour(t), minute(t), second(t), day(t), month(t), year(t));
  lcd.printRow(3, BUFFER);
}

bool timeToRestart() {
  if (timeStatus() != timeSet)
    return false;
  time_t t = myTZ.toLocal(now());
  if (hour(t) % 2 == 0 && minute(t) == 0 && second(t) < 7) {
    return true;
  }

  return false;
}

