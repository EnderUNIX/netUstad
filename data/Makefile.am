## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration
if OS_FREEBSD
SUBDIRS = freebsd images
endif
if OS_LINUX
SUBDIRS = linux images
endif

netustad_confdir = ${prefix}/etc/netustad
netustad_conf_DATA = netustad.conf netustad.conf-dist

netustad_certdir = ${prefix}/share/netustad/cert
netustad_cert_DATA =  netustad.crt netustad.csr netustad.key

netustad_ctldir = ${prefix}/bin
netustad_ctl_SCRIPTS = netustadctl

EXTRA_DIST = $(netustad_conf_DATA) $(netustad_ctl_SCRIPTS) $(netustad_cert_DATA) chperm.sh
