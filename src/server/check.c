#include <stdio.h>
#include <regex.h>
#include <sys/types.h>


#define PATTERN "^[0-9]{0,3}.[0-9]{0,3}.[0-9]{0,3}.[0-9]{0,3}$"

int checkIP(char *ip) {
	regex_t preg;
	int err, regerr;
	err = regcomp (&preg, PATTERN, REG_EXTENDED);
	if (err != 0) {
		char buff[512];
		regerror(err, &preg, buff, sizeof(buff));
		printf("buff - %s\n", buff)	;
		retu
	}

	regmatch_t pm;
	regerr = regexec (&preg, ip, 0, &pm, 0);
	if (regerr == 0) {
		printf("IP true\n");
		return 1;
	}
	else {
		printf("False\n");
		char errbuf[512];
		regerror(regerr, &preg, errbuf, sizeof(errbuf));
		printf("%s\n", errbuf);
	}
	return 0;
}
