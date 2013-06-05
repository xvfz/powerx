//gcc powerx.c -o powerx
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSDIR "/sys/class/power_supply/"

int acpower(){
	char ac[50] = "/sys/class/power_supply/AC/online" ;
	int fd, acnw;
	char buff[1];
	if ((fd = open(ac,  O_RDONLY)) < 0){
		printf("ac file not opened\n");
	}
	
	read(fd, &buff, 1);
	acnw=atoi(buff);
	if (acnw == 1){
		return 1;
	}
	else{
		return 0;
	}
}

int main(int argc, char **argv){
	int fd, rez;
	float pw, vl, cr;
	_Bool prclr = 0;
	char *battery = "BAT0";
       	char buff[1], pwnw[50], vlnw[50], crnw[50];
	
	


	while (( rez = getopt(argc, argv, "hcb:" )) != -1 ){
		switch (rez){
			case 'c': prclr = 1; break;
			case 'b': battery = optarg; break;
			case 'h': printf("powerx [-c, -h, -b]\n\
			 c -enable color\n\
			 h -print this help\n\
			 b -choise battery(BAT0 is default)\n"); return 0;
		}
	}

	sprintf(pwnw, "%s%s/power_now", SYSDIR, battery);
	sprintf(vlnw, "%s%s/voltage_now", SYSDIR, battery);
	sprintf(crnw, "%s%s/current_now", SYSDIR, battery);

	if (( fd = open(pwnw, O_RDONLY)) > 0 ){
		read(fd, &buff, 10);
		pw = atof(buff);
		close(fd);
		pw = pw/1000000;
	}
	else{
		if (( fd = open(vlnw, O_RDONLY)) < 0 ){
			printf("%s not readed\n", vlnw);
			return 1;
		}
		read(fd, &buff, 10);
		vl = atof(buff);
		close(fd);

		if (( fd = open(crnw, O_RDONLY)) < 0 ){
			printf("%s not readed\n", crnw);
			return 1;
		}
		read(fd, &buff, 10);
		cr = atof(buff);
		close(fd);
		
		pw = vl*cr/1000000000000;
	}

	if ( prclr != 0 ){
		if ( acpower() == 1 ){
		printf("\e[32m%.2fW\e[0m\n", pw); 
		}
		else{
		printf("\e[33m%.2fW\e[0m\n", pw); 
		}
		return 0;
	}

	printf("%.2fW\n", pw);
	return 0;
}

