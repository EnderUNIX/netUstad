## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration


netustad_freebsddir = $(prefix)/@NO_PREFIX_PACKAGE_DATA_DIR@/@PACKAGE@/script

netustad_freebsd_SCRIPTS = \
	addform \
	authform \
	editform \
	footer \
	header \
	if_edit \
	if_table \
	loadindex \
	maketable \
	nat_add \
	nat_del \
	nat_edit \
	rt_add \
	rt_del \
	rt_edit \
	rt_table \
	shownat \
	showrule \
	stats \
	addrule.content \
	editrule.content \
	writeconfig

netustad_freebsd_DATA = \
	action1.inc \
	action2.inc \
	edit-parse \
	if-edit \
	if-parse \
	intf-parse \
	nat-edit \
	nat-parse \
	netustad.css \
	proto.inc \
	rt-edit \
	rt-parse \
	ruleoption1.inc \
	ruleoption2.inc \
	service-parse \
	stats-parse \
	static-parse
	 

EXTRA_DIST = $(netustad_freebsd_SCRIPTS) $(netustad_freebsd_DATA)

install-data-hook:
	/bin/sh ../chperm.sh 700 ${netustad_freebsddir} ${netustad_freebsd_SCRIPTS}; \
	/bin/sh ../chperm.sh 600 ${netustad_freebsddir} ${netustad_freebsd_DATA}
