; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

map ../../../bin/rom/rom.map

; TX buffer usage counter
var txLength
; TX buffer
var txBuffer[39]

; RX buffer usage counter
var rxLength
; RX buffer
var rxBuffer[39]

; Displayed lines counter
var lineQuantity

; First label is the entry point
.entry:
	call .main
	ret

; No arguments
; No return
.main:
	push A
	; Reset counters and buffers
	str 0, *txLength
	str 0, *txBuffer
	str 0, *rxLength
	str 0, *rxBuffer
	str 0, *lineQuantity
	; Clear screen
	call .xLcdClear
	; Show input
	call .showInput
	; Refresh screen
	call .xLcdRefresh
	; Keyboard and rs232 event loop
.main_loop:
	; Check if key was clicked
	call .xKeyboardIsDataReady
	cmp 0, A
	jne .main_processKey
	; Check if RX data is ready to read
	call .xRS232IsRxDataReady
	cmp 0, A
	jne .main_processReceive
	; Jump back to the beginning of event loop
	jmp .main_loop
	; Start processing key press event
.main_processKey:
	call .xKeyboardGetChar
	; Exit if clicked key is ESC
	cmp 0x1b, A
	je .main_end
	call .processKey
	; Jump back to the beginning of event loop
	jmp .main_loop
	; Start processing RX data event
.main_processReceive:
	call .xRS232Receive
	call .processReceive
	; Jump back to the beginning of event loop
	jmp .main_loop
.main_end:
	pop A
	ret

; Arguments:
;	A - keycode
; No return
.processKey:
	push A
	; Check for backspace
	cmp 0x08, A
	je .processKey_backspace
	; Check for enter
	cmp 0x0d, A
	je .processKey_enter
	; Check for not printable char
	; 0x20 > A
	cmp 0x20, A
	jg .processKey_notPrint
	; 0x80 <= A
	cmp 0x80, A
	jle .processKey_notPrint
	push B
	; Check current length of buffer
	ldr *txLength, B
	; 38 =< B
	cmp 38, B
	jle .processKey_bufferFull
	push X
	push Y
	; Set address to TX buffer
	mov low(*txBuffer), X
	mov high(*txBuffer), Y
	; Move to working position
	add B, X
	incc Y
	; Store keycode
	str A
	; Clear register
	clr A
	; Move to next position
	inc X
	incc Y
	; Store termination code
	str A
	; Increment TX buffer position
	inc B
	; Store current TX buffer position
	str B, *txLength
	; Show input
	call .showInput
	; Refresh screen
	call .xLcdRefresh
.processKey_end:
	pop Y
	pop X
.processKey_bufferFull:
	pop B
.processKey_notPrint:
	pop A
	ret
	; Remove last char from TX buffer
.processKey_backspace:
	; Load TX buffer position
	ldr *txLength, A
	; 0 == A
	cmp 0, A
	; Do nothing if buffer is empty
	je .processKey_notPrint
	push X
	push Y
	; Decrement buffer position
	dec A
	; Store current TX buffer position
	str A, *txLength
	; Set address to TX buffer
	mov low(*txBuffer), X
	mov high(*txBuffer), Y
	; Move to working position
	add A, X
	incc Y
	; Clear register
	clr A
	; Store termination code
	str A
	; Show input
	call .showInput
	; Refresh screen
	call .xLcdRefresh
	pop Y
	pop X
	pop A
	ret
	; Enter key was pressed
.processKey_enter:
	; Load TX buffer position
	ldr *txLength, A
	; 0 == A
	cmp 0, A
	; Do nothing if buffer is empty
	je .processKey_notPrint
	; Send buffer content
	call .sendString
	; Print buffer content
	call .printSentString
	; Reset TX counter and TX buffer
	str 0, *txLength
	str 0, *txBuffer
	; Show input
	call .showInput
	; Refresh screen
	call .xLcdRefresh
	pop A
	ret

; Arguments
;	A - RX data
; No return
.processReceive:
	; Check for end of string
	cmp 0, A
	je .processReceive_endString
	; Check for LF code
	cmp 0x0a, A
	je .processReceive_endString
	; Check for not printable char
	; 0x20 > A
	cmp 0x20, A
	jg .processReceive_end
	; 0x80 <= A
	cmp 0x80, A
	jle .processReceive_end
	push B
	push X
	push Y
	; Load RX buffer position
	ldr *rxLength, B
	; Set address to RX buffer
	mov low(*rxBuffer), X
	mov high(*rxBuffer), Y
	; Move to working position
	add B, X
	incc Y
	; Store RX data
	str A
	; Move to next position
	inc X
	incc Y
	; Store termination code
	str 0
	; Increment RX buffer position
	inc B
	; Store current RX buffer position
	str B, *rxLength
	pop Y
	pop X
	; Check current length of buffer
	; 38 > B
	cmp 38, B
	pop B
	; Go to end if is is free space in RX buffer
	jg .processReceive_end
	push A
	; Prepare termination code
	clr A
	; Call itself for process full buffer
	call .processReceive
	pop A
	ret
.processReceive_endString:
	; Load RX buffer position 
	ldr *rxLength, A
	; 0 == A
	cmp 0, A
	; Do not print empty string
	je .processReceive_end
	; Print buffer content
	call .printReceivedString
	; Reset RX counter and RX buffer
	str 0, *rxLength
	str 0, *rxBuffer
	; Show input
	call .showInput
	; Refresh screen
	call .xLcdRefresh
.processReceive_end:
	ret

; No arguments
; No return
.showInput:
	push A
	push B
	push X
	push Y
	; Set draw start position
	mov 0, X
	mov 28, Y
	call .xStringSetCursor
	; Set clear char
	mov ' ', A
	; Reset counter
	clr B
.showInput_blankLoop:
	; Print clear char
	call .xStringPrintChar
	; Draw 39+1 times
	loope 39, B, .showInput_blankLoop
	; Change colors for input
	mov 0xf, A
	call .xLcdSetBackground
	mov 0x0, A
	call .xLcdSetForeground
	; Set clear char
	mov ' ', A
	; Reset counter
	clr B
.showInput_inputLoop:
	call .xStringPrintChar
	; Draw 39+1 times
	loope 39, B, .showInput_inputLoop
	; Go back at beginning of row
	mov 0, A
	call .xStringSetCursorX
	; Print prompt
	mov '>', A
	call .xStringPrintChar
	; Set address to TX buffer
	mov low(*txBuffer), X
	mov high(*txBuffer), Y
	; Print buffer content
	call .xStringPrint
	; Print cursor
	mov 0xDB, A
	call .xStringPrintChar
	; Turn back default colors
	mov 0x0, A
	call .xLcdSetBackground
	mov 0xf, A
	call .xLcdSetForeground
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; No return
.sendString:
	push A
	push B
	push X
	push Y
	; Set address to TX buffer
	mov low(*txBuffer), X
	mov high(*txBuffer), Y
	; Load TX buffer position 
	ldr *txLength, B
.sendString_loop:
	; Load char to send
	ldr A
	; Check for end of string
	cmp 0, A
	jne .sendString_send
	; Set LF code
	mov 0x0a, A
.sendString_send:
	; Send char
	call .xRS232Send
	; Move to next position
	inc X
	incc Y
	; Repeat B+1 times
	loopz B, .sendString_loop
	pop Y
	pop X
	pop B
	pop A
	ret

; Arguments:
;	X - low address
;	Y - high address
; No return
.printString:
	push A
	; Load drawn line quantity
	ldr *lineQuantity, A
	; 27 <= A
	cmp 27, A
	; Count only 27+1 first lines else scroll screen
	jle .printString_scroll
	; Increment counter
	inc A
	; Store counter to line quantity
	str A, *lineQuantity
	jmp .printString_print
.printString_scroll:
	; Scroll screen to make a space to draw a new line of text
	call .xLcdScroll
.printString_print:
	push X
	push Y
	; Move cursor to right place to print
	mov 0, X
	mov A, Y
	call .xStringSetCursor
	pop Y
	pop X
	; Print string from buffer
	call .xStringPrint
	pop A
	ret

; No arguments
; No return
.printSentString:
	push X
	push Y
	; Set address to TX buffer
	mov low(*txBuffer), X
	mov high(*txBuffer), Y
	; Print buffer on screen
	call .printString
	pop Y
	pop Y
	ret

; No arguments
; No return
.printReceivedString:
	push A
	; Set foregound color to green
	mov 0x0a, A
	call .xLcdSetForeground
	push X
	push Y
	; Set address to RX buffer
	mov low(*rxBuffer), X
	mov high(*rxBuffer), Y
	; Print buffer on screen
	call .printString
	pop Y
	pop X
	; Set foregound color to default
	mov 0x0f, A
	call .xLcdSetForeground
	pop A
	ret
