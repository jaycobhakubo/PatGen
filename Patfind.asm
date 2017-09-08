;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; patfind - assembly pattern mask checking functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.386p		; 386 protected mode
	OPTION	OLDSTRUCTS
	.model flat


	.DATA
ignoremask dd	0

bestptr	dd	?
nummasks dd	?
hitmask	dd	?
iicount	dd	?
jjcount	dd	?
outmasks dd	?

bestcnt	dd	?
bestind dd	?
index	dd	?
rc	dd	?

bitsinbyte db	256 dup(0)

DELIM	equ	0FFFFFFFFh

        .CODE


	public	FindBestMask_
;
; returns # of hits away from bingo
;
; int FindBestMask(DWORD hitmask, DWORD* masklist, DWORD nummasks, DWORD* bestindex);
;
;
; input: eax - hitmask
;	 edx - ptr to masklist
;	 ebx - number of masks in masklist
;	 ecx - ptr to store index of best mask
;
FindBestMask_ PROC
	pushad
;
	mov	hitmask,eax
	mov	bestptr,ecx
	mov	nummasks,ebx
; see if we need to generate the bitsinbyte table
	call	setcounttable
;
; do the mask checking
;
	mov	edi,edx
	mov	bestcnt,100
	mov	ebx,OFFSET bitsinbyte
	mov	index,0
mloop:
	mov	eax,[edi]	; get next pattern mask
;
	cmp	eax,ignoremask
	je	booger		; ignore it
;
	mov	ebp,eax		; save it
	and	eax,hitmask	; and with hitmask of card
	xor	eax,ebp		; eax now contains bits in list and not in hitmask
; here the number of bits in eax is the number of hits away the card is
	xor	edx,edx
; count lower byte
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	mov	ecx,edx
; now count next byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; count 3rd byte
	shr	eax,16	
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; lastly count upper byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; see if this is the best so far
	cmp	ecx,bestcnt
	jge	booger
	mov	bestcnt,ecx
	mov	ecx,index
	mov	bestind,ecx
	cmp	bestcnt,0	; found a winner stop looking
	je	onbeach
booger:
; move to next mask
	inc	index
	add	edi,4
	dec	nummasks
	jne	mloop	
onbeach:
; done, leave
	mov	edx,bestptr
	mov	eax,bestind
	mov	[edx],eax
;
	popad
	mov	eax,bestcnt
	cmp	eax,100
	jne	@F
	mov	eax,0
@@:
	ret
FindBestMask_ ENDP

;CountHits_ - counts number of hits in a mask
	public	CountHits_
CountHits_ PROC
	pushad
;
	push	eax
	call	setcounttable
	pop	eax
	xor	edx,edx
	mov	ebx,OFFSET bitsinbyte
; count lower byte
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	mov	ecx,edx
; now count next byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; count 3rd byte
	shr	eax,16	
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; lastly count upper byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
	mov	rc,ecx
;
	popad
	mov	eax,rc
	ret
CountHits_ ENDP


; CheckMask_ - return # of hits away for 1 hitmask and 1 target mask
	public	CheckMask_
; int CheckMask(DWORD hitmask, DWORD targetmask);
; input: eax - hitmask
;	 edx - target mask
CheckMask_ PROC
	pushad
;
	push	eax
	push	edx
	call	setcounttable
	pop	edx
	pop	eax
;
	mov	ebp,edx		; save it
	and	eax,edx		; and with hitmask of card
	xor	eax,ebp		; eax now contains bits in list and not in hitmask
; here the number of bits in eax is the number of hits away the card is
	xor	edx,edx
; count lower byte
	mov	ebx,OFFSET bitsinbyte
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	mov	ecx,edx
; now count next byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; count 3rd byte
	shr	eax,16	
	mov	dl,al
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
; lastly count upper byte
	mov	dl,ah
	mov	dl,BYTE PTR [ebx+edx]
	add	ecx,edx
	mov	rc,ecx
;
	popad
	mov	eax,rc
	ret
CheckMask_ ENDP

SetIgnoreMask_ PROC
	mov	ignoremask,eax
	ret
SetIgnoreMask_ ENDP

setcounttable PROC
	cmp	bitsinbyte+1,1
	je	already
; generate the bitsinbyte table
	xor	edi,edi
flob:
	mov	ebx,edi
	mov	ecx,8
	xor	eax,eax
@@:
	test	ebx,1
	je	bob
	inc	eax
bob:
	shr	ebx,1
	dec	ecx
	jne	@B
	mov	bitsinbyte[edi],al
	inc	edi
	cmp	edi,256
	jl	flob
already:
	ret
setcounttable ENDP

; ElimSubSets - eliminates supersets of other masks in a mask list
; ElimSubSets(DWORD* list, DWORD num);
; 	eax - ptr to list
;	edx - number of masks in list
@ElimSubSets@8 PROC
;	mov	eax,edx
;	ret
IF 0
IFDEF ISMASTER
	mov	eax,edx
	ret
ENDIF
ENDIF
	pushad
	mov eax,ecx
;
;	mov	nummasks,edx
	mov	ebp,edx
	mov	outmasks,0
	mov	bestptr,eax
;
	mov	esi,eax
	mov	DWORD PTR [esi+edx*4],DELIM
	mov	iicount,0
oloop:
	mov	edi,esi
	add	edi,4
	mov	ecx,iicount
;
	cmp	ecx,34
	jne	upo
	mov	ecx,ecx
upo:
;
	inc	ecx
	mov	eax,[esi]
	cmp	eax,DELIM
	je	SHORT inext2
iloop:
	mov	ebx,[edi]
	cmp	ebx,DELIM
	je	inext
; check if eax is a subset of ebx
	mov	edx,eax
	and	edx,ebx
	cmp	edx,eax
	jne	ii2
; eax is a subset of ebx, elimate ebx masks
	mov	DWORD PTR [edi],DELIM
;	dec	outmasks
	jmp	SHORT inext
ii2:
	cmp	edx,ebx
	jne	inext
; ebx is a subset of eax, elimate eax masks
;	mov	DWORD PTR [esi],DELIM
;	dec	outmasks
	jmp	SHORT inext2
inext:
; move to next
	add	edi,4
	inc	ecx
	cmp	ecx,ebp
	jl	iloop
; good mask to output
	mov	ebx,outmasks
	mov	ecx,bestptr
	mov	DWORD PTR [ecx+ebx*4],eax
	inc	outmasks
inext2:
; move to next
	add	esi,4
	inc	iicount
	mov	eax,iicount
	inc	eax
	cmp	eax,ebp
;	jl	oloop
	jle	oloop
;
	popad
	mov	eax,outmasks
	ret
@ElimSubSets@8 ENDP

        END


