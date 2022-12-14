; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include io.asm
include keyboard.asm
include led.asm
include lcd.asm
include rs232.asm
include rtc.asm
include speaker.asm
include fs.asm
include string.asm
include terminal.asm

; First label is the entry point
.entry:
	; Init IO data bus
	call .xIOInit
	; Init screen cursor
	call .xStringInit

.main:
	; Turn on error led
	call .xLedRunOn
	; Run terminal
	call .xTerminal
	
; Endless loop
.loop:
	jmp .loop
