; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

map ../../../bin/rom/rom.map

; Header label
const header "     0 1 2 3 4 5 6 7 8 9 a b c d e f"

; Second part of row prefix
const prefixEnd "0 : "

; First label is the entry point
.entry:
	call .main
	ret

; No arguments
; No return
.main:
	push A
	push X
	push Y
	; Load address to header
	mov low(*header), X
	mov high(*header), Y
	; Print header
	call .xStringPrint
	; Reset loop counter
	clr A
.main_loop:
	push A
	; Calc column number
	and 0x0f, A
	cmp 0, A
	pop A
	jne .main_loop_print_char
	; Print new line every 16 chars
	call .xStringNewLine
	; Print prefix fo current row
	call .printPrefix
.main_loop_print_char:
	call .xStringPrintChar
	push A
	; Calc column number
	and 0x0f, A
	cmp 0xf, A
	pop A
	; Do not print space for last char in row
	je .main_loop_no_space
	push A
	; Print space between chars
	mov ' ', A
	call .xStringPrintChar
	pop A
.main_loop_no_space:
	; Repeat loop 255+1 times
	loope 255, A, .main_loop
	; Refresh the screen
	call .xLcdRefresh
	pop Y
	pop X
	pop A
	ret

; Arguments:
;	A - Char number
; No return
.printPrefix:
	push A
	push X
	push Y
	; Calc row number
	swap A
	and 0x0f, A
	; Print high part of hex
	call .printHalfHex
	; Load address to prefix end
	mov low(*prefixEnd), X
	mov high(*prefixEnd), Y
	; Print prefix end
	call .xStringPrint
	pop Y
	pop X
	pop A
	ret

; Arguments:
;	A - Char number (4 bit)
; No return
.printHalfHex:
	push A
	; Change 4 bit hex to ASCII char
	cmp 0x9, A
	jge .printHalfHex_number
	cmp 0xf, A
	jge .printHalfHex_letter
	jmp .printHalfHex_end
	; Print digit
.printHalfHex_number:
	add '0', A
	call .xStringPrintChar
	jmp .printHalfHex_end
	; Print letter
.printHalfHex_letter:
	sub 0xa, A
	add 'a', A
	call .xStringPrintChar
.printHalfHex_end:
	pop A
	ret
