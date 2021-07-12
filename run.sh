#!/bin/bash

[ $# -lt 1 ] && {
	echo 'Usage: run.sh [4.1] [4.2] [4.3] [4.4] [5.1] [5.2] [5.3] [5.4] [5.5] [5.6] [5.7] [5.8] [5.9] [5.10] [5.11] [5.12] [5.13] [5.14]'
	exit 1
}

[ -e /etc/apache/envvars ] && . /etc/apache/envvars
. conf.inc
. audits.inc

while [ $# -gt 0 ]; do
	print 93 "Running audit $1 ..."
	run $1
	print_fail_pass $?
	shift
done
