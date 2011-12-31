## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration
netustad_linuxdir = $(prefix)/@NO_PREFIX_PACKAGE_DATA_DIR@/@PACKAGE@/script

netustad_linux_SCRIPTS = \
	addform \
	addform.content \
	authform \
	editform \
	editform.content \
	footer \
	header \
	if_edit \
	if_table \
	loadindex \
	nat_add \
	nat_del \
	nat_edit \
	nat_table \
	rt_add \
	rt_del \
	rt_edit \
	rt_table \
	showrule \
	stats
netustad_linux_DATA = \
	chain-parse \
	edit-parse \
	intf-parse \
	maketable \
	nat-edit \
	nat-parse \
	if-edit \
	if-parse \
	netustad.css \
	rt-edit \
	rt-parse \
	stats-parse
	

EXTRA_DIST = $(netustad_linux_SCRIPTS) $(netustad_linux_DATA)

install-data-hook: 
	/bin/sh ../chperm.sh 700 ${netustad_linuxdir} ${netustad_linux_SCRIPTS}; \
	/bin/sh ../chperm.sh 600 ${netustad_linuxdir} ${netustad_linux_DATA}
