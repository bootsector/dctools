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

int raw2iso(char *infilename, char *outfilename) {
	FILE *infile, *outfile;

	unsigned char raw_buffer[2352];

	int c, sector, totalsectors;
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

	if((filesize % 2352) != 0) {
		printf("Error: Input file size is not a multiple of 2352\n");
		fclose(infile);
		fclose(outfile);
		return 1;
	}

	sector = 1;
	totalsectors = filesize / 2352;

	for(;;) {
		c = fread(raw_buffer, sizeof(raw_buffer), 1, infile);

		if(c < 1)
			break;

		fwrite(raw_buffer + 16, 2048, 1, outfile);

		printf("\r%d%% completed...", sector * 100 / totalsectors);

		sector++;
	}

	printf("\r100%% completed!  \n");

	fclose(infile);
	fclose(outfile);

	return 0;
}
