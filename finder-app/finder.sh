#!/bin/sh
if [ $# -ne 2 ]; then
	echo "Error: Expected two arguments - directory for search and string to search for"
	exit 1
fi

filesdir="$1"
searchstr="$2"

if [ ! -d "$filesdir" ]; then
	echo "Error: '$filesdir' is not a valid directory."
	exit 1
fi

numfiles=$(find $filesdir -type f | wc -l)
matchcount=$(grep -r -- "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $numfiles and the number of matching lines are $matchcount"




