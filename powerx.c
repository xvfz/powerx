#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSDIR "/sys/class/power_supply/"

int print_help()
{
	printf("Usage:\n");
	printf("powerx [-c, -h, -r, -v] [-a, -b] .. DIR\n\n");
	printf("-a\tChoise your battery (default  BAT0)\n");
	printf("-b\tChoise your ac adapter (default is AC)\n");
	printf("-c\tCnable color (depends of ac adapter)\n");
	printf("-v\tVerbosebose output (print readed files)\n");
	printf("-r\tRaw mode for use with WM or scripts\n");
	printf("\toutput only digits without \"W\" and color\n");
	printf("\n-h\tPrint this help\n\n");
	printf("To find out which file you need cast \"ls %s\"\n", SYSDIR);
}

int acpower(char *ac_adapter, _Bool verbose)
{
	int fd, ac_now;
	char ac[50], buff[1];
	
	sprintf(ac, "%s%s/online", SYSDIR, ac_adapter);
	
	if ((fd = open(ac, O_RDONLY)) < 0)
	{
		printf("[ERR]\t%s not readed\n", ac);
	}
       	else if (verbose)
	{
		printf("\t%s readed\n", ac);
	}
	
	read(fd, &buff, 1);
	ac_now=atoi(buff);
	
	if (ac_now == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int main(int argc, char **argv)
{
	int fd, args;
	float power, voltage, current;
	_Bool color_out = 0, raw = 0, verbose = 0;
	char *battery = "BAT0";
	char *ac_adapter = "AC";
       	char buff[1], power_now[50], voltage_now[50], current_now[50];
	

	while (( args = getopt(argc, argv, "a:b:crvh" )) != -1 )
	{
		switch (args)
		{
			case 'a': ac_adapter  = optarg;	break;
			case 'b': battery     = optarg;	break;
			case 'c': color_out   = 1; 	break;
			case 'r': raw         = 1;      break;
			case 'v': verbose     = 1;      break;
			case 'h': print_help();		return 0;
		}
	}

	sprintf(power_now,   "%s%s/power_now",   SYSDIR, battery);
	sprintf(voltage_now, "%s%s/voltage_now", SYSDIR, battery);
	sprintf(current_now, "%s%s/current_now", SYSDIR, battery);

	if (( fd = open(power_now, O_RDONLY)) > 0 )
	{
		read(fd, &buff, 50);
		power = atoi(buff);
		close(fd);
		power = power/1000000;
		if (verbose)
		{
			printf("\t%s readed\n", voltage_now);
		}
	}
	else
	{
		if (( fd = open(voltage_now, O_RDONLY)) < 0 )
		{
			printf("[ERR]\t%s not readed\n", voltage_now);
			return 1;
		}
		else if (verbose)
		{
			printf("\t%s readed\n", voltage_now);
		}

		read(fd, &buff, 50);
		voltage = atoi(buff);
		close(fd);

		if (( fd = open(current_now, O_RDONLY)) < 0 )
		{
			printf("[ERR]\t%s not readed\n", current_now);
			return 1;
		}
		else if (verbose)
		{
			printf("\t%s readed\n", current_now);
		}
		
		read(fd, &buff, 50);
		current = atoi(buff);
		close(fd);
		power = voltage*current/1000000000000;
	}

	if ( color_out != 0 && raw == 0 )
	{
		if ( acpower(ac_adapter, verbose) == 1 )
		{
			printf("\e[32m%.2fW\e[0m\n", power);
		}
		else
		{
			printf("\e[33m%.2fW\e[0m\n", power);
		}
		return 0;
	}
	

	if ( raw != 0 )
	{
		printf("%.2f", power);
	}
	else
	{
		printf("%.2fW\n", power);
	}
	return 0;
}

