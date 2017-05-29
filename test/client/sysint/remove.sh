#!/bin/bash
#
# A PVFS2 test script using only sysint test programs
#
# This script recursively displays or removes directories
# (most often generated by test programs).
#
# Display example: "sh remove.sh /foo"
# Remove  example: "sh remove.sh /foo 1"
#

if [ -z "$1" ]; then
	echo "Usage sh remove.sh <filename>"
	exit 1
else
	echo "Starting at: $1"
fi

if [ "$2" = "1" ]; then
 echo "Recursively removing files:" 
 for f in `./find $1 | awk '{print $2}' | sort -r`; do ./remove $f; done
else
 echo "Displaying files that will be removed if '1' option is used"
 for f in `./find $1 | awk '{print $2}' | sort -r`; do echo $f; done
fi