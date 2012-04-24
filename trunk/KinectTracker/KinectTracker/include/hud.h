//CONSTANTS
//Height Bar - Locked to Top Left
#define HUD_HEIGHT_BAR_X 5
#define HUD_HEIGHT_BAR_Y 5
//Roll Gauge - Locked to Top Left
#define HUD_ROLL_RADIUS 30
#define HUD_ROLL_X 95
#define HUD_ROLL_Y 48
//Pitch Gauge - Locked to Top Left
#define HUD_PITCH_RADIUS 30
#define HUD_PITCH_X 95
#define HUD_PITCH_Y 130


//FUNCTION DECLARATIONS
void drawHud();
void drawHeightHud(int topx, int topy, float height);
void drawRollHud(int cx, int cy, int r, float roll);
void drawPitchHud(int cx, int cy, int r, float pitch);
void drawTopDownMap(int cx, int cy, int r);
void drawPlacingHelp(int topRx, int topRy);
void drawModelInfo(int botRx, int botRy);
void drawKeyboardHelp(int botLx, int botLy);