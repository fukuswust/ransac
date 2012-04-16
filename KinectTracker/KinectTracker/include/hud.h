//CONSTANTS
#define HUD_DEBUG
//FPS - Locked to Bottom Left
#define HUD_FPS_X 5
#define HUD_FPS_Y 7
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
//Top Down Map - Locked to Top Right
#define HUD_MAP_RADIUS 150
#define HUD_MAP_X (5 + HUD_MAP_RADIUS)
#define HUD_MAP_Y (5 + HUD_MAP_RADIUS)


//FUNCTION DECLARATIONS
void drawHud();
void drawHeightHud(int topx, int topy, float height);
void drawRollHud(int cx, int cy, int r, float roll);
void drawPitchHud(int cx, int cy, int r, float pitch);
void drawTopDownMap(int cx, int cy, int r);
void drawFps(int cx, int cy);