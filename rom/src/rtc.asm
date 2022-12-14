; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include io.asm

; Date and time buffer
var year
var month
var day
var hour
var minute
var second

; No arguments
; No return
external .xRTCRead:
	push A
	push B
	push X
	push Y
	; Select RT command register
	mov 10, A
	call .xIOSetRegister
	; Write read command for read current date and time from RTC
	mov 0, A
	call .xIOWrite
	; Set address to first field
	mov low(*year), X
	mov high(*year), Y
	; Clear loop counter
	clr B
	; Read loop
.xRTCRead_loop:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Write field number to read
	mov B, A
	call .xIOWrite
	; Select RTC field register
	mov 9, A
	call .xIOSetRegister
	; Read field
	call .xIORead
	; Store field value to memory
	str A
	; Move to next field
	inc X
	incc Y
	; Repeat 5+1 times to read all RTC fields
	loope 5, B, .xRTCRead_loop
	pop Y
	pop X
	pop B
	pop A
	ret
	
; No arguments
; No return
external .xRTCWrite:
	push A
	push B
	push X
	push Y
	; Set address to first field
	mov low(*year), X
	mov high(*year), Y
	; Clear loop counter
	clr B
	; Write loop
.xRTCWrite_loop:
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	; Write field number to write
	mov B, A
	call .xIOWrite
	; Select RTC field register
	mov 9, A
	call .xIOSetRegister
	; Load field value from memory
	ldr A
	; Write field
	call .xIOWrite
	; Move to next field
	inc X
	incc Y
	; Repeat 5+1 times to write all RTC fields
	loope 5, B, .xRTCWrite_loop
	; Select RT command register
	mov 10, A
	call .xIOSetRegister
	; Write write command for write date and time to RTC
	mov 1, A
	call .xIOWrite
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; Return:
;	A - year
external .xRTCGetYear:
	ldr *year, A
	ret

; Arguments:
;	A - year
; No return
external .xRTCSetYear:
	str A, *year
	ret

; No arguments
; Return:
;	A - month
external .xRTCGetMonth:
	ldr *month, A
	ret

; Arguments:
;	A - month
; No return
external .xRTCSetMonth:
	str A, *month
	ret

; No arguments
; Return:
;	A - day
external .xRTCGetDay:
	ldr *day, A
	ret

; Arguments:
;	A - day
; No return
external .xRTCSetDay:
	str A, *day
	ret

; No arguments
; Return:
;	A - hour
external .xRTCGetHour:
	ldr *hour, A
	ret

; Arguments:
;	A - hour
; No return
external .xRTCSetHour:
	str A, *hour
	ret

; No arguments
; Return:
;	A - minute
external .xRTCGetMinute:
	ldr *minute, A
	ret

; Arguments:
;	A - minute
; No return
external .xRTCSetMinute:
	str A, *minute
	ret

; No arguments
; Return:
;	A - second
external .xRTCGetSecond:
	ldr *second, A
	ret

; Arguments:
;	A - second
; No return
external .xRTCSetSecond:
	str A, *second
	ret
