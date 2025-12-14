.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib ../Debug/StaticLibrary.lib
ExitProcess PROTO:DWORD 
.stack 4096


 outlich PROTO : DWORD

 outrad PROTO : DWORD

 concat PROTO : DWORD, : DWORD, : DWORD

 power PROTO : DWORD, : DWORD, : DWORD

 factorial PROTO : DWORD, : DWORD

 compare PROTO : DWORD, : DWORD, : DWORD

 rnd PROTO : DWORD, : DWORD, : DWORD

 slength PROTO : DWORD, : DWORD

 atoii  PROTO : DWORD,  : DWORD

 copystr PROTO : DWORD, : DWORD

.const
		newline byte 13, 10, 0
		LTRL1 sdword 10
		LTRL2 sdword 12
		LTRL3 byte 'Hello', 0
		LTRL4 sdword 77
		LTRL5 byte 'sum =', 0
		LTRL6 byte 'mul =', 0
		LTRL7 byte 'octal =', 0
		LTRL8 sdword 5
		LTRL9 byte 'x equals 5', 0
		LTRL10 byte 'x equals 10', 0
		LTRL11 byte 'x is different', 0
.data
		temp sdword ?
		buffer byte 256 dup(0)
		mainx dword 0
		mainy dword 0
		mainmsg dword ?
		mainsum dword 0
		mainmul dword 0
		mainoct dword 0
		mainresult dword 0
		mainless dword 0
.code

;----------- MAIN ------------
main PROC
push LTRL1

pop ebx
mov mainx, ebx

push LTRL2

pop ebx
mov mainy, ebx

mov mainmsg, offset LTRL3
push mainx
push mainy
pop ebx
pop eax
add eax, ebx
push eax

pop ebx
mov mainsum, ebx

push mainx
push mainy
pop ebx
pop eax
imul eax, ebx
push eax

pop ebx
mov mainmul, ebx

push LTRL4

pop ebx
mov mainoct, ebx

push mainx
push mainy
pop ebx
pop eax
cmp eax, ebx
setg al
movzx eax, al
push eax

pop ebx
mov mainresult, ebx

push mainx
push mainy
pop ebx
pop eax
cmp eax, ebx
setle al
movzx eax, al
push eax

pop ebx
mov mainless, ebx


push mainmsg
call outrad


push offset LTRL5
call outrad


push mainsum
call outlich


push offset LTRL6
call outrad


push mainmul
call outlich


push offset LTRL7
call outrad


push mainoct
call outlich

mov eax, mainx

cmp eax, 5
jne case_next1


push offset LTRL9
call outrad

jmp switch_end1

case_next1:
cmp eax, 10
jne case_next1


push offset LTRL10
call outrad

jmp switch_end1

default_case1:


push offset LTRL11
call outrad

jmp switch_end1

switch_end1:

push 0
call ExitProcess
main ENDP
end main
