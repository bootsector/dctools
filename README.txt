raw2iso:

Simple tool for converting RAW 2352 XA/CD images to ISO format.

Usage: raw2iso <input_file> <output_file>


iso2raw:

Simple tool for converting ISO images to RAW 2352 XA/CD images.

Usage: iso2raw <input_file> <output_file>


extract:

A simple ISO extract tool by bootsector

Usage: extract <in_file> <lba_offset> <out_dir> <out_boot_file> <out_sortfile>


Where:

<in_file> 		- This is the .iso file (not RAW) containing data to be extracted

<lba_offset>	- This should be 0 for "ordinary" ISO files. For DreamCast 
				track03.iso files, this should be 45000
		
<out_dir>		- The directory where the ISO extracted files should be put on

<out_boot_file>	- The file to write the boot sector (first 16 sectors) to

<out_sortfile>	- The sort file name to be used with mkisofs


For CDDA DreamCast ISOs it's still possible to use this tool in order to extract
all the files.

For example, let's say your game have a GDI structure like this:

track03.bin
track04.raw - 60133584 bytes
track05.raw - 272441568 bytes
track06.bin

First, convert both track03.bin and track05.bin using the raw2iso tool, like:

raw2iso track03.bin track03.iso
raw2iso track06.bin track06.iso

Create a dummy file which size (in bytes) follows the formula:

DUMMY_SIZE = ((lenght in bytes of track04.raw + lenght in bytes of track05.raw) / 2352 + 300) * 2048

Which translates to:

DUMMY_SIZE = ((60133584 + 272441568) / 2352 + 300) * 2048

So:

DUMMY_SIZE = 290203648 bytes

Then use the following command to get a dummy file:

dd if=/dev/zero of=DUMMY.BIN bs=290203648 count=1

Then merge the ISO files with your intermediate dummy file like:

cat track03.iso DUMMY.BIN track05.iso > full_iso.iso

Now you can extract the full contents with the following command:

extract full_iso.iso 45000 ./data IP.BIN srtfile.srt

Enjoy! :)

