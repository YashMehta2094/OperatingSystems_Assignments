#!/usr/bin/env bash

script_path="$(realpath "$0")"

vowel_present(){
	[[ "$(basename "$1")" =~ [aeiouAEIOU] ]]
}

backup_dir(){
	local src_dir="$1"
	local dest_dir="$2"
	local output_file="$3"
	
	mkdir -p "$dest_dir"
	
	start_time=$(date +%s%N)
	file_count=0
	
	while IFS= read -r file; do
		if vowel_present "$file"; then 
			relative_path="${file#$src_dir/}"
			dest_file="$dest_dir/$relative_path"
			mkdir -p "$(dirname "$dest_file")"
			if [ ! -f "$dest_file" ] || [ "$file" -nt "$dest_file" ]; then
				cp "$file" "$dest_file"
				file_count=$((file_count + 1))
			fi
		fi
	done< <(find "$src_dir" -type f)
	
	end_time=$(date +%s%N)
	runtime=$(((end_time - start_time)/1000000))
	
	if [ ! -f "$output_file" ]; then
		echo "PID, Runtime (miliseconds), Files Copied">>"$output_file"
	fi
	
	echo "$$, $runtime, $file_count" >> "$output_file"
}

if [ "$#" -ne 6 ]; then
	echo "Correct use: $0 -s <source> -d <dest> -o <output>"
	exit 1
fi

while getopts "s:d:o:" opt; do
	case $opt in
		s) src_dir="$OPTARG" ;;
		d) dest_dir="$OPTARG" ;;
		o) output_file="$OPTARG" ;;
		*) echo "Correct use: $0 -s <source> -d <dest> -o <output>"
		   exit 1 ;;
	esac
done

backup_dir "$src_dir" "$dest_dir" "$output_file"


	   
		  
