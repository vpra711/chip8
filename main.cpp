#include "game.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		return 0;
	}

	initialize(argv[1]);
	run();
	close();
	return 0;
}
