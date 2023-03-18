; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

map ../../../bin/rom/rom.map

; 4 full rectangles to see color
const colorBar 0xDB, 0xDB, 0xDB, 0xDB, ' ', 0x00

; Color names
const color_0 "black          "
const color_1 "dark blue      "
const color_2 "green          "
const color_3 "persian green  "
const color_4 "bright red     "
const color_5 "flirt          "
const color_6 "buddha gold    "
const color_7 "eagle          "
const color_8 "abbey          "
const color_9 "dodger blue    "
const color_a "screaming green"
const color_b "aquamarine     "
const color_c "sunset orange  "
const color_d "pink flamingo  "
const color_e "gorse          "
const color_f "white          "

; First label is the entry point
.entry:
	call .main
	ret

; No arguments
; No return
.main:
	push A
	; Clear loop counter
	clr A
.main_loop:
	; Show dark color on left
	call .printColor
	; Show light color on right
	xor 0b1000, A
	call .printColor
	xor 0b1000, A
	; After last color do not print new line
	cmp 0xf, A
	je .main_loop_no_newline
	call .xStringNewLine
.main_loop_no_newline:
	loope 7, A, .main_loop
	; Refresh the screen
	call .xLcdRefresh
	pop A
	ret

; Arguments:
; 	A - color (4 bit)
; No return
.printColor:
	push A
	push X
	push Y
	; Color should be 4 bit
	cmp 0xf, A
	; 0xf < A
	jl .printColor_end
	; Set foreground to color from reg A
	call .xLcdSetForeground
	; Load address to color bar
	mov low(*colorBar), X
	mov high(*colorBar), Y
	; Print color bar with a sample of color
	call .xStringPrint
	push A
	; Set foreground color to default
	mov 0xf, A
	call .xLcdSetForeground
	pop A
	; Load address to color name
	mov low(*color_0), X
	mov high(*color_0), Y
	; Every color name takes 16 bytes of memory
	swap A
	; Move address to current color name
	add A, X
	incc Y
	; Print color name
	call .xStringPrint
.printColor_end:
	pop Y
	pop X
	pop A
	ret
