#!/bin/sh

for i in $@ 
	do
		case $i in 
		$0)
			;;
		$1)
			;;
		$2)
			;;
		*)	
			chmod $1 $2/$i
			if test $? -eq 0; then
				echo "chmod $1 "$2/$i"  done" 
			else
				echo "chmod $1 "$2/$i"  failed"
				exit 1
			fi
		esac

	done

