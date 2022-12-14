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
; 	A - foreground color (4 bit)
; No return
external .xLcdSetForeground:
	push A
	; Set lcd color register
	mov 4, A
	call .xIOSetRegister
	pop A
	; Write foreground color
	call .xIOWriteLowHalf
	ret

; Arguments:
; 	A - background color (4 bit)
; No return
external .xLcdSetBackground:
	push A
	push A
	; Set lcd color register
	mov 4, A
	call .xIOSetRegister
	pop A
	; Swap to get value in high half
	swap A
	; Write background color
	call .xIOWriteHighHalf
	pop A
	ret	

; Arguments:
; 	A - char to display
; No return
external .xLcdSetChar:
	push A
	; Set lcd char register
	mov 5, A
	call .xIOSetRegister
	pop A
	; Write char
	call .xIOWrite
	ret

; No arguments
; No return
external .xLcdClear:
	push A
	; Set lcd command register
	mov 6, A
	call .xIOSetRegister
	; Write clear command
	mov 0, A
	call .xIOWriteLowHalf
	pop A
	ret

; No arguments
; No return
external .xLcdRefresh:
	push A
	; Set lcd command register
	mov 6, A
	call .xIOSetRegister
	; Write refresh command
	mov 1, A
	call .xIOWriteLowHalf
	pop A
	ret

; No arguments
; No return
external .xLcdScroll:
	push A
	; Set lcd command register
	mov 6, A
	call .xIOSetRegister
	; Write scroll command
	mov 2, A
	call .xIOWriteLowHalf
	pop A
	ret
