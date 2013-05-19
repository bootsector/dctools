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
#include "raw2iso.h"

int main(int argc, char *argv[]) {
	printf("raw2iso v1.0\n(c) bootsector@ig.com.br - 05/2013\n\n");

	if(argc != 3) {
		printf("Usage:\nraw2iso <input_file> <output_file>\n\n");
		return 1;
	}

	return(raw2iso(argv[1], argv[2]));
}
