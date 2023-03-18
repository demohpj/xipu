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
const fileListHeader "File list:"

; Error messages
const fileListIsEmpty "Warning: File list is empty"
const fileListOpenError "Error: Cannot open the file dir"

; Read buffer to store one sector
var readBuffer[256]

; App quantity in app dir
var fileQuantity

; App counter
var fileCount

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
	; List file
	call .xFSListFile
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
	; Save file quantity
	push A
	push B
	swap A
	and 0x0f, A
	swap B
	and 0xf0, B
	or B, A
	str A, *fileQuantity
	pop B
	pop A
	str 0, *fileCount
	; Calc how many block are needed to read
	dec A
	decc B
	; Set starting block
	clr A
	; Print header
	mov low(*fileListHeader), X
	mov high(*fileListHeader), Y
	call .xStringPrint
	call .xStringNewLine
	; Run copy loop B+1 times
.main_loop:
	; Set buffer address
	mov low(*readBuffer), X
	mov high(*readBuffer), Y
	; Read block
	call .xFSReadBlock
	inc A
	; Print files from block
	call .printBlock
	; Exit loop if all files was printed
	push A
	push B
	ldr *fileCount, A
	ldr *fileQuantity, B
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
	mov low(*fileListOpenError), X
	mov high(*fileListOpenError), Y
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
	mov low(*fileListIsEmpty), X
	mov high(*fileListIsEmpty), Y
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
	; Print file name
	call .printName
	; Exit loop if all files was printed
	push B
	xchg *fileCount, A
	ldr *fileQuantity, B
	inc A
	cmp A, B
	pop B
	xchg *fileCount, A
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
	push X
	push Y
	ldr *fileCount, A
	and 0x1, A
	cmp 0, A
	jne .printName_noNewLine
	; Print name in new line
	call .xStringNewLine
	jmp .printName_noOffset
.printName_noNewLine:
	mov 20, A
	call .xStringSetCursorX
.printName_noOffset:
	; Print name
	call .xStringPrint
	mov ' ', A
	call .xStringPrintChar
	add 14, X
	incc Y
	ldr A
	inc X
	incc Y
	ldr B
	call .xStringPrintU16
	pop Y
	pop X
	pop B
	pop A
	ret
