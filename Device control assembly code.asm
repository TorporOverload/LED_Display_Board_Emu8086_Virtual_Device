#start=LED_Display_Board.exe#

; LED DISPLAY BOARD CONTROLLER PROGRAM
;
; IMPORTANT: This program must be run in EMU8086 with Administrator privileges
; for proper port I/O operations. Without admin rights, the program may fail
; to communicate with the LED display properly.
;
;
; NOTE: This program requires Microsoft Visual C++ runtime 
;       You can download it here: https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist?view=msvc-170
;  
;
;
; PORT MAPPINGS:
; - Port 10: Speed control (0-20)
; - Port 20: Display status (1=writing, 0=done writing, 99=terminate)
; - Ports 150-251: Character display positions
;
; PROGRAM FLOW:
; 1. Get text input from user
; 2. Get speed setting (0-20)
; 3. Display text on LED board
; 4. Accept commands (update/exit)


.MODEL SMALL
.STACK 100H
.DATA

PROMPT_STRING DB 'Enter a string: $'
PROMPT_NUMBER DB 'Enter speed (0-20): $'
PROMPT_INVALID_NUMBER DB 'Invalid! The value for speed should be between 0 and 20.$'
PROMPT_PROCESSING DB 'Processing. Please wait...$'
PROMPT_INVALID_COMMAND DB 'Invalid Command. Enter "exit" to exit the program or "update" to change the text.$'
PROMPT_COMMAND_MSG DB 'Displaying the text. Enter "exit" to exit the program or "update" to change the text: $'

; Bytes 2-102: String characters
INPUT_STRING DB 102            ; Max length byte
    DB 0                       ; Actual length byte (filled by DOS)
    DB 101 DUP (?)            ; Character buffer

; Variables for speed input processing
INPUT_NUMBER DB 0              ; Final speed value (0-20)
TEMP_NUM DB 0                 ; Temporary storage for number validation

INPUT_COMMAND DB 12           ; Max length byte
    DB 0                      ; Actual length byte
    DB 11 DUP (?)            ; Command buffer

; Valid command strings for comparison
EXIT_CMD DB 'exit'            ; Exit command string
UPDATE_CMD DB 'update'        ; Update command string
END_MARKER DB 0FFH            ; End-of-string marker for LED display (0xFF)

.CODE

MAIN PROC
    
; INITIALIZATION
    ; Set LED display status to "writing" mode via port 20
    MOV AL, 1                  ; AL = 1 (writing mode)
    MOV DX, 20                 ; DX = port 20 (status port)
    OUT DX, AL                 ; Send status to port
    
    ; Initialize data segment registers
    MOV AX, @DATA             ; Get data segment address
    MOV DS, AX                ; Set DS to data segment
    MOV ES, AX                ; Set ES to data segment (for string operations)

; GET INITIAL STRING INPUT
    ; Display string prompt
    LEA DX, PROMPT_STRING     ; Load prompt address
    MOV AH, 9                 ; DOS print string function
    INT 21H                   ; Call DOS

    ; Get string input using DOS buffered input
    LEA DX, INPUT_STRING      ; Load input buffer address
    MOV AH, 0Ah              ; DOS buffered input function
    INT 21H                  ; Call DOS

; SPEED INPUT PROCESSING
GET_NUMBER:
    ; Print newline for formatting
    MOV AH, 2                 ; DOS character output function
    MOV DL, 13                ; Carriage return
    INT 21H
    MOV DL, 10                ; Line feed
    INT 21H

    ; Display speed prompt
    LEA DX, PROMPT_NUMBER
    MOV AH, 9
    INT 21H

    ; Clear registers for number input processing
    XOR BL, BL                ; BL will store first digit
    XOR BH, BH                ; BH will store second digit (if any)

; FIRST DIGIT PROCESSING
    MOV AH, 01                ; DOS character input function
    INT 21H                   ; Get first character
    CMP AL, 13                ; Check for immediate Enter key
    JE INVALID_INPUT_NUMBER   ; If Enter pressed, invalid input

    ; Validate first digit (must be 0-9)
    CMP AL, '0'               ; Less than '0'?
    JB INVALID_INPUT_NUMBER   ; If yes, invalid
    CMP AL, '9'               ; Greater than '9'?
    JA INVALID_INPUT_NUMBER   ; If yes, invalid
    SUB AL, '0'               ; Convert ASCII to numeric
    MOV BL, AL                ; Store first digit

; SECOND DIGIT PROCESSING (OPTIONAL)
    MOV AH, 01                ; Get second character
    INT 21H
    CMP AL, 13                ; Check if Enter pressed
    JE VALIDATE_SINGLE_DIGIT  ; If yes, process single digit

    ; Validate second digit
    CMP AL, '0'
    JB INVALID_INPUT_NUMBER
    CMP AL, '9'
    JA INVALID_INPUT_NUMBER
    SUB AL, '0'               ; Convert ASCII to numeric
    MOV BH, AL                ; Store second digit

    ; Ensure Enter key is pressed next
    MOV AH, 01
    INT 21H
    CMP AL, 13
    JNE INVALID_INPUT_NUMBER

; TWO-DIGIT NUMBER CALCULATION
    MOV AL, BL                ; Get first digit
    MOV CL, 10                ; Multiplier = 10
    MUL CL                    ; AL = first digit * 10
    ADD AL, BH                ; Add second digit
    MOV TEMP_NUM, AL          ; Store result

    ; Validate final number (must be = 20)
    CMP TEMP_NUM, 20
    JG INVALID_INPUT_NUMBER

    ; Store valid number
    MOV AL, TEMP_NUM
    MOV INPUT_NUMBER, AL

    JMP VALID_INPUT

; SINGLE DIGIT VALIDATION
VALIDATE_SINGLE_DIGIT:
    MOV AL, BL                ; Get the single digit
    CMP AL, 20                ; Must be = 20
    JG INVALID_INPUT_NUMBER
    MOV INPUT_NUMBER, AL
    JMP VALID_INPUT

; INVALID NUMBER HANDLING
INVALID_INPUT_NUMBER:
    ; Print newline
    MOV AH, 2
    MOV DL, 13
    INT 21H
    MOV DL, 10
    INT 21H
    
    ; Display error message
    LEA DX, PROMPT_INVALID_NUMBER
    MOV AH, 9
    INT 21H
    JMP GET_NUMBER            ; Try again

; SPEED SETTING AND DISPLAY INITIALIZATION
VALID_INPUT:
    ; Send speed to LED controller
    MOV AL, INPUT_NUMBER      ; Get validated speed
    MOV DX, 10                ; Port 10 = speed control
    OUT DX, AL                ; Set speed

; TEXT DISPLAY PROCESSING
PRINT_LOOP:
    ; Print newline and processing message
    MOV AH, 2
    MOV DL, 13
    INT 21H
    MOV DL, 10
    INT 21H

    LEA DX, PROMPT_PROCESSING
    MOV AH, 9
    INT 21H

    ; Initialize string processing
    LEA SI, INPUT_STRING+2    ; SI points to first character (skip length bytes)
    MOV CL, INPUT_STRING+1    ; CL = string length
    MOV DX, 150               ; Start at port 150

; CHARACTER OUTPUT LOOP
PRINT_CHAR:
    CMP CL, 0                 ; Check if all characters processed
    JE PRINT_END_MARKER       ; If yes, add end marker

    ; Output character to LED display
    MOV AL, [SI]              ; Get character
    OUT DX, AL                ; Send to current port

    ; Handle port wraparound (150-251)
    INC DX                    ; Next display position
    CMP DX, 252               ; Past last position?
    JL CONTINUE_PRINT         ; If no, continue
    MOV DX, 150               ; If yes, wrap to start

CONTINUE_PRINT:
    INC SI                    ; Next character
    DEC CL                    ; Decrement counter
    JMP PRINT_CHAR           ; Process next character

; END MARKER AND STATUS UPDATE
PRINT_END_MARKER:
    ; Add end marker to display
    MOV AL, END_MARKER        ; 0xFF end marker
    OUT DX, AL                ; Send to next port

    ; Update display status to "done"
    MOV AL, 0                 ; Status = done writing
    MOV DX, 20                ; Status port
    OUT DX, AL

    ; Print newline
    MOV AH, 2
    MOV DL, 13
    INT 21H
    MOV DL, 10
    INT 21H

; COMMAND PROCESSING
GET_COMMAND:
    ; Display command prompt
    LEA DX, PROMPT_COMMAND_MSG
    MOV AH, 9
    INT 21H

    ; Get command input
    LEA DX, INPUT_COMMAND
    MOV AH, 0Ah
    INT 21H

    ; Validate command length
    MOV AL, INPUT_COMMAND+1   ; Get actual length
    CMP AL, 4                 ; Length = 4 for "exit"?
    JE CHECK_EXIT
    CMP AL, 6                 ; Length = 6 for "update"?
    JE CHECK_UPDATE
    JMP INVALID_COMMAND

; EXIT COMMAND VALIDATION
CHECK_EXIT:
    ; Compare input with "exit"
    LEA SI, EXIT_CMD          ; SI = expected command
    LEA DI, INPUT_COMMAND+2   ; DI = user input
    MOV CX, 4                 ; Compare 4 characters
    CLD                       ; Clear direction flag (forward comparison)
    REPE CMPSB               ; Compare strings
    JNE INVALID_COMMAND      ; If not equal, invalid
    JMP EXIT_PROGRAM         ; If equal, exit

; UPDATE COMMAND VALIDATION
CHECK_UPDATE:
    ; Compare input with "update"
    LEA SI, UPDATE_CMD        ; SI = expected command
    LEA DI, INPUT_COMMAND+2   ; DI = user input
    MOV CX, 6                 ; Compare 6 characters
    CLD                       ; Clear direction flag
    REPE CMPSB               ; Compare strings
    JNE INVALID_COMMAND      ; If not equal, invalid
    
    ; Print newline before restart
    MOV AH, 2
    MOV DL, 13
    INT 21H
    MOV DL, 10
    INT 21H
    JMP MAIN                 ; Restart program

; INVALID COMMAND HANDLING
INVALID_COMMAND:
    ; Print newline
    MOV AH, 2
    MOV DL, 13
    INT 21H
    MOV DL, 10
    INT 21H
    
    ; Display error message
    LEA DX, PROMPT_INVALID_COMMAND
    MOV AH, 9
    INT 21H
    JMP GET_COMMAND          ; Try again

EXIT_PROGRAM:
    JMP EXIT

; PROGRAM TERMINATION
EXIT:
    ; Signal termination to LED controller
    MOV AL, 99                ; Termination status
    MOV DX, 20                ; Status port
    OUT DX, AL                ; Send status

    ; Terminate program
    MOV AH, 4Ch               ; DOS terminate function
    INT 21h

MAIN ENDP
END MAIN
