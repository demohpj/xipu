; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

map ../../../bin/rom/rom.map

; Start message
const appListHeader "App list:"

; Error messages
const appListIsEmpty "Warning: App list is empty"
const appListOpenError "Error: Cannot open the app dir"

; Read buffer to store one sector
var readBuffer[256]

; App quantity in app dir
var appQuantity

; App counter
var appCount

; First label is the entry point
.entry:
	call .main
	ret

; No arguments
; No return
.main:
	push A
	push B
	push X
	push Y
	; List app
	call .xFSListApp
	cmp 0, A
	je .main_error
	; Get list size
	call .xFSSize
	; Do not process empty list
	push B
	or A, B
	; 0 == B
	cmp 0, B
	pop B
	je .main_empty
	; Save app quantity
	push A
	push B
	swap A
	and 0x0f, A
	swap B
	and 0xf0, B
	or B, A
	str A, *appQuantity
	pop B
	pop A
	str 0, *appCount
	; Calc how many block are needed to read
	dec A
	decc B
	; Set starting block
	mov 0, A
	; Print header
	mov low(*appListHeader), X
	mov high(*appListHeader), Y
	call .xStringPrint
	call .xStringNewLine
	call .xStringNewLine
	; Run copy loop B+1 times
.main_loop:
	; Set buffer address
	mov low(*readBuffer), X
	mov high(*readBuffer), Y
	; Read block
	call .xFSReadBlock
	inc A
	; Print apps from block
	call .printBlock
	; Exit loop if all apps was printed
	push A
	push B
	ldr *appCount, A
	ldr *appQuantity, B
	cmp A, B
	pop B
	pop A
	jge .main_loop_exit
	loopz B, .main_loop
.main_loop_exit:
	; Refresh the screen
	call .xLcdRefresh
	pop Y
	pop X
	pop B
	pop A
	ret
.main_error:
	; Print open error
	mov low(*appListOpenError), X
	mov high(*appListOpenError), Y
	call .xStringPrint
	; Refresh the screen
	call .xLcdRefresh
	pop Y
	pop X
	pop B
	pop A
	ret
.main_empty:
	; Print empty list message
	mov low(*appListIsEmpty), X
	mov high(*appListIsEmpty), Y
	call .xStringPrint
	; Refresh the screen
	call .xLcdRefresh
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; No return
.printBlock:
	push A
	push B
	push X
	push Y
	; Set buffer address
	mov low(*readBuffer), X
	mov high(*readBuffer), Y
	mov 15, B
	; Run print loop B+1 times
.printBlock_loop:
	; Print app name
	call .printName
	; Exit loop if all apps was printed
	push B
	xchg *appCount, A
	ldr *appQuantity, B
	inc A
	cmp A, B
	pop B
	xchg *appCount, A
	jge .main_loop_exit
	; Go to next entry
	add 16, X
	incc Y
	loopz B, .printBlock_loop
.printBlock_loop_exit:
	pop Y
	pop X
	pop B
	pop A
	ret

; No arguments
; No return
.printName:
	push A
	push B
	; Print a comma if it is not a first name
	ldr *appCount, A
	cmp 0, A
	je .printName_noComma
	mov '\,', A
	call .xStringPrintChar
	mov ' ', A
	call .xStringPrintChar
.printName_noComma:
	; Check if it is enough space for next name in line
	call .xStringLength
	mov A, B
	call .xStringGetCursorX
	add B, A
	; 40 > A
	cmp 39, A
	jg .printName_noNewLine
	; Print name in new line
	call .xStringNewLine
.printName_noNewLine:
	; Print name
	call .xStringPrint
	pop B
	pop A
	ret
