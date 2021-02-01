#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "genie/dat/DatFile.h"

#define SUBSTRACT_FLAG(amount) (*((uint16_t*)&(amount)+1))
#define GET_SUBSTRACT_FLAG(amount) (SUBSTRACT_FLAG(amount) == UINT16_MAX)
#define SET_SUBSTRACT_FLAG(amount) SUBSTRACT_FLAG(amount) = UINT16_MAX
#define UNSET_SUBSTRACT_FLAG(amount) SUBSTRACT_FLAG(amount) = 0

#define LOBYTE(x) (*((uint8_t*)&(x)))
#define SLOBYTE(x) (*((int8_t*)&(x)))

#define HIBYTE(x) (*((uint8_t*)&(x)+1))
#define SHIBYTE(x) (*((int8_t*)&(x)+1))

//#define ENABLE_TESTING

enum
{
	ARG_EXE = 0,
	ARG_DAT_FILE,
	ARG_GAME_VERSION,
	ARG_EFFECT_ID,
	ARG_COMMAND_ID,
	ARG_NUM,
};

#define streq(s1, s2) (strcmp(s1, s2) == 0)

int main(int argc, char **argv)
{

	if (argc < ARG_NUM)
	{
		printf("usage: aoe2subeffect <dat> <up/de2> <effect id> <command id>\n");

		return 1;
	}

	genie::DatFile *df = new genie::DatFile();
	genie::GameVersion version;

	bool testing = false;

	if (streq(argv[ARG_GAME_VERSION], "up"))
	{
		df->setGameVersion(genie::GV_TC);
		version = genie::GV_TC;
	}
	else if (streq(argv[ARG_GAME_VERSION], "de2"))
	{
		df->setGameVersion(genie::GV_LatestDE2);
		version = genie::GV_LatestDE2;
	}
#ifdef ENABLE_TESTING
	else if (streq(argv[ARG_GAME_VERSION], "testup"))
	{
		df->setGameVersion(genie::GV_TC);
		version = genie::GV_TC;

		testing = true;
	}
	else if (streq(argv[ARG_GAME_VERSION], "testde2"))
	{
		df->setGameVersion(genie::GV_LatestDE2);
		version = genie::GV_LatestDE2;

		testing = true;
	}
#endif
	else
	{
		printf("error: version needs to be up or de2\n");

		return 1;
	}

	struct stat st;

	if (stat(argv[ARG_DAT_FILE], &st) != 0)
	{
		printf("error: couldn't open %s\n", argv[ARG_DAT_FILE]);

		return 1;
	}

	char *dat_bak;
	char *dat_tmp;

	if (!testing)
	{
		dat_bak = (char *)calloc(strlen(argv[ARG_DAT_FILE]) + 4 + 1, 1);
		sprintf(dat_bak, "%s.bak", argv[ARG_DAT_FILE]);

		dat_tmp = (char *)calloc(strlen(argv[ARG_DAT_FILE]) + 4 + 1, 1);
		sprintf(dat_tmp, "%s.tmp", argv[ARG_DAT_FILE]);

		if (stat(dat_tmp, &st) == 0 && unlink(dat_tmp) != 0)
		{
			printf("error: tempfile couldn't be reserved\n");

			return 1;
		}

		rename(dat_bak, dat_tmp);
		rename(argv[ARG_DAT_FILE], dat_bak);

		df->load(dat_bak);
	}
	else
	{
		df->load(argv[ARG_DAT_FILE]);
	}

	int32_t effect_id = atol(argv[ARG_EFFECT_ID]);

	if (effect_id > df->Effects.size())
	{
		printf("error: effect ID %d doesn't exist\n", effect_id);

		delete df;

		if (!testing)
		{
			rename(dat_bak, argv[ARG_DAT_FILE]);
			rename(dat_tmp, dat_bak);
		}

		return 1;
	}

	genie::Effect &effect = df->Effects.at(effect_id);

	int32_t command_id = atol(argv[ARG_COMMAND_ID]);

	if (command_id > effect.EffectCommands.size())
	{
		printf("error: command ID %d doesn't exist\n", command_id);

		delete df;

		if (!testing)
		{
			rename(dat_bak, argv[ARG_DAT_FILE]);
			rename(dat_tmp, dat_bak);
		}

		return 1;
	}

	genie::EffectCommand &command = effect.EffectCommands.at(command_id);

	if (!testing)
	{
		int32_t D = (int32_t)command.D;
		int32_t NewD = 0;
		int32_t type = D >> 8;
		int32_t value = (uint8_t)D;

		printf("info: old byte representation: %08X\n", D);

		SET_SUBSTRACT_FLAG(NewD);

		value *= -1;
		SLOBYTE(NewD) = (int8_t)value;

		if (version == genie::GV_LatestDE2)
		{
			type = UINT8_MAX - type;
		}

		HIBYTE(NewD) = (uint8_t)type;

		printf("info: new byte representation: %08X\n", NewD);

		command.D = (float)NewD;

		df->saveAs(argv[ARG_DAT_FILE]);
	}
	else
	{
		int32_t D = (int32_t)command.D;

		printf("info: byte representation %08X\n", D);
	}

	delete df;

	unlink(dat_tmp);

	return 0;
}
