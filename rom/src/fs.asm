; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include io.asm

; Arguments:
; 	X - low address
;	Y - high address
; No return
external .xFSSetName:
	push A
	push B
	push X
	push Y
	mov 0, B
.xFSSetName_loop:
	; Check if it is a max length of name
	; 11 < B
	cmp 11, B
	jl .xFSSetName_end
	; Load char from RAM
	ldr A
	push A
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Write offset of name
	mov B, A
	call .xIOWrite
	; Select fs name register
	mov 13, A
	call .xIOSetRegister
	; Write char of name
	pop A
	call .xIOWrite
	; Check if it is end of string
	cmp 0, A
	je .xFSSetName_end
	; Incrementing address
	inc X
	incc Y
	; Increment offset of name
	inc B
	jmp .xFSSetName_loop
.xFSSetName_end:
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; Return:
; 	A - success (0 - error, 1 - ok)
global .xFSOpenApp:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Select open app command
	mov 0, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Clear value in A
	mov 0, A
	; Open app and get success status
	call .xIOReadLowHalf
	ret
	
; No arguments
; Return:
; 	A - success (0 - error, 1 - ok)
external .xFSOpenFile:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Select open file command
	mov 1, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Clear value in A
	mov 0, A
	; Open app and get success status
	call .xIOReadLowHalf
	ret

; No arguments
; Return:
; 	A - success (0 - error, 1 - ok)
external .xFSListApp:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Select list app dir command
	mov 2, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Clear value in A
	mov 0, A
	; Open app and get success status
	call .xIOReadLowHalf
	ret

; No arguments
; Return:
; 	A - success (0 - error, 1 - ok)
external .xFSListFile:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Select list file dir command
	mov 3, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Clear value in A
	mov 0, A
	; Open app and get success status
	call .xIOReadLowHalf
	ret

; No arguments
; Return:
; 	A - low byte
;	B - high byte
external .xFSSize:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Select size command
	mov 4, A
	call .xIOWrite
	; Select high address register
	mov 3, A
	call .xIOSetRegister
	; Select high byte of size
	mov 1, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Read high byte
	call .xIORead
	mov A, B
	; Select high address register
	mov 3, A
	call .xIOSetRegister
	; Select low byte of size
	mov 0, A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Read low byte
	call .xIORead
	ret

; Arguments:
; 	A - block number
;	X - low address
;	Y - high address
; No return
external .xFSReadBlock:
	push A
	push B
	push X
	push Y
	push A
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Write block number
	pop A
	call .xIOWrite
	; Select fs command register
	mov 15, A
	call .xIOSetRegister
	; Write read block command
	mov 5, A
	call .xIOWriteLowHalf
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Write byte number
	clr A
	call .xIOWrite
	; Select fs data register
	mov 14, A
	call .xIOSetRegister
	clr B
.xFSReadBlock_loop:	
	; Read byte from fs data
	call .xIORead
	; Copy byte to memory
	str A
	; Increment buffer address
	inc X
	incc Y
	loope 255, B, .xFSReadBlock_loop
	pop Y
	pop X
	pop B
	pop A
	ret
