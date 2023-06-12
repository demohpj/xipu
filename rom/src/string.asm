; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include lcd.asm
include keyboard.asm

; Current cursor position
var xStringCursorX
var xStringCursorY

; No arguments
; No return
global .xStringInit:
	push A
	; Reset cursor position
	mov 0, A
	str A, *xStringCursorX
	str A, *xStringCursorY
	pop A
	ret

; Arguments:
; 	X - column
;	Y - row
; No return
external .xStringSetCursor:
	push X
	push Y
	; Check maximum column
	; 40 >= X
	cmp 40, X
	jge .xStringSetCursor_x_ok
	; Value is too high, set default
	mov 0, X
.xStringSetCursor_x_ok:
	; Check maximum row
	; 30 > Y
	cmp 30, Y
	jg .xStringSetCursor_y_ok
	; Value is too high, set default
	mov 0, Y
.xStringSetCursor_y_ok:
	; Save new cursor position
	str X, *xStringCursorX
	str Y, *xStringCursorY
	pop Y
	pop X
	ret

; Arguments:
; 	A - column
; No return
external .xStringSetCursorX:
	push X
	push Y
	; Write new column position to set
	mov A, X
	; Read current row position to set
	ldr *xStringCursorY, Y
	; Set new column position and leave row position
	call .xStringSetCursor
	pop Y
	pop X
	ret

; No arguments
; Return:
;	A - column
external .xStringGetCursorX:
	; Get current column position
	ldr *xStringCursorX, A
	ret

; Arguments:
; 	A - row
; No return
external .xStringSetCursorY:
	push X
	push Y
	; Write new row position to set
	mov A, Y
	; Read current column position to set
	ldr *xStringCursorX, X
	; Set new row position and leave column position
	call .xStringSetCursor
	pop Y
	pop X
	ret

; No arguments
; Return:
;	A - row
external .xStringGetCursorY:
	; Get current row position
	ldr *xStringCursorY, A
	ret

; No arguments
; No return
external .xStringNewLine:
	push A
	; Set cursor at begin of line
	mov 0, A
	str A, *xStringCursorX
	; Set cursor at next line
	ldr *xStringCursorY, A
	inc A
	; Check if scrolling is needed
	; 30 > A
	cmp 30, A
	jg .xStringNewLine_no_scroll
	; Scroll and update cursor row
	call .xLcdScroll
	mov 29, A
.xStringNewLine_no_scroll:
	str A, *xStringCursorY
	pop A
	ret

; Arguments:
; 	A - char to print
; No return
external .xStringPrintChar:
	push A
	push B
	; Select low address register
	mov 2, A
	call .xIOSetRegister
	ldr *xStringCursorX, A
	mov 0, B
	; Check maximum column
	; 40 > A
	cmp 40, A
	jg .xStringPrintChar_x_ok
	; Set cursor at begin of line
	mov 0, A
	; Scrolling is needed
	mov 1, B
.xStringPrintChar_x_ok:
	call .xIOWrite
	inc A
	str A, *xStringCursorX
	ldr *xStringCursorY, A
	; Check if scrolling is needed
	; 1 != B
	cmp 1, B
	jne .xStringPrintChar_y_ok
	inc A
	; Check maximum row
	; 30 > A
	cmp 30, A
	jg .xStringPrintChar_no_scroll
	; Scroll and update cursor row
	call .xLcdScroll
	mov 29, A
.xStringPrintChar_no_scroll:
	str A, *xStringCursorY
.xStringPrintChar_y_ok:
	mov 3, A
	; Select high address register
	call .xIOSetRegister
	ldr *xStringCursorY, A
	call .xIOWrite
	pop B
	pop A
	call .xLcdSetChar
	ret

; Arguments:
; 	X - low address
;	Y - high address
; No return
global .xStringPrintRom:
	push X
	push Y
	push A
.xStringPrintRom_loop:
	; Load char from ROM
	ldf A
	; Check if it is end of string
	cmp 0, A
	je .xStringPrintRom_end
	; Print char
	call .xStringPrintChar
	; Incrementing address
	inc X
	incc Y
	jmp .xStringPrintRom_loop
.xStringPrintRom_end:
	pop A
	pop Y
	pop X
	ret

; Arguments:
; 	X - low address
;	Y - high address
; No return
external .xStringPrint:
	push X
	push Y
	push A
.xStringPrint_loop:
	; Load char from RAM
	ldr A
	; Check if it is end of string
	cmp 0, A
	je .xStringPrint_end
	; Print char
	call .xStringPrintChar
	; Incrementing address
	inc X
	incc Y
	jmp .xStringPrint_loop
.xStringPrint_end:
	pop A
	pop Y
	pop X
	ret

; Multiplier values
const u8toDecTable 100, 10, 1

; Current multiplier value
var u8toDec

; Current working value
var u8Value

; Allow print digits flag
var u8LetPrint

; Arguments:
; 	A - value
; No return
external .xStringPrintU8:
	push A
	push B
	push X
	push Y
	; Set local variables
	str A, *u8Value
	str 0, *u8LetPrint
	; Reset loop counter
	clr B
	; Print every digit in one execution of loop
.xStringPrintU8_loop:
	; Load address to multiplier table
	mov low(*u8toDecTable), X
	mov high(*u8toDecTable), Y
	; Move address to current multipier
	add B, X
	incc Y
	; Load multiplier
	ldf A
	; Store multiplier in temporary variable
	str A, *u8toDec
	; Save to stack main loop counter
	push B
	; Load current working value
	ldr *u8Value, X
	; Reset loop counter
	clr B
	; Convert hex to dec using multiplier
.xStringPrintU8_minusLoop:
	; Load multiplier
	ldr *u8toDec, A
	; Try substract
	sub A, X
	ldr *u8Value, A
	cmp A, X
	; A < X
	jl .xStringPrintU8_minusLoopEnd
	str X, *u8Value
	; Increment try counter
	inc B
	jmp .xStringPrintU8_minusLoop
	; End of substracting loop
.xStringPrintU8_minusLoopEnd:
	; Load let print flag
	ldr *u8LetPrint, A
	cmp 0, A
	jne .xStringPrintU8_minusPrint
	cmp 0, B
	je .xStringPrintU8_minusDontPrint
	; Set true to let print flag
	str 1, *u8LetPrint
	; Print digit
.xStringPrintU8_minusPrint:
	; Load digit to print
	mov B, A
	; Convert to ASCII char
	add '0', A
	; Print char
	call .xStringPrintChar
	; Do not print zeros at begin
.xStringPrintU8_minusDontPrint:
	; Get back counter value of main loop
	pop B
	; Execute loop only 2+1 times
	loope 2, B, .xStringPrintU8_loop
	; Check if nothing was printed
	ldr *u8LetPrint, A
	cmp 0, A
	jne .xStringPrintU8_dontPrintZero
	; It needs to print zero if nothing was printed before
	mov '0', A
	call .xStringPrintChar
.xStringPrintU8_dontPrintZero:
	pop Y
	pop X
	pop B
	pop A
	ret

; Multiplier values
const u16toDecTableLow 0x10, 0xe8, 0x64, 0x0a, 0x01
const u16toDecTableHigh 0x27, 0x03, 0x00, 0x00, 0x00

; Current multiplier value
var u16toDecLow
var u16toDecHigh

; Current working value
var u16ValueLow
var u16ValueHigh

; Allow print digits flag
var u16LetPrint

; Arguments:
; 	A - low value
;	B - high value
; No return
external .xStringPrintU16:
	push A
	push B
	push X
	push Y
	; Set local variables
	str A, *u16ValueLow
	str B, *u16ValueHigh
	str 0, *u16LetPrint
	; Reset loop counter
	clr B
.xStringPrintU16_loop:
	; Load address to multiplier low table
	mov low(*u16toDecTableLow), X
	mov high(*u16toDecTableLow), Y
	; Move address to current multipier low
	add B, X
	incc Y
	; Load multiplier low
	ldf A
	; Store multiplier low in temporary variable
	str A, *u16toDecLow
	; Load address to multiplier high table
	mov low(*u16toDecTableHigh), X
	mov high(*u16toDecTableHigh), Y
	; Move address to current multipier high
	add B, X
	incc Y
	; Load multiplier high
	ldf A
	; Store multiplier high in temporary variable
	str A, *u16toDecHigh
	; Save to stack main loop counter
	push B
	; Load current working values
	ldr *u16ValueLow, X
	ldr *u16ValueHigh, Y
	; Reset loop counter
	clr B
	; Convert hex to dec using multiplier
.xStringPrintU16_minusLoop:
	; Load multiplier low
	ldr *u16toDecLow, A
	; Try substract low
	sub A, X
	; Load multiplier high
	ldr *u16toDecHigh, A
	; Try substract high
	subc A, Y
	ldr *u16ValueHigh, A
	cmp A, Y
	; A < Y
	jl .xStringPrintU16_minusLoopEnd
	str X, *u16ValueLow
	str Y, *u16ValueHigh
	; Increment try counter
	inc B
	jmp .xStringPrintU16_minusLoop
	; End of substracting loop
.xStringPrintU16_minusLoopEnd:
	; Load let print flag
	ldr *u16LetPrint, A
	cmp 0, A
	jne .xStringPrintU16_minusPrint
	cmp 0, B
	je .xStringPrintU16_minusDontPrint
	; Set true to let print flag
	str 1, *u16LetPrint
	; Print digit
.xStringPrintU16_minusPrint:
	; Load digit to print
	mov B, A
	; Convert to ASCII char
	add '0', A
	; Print char
	call .xStringPrintChar
	; Do not print zeros at begin
.xStringPrintU16_minusDontPrint:
	; Get back counter value of main loop
	pop B
	; Execute loop only 4+1 times
	loope 4, B, .xStringPrintU16_loop
	; Check if nothing was printed
	ldr *u16LetPrint, A
	cmp 0, A
	jne .xStringPrintU16_dontPrintZero
	; It needs to print zero if nothing was printed before
	mov '0', A
	call .xStringPrintChar
.xStringPrintU16_dontPrintZero:
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; No return
.xStringReadBackspace:
	push A
	push X
	push Y
	; Load cursor position
	ldr *xStringCursorX, X
	ldr *xStringCursorY, Y
	; Erasing last char
	mov 0x00, A
	call .xStringPrintChar
	; Move left cursor
	dec X
	; Save cursor position
	str X, *xStringCursorX
	str Y, *xStringCursorY	
	pop Y
	pop X
	pop A
	ret
	
; No arguments
; No return
.xStringReadShowPrompt:
	push A
	push X
	push Y
	; Load cursor position
	ldr *xStringCursorX, X
	ldr *xStringCursorY, Y
	; Print cursor
	mov 0xDB, A
	call .xStringPrintChar
	; Save cursor position
	str X, *xStringCursorX
	str Y, *xStringCursorY
	pop Y
	pop X
	pop A
	ret

; Arguments:
;	A - buffer length
; 	X - low address
;	Y - high address
; No return
external .xStringRead:
	; Push registers to stack
	push A
	push B
	push X
	push Y
	mov A, B
	call .xStringReadShowPrompt
	call .xLcdRefresh
.xStringRead_loop:
	; Push buffer length
	push A
	call .xKeyboardGetChar
	; Check for backspace
	cmp 0x08, A
	je .xStringRead_backspace
	; Check for enter
	cmp 0x0d, A
	je .xStringRead_enter
	; Check for not printable char
	; 0x20 > A
	cmp 0x20, A
	jg .xStringRead_not_print
	; 0x80 <= A
	cmp 0x80, A
	jle .xStringRead_not_print
	; Respect buffer size limit
	; 1 >= B
	cmp 1, B
	jge .xStringRead_not_print
	dec B
	; Print readed char
	call .xStringPrintChar
	call .xStringReadShowPrompt
	call .xLcdRefresh
	; Save char to buffer
	str A
	; Incrementing address
	inc X
	incc Y
	; Pop buffer length
	pop A
	jmp .xStringRead_loop
.xStringRead_not_print:
	; Pop buffer length
	pop A
	jmp .xStringRead_loop
.xStringRead_backspace:
	; Pop buffer length
	pop A
	; Do not go back too far
	cmp A, B
	je .xStringRead_loop
	inc B
	; Decrementing address
	dec X
	decc Y
	; Moving back prompt
	call .xStringReadBackspace
	call .xStringReadShowPrompt
	call .xLcdRefresh
	jmp .xStringRead_loop
.xStringRead_enter:
	; Pop buffer length
	pop A
	; Add null terminator to buffer
	str 0x00
	; Pop from stack
	pop Y
	pop X
	pop B
	pop A
	ret
	
; Arguments:
; 	X - low address
;	Y - high address
; Return:
;	A - length
external .xStringLength:
	push X
	push Y
	push B
	mov 0, A
.xStringLength_loop:
	ldr B
	; 0 == B
	cmp 0, B
	; End of string found
	je .xStringLength_end
	; Increment the string pointer
	inc X
	incc Y
	; Increment string length
	inc A
	jmp .xStringLength_loop
.xStringLength_end:
	pop B
	pop Y
	pop X
	ret
	
var charToFind
	
; Arguments:
;	A - char to find
; 	X - low address
;	Y - high address
; Return:
;	A - offset
external .xStringFind:
	push X
	push Y
	push B
	str A, *charToFind
	mov 0, A
.xStringFind_loop:
	push A
	ldr *charToFind, A
	ldr B
	; A == B
	cmp A, B
	pop A
	; Char found
	je .xStringFind_end
	; 0 == B
	cmp 0, B
	; End of string found
	je .xStringFind_stringEnded
	; Increment the string pointer
	inc X
	incc Y
	; Increment string length
	inc A
	jmp .xStringFind_loop
.xStringFind_stringEnded:
	mov 0, A
.xStringFind_end:
	pop B
	pop Y
	pop X
	ret
