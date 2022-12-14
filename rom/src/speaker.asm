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
; 	A - note
; No return
external .xSpeakerSetNote:
	push A
	; Set address low register
	mov 2, A
	call .xIOSetRegister
	; Select note field
	mov 0, A
	call .xIOWrite
	; Set speaker field register
	mov 11, A
	call .xIOSetRegister
	pop A
	; Write note
	call .xIOWrite
	ret

; Arguments:
; 	A - time
; No return
external .xSpeakerSetTime:
	push A
	; Set address low register
	mov 2, A
	call .xIOSetRegister
	; Select time field
	mov 1, A
	call .xIOWrite
	; Set speaker field register
	mov 11, A
	call .xIOSetRegister
	pop A
	; Write time
	call .xIOWrite
	ret

; Arguments:
; 	A - fill
; No return
external .xSpeakerSetFill:
	push A
	; Set address low register
	mov 2, A
	call .xIOSetRegister
	; Select fill field
	mov 2, A
	call .xIOWrite
	; Set speaker field register
	mov 11, A
	call .xIOSetRegister
	pop A
	; Write fill
	call .xIOWriteLowHalf
	ret

; Arguments:
; 	A - volume
; No return
external .xSpeakerSetVolume:
	push A
	; Set address low register
	mov 2, A
	call .xIOSetRegister
	; Select volume field
	mov 3, A
	call .xIOWrite
	; Set speaker field register
	mov 11, A
	call .xIOSetRegister
	pop A
	; Write volume
	call .xIOWriteLowHalf
	ret

; No arguments
; No return
external .xSpeakerClear:
	push A
	; Set speaker command register
	mov 12, A
	call .xIOSetRegister
	; Write clear command
	mov 0, A
	call .xIOWriteLowHalf
	pop A
	ret

; No arguments
; No return
external .xSpeakerAddNote:
	push A
	; IO transmission
	mov 12, A
	call .xIOSetRegister
	; Write add note command
	mov 1, A
	call .xIOWriteLowHalf
	pop A
	ret
