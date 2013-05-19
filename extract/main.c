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
#include "iso9660.h"

int main(int argc, char *argv[]) {
	int i, j;

	printf("extract v1.0\nA simple ISO extract tool by bootsector\n(c) 05/13 bootsector@ig.com.br\n\n");

	if(argc != 6) {
		printf("Usage:\nextract <in_file> <lba_offset> <out_dir> <out_boot_file> <out_sortfile>\n\n");
		return 1;
	}

	if(iso9660_open(argv[1], atol(argv[2]))) {
		return 1;
	}

	i = iso9660_num_dirs();

	// Create full directory tree under <output_dir>
	for(j = 1; j <= i; j++) {
		iso9660_mkdir_tree(j, argv[3]);
	}

	// Extract files
	for(j = 1; j <= i; j++) {
		iso9660_extract_files(j, argv[3]);
	}

	// Dump bootsector as "IP.BIN" in the local dir
	iso9660_dump_bootsector(argv[4]);

	// Write sortfile
	iso9660_write_sortfile(argv[5]);

	iso9660_close();

	return 0;
}
