; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include io.asm

; No arguments
; No return
external .xLedRunOff:
	push A
	; Set led register
	mov 1, A
	call .xIOSetRegister
	; Write run led off command
	mov 0, A
	call .xIOWriteLowHalf
	pop A
	ret
	
; No arguments
; No return
external .xLedRunOn:
	push A
	; Set led register
	mov 1, A
	call .xIOSetRegister
	; Write run led on command
	mov 1, A
	call .xIOWriteLowHalf
	pop A
	ret

; No arguments
; No return
external .xLedErrorOff:
	push A
	; Set led register
	mov 1, A
	call .xIOSetRegister
	; Write error led off command
	mov 2, A
	call .xIOWriteLowHalf
	pop A
	ret
	
; No arguments
; No return
external .xLedErrorOn:
	push A
	; Set led register
	mov 1, A
	call .xIOSetRegister
	; Write error led on command
	mov 3, A
	call .xIOWriteLowHalf
	pop A
	ret

