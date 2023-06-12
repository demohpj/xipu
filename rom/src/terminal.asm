; Author: Pawel Jablonski
; E-mail: pj@xirx.net
; WWW: xirx.net
; GIT: git.xirx.net
;
; License: You can use this code however you like
; but leave information about the original author.
; Code is free for non-commercial and commercial use.

include lcd.asm
include string.asm
include speaker.asm
include fs.asm

; Command buffer
var commandBuffer[39]

; No arguments
; No return
global .xTerminal:
	; Reset
	call .xTerminalReset
	; Print start message
	call .xTerminalStartMessage
	; Play start sound
	call .xTerminalStartSound
	; Refresh screen
	call .xLcdRefresh
	
.xTerminal_loop:
	; Get command
	call .xTerminalGetCommand
	; Check command length
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	call .xStringLength
	; 0 == A
	cmp 0, A
	je .xTerminal_loop
	; Run app if command is not empty
	call .xTerminalRunApp
	; Get another command
	jmp .xTerminal_loop
	
; No arguments
; No return
external .xTerminalReset:
	push X
	push Y
	; Reset color
	call .xTerminalColorReset
	; Clear screen
	call .xLcdClear
	; Reset cursor position
	mov 0, X
	mov 0, Y
	call .xStringSetCursor
	pop Y
	pop X
	ret

; No arguments
; No return
external .xTerminalColorReset:
	push A
	; Change colors to default
	mov 0x0, A
	call .xLcdSetBackground
	mov 0xf, A
	call .xLcdSetForeground
	pop A
	ret

; Start message to show
const messageSystem "XiOS 0.1"
const messageFreeRom "32K Program memory free"
const messageFreeRam "28K Data memory free"

; No arguments
; No return
.xTerminalStartMessage:
	; Print name and system version
	mov low(*messageSystem), X
	mov high(*messageSystem), Y
	call .xStringPrintRom
	call .xStringNewLine
	call .xStringNewLine
	; Print information about ROM for apps
	mov low(*messageFreeRom), X
	mov high(*messageFreeRom), Y
	call .xStringPrintRom
	call .xStringNewLine
	; Print information about RAM for apps
	mov low(*messageFreeRam), X
	mov high(*messageFreeRam), Y
	call .xStringPrintRom
	ret

; No arguments
; No return
.xTerminalStartSound:
	push A
	; Set fill parameter
	mov 8, A
	call .xSpeakerSetFill
	; Set volume parameter
	mov 8, A
	call .xSpeakerSetVolume
	; Set time parameter
	mov 24, A
	call .xSpeakerSetTime
	; Play e6 tone
	mov 68, A
	call .xSpeakerSetNote
	call .xSpeakerAddNote
	; Play e5 tone
	mov 56, A
	call .xSpeakerSetNote
	call .xSpeakerAddNote
	pop A
	ret

; No arguments
; No return
.xTerminalGetCommand:
	; Reset color
	call .xTerminalColorReset
	; Do not make a new line if the cursor is at beginning
	call .xStringGetCursorY
	cmp 0, A
	je .xTerminalGetCommand_no_newLine
	call .xStringNewLine
	call .xStringNewLine
.xTerminalGetCommand_no_newLine:
	; Show command line
	mov '>', A
	call .xStringPrintChar
	call .xLcdRefresh
	mov 39, A
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	call .xStringRead
	call .xStringNewLine
	ret

; Error messages for run app
const appNameLengthError "Error: Bad length of app name"
const appNameUnknownNameError "Error: Unknown app"

; Address of app argument string
var appArgX
var appArgY

; No arguments
; No return
.xTerminalRunApp:
	push A
	push X
	push Y
	; Load command buffer address
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	; Load first char
	ldr A
	; First char cannot be a space
	cmp ' ', A
	je .xTerminalRunApp_appNameLengthError
	; Find a space after command
	mov ' ', A
	call .xStringFind
	; If the space is not found then it is no arguments
	cmp 0, A
	je .xTerminalRunApp_no_arg
	; Move address at space after app name
	add A, X
	incc Y
	; Replace a space by end of string char
	str 0
	; Move address at begin of arguments
	inc X
	incc Y
	call .xStringLength
	; It is no arguments if string length is equal zero
	cmp 0, A
	je .xTerminalRunApp_no_arg
	jmp .xTerminalRunApp_checkAppName
	; Set arguments address for case where no argument was found
.xTerminalRunApp_no_arg:
	; Load command buffer address
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	call .xStringLength
	; Move address at end+1 of app name
	inc A
	add A, X
	incc Y
	; Store end of string char
	str 0
	; Check app name
.xTerminalRunApp_checkAppName:
	; Store arguments address to local variables
	str X, *appArgX
	str Y, *appArgY
	; Load command buffer address
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	; Check app name length
	call .xStringLength
	; If app name lenght is equal to zero then show error message
	cmp 0, A
	je .xTerminalRunApp_appNameLengthError
	; If app name lenght is greather than 8 then show error message
	cmp 8, A
	; 8 < A
	jl .xTerminalRunApp_appNameLengthError
	; Run load app procedure
	call .xTerminalLoadApp
	; If load app procedure returned flase then show error message
	cmp 0, A
	je .xTerminalRunApp_appUnknownNameError
	; Make a new line before app output
	call .xStringNewLine
	; Load args address
	ldr *appArgX, X
	ldr *appArgY, Y
	; Run app
	call 0x8000
	pop Y
	pop X
	pop A
	ret
	; Show length error message
.xTerminalRunApp_appNameLengthError:
	call .xStringNewLine
	mov low(*appNameLengthError), X
	mov high(*appNameLengthError), Y
	call .xStringPrintRom
	pop Y
	pop X
	pop A
	ret
	; Show unknow name of app error message
.xTerminalRunApp_appUnknownNameError:
	call .xStringNewLine
	mov low(*appNameUnknownNameError), X
	mov high(*appNameUnknownNameError), Y
	call .xStringPrintRom
	pop Y
	pop X
	pop A
	ret

; No arguments
; Return:
; 	A - success (0 - error, 1 - ok)
.xTerminalLoadApp:
	push B
	push X
	push Y
	; Set name of app
	mov low(*commandBuffer), X
	mov high(*commandBuffer), Y
	call .xFSSetName
	; Open app
	call .xFSOpenApp
	cmp 0, A
	je .xTerminalLoadApp_fail
	; Get app size and compare to max loadable size
	call .xFSSize
	; 0x80 < B
	cmp 0x80, B
	jl .xTerminalLoadApp_fail
	; Do not load empty file
	push B
	or A, B
	; 0 == B
	cmp 0, B
	pop B
	je .xTerminalLoadApp_fail
	; Calc how many block are needed to read
	dec A
	decc B
	; Set starting block
	mov 0, A
	; Set starting address
	mov 0x00, X
	mov 0x80, Y
	; Run copy loop B+1 times
.xTerminalLoadApp_loop:
	call .xFSReadBlock
	inc A
	inc Y
	loopz B, .xTerminalLoadApp_loop
	pop Y
	pop X
	pop B
	; Return true for success
	mov 1, A
	ret
	; Fail exit of load app
.xTerminalLoadApp_fail:
	pop Y
	pop X
	pop B
	; Return false for fail
	mov 0, A
	ret
	
