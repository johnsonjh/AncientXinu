#!/bin/sh
: 'lorder11 -- file names for an archive so A<B if A references B'
: '				   ...another one for sports fans'
:
: 'lorder11 file...'
:
trap "rm -f $$sym?ef; exit" 0 1 2 13 15
case $# in
     0) echo usage: lorder11 file ...
	exit
	;;
     1) case $1 in
	*.o)	set $1 $1
	esac
esac
nm11 -g $* | sed '
	/^$/d
	/:$/{
		/\.o:/!d
		s/://
		h
		s/.*/& &/
		p
		d
	}
	/[TD] /{
		s/.* //
		G
		s/\n/ /
		w '$$symdef'
		d
	}
	s/.* //
	G
	s/\n/ /
	w '$$symref'
	d
'
sort $$symdef -o $$symdef
sort $$symref -o $$symref
join $$symref $$symdef | sed 's/[^ ]* *//'
