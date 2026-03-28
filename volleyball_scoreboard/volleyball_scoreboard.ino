#include <TFT_eSPI.h>
#include <SPI.h>

// App de placar de vôlei (melhor de 3 sets)
// Regras:
// - Set vence com 21 pontos OU mais, desde que tenha 2 de vantagem.
// - A partir de 20x20 segue até abrir 2 pontos.
// - Partida em melhor de 3 (vence quem fizer 2 sets).

TFT_eSPI tft = TFT_eSPI();

struct Button {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  const char *label;
  uint16_t color;
};

const char *TEAM_A = "TIME A";
const char *TEAM_B = "TIME B";

int pointsA = 0;
int pointsB = 0;
int setsA = 0;
int setsB = 0;

bool setFinished = false;
bool matchFinished = false;
String banner = "Toque para pontuar";

Button btnAPlus;
Button btnAMinus;
Button btnBPlus;
Button btnBMinus;
Button btnReset;

uint32_t lastTouchMs = 0;
const uint16_t touchDebounceMs = 180;

const uint16_t COLOR_BG = TFT_WHITE;
const uint16_t COLOR_PANEL = 0xE71C;  // cinza claro
const uint16_t COLOR_ACCENT_A = 0x06DF;
const uint16_t COLOR_ACCENT_B = 0x05FF;
const uint16_t COLOR_PLUS = 0x07E0;
const uint16_t COLOR_MINUS = 0xFBE0;
const uint16_t COLOR_TEXT = TFT_BLACK;
const uint16_t COLOR_RESET = 0xFD20;

void drawButton(const Button &b) {
  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, b.color);
  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, TFT_BLACK);
  tft.setTextColor(COLOR_TEXT, b.color);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(b.label, b.x + b.w / 2, b.y + b.h / 2, 2);
}

void drawHeader() {
  tft.fillRect(0, 0, tft.width(), 40, COLOR_PANEL);
  tft.drawFastHLine(0, 40, tft.width(), TFT_DARKGREY);

  tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
  tft.setTextDatum(ML_DATUM);
  tft.drawString("SETS", 10, 20, 2);

  tft.setTextDatum(MC_DATUM);
  String setScore = String(setsA) + " x " + String(setsB);
  tft.drawString(setScore, tft.width() / 2, 20, 4);

  tft.setTextDatum(MR_DATUM);
  tft.drawString("Melhor de 3", tft.width() - 10, 20, 2);
}

void drawTeamPanel(int x, int w, const char *name, int points, uint16_t accent) {
  int top = 48;
  int h = tft.height() - 100;

  tft.fillRoundRect(x, top, w, h, 12, COLOR_PANEL);
  tft.drawRoundRect(x, top, w, h, 12, TFT_DARKGREY);

  tft.fillRect(x, top, w, 32, accent);
  tft.drawFastHLine(x, top + 32, w, TFT_DARKGREY);

  tft.setTextColor(TFT_BLACK, accent);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(name, x + w / 2, top + 16, 2);

  tft.setTextColor(TFT_BLACK, COLOR_PANEL);
  tft.drawString(String(points), x + w / 2, top + h / 2, 7);
}

void drawFooter() {
  int y = tft.height() - 46;
  tft.fillRect(0, y, tft.width(), 46, COLOR_PANEL);
  tft.drawFastHLine(0, y, tft.width(), TFT_DARKGREY);

  tft.setTextColor(COLOR_TEXT, COLOR_PANEL);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(banner, tft.width() / 2, y + 12, 2);

  drawButton(btnReset);
}

void renderAll() {
  tft.fillScreen(COLOR_BG);
  drawHeader();

  int margin = 8;
  int gap = 8;
  int panelW = (tft.width() - margin * 2 - gap) / 2;

  drawTeamPanel(margin, panelW, TEAM_A, pointsA, COLOR_ACCENT_A);
  drawTeamPanel(margin + panelW + gap, panelW, TEAM_B, pointsB, COLOR_ACCENT_B);

  int btnY = tft.height() - 90;
  int btnW = 70;
  int btnH = 36;

  btnAPlus = {margin + 20, btnY, btnW, btnH, "+ A", COLOR_PLUS};
  btnAMinus = {margin + 100, btnY, btnW, btnH, "- A", COLOR_MINUS};
  btnBPlus = {margin + panelW + gap + 20, btnY, btnW, btnH, "+ B", COLOR_PLUS};
  btnBMinus = {margin + panelW + gap + 100, btnY, btnW, btnH, "- B", COLOR_MINUS};
  btnReset = {tft.width() / 2 - 52, tft.height() - 30, 104, 24, "Reset", COLOR_RESET};

  drawButton(btnAPlus);
  drawButton(btnAMinus);
  drawButton(btnBPlus);
  drawButton(btnBMinus);

  drawFooter();
}

bool inside(const Button &b, int16_t tx, int16_t ty) {
  return tx >= b.x && tx <= (b.x + b.w) && ty >= b.y && ty <= (b.y + b.h);
}

void beginNextSet() {
  pointsA = 0;
  pointsB = 0;
  setFinished = false;
  banner = "Novo set iniciado";
}

void resetMatch() {
  pointsA = 0;
  pointsB = 0;
  setsA = 0;
  setsB = 0;
  setFinished = false;
  matchFinished = false;
  banner = "Placar reiniciado";
}

bool setWonByA() {
  return (pointsA >= 21 && (pointsA - pointsB) >= 2);
}

bool setWonByB() {
  return (pointsB >= 21 && (pointsB - pointsA) >= 2);
}

void validateSetEnd() {
  if (setFinished || matchFinished) return;

  if (setWonByA()) {
    setFinished = true;
    setsA++;
    banner = String(TEAM_A) + " venceu o set";
  } else if (setWonByB()) {
    setFinished = true;
    setsB++;
    banner = String(TEAM_B) + " venceu o set";
  }

  if (setFinished) {
    if (setsA == 2 || setsB == 2) {
      matchFinished = true;
      banner = (setsA == 2) ? String(TEAM_A) + " venceu a partida" : String(TEAM_B) + " venceu a partida";
    } else {
      delay(900);
      beginNextSet();
    }
  }
}

void addPointA() {
  if (matchFinished) return;
  pointsA++;
  banner = String(TEAM_A) + " pontuou";
  validateSetEnd();
}

void addPointB() {
  if (matchFinished) return;
  pointsB++;
  banner = String(TEAM_B) + " pontuou";
  validateSetEnd();
}

void removePointA() {
  if (matchFinished) return;
  if (pointsA > 0) pointsA--;
  banner = "Ajuste manual em " + String(TEAM_A);
}

void removePointB() {
  if (matchFinished) return;
  if (pointsB > 0) pointsB--;
  banner = "Ajuste manual em " + String(TEAM_B);
}

void setup() {
  tft.init();
  tft.setRotation(1);  // horizontal (landscape)
  tft.fillScreen(COLOR_BG);
  renderAll();
}

void loop() {
  uint16_t tx, ty;
  if (!tft.getTouch(&tx, &ty)) {
    return;
  }

  uint32_t now = millis();
  if (now - lastTouchMs < touchDebounceMs) {
    return;
  }
  lastTouchMs = now;

  if (inside(btnAPlus, tx, ty)) {
    addPointA();
  } else if (inside(btnAMinus, tx, ty)) {
    removePointA();
  } else if (inside(btnBPlus, tx, ty)) {
    addPointB();
  } else if (inside(btnBMinus, tx, ty)) {
    removePointB();
  } else if (inside(btnReset, tx, ty)) {
    resetMatch();
  }

  renderAll();
  delay(80);
}
