#include "animation.h"

char afc_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];
CRGB af_w8v1r[ANIMATION_FRAME_WIDTH * ANIMATION_FRAMES];

char afc_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];
CRGB af_f_slow_stop[afc_f_slow_stop_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_ANIMATION_FRAMES];

char afc_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];
CRGB af_f_slow_stop_c[afc_f_slow_stop_c_ANIMATION_FRAME_WIDTH * afc_f_slow_stop_c_ANIMATION_FRAMES];

char afc_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];
CRGB af_l_pulsar_a[afc_l_pulsar_a_ANIMATION_FRAME_WIDTH * afc_l_pulsar_a_ANIMATION_FRAMES];

void createAnimationFrames() {
	
/*
		strcpy(animationFramesChars, "					1234	 ");
		strcat(animationFramesChars, "					 4323	 ");
		strcat(animationFramesChars, "				 4321	 2 ");
		strcat(animationFramesChars, "			 4321			1");
		strcat(animationFramesChars, "		 4321				 ");
		strcat(animationFramesChars, "		3234				 ");
		strcat(animationFramesChars, "	 2	1234			 ");
		strcat(animationFramesChars, "	1			1234		 ");

		strcpy(animationFramesCharsReverse, "		4321					");
		strcat(animationFramesCharsReverse, "	 3234						");
		strcat(animationFramesCharsReverse, " 2	 1234					");
		strcat(animationFramesCharsReverse, "1		 1234				");
		strcat(animationFramesCharsReverse, "				 1234			");
		strcat(animationFramesCharsReverse, "					4323		");
		strcat(animationFramesCharsReverse, "				4321	2		");
		strcat(animationFramesCharsReverse, "			4321		 1	");

//										12345678901234567890123
		strcpy(afc_w8v1, "					123456788		 ");
		strcat(afc_w8v1, "					 12345688		 ");
		strcat(afc_w8v1, "						12348876	 ");
		strcat(afc_w8v1, "						 128876 4	 ");
		strcat(afc_w8v1, "							88765	 2 ");
		strcat(afc_w8v1, "						 887654		1");
		strcat(afc_w8v1, "						8876543		 ");
		strcat(afc_w8v1, "					 88765432		 ");
		strcat(afc_w8v1, "					887654321		 ");
		strcat(afc_w8v1, "				 887654321		 ");
		strcat(afc_w8v1, "				887654321			 ");
		strcat(afc_w8v1, "			 887654321			 ");
		strcat(afc_w8v1, "			887654321				 ");
		strcat(afc_w8v1, "		 887654321				 ");
		strcat(afc_w8v1, "		887654321					 ");
		strcat(afc_w8v1, "		88654321					 ");
		strcat(afc_w8v1, "	 67884321						 ");
		strcat(afc_w8v1, "	4 678821						 ");
		strcat(afc_w8v1, " 2	56788							 ");
		strcat(afc_w8v1, "1		456788						 ");
		strcat(afc_w8v1, "		3456788						 ");
		strcat(afc_w8v1, "		23456788					 ");
		strcat(afc_w8v1, "		123456788					 ");
		strcat(afc_w8v1, "		 123456788				 ");
		strcat(afc_w8v1, "			123456788				 ");
		strcat(afc_w8v1, "			 123456788			 ");
		strcat(afc_w8v1, "				123456788			 ");
		strcat(afc_w8v1, "				 123456788		 ");


//										 12345678901234567890123
		strcpy(afc_w8v1r, "		 887654321					");
		strcat(afc_w8v1r, "		 88654321						");
		strcat(afc_w8v1r, "		67884321						");
		strcat(afc_w8v1r, "	 4 678821							");
		strcat(afc_w8v1r, " 2	 56788							");
		strcat(afc_w8v1r, "1	 456788							");
		strcat(afc_w8v1r, "		 3456788						");
		strcat(afc_w8v1r, "		 23456788						");
		strcat(afc_w8v1r, "		 123456788					");
		strcat(afc_w8v1r, "			123456788					");
		strcat(afc_w8v1r, "			 123456788				");
		strcat(afc_w8v1r, "				123456788				");
		strcat(afc_w8v1r, "				 123456788			");
		strcat(afc_w8v1r, "					123456788			");
		strcat(afc_w8v1r, "					 123456788		");
		strcat(afc_w8v1r, "						12345688		");
		strcat(afc_w8v1r, "						 12348876		");
		strcat(afc_w8v1r, "							128876 4	");
		strcat(afc_w8v1r, "							 88765	2 ");
		strcat(afc_w8v1r, "							887654	 1");
		strcat(afc_w8v1r, "						 8876543		");
		strcat(afc_w8v1r, "						88765432		");
		strcat(afc_w8v1r, "					 887654321		");
		strcat(afc_w8v1r, "					887654321			");
		strcat(afc_w8v1r, "				 887654321			");
		strcat(afc_w8v1r, "				887654321				");
		strcat(afc_w8v1r, "			 887654321				");
		strcat(afc_w8v1r, "			887654321					");

		//											 12345678901234567890123
		strcpy(afc_2_sparkle_a, "123456788							");
		strcat(afc_2_sparkle_a, " 123456788							");
		strcat(afc_2_sparkle_a, "	 123456788						");
		strcat(afc_2_sparkle_a, "		 12345677						");
		strcat(afc_2_sparkle_a, "			1234566						");
		strcat(afc_2_sparkle_a, "				123455					");
		strcat(afc_2_sparkle_a, "				 12344					");
		strcat(afc_2_sparkle_a, "					 1233					");
		strcat(afc_2_sparkle_a, "						122					");
		strcat(afc_2_sparkle_a, "							11				");
		strcat(afc_2_sparkle_a, "												");
		strcat(afc_2_sparkle_a, "								 1			");
		strcat(afc_2_sparkle_a, "								 1	1		");
		strcat(afc_2_sparkle_a, "							1	 21 2		");
		strcat(afc_2_sparkle_a, "							1	 1	3		");
		strcat(afc_2_sparkle_a, "							21	1 4		");
		strcat(afc_2_sparkle_a, "							31 11 5		");
		strcat(afc_2_sparkle_a, "							42122 6		");
		strcat(afc_2_sparkle_a, "							51213 7		");
		strcat(afc_2_sparkle_a, "												");
		strcat(afc_2_sparkle_a, "						1 71414 2	 1");
		strcat(afc_2_sparkle_a, "						2 51626271 1");
		strcat(afc_2_sparkle_a, "				 1	1 42837422 1");
		strcat(afc_2_sparkle_a, "				 2	2131428681	");
		strcat(afc_2_sparkle_a, "					 13222813472	");
		strcat(afc_2_sparkle_a, "				 2	2313428623 1");
		strcat(afc_2_sparkle_a, "				1 1 14 2833472 2");
		strcat(afc_2_sparkle_a, "				 2	 5 1628681 3");
		strcat(afc_2_sparkle_a, "						16	41342	 2");
		strcat(afc_2_sparkle_a, "				 2 127 122367	 1");
		strcat(afc_2_sparkle_a, "				 1 23811 4846		");
		strcat(afc_2_sparkle_a, "				1	 1262	 6325	 1");
		strcat(afc_2_sparkle_a, "				1		183	 62 4	 2");
		strcat(afc_2_sparkle_a, "				 1	 641 8123	 3");
		strcat(afc_2_sparkle_a, "				 2	185143242	 4");
		strcat(afc_2_sparkle_a, "						 64168161	 5");
		strcat(afc_2_sparkle_a, "						185 8314	 4");
		strcat(afc_2_sparkle_a, "						274 46 6	 5");
		strcat(afc_2_sparkle_a, "						36218614	 4");
		strcat(afc_2_sparkle_a, "						254244262	 2");
		strcat(afc_2_sparkle_a, "						147382144	 1");
		strcat(afc_2_sparkle_a, "						235261262		");
		strcat(afc_2_sparkle_a, "						327141145		");
		strcat(afc_2_sparkle_a, "						215 2 122		");
		strcat(afc_2_sparkle_a, "						1 7		216		");
*/	
		
		// The L animations are loops. 
		//
		// These always start with pixels 11 and 12 (counting from 0) at 8, and return to the same position. 
		
		//											123456789012345678901234
		strcpy(afc_l_pulsar_a, "					 88						");
		strcat(afc_l_pulsar_a, "					1881					");
		strcat(afc_l_pulsar_a, "					2882					");
		strcat(afc_l_pulsar_a, "				 138831					");
		strcat(afc_l_pulsar_a, "				 248842					");
		strcat(afc_l_pulsar_a, "				13588531				");
		strcat(afc_l_pulsar_a, "			 1246886421				");
		strcat(afc_l_pulsar_a, "			123578875321			");
		strcat(afc_l_pulsar_a, "		 12346888864321			");
		strcat(afc_l_pulsar_a, "		1234578888754321		");
		strcat(afc_l_pulsar_a, "	 123456888888654321		");
		strcat(afc_l_pulsar_a, "	12345677888877654321	");
		strcat(afc_l_pulsar_a, " 1234567868888687654321 ");
		strcat(afc_l_pulsar_a, "123456787578875787654321");
		strcat(afc_l_pulsar_a, "234567876468864678765432");
		strcat(afc_l_pulsar_a, "123456765358853567654321");
		strcat(afc_l_pulsar_a, " 1234565424884245654321 ");
		strcat(afc_l_pulsar_a, "	12345431388313454321	");
		strcat(afc_l_pulsar_a, "	 123432 2882 2343210	");
		strcat(afc_l_pulsar_a, "		12321 1881 123210		");
		strcat(afc_l_pulsar_a, "		 121	1881	1210		");
		strcat(afc_l_pulsar_a, "			1		 88		 1			");
		
		
		// The F animations are fragments.
		
		// The plan is to cut them into "intro" and "outro" animations, with loop animations inserted in between
		// Hopefully we can have enough SRAM for multiple "intro" and "outro" animations, so that the motion doesn't become predictable. 
		// Also, incorporating auto-trails and controllable acceleration factors in UpdateTravel methods means that we reduce the number
		// of frames of animation that we have to render into RAM. 

		//											 12345678901234567
		strcpy(afc_f_slow_stop, "123456788				");
		strcat(afc_f_slow_stop, " 123456778				");
		strcat(afc_f_slow_stop, "	 123456678			");
		strcat(afc_f_slow_stop, "		12345568			");
		strcat(afc_f_slow_stop, "		 12344578			");
		strcat(afc_f_slow_stop, "			1233468			");
		strcat(afc_f_slow_stop, "			 122357			");
		strcat(afc_f_slow_stop, "				112468		");
		strcat(afc_f_slow_stop, "					1358		");
		strcat(afc_f_slow_stop, "					 248		");
		strcat(afc_f_slow_stop, "					 158		");
		strcat(afc_f_slow_stop, "						68		");
		strcat(afc_f_slow_stop, "					 178		");
		strcat(afc_f_slow_stop, "					 288		");
		strcat(afc_f_slow_stop, "					 388		");
		strcat(afc_f_slow_stop, "					 488		");
		strcat(afc_f_slow_stop, "					 588		");
		strcat(afc_f_slow_stop, "					 688		");
		strcat(afc_f_slow_stop, "					 788		");
		strcat(afc_f_slow_stop, "					 887		");
		strcat(afc_f_slow_stop, "					 886		");
		strcat(afc_f_slow_stop, "					 885		");
		strcat(afc_f_slow_stop, "					 884		");
		strcat(afc_f_slow_stop, "					 883		");
		strcat(afc_f_slow_stop, "					2882		");
		strcat(afc_f_slow_stop, "				 23871		");
		strcat(afc_f_slow_stop, "				 3486			");
		strcat(afc_f_slow_stop, "				 4585			");
		strcat(afc_f_slow_stop, "				 5684			");
		strcat(afc_f_slow_stop, "				 6783			");
		strcat(afc_f_slow_stop, "				 7882			");
		strcat(afc_f_slow_stop, "				48871			");
		strcat(afc_f_slow_stop, "				5876			");
		strcat(afc_f_slow_stop, "				6865			");
		strcat(afc_f_slow_stop, "				7854			");
		strcat(afc_f_slow_stop, "			 48843			");
		strcat(afc_f_slow_stop, "			 58832			");
		strcat(afc_f_slow_stop, "			 68821			");
		strcat(afc_f_slow_stop, "			 7881				");
		strcat(afc_f_slow_stop, "			5888				");
		strcat(afc_f_slow_stop, "			6887				");
		strcat(afc_f_slow_stop, "			7876				");
		strcat(afc_f_slow_stop, "			8865				");
		strcat(afc_f_slow_stop, "			8854				");
		strcat(afc_f_slow_stop, "			7883				");
		strcat(afc_f_slow_stop, "			6882				");
		strcat(afc_f_slow_stop, "			5788				");
		strcat(afc_f_slow_stop, "			4688				");
		strcat(afc_f_slow_stop, "			3588				");
		strcat(afc_f_slow_stop, "			24788				");
		strcat(afc_f_slow_stop, "			13688				");
		strcat(afc_f_slow_stop, "			 2578				");
		strcat(afc_f_slow_stop, "			 14688			");
		strcat(afc_f_slow_stop, "				3588			");
		strcat(afc_f_slow_stop, "				2488			");
		strcat(afc_f_slow_stop, "				13788			");
		strcat(afc_f_slow_stop, "				 2688			");
		strcat(afc_f_slow_stop, "				 1588			");
		strcat(afc_f_slow_stop, "				 14788		");
		strcat(afc_f_slow_stop, "				 13688		");
		strcat(afc_f_slow_stop, "				 125788		");
		strcat(afc_f_slow_stop, "				 1246788	");
		strcat(afc_f_slow_stop, "				 12356788 ");
		strcat(afc_f_slow_stop, "				 123456788");
		strcat(afc_f_slow_stop, "									");
		
		//												 1234567890123456789012345
		strcpy(afc_f_slow_stop_c, "123456788								");
		strcat(afc_f_slow_stop_c, " 1234567882							");
		strcat(afc_f_slow_stop_c, "	 1234567782							");
		strcat(afc_f_slow_stop_c, "		1234566782						");
		strcat(afc_f_slow_stop_c, "		 123455684						");
		strcat(afc_f_slow_stop_c, "			123445782						");
		strcat(afc_f_slow_stop_c, "			 12334684						");
		strcat(afc_f_slow_stop_c, "				1223586						");
		strcat(afc_f_slow_stop_c, "				 1124782					");
		strcat(afc_f_slow_stop_c, "					 13684					");
		strcat(afc_f_slow_stop_c, "						2586					");
		strcat(afc_f_slow_stop_c, "						1487					");
		strcat(afc_f_slow_stop_c, "						 378					");
		strcat(afc_f_slow_stop_c, "						 268					");
		strcat(afc_f_slow_stop_c, "						 158					");
		strcat(afc_f_slow_stop_c, "							48					");
		strcat(afc_f_slow_stop_c, "							38					");
		strcat(afc_f_slow_stop_c, "							28					");
		strcat(afc_f_slow_stop_c, "							18					");
		strcat(afc_f_slow_stop_c, "							18					");
		strcat(afc_f_slow_stop_c, "							38					");
		strcat(afc_f_slow_stop_c, "							58					");
		strcat(afc_f_slow_stop_c, "							78					");
		strcat(afc_f_slow_stop_c, "							88					");
		strcat(afc_f_slow_stop_c, "							88					");
		strcat(afc_f_slow_stop_c, "						 288					");
		strcat(afc_f_slow_stop_c, "						 488					");
		strcat(afc_f_slow_stop_c, "						 688					");
		strcat(afc_f_slow_stop_c, "						 788					");
		strcat(afc_f_slow_stop_c, "						 887					");
		strcat(afc_f_slow_stop_c, "						 886					");
		strcat(afc_f_slow_stop_c, "						2885					");
		strcat(afc_f_slow_stop_c, "						4884					");
		strcat(afc_f_slow_stop_c, "						6884					");
		strcat(afc_f_slow_stop_c, "						7883					");
		strcat(afc_f_slow_stop_c, "						8873					");
		strcat(afc_f_slow_stop_c, "						8862					");
		strcat(afc_f_slow_stop_c, "						8852					");
		strcat(afc_f_slow_stop_c, "					 28841					");
		strcat(afc_f_slow_stop_c, "					 48841					");
		strcat(afc_f_slow_stop_c, "					 6883						");
		strcat(afc_f_slow_stop_c, "					 7873						");
		strcat(afc_f_slow_stop_c, "					 8862						");
		strcat(afc_f_slow_stop_c, "					28852						");
		strcat(afc_f_slow_stop_c, "					48841						");
		strcat(afc_f_slow_stop_c, "					68841						");
		strcat(afc_f_slow_stop_c, "					7883						");
		strcat(afc_f_slow_stop_c, "				 28873						");
		strcat(afc_f_slow_stop_c, "				 48862						");
		strcat(afc_f_slow_stop_c, "				 68852						");
		strcat(afc_f_slow_stop_c, "				 78841						");
		strcat(afc_f_slow_stop_c, "				388741						");
		strcat(afc_f_slow_stop_c, "				58863							");
		strcat(afc_f_slow_stop_c, "				78853							");
		strcat(afc_f_slow_stop_c, "				88742							");
		strcat(afc_f_slow_stop_c, "				88642							");
		strcat(afc_f_slow_stop_c, "				87531							");
		strcat(afc_f_slow_stop_c, "				86431							");
		strcat(afc_f_slow_stop_c, "				8542							");
		strcat(afc_f_slow_stop_c, "				8432							");
		strcat(afc_f_slow_stop_c, "				8321							");
		strcat(afc_f_slow_stop_c, "				821								");
		strcat(afc_f_slow_stop_c, "				81								");
		strcat(afc_f_slow_stop_c, "				8									");
		strcat(afc_f_slow_stop_c, "				82								");
		strcat(afc_f_slow_stop_c, "				84								");
		strcat(afc_f_slow_stop_c, "				86								");
		strcat(afc_f_slow_stop_c, "				88								");
		strcat(afc_f_slow_stop_c, "				788								");
		strcat(afc_f_slow_stop_c, "				688								");
		strcat(afc_f_slow_stop_c, "				5788							");
		strcat(afc_f_slow_stop_c, "				46788							");
		strcat(afc_f_slow_stop_c, "				356788						");
		strcat(afc_f_slow_stop_c, "				24567788					");
		strcat(afc_f_slow_stop_c, "				1345667788				");
		strcat(afc_f_slow_stop_c, "				123455667788			");
		strcat(afc_f_slow_stop_c, "				 123445566788			");
		strcat(afc_f_slow_stop_c, "					1233445567788		");
		strcat(afc_f_slow_stop_c, "					 1223344566788	");
		strcat(afc_f_slow_stop_c, "							12334456788 ");
		strcat(afc_f_slow_stop_c, "								 123456788");
		strcat(afc_f_slow_stop_c, "													");
}