/*
* DCTools - DreamCast CD-R authoring tools.
* Copyright (c) 2013 Bruno Freitas - bootsector@ig.com.br
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

int iso2raw(char *infilename, char *outfilename) {
	FILE *infile, *outfile;

	unsigned char sec[16];
	unsigned char sub[8];
	unsigned char buf[2048];
	unsigned char edc[4];
	unsigned char ecc[276];

	int c, thesec, sector, totalsectors;
	long int filesize;

	if((infile = fopen(infilename, "rb")) == NULL) {
		printf("Error opening input file %s\n", infilename);
		return 1;
	}

	if((outfile = fopen(outfilename, "wb")) == NULL) {
		printf("Error opening output file %s\n", outfilename);
		return 1;
	}

	fseek(infile, 0, SEEK_END);
	filesize = ftell(infile);
	fseek(infile, 0, SEEK_SET);

	if((filesize % 2048) != 0) {
		printf("Error: Input file size is not a multiple of 2048\n");
		fclose(infile);
		fclose(outfile);
		return 1;
	}

	memset(sec, 0xFF, sizeof(sec));
	sec[0] = 0;
	sec[11] = 0;
	sec[12] = 0;
	sec[13] = 2;
	sec[14] = 0;
	sec[15] = 2;

	memset(sub, 0x00, sizeof(sub));
	memset(edc, 0x01, sizeof(edc));
	memset(ecc, 0x02, sizeof(ecc));

	thesec = 0;

	sector = 1;
	totalsectors = filesize / 2048;

	for(;;) {
		c = fread(buf, sizeof(buf), 1, infile);

		if(c < 1)
			break;

		fwrite(sec, sizeof(sec), 1, outfile);
		fwrite(sub, sizeof(sub), 1, outfile);
		fwrite(buf, sizeof(buf), 1, outfile);
		fwrite(edc, sizeof(edc), 1, outfile);
		fwrite(ecc, sizeof(ecc), 1, outfile);

		thesec++;

		if(thesec > 74) {
			thesec = 0;
			sec[13]++;
		}

		sec[14] = ((thesec / 10) << 4) | (thesec - ((thesec / 10) * 10));

		printf("\r%d%% completed...", sector * 100 / totalsectors);

		sector++;
	}

	printf("\r100%% completed!  \n");

	fclose(infile);
	fclose(outfile);

	return 0;
}
