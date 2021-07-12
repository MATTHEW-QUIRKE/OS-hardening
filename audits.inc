. lib.inc

function run {
	[ -e "$APACHE_CONF_FILE" ] || {
		log 0 'Apache config file not found' "APACHE_CONF_FILE"
		return 1
	}

	case "$1" in 
		('4.1') a4_1 "$APACHE_CONF_FILE" ;;
		('4.2') a4_2 "$APACHE_CONF_FILE" ;;
		('4.3') a4_3 "$APACHE_CONF_FILE" ;;
		('4.4') a4_4 "$APACHE_CONF_FILE" ;;
		('5.1') a5_1 "$APACHE_CONF_FILE" ;;
		('5.2') a5_2 "$APACHE_CONF_FILE" ;;
		('5.3') a5_3 "$APACHE_CONF_FILE" ;;
		('5.4') a5_4 "$APACHE_CONF_FILE" ;;
		('5.5') a5_5 "$APACHE_CONF_FILE" ;;
		('5.6') a5_6 "$APACHE_CONF_FILE" ;;
		('5.7') a5_7 "$APACHE_CONF_FILE" ;;
		('5.8') a5_8 "$APACHE_CONF_FILE" ;;
		('5.9') a5_9 "$APACHE_CONF_FILE" ;;
		('5.10') a5_10 "$APACHE_CONF_FILE" ;;
		('5.11') a5_11 "$APACHE_CONF_FILE" ;;
		('5.12') a5_12 "$APACHE_CONF_FILE" ;;
		('5.13') a5_13 "$APACHE_CONF_FILE" ;;
		('5.14') a5_14 "$APACHE_CONF_FILE" ;;
	esac
}

# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find a root <Directory> element.
# 2. Ensure that either one of the following two methods are configured:
# Using the deprecated Order/Deny/Allow method:
# 1. Ensure there is a single Order directive with the value of deny, allow
# 2. Ensure there is a Deny directive, and with the value of from all.
# 3. Ensure there are no Allow or Require directives in the root <Directory>
# element.
# Using the Require method:
# 4. Ensure there is a single Require directive with the value of all denied
# 5. Ensure there are no Allow or Deny directives in the root <Directory>
# element.

function a4_1 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"
	directory="`get_root_directory "$conf"`"
	if ! testre "$directory" "^\s*Require\s\s*all\s\s*denied\s*$"; then
		log 4.1 '"Require all denied" not found within the root directory configuration' "`get_directory_name \"$directory\"`"
		return 2
	fi

	count=`match_count "$directory" "^\s*Require\s"`
	if [ $count -ne 1 ]; then
		log 4.1 "There're more than one Require directive within the root directory configuration" "`get_directory_name \"$directory\"`"
		return 3
	fi

	if testre "$directory" "^\s*(Deny|Allow)\s"; then
		log 4.1 "There's at least one Allow or Deny directives in the root directory configuration" "`get_directory_name \"$directory\"`"
		return 4
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find all <Directory> elements.
# 2. Ensure that either one of the following two methods are configured:
# Use the deprecated Order/Deny/Allow method:
# 1. Ensure there is a single Order directive with the value of Deny,Allow for each.
# 2. Ensure the Allow and Deny directives, have values that are appropriate for the purposes of the directory.
# Use the Require method:
# 1. Ensure that the Order/Deny/Allow directives are NOT used for the directory.
# 2. Ensure the Require directives have values that are appropriate for the
# purposes of the directory.
function a4_2 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_directories "$conf"
	for directory in "${directories[@]}"; do
		if testre "$directory" "^\s*(Order|Deny|Allow)\s"; then
			log 4.2 "Order/Deny/Allow directives should not be used for the directory configuration"  "`get_directory_name \"$directory\"`"
			return 2
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find a root element.
# 2. Ensure there is a single AllowOverride directive with the value of None.
# 3. Ensure there are no AllowOverrideList directives present.

function a4_3 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	directory="`get_root_directory "$conf"`"
	if ! testre "$directory" "^\s*AllowOverride\s\s*None\s*$"; then
		log 4.3 '"AllowOverride None" not found within the root directory configuration' "`get_directory_name \"$directory\"`"
		return 2
	fi

	count=`match_count "$directory" "^\s*AllowOverride\s"`
	if [ $count -ne 1 ]; then
		log 4.3 "There're more than one AllowOverride directive within the root directory configuration"  "`get_directory_name \"$directory\"`"
		return 3
	fi

	if testre "$directory" "^\s*AllowOverrideList\s$"; then
		log 4.3 "There's at least one AllowOverrideList found within the root directory configuration"  "`get_directory_name \"$directory\"`"
		return 4
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find any AllowOverride directives.
# 2. Ensure there the value for AllowOverride is None.
# grep -i AllowOverride $APACHE_PREFIX/conf/httpd.conf
# 3. Ensure there are no AllowOverrideList directives present.

function a4_4 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	if [ ! -z "$(match "$conf" "^\s*AllowOverride\s\s*" | grep -ivE  "^\s*AllowOverride\s\s*None\s*$")" ]; then
		log 4.4 '"There is at least AllowOverride directive that its value is not None' "`get_directory_name \"$directory\"`"
		return 2
	fi

	count=`match_count "$directory" "^\s*AllowOverrideList\s"`
	if [ $count -ne 1 ]; then
		log 4.4 "There's at least one AllowOverrideList directive in the configuration" "`get_directory_name \"$directory\"`"
		return 3
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find a root <Directory> element.
# 2. Ensure there is a single Options directive with the value of None.

function a5_1 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	directory="`get_root_directory "$conf"`"
	if ! testre "$directory" "^\s*Options\s\s*None\s*$"; then
		log 5.1 '"Options None" not found within the root directory configuration' "`get_directory_name \"$directory\"`"
		return 2
	fi

	count=`match_count "$directory" "^\s*Options\s"`
	if [ $count -ne 1 ]; then
		log 5.1 "There're more than one Options directive within the root directory configuration" "`get_directory_name \"$directory\"`"
		return 3
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find the document root <Directory> elements.
# 2. Ensure there is a single Options directive with the value of None or Multiviews.

function a5_2 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_directories "$conf"
	populate_document_roots "$conf"

	for document_root in "${document_roots[@]}"; do
		directory_conf="$(get_directory_for_document_root "/var/www/html")"
		log 5.2 "Checking $document_root"
		# echo "$directory_conf"

		if ! testre "$directory_conf" "^\s*Options\s\s*(None|Multiviews)\s*$"; then
			log 5.2 'No Options directive with value of None or Multiviews found within the directory configuration' "$document_root"
			return 2
		fi

		count=`match_count "$directory_conf" "^\s*Options\s"`
		if [ $count -ne 1 ]; then
			log 5.2 "There're more than one Options directive within the root directory configuration" "$document_root"
			return 3
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Search the Apache configuration files (httpd.conf and any included configuration files) to find the all Directory elements.
# 2. Ensure that the Options directives do not enable Includes.

function a5_3 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_directories "$conf"
	for directory in "${directories[@]}"; do
		if ! testre "$directory_conf" "^\s*Options\s\s*(-Includes|-IncludesNOEXEC)\s*$"; then
			if testre "$directory_conf" "^\s*Options\s\s*(All|Includes|IncludesNOEXEC)\s*$"; then
				log 5.3 'Options directives enable Includes'  "`get_directory_name \"$directory\"`"
				return 2
			fi
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Verify the document root directory and the configuration files do not provide for default index.html or welcome page,
# 2. Ensure the Apache User Manual content is not installed by checking the configuration files for manual location directives.
# 3. Verify the Apache configuration files do not have the Server Status handler configured.
# 4. Verify that the Server Information handler is not configured.
# 5. Verify that any other handler configurations such as perl-status is not enabled.

function a5_4 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_document_roots "$conf"
	for document_root in "${document_roots[@]}"; do
		file="${document_root}/index.html"
		if grep -i 'Apache2 Ubuntu Default Page' "$file" >/dev/null 2>/dev/null ; then
			log 5.4 'Default file is not removed' "$file"
			return 2
		fi
	done

	if testre "$conf" "^\s*SetHandler\s\s*server-status\s*$"; then
		log 5.4 'server-status is enabled'
		return 4
	fi

	if testre "$conf" "^\s*SetHandler\s\s*server-info\s*$"; then
		log 5.4 'server-info is enabled'
		return 5
	fi

	if testre "$conf" "^\s*SetHandler\s\s*.*\s*$"; then
		log 5.4 'At least one SetHandler is found'
		return 6
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Locate cgi-bin files and directories enabled in the Apache configuration via Script, ScriptAlias or ScriptAliasMatch or ScriptInterpreterSource directives.
# 2. Ensure the printenv CGI is not installed in any configured cgi-bin directory.

function a5_5 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_document_roots "$conf"
	for document_root in "${document_roots[@]}"; do
		if [ ! -z "find ${document_root} -iname printenv" ]; then
			log 5.5 "printenv file hasn't been removed" "$document_root"
			return 2
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Locate cgi-bin files and directories enabled in the Apache configuration via Script, ScriptAlias or ScriptAliasMatch other ScriptInterpreterSource directives.
# 2. Ensure the test-cgi script is not installed in any configured cgi-bin directory.

function a5_6 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_document_roots "$conf"
	for document_root in "${document_roots[@]}"; do
		if [ ! -z "find ${document_root} -iname test-cgi" ]; then
			log 5.6 "test-cgi file hasn't been removed" "$document_root"
			return 2
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Locate the Apache configuration files and included configuration files.
# 2. Search for all <Directory> directives other than the on the OS root directory.
# 3. Ensure that either one of the following two methods are configured:
# Using the deprecated Order/Deny/Allow method:
# 1. Ensure that group contains a single Order directive within the <Directory> directive with a value of deny, allow
# 2. Verify the <LimitExcept> directive does not include any HTTP methods other than GET, POST, and OPTIONS. (It may contain fewer methods.)
# Using the Require method:
# 3. Ensure there is a single Require directive with the value of all denied
# 4. Ensure there are no Allow or Deny directives in the root element.

function a5_7 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	populate_directories "$conf"
	for directory in "${directories[@]}"; do
		if ! testre "$directory" "^\s*Require\s\s*all\s\s*denied\s*$"; then
			log 5.7 '"Require all denied" not found within the directory configuration' "`get_directory_name \"$directory\"`"
			return 2
		fi

		count=`match_count "$directory" "^\s*Require\s"`
		if [ $count -ne 1 ]; then
			log 5.7 "There're more than one Require directive within the directory configuration" "`get_directory_name \"$directory\"`"
			return 3
		fi

		if testre "$directory" "^\s*(Deny|Allow)\s"; then
			log 5.7 "There's at least one Allow or Deny directives in the directory configuration" "`get_directory_name \"$directory\"`"
			return 4
		fi
	done

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Locate the Apache configuration files and included configuration files.
# 2. Verify there is a single TraceEnable directive configured with a value of off.

function a5_8 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	if ! testre "$conf" "^\s*TraceEnable\s\s*off\s*$"; then
		log 5.8 '"TraceEnable off" not found within the configuration'
		return 2
	fi

	return 0
}


# Perform the following to determine if the recommended state is implemented:
# 1. Locate the Apache configuration files and included configuration files.
# 2. Verify there is a rewrite condition within the global server context that disallows
# requests that do not include the HTTP/1.1 header as shown below.
# 	RewriteEngine On
# 	RewriteCond %{THE_REQUEST} !HTTP/1\.1$
# 	RewriteRule .* - [F]
# 3. Verify the following directives are included in each section so that the main server settings will be inherited.
# 	RewriteEngine On
# 	RewriteOptions Inherit

function a5_9 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"
	global_context="$(get_global_context "$conf")"

	grep -A1 -iE '^\s*RewriteCond\s*\%\{THE_REQUEST\}\s*\!HTTP\/1\\\.1\$\s*$' <<< "$global_context" | tail -n1 |
		grep -iE "^\s*RewriteRule\s*\.\*\s*\-\s*\[F\]\s*$" > /dev/null
	if [ $? -gt 0 ]; then
		log 5.9 'Other HTTP protocol versions should be restricted'
		return 2
	fi

	populate_directories "$conf"
	for directory in "${directories[@]}"; do
		if ! testre "$directory" "^\s*RewriteOptions\s\s*Inherit\s*$"; then
			log 5.9 'Other HTTP protocol versions should be restricted' "`get_directory_name \"$directory\"`"
			return 3
		fi
	done

	return 0
}

# Verify that a FilesMatch directive similar to the one below is present in the apache configuration and not commented out. The deprecated Deny from All directive may be used instead of the Require directive.
# <FilesMatch "^\.ht">
# Require all denied
# </FilesMatch>

function a5_10 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	files_match="$(perl -ne 'print if /^ *<FilesMatch  *\"\^\\\.ht\" *>/i .. /<\/FilesMatch/i' <<< "$conf")"

	if ! testre "$files_match" "^\s*Require\s\s*all\s\s*denied\s*$"; then
		log 5.10 '"Require all denied" not found within the FilesMatch tag for .ht* files'
		return 2
	fi

	count=`match_count "$files_match" "^\s*Require\s"`
	if [ $count -ne 1 ]; then
		log 5.10 "There're more than one Require directive within the FilesMatch tag for .ht* files"
		return 3
	fi

	return 0
}


# Perform the following steps to determine if the recommended state is implemented:
# 1. Verify that the FilesMatch directive that denies access to all files is present as shown in step 3 of the remediation.
# 2. Verify that there is another FilesMatch directive similar to the one in step 4 of the remediation, with an expression that matches the approved file extensions.

function a5_11 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	files_match="$(perl -ne 'print if /^ *<FilesMatch  *\"\^\.\*\$\" *>/i .. /<\/FilesMatch/i' <<< "$conf")"

	if ! testre "$files_match" "^\s*Require\s\s*all\s\s*denied\s*$"; then
		log 5.11 '"Require all denied" not found within the FilesMatch tag for the all file extensions'
		return 2
	fi

	count=`match_count "$files_match" "^\s*Require\s"`
	if [ $count -ne 1 ]; then
		log 5.11 "There're more than one Require directive within the FilesMatch tag for the all file extensions"
		return 3
	fi

	files_match="$(perl -ne 'print if /^ *<FilesMatch  *\"\^\.\*\\\.[^$]*\$\" *>/i .. /<\/FilesMatch/i' <<< "$conf")"
	# I'm not sure if it's necessary
	if ! testre "$files_match" "^\s*Require\s\s*all\s\s*granted\s*$"; then
		log 5.11 '"Require all granted" not found within the FilesMatch tag for the allowed file extentions'
		return 4
	fi

	return 0
}


# Perform the following steps to determine if the recommended state is implemented:
# 1. Locate the Apache configuration files and included configuration files.
# 2. Verify there is a rewrite condition within the global server context that disallows IP
# based requests by requiring a HTTP HOST header similar to the example shown below.
# 	RewriteCond %{HTTP_HOST} !^www\.example\.com [NC]
# 	RewriteCond %{REQUEST_URI} !^/error [NC]
# 	RewriteRule ^.(.*) - [L,F]


function a5_12 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"
	global_context="$(get_global_context "$conf")"

	if ! testre "$global_context" "^\s*RewriteRule\s*\^\.\(\.\*\)\s*-\s*\[L\,F\]"; then
		log 5.12 'IP based requests are allowed'
		return 2
	fi

	return 0
}


# Perform the following steps to determine if the recommended state is implemented:
# Verify that no Listen directives are in the Apache configuration file with no IP address specified, or with an IP address of all zero's.

function a5_13 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	if testre "$conf" "^\s*Listen\s*(|0\.0\.0\.0|\[\:\:ffff\:0\.0\.0\.0\])\:[0-9][0-9]*"; then
		log 5.13 "The specific IP address or addresses intended should be explicitly specified"
		return 2
	fi

	return 0
}


# Perform the following steps to determine if the recommended state is implemented:
# Ensure a Header directive for X-Frame-Options is present in the Apache configuration and has the condition always, an action of append and a value of SAMEORIGIN or DENY

function a5_14 {
	conf_file="$1"
	conf="`read_conf_file $conf_file`"

	if testre "$conf" "^\s*Header\s*always\s*append\s*X-Frame-Options\s*(SAMEORIGIN|DENY)\s*$"; then
		log 5.14 "Browser Frame Options are not restricted"
		return 2
	fi

	return 0
}