#include "colorsets.h"

CRGB colorSets[NUM_COLORSETS][NUM_COLORS_PER_SET];

void createColorsets() {
// Blue.
		colorSets[0][0] = CRGB::Black;
		colorSets[0][1] = 0x000000;
		colorSets[0][2] = 0x000000;
		colorSets[0][3] = 0x000001;
		colorSets[0][4] = 0x000003;
		colorSets[0][5] = 0x010106;
		colorSets[0][6] = 0x02020C;
		colorSets[0][7] = 0x050519;
		colorSets[0][8] = 0x0A0A33;

// Yellow-green.
#if NUM_COLORSETS > 1
		colorSets[1][0] = 0x000000;
		colorSets[1][1] = 0x000000;
		colorSets[1][2] = 0x000100;
		colorSets[1][3] = 0x010200;
		colorSets[1][4] = 0x030500;
		colorSets[1][5] = 0x060A01;
		colorSets[1][6] = 0x0D1503;
		colorSets[1][7] = 0x1B2A06;
		colorSets[1][8] = 0x36540C;
#endif

#if NUM_COLORSETS > 2
// Amber.
		colorSets[2][0] = 0x000000;
		colorSets[2][1] = 0x000000;
		colorSets[2][2] = 0x000000;
		colorSets[2][3] = 0x010100;
		colorSets[2][4] = 0x030300;
		colorSets[2][5] = 0x060601;
		colorSets[2][6] = 0x0C0C02;
		colorSets[2][7] = 0x191905;
		colorSets[2][8] = 0x33330A;
#endif

#if NUM_COLORSETS > 3
// Reddish-purple.
		colorSets[3][0] = 0x000000;
		colorSets[3][1] = 0x000000;
		colorSets[3][2] = 0x010001;
		colorSets[3][3] = 0x030102;
		colorSets[3][4] = 0x060204;
		colorSets[3][5] = 0x0C0508;
		colorSets[3][6] = 0x180A11;
		colorSets[3][7] = 0x311523;
		colorSets[3][8] = 0x632A47;
#endif

#if NUM_COLORSETS > 4
// Original purple.
		colorSets[4][0] = 0x000000;
		colorSets[4][1] = 0x000000;
		colorSets[4][2] = 0x010001;
		colorSets[4][3] = 0x030102;
		colorSets[4][4] = 0x060305;
		colorSets[4][5] = 0x0C060A;
		colorSets[4][6] = 0x180C14;
		colorSets[4][7] = 0x311828;
		colorSets[4][8] = 0x633051;
#endif
}
