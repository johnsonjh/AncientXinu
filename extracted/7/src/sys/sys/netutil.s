/* netutil.s - hs2net, net2hs, hl2net, net2hl, vax2hl hl2vax */

/*------------------------------------------------------------------------
/* Utility routines that map host data to/from network byte/word order
/* (needed because host is "Little Endian", network is "Big Endian")
/*------------------------------------------------------------------------

	.globl	_hs2net, _hl2net, _net2hs, _net2hl
	.text
_hl2net:			/ Entry point that maps longs to/from
_net2hl:			/  host/network byte order
	mov	4(sp),r1	/ get high-order word and put in
	swab	r1		/  network byte order

_hs2net:			/ Entry point that maps shorts to/from
_net2hs:			/  host/network byte order
	mov	2(sp),r0	/ pick up short argument
	swab	r0		/ swap bytes of result
	rts	pc		/ return to caller

/*------------------------------------------------------------------------
/* auxiliary routines that map from host to/from VAX longword order
/*------------------------------------------------------------------------

	.globl	_hl2vax, _vax2hl
	.text
_hl2vax:			/ Reverse order of shorts within long
_vax2hl:			/  when mapping to/from VAX/PDP11 long
	mov	2(sp),r1
	mov	4(sp),r0
	rts	pc
