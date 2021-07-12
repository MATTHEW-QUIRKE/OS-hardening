function read_conf_file {
	fullpath=$(echo $1 | tr -d \'\")
	[ ! -e  "$fullpath" ] && {
		return
	}
	[ "$DEBUG" = true ] && echo "#$2" $fullpath
	f=`basename $fullpath`
	pwd="`pwd`"
	cd `dirname $fullpath` > /dev/null


	sed '/^[[:blank:]]*#/d;s/#.*//;/^[[:blank:]]*$/d' $f | while read -r line; do
	#	printf "$line"
		if grep -e '^[[:blank:]]*Include' <<< "$line" >/dev/null ; then
			for file in $(echo $line | tr -d \'\" | awk '{$1=""; print $0}'); do
				read_conf_file $file "#$2"
			done
		else
			echo "${line}"
		fi
	done
	cd "$pwd"
}

function match {
	str="$1"
	pattern="$2"
	grep -iE "$pattern" <<< "$str"
}
function testre {
	str="$1"
	pattern="$2"
	match "$str" "$pattern" >/dev/null
}
function match_count {
	str="$1"
	pattern="$2"
	match "$str" "$pattern" | wc -l
}

function populate_directories {
	conf="$1"
	i=0
	cur=
	while read -r line; do
		if testre "${line}" "^ *<Directory[ '\"]*\/"; then
			cur="$line"
		elif testre "$line" "<\/Directory"; then
			cur=$(printf "%s\n%s" "$cur" "$line")
			# echo 'AAAA'
			# echo "$cur"
			# echo 'BBBB'
			directories[$i]="$cur"
			((i++))
			cur=""
		elif [ ! -z "$cur" ]; then
			cur=$(printf "%s\n%s" "$cur" "$line")
		fi
	done <<< "$conf"
# echo $i "${arr[2]}"
	# {!retvar}=$arr
}

function get_directory_name {
	# TODO
	# echo "$1" | head -n1 | sed 's/^\s*<Directory *['"'"'"]*//i;s/['"'"'"\s]*>$//'
	echo "$1" | head -n1 | sed 's/^\s*<Directory *['"'"'"]*//;s/['"'"'"\s]*>$//' | xargs
}

function populate_document_roots {
	conf="$1"
	document_roots=()
	document_roots_str="$(match "$conf" "^\s*DocumentRoot\s" | sed 's/^\s*DocumentRoot *//')"
	while read -r line; do
		document_roots+=("$line")
	done <<< "$document_roots_str"
}

function get_directory_for_document_root {
	document_root="$1"
	
	for directory in "${directories[@]}"; do
		dir_pattern="`get_directory_name \"$directory\"`"

		if [ "${dir_pattern: -1}" == "/" ]; then
			dir_pattern="${dir_pattern}*"
		else
			dir_pattern="${dir_pattern}/*"
		fi
		
		case "$document_root" in 
			($dir_pattern) echo "$directory" | sed '1d;$d' ;;
		esac
	done
}

function get_root_directory {
	conf="$1"
	perl -ne 'print if /^ *<Directory[ ''"]*\/[ ''"]*>/i .. /<\/Directory/i' <<< "$conf"
}

function get_global_context {
	conf="$1"
	while read -r line; do
		if testre "$line" '^[[:blank:]]*</' ; then
			inside_tag=false
		elif testre "$line" '^[[:blank:]]*<' ; then
			inside_tag=true
		elif [ "$inside_tag" != "true" ]; then
			echo "${line}"
		fi
	done <<< "$conf"
}

function log {
	audit="$1"
	shift
	msg="$@"
	[ "$VERBOSE" = true ] && echo "$audit" "$msg"
}

function print {
	color=$1
	txt=$2
	printf "\e[${color}m$txt\033[0m\n"
}

function print_fail_pass {
	if [ $1 -eq 0 ]; then
		print 92 'PASS'
	else
		print 91 'FAIL'
	fi
}